#include <assert.h>
#include <stdint.h>
#include "mb.h"
#include "snapshot_registers.h"
#include "encoder_protocol.h"

extern int start_calibrate, auto_cal, start_offset_cal, offset_phase;
extern int start_angk_cal, anglek_phase, offset_cur, offset_start, offset_end;
extern int avg_minmax_num, old_sector;
extern float offset_minmax[128], buf_x3[128];
extern unsigned char encoder_state;
extern uint8_t backlight_width_en, i2c3_tx_wp;

static eMBErrorCode command(uint16_t c)
{
    UCHAR b[] = {c >> 8, c & 255};
    return eMBRegHoldingCB(b, ENCODER_CAL_COMMAND + 1, 1, MB_REG_WRITE);
}
static uint16_t reg(uint16_t addr)
{
    UCHAR b[2];
    assert(eMBRegInputCB(b, addr + 1, 1) == MB_ENOERR);
    return (uint16_t)(b[0] << 8 | b[1]);
}
int main(void)
{
    assert(reg(ENCODER_CAL_VERSION) == ENCODER_CAL_MAGIC);
    assert(command(0x12) == MB_EINVAL); /* Pass 2 requires pass 1. */
    assert(command(0x15) == MB_EINVAL);
    assert(command(0x111) == MB_EINVAL);
    buf_x3[2] = 123;
    assert(command(0x14) == MB_ENOERR);
    assert(start_angk_cal == 1 && anglek_phase == 0 && !backlight_width_en);
    assert(buf_x3[2] == 0 && reg(ENCODER_CAL_STATE) == 0x70);
    assert(reg(SNAP_ENCODER_STATE) == 0); /* LIVE does not mutate frozen. */
    assert(command(0x13) == MB_ETIMEDOUT);
    assert(command(0x14) == MB_ETIMEDOUT); /* No accidental restart. */
    assert(command(0x10) == MB_ENOERR);
    assert(!start_angk_cal && anglek_phase == 101 && backlight_width_en);
    assert(reg(ENCODER_CAL_STATE) == 0);
    offset_cur = 999;
    offset_minmax[10] = 12;
    assert(command(0x13) == MB_ENOERR);
    assert(start_offset_cal && offset_phase == 0 && offset_cur == 63);
    assert(offset_minmax[10] == 0 && encoder_state == 0x50);
    assert(command(0x10) == MB_ENOERR);
    assert(!start_offset_cal);
    i2c3_tx_wp = 0;
    assert(reg(ENCODER_EEPROM_READY) == 0);
    assert(command(0x11) == MB_ETIMEDOUT);
    assert(command(0x10) == MB_ENOERR); /* STOP allowed during EEPROM transfer. */
    assert(i2c3_tx_wp == 0); /* Never fake EEPROM completion. */
    i2c3_tx_wp = 1;
    assert(command(0x11) == MB_ENOERR);
    assert(start_calibrate == 1 && encoder_state == 0x10 && old_sector == -1);
    assert(command(0x12) == MB_ETIMEDOUT);
    start_calibrate = 0; encoder_state = 0x20; /* Algorithm finished. */
    assert(command(0x12) == MB_ENOERR);
    assert(start_calibrate == 2 && encoder_state == 0x30);
    auto_cal = 2;
    assert(command(0x10) == MB_ENOERR);
    assert(!auto_cal && !start_calibrate && !encoder_state);
    UCHAR b[32];
    assert(eMBRegInputCB(b, ENCODER_CAL_STATE + 1, 3) == MB_ENOERR);
    assert(eMBRegInputCB(b, ENCODER_CAL_STATE + 1, 4) == MB_ENOREG);
    assert(eMBRegInputCB(b, ENCODER_CAL_STATE, 2) == MB_ENOREG);
    assert(eMBRegInputCB(b, ENCODER_CAL_STATE + 1, 0) == MB_ENOREG);
    assert(eMBRegInputCB(b, 0xffff, 2) == MB_ENOREG);
    assert(eMBRegHoldingCB(b, ENCODER_CAL_COMMAND + 1, 2, MB_REG_WRITE) == MB_ENOREG);
    assert(eMBRegHoldingCB(b, ENCODER_CAL_COMMAND + 1, 1, MB_REG_READ) == MB_ENOREG);
    return 0;
}
