#include <assert.h>
#include <stdint.h>
#include <math.h>
#include "mb.h"
#include "snapshot_registers.h"

static uint16_t reg(uint16_t address)
{
    UCHAR bytes[2];
    assert(eMBRegInputCB(bytes, address + 1, 1) == MB_ENOERR);
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}
static void snap(uint16_t seq)
{
    UCHAR bytes[2] = {seq >> 8, (UCHAR)seq};
    assert(eMBRegHoldingCB(bytes, 1, 1, MB_REG_WRITE) == MB_ENOERR);
}
int main(void)
{
    assert(reg(SNAP_READY) == 0);
    snap(42); /* No sample yet. */
    assert(reg(SNAP_SEQ) == 42 && reg(SNAP_READY) == 0);
    snapshot_publish(123.456f, 19, 0, 0x40, 12345);
    snap(42);
    assert(reg(SNAP_SEQ) == 42 && reg(SNAP_READY) == 1);
    assert(((uint32_t)reg(SNAP_ANGLE_HI) << 16 | reg(SNAP_ANGLE_LO)) == 123456001U); /* Rounded float input, not decimal literal. */
    assert(reg(SNAP_SECTOR) == 19 && reg(SNAP_ENCODER_STATE) == 0x40);
    assert(reg(SNAP_SAMPLE_LO) == 1 && reg(SNAP_TIME_LO) == 12345);
    snapshot_publish(255.000f, 100, 0, 0, 12350);
    snap(42); /* Retry must not change an already committed frame. */
    assert(reg(SNAP_SECTOR) == 19 && reg(SNAP_SAMPLE_LO) == 1);
    assert(((uint32_t)reg(SNAP_ANGLE_HI) << 16 | reg(SNAP_ANGLE_LO)) == 123456001U);
    snap(43);
    assert(reg(SNAP_SECTOR) == 100 && reg(SNAP_SAMPLE_LO) == 2);
    assert(((uint32_t)reg(SNAP_ANGLE_HI) << 16 | reg(SNAP_ANGLE_LO)) == 255000000U);
    snapshot_publish(180.0f, 20, 1, 0, 12360);
    snap(44);
    assert(reg(SNAP_SEQ) == 44 && reg(SNAP_READY) == 0);
    UCHAR out[22] = {0};
    assert(eMBRegInputCB(out, 1, SNAP_REG_COUNT) == MB_ENOERR);
    assert(out[0] == 0 && out[1] == 44 && out[2] == 0 && out[3] == 0);
    assert(eMBRegInputCB(out, 0, 1) == MB_ENOREG);
    assert(eMBRegInputCB(out, 11, 2) == MB_ENOREG);
    assert(eMBRegHoldingCB(out, 2, 1, MB_REG_WRITE) == MB_ENOREG);
    assert(eMBRegHoldingCB(out, 1, 1, MB_REG_READ) == MB_ENOREG);
    snapshot_publish(0.0f / 0.0f, 20, 0, 0, 12370);
    snap(45);
    assert(reg(SNAP_READY) == 0);
    /* Microdegree scaling, rounding, endpoints and invalid inputs. */
    const struct { float deg; uint32_t udeg; } cases[] = {
        {0.0f, 0U}, {90.5f, 90500000U},
        {12.345678f, 12345678U},
        {0.0000014f, 1U}, {0.0000016f, 2U},
        {0.0078125f, 7813U}, /* Exact half: round up. */
        {359.999969482421875f, 359999969U} /* Largest float below 360. */
    };
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        snapshot_publish(cases[i].deg, 20, 0, 0, 12380 + i);
        snap((uint16_t)(46 + i));
        assert(reg(SNAP_READY) == 1);
        uint32_t angle = (uint32_t)reg(SNAP_ANGLE_HI) << 16 | reg(SNAP_ANGLE_LO);
        assert(angle == cases[i].udeg);
    }
    const float invalid[] = {-1.0f, 360.0f, INFINITY, -INFINITY, NAN};
    for (unsigned i = 0; i < sizeof invalid / sizeof invalid[0]; ++i) {
        snapshot_publish(invalid[i], 20, 0, 0, 12400 + i);
        snap((uint16_t)(60 + i));
        assert(reg(SNAP_READY) == 0);
    }
    snapshot_publish(90.5f, 144, 0, 0, 12410);
    snap(70);
    assert(reg(SNAP_READY) == 0);
    return 0;
}
