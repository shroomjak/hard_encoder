/* Harness for tests/calibration_state_test.py, not a standalone translation unit. */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mb.h"
#include "encoder_protocol.h"

/* EXTRACTED_DECLARATIONS */
unsigned char encoder_state;
uint8_t i2c3_tx_wp = 1, backlight_width_en = 1;
uint32_t cycles_max;
int old_sector, cur_sector, cal_sector;
int16_t lsector, rsector;
float s_l1, s_l2, pix_dif, offset = 68;
float ang_tab[BIT_TAB_SIZE], buf_k[128];
uint8_t eeprom_buf[eeprom_len];
static int I2c3Handle;
#define GPIOA 0
#define LL_GPIO_PIN_9 9
#define I2C_MEMADD_SIZE_8BIT 1
static unsigned eeprom_writes;
static uint16_t last_device, last_length;
static void LL_GPIO_ResetOutputPin(int port, int pin)
{
    assert(port == GPIOA && pin == LL_GPIO_PIN_9);
}
static int HAL_I2C_Mem_Write_IT(int *handle, uint16_t device, uint16_t address,
                              int size, uint8_t *data, uint16_t length)
{
    assert(handle == &I2c3Handle && !i2c3_tx_wp);
    assert(address == 0 && size == I2C_MEMADD_SIZE_8BIT);
    assert(data >= eeprom_buf && data + length <= eeprom_buf + sizeof eeprom_buf);
    ++eeprom_writes;
    last_device = device; last_length = length;
    return 0; /* Completion is asynchronous; test releases i2c3_tx_wp later. */
}

/* EXTRACTED_FUNCTIONS */

static eMBErrorCode command(uint16_t value)
{
    UCHAR data[] = {(UCHAR)(value >> 8), (UCHAR)value};
    return eMBRegHoldingCB(data, ENCODER_CAL_COMMAND + 1, 1, MB_REG_WRITE);
}
static uint16_t live(uint16_t address)
{
    UCHAR data[2];
    assert(eMBRegInputCB(data, address + 1, 1) == MB_ENOERR);
    return (uint16_t)((uint16_t)data[0] << 8 | data[1]);
}
static void assert_state(uint16_t expected)
{
    assert(live(ENCODER_CAL_STATE) == expected);
}
static void finish_eeprom(uint16_t device, uint16_t length)
{
    assert(live(ENCODER_EEPROM_READY) == 0);
    assert(last_device == device && last_length == length);
    assert(command(0x14) == MB_ETIMEDOUT); /* Done != EEPROM ready. */
    i2c3_tx_wp = 1;
    assert(live(ENCODER_EEPROM_READY) == 1);
}
static void run_bufk(void)
{
    unsigned before = eeprom_writes;
    assert(command(0x14) == MB_ENOERR);
    assert_state(0x70);
    assert(!backlight_width_en && anglek_phase == 0);
    for (unsigned i = 0; i < 128; ++i) assert(buf_x3[i] == 0);
    /* Synthetic uniform illumination and finite quality metric drive every phase. */
    for (unsigned frame = 0; frame < 100 && start_angk_cal; ++frame) {
        for (unsigned i = 0; i < 128; ++i) buf_x3[i] = 2000;
        avg_minmax_num = offset_avg_num + 1;
        min_max = 0.01f;
        angk_cal();
    }
    assert_state(0x80);
    assert(!start_angk_cal && anglek_phase == 101 && backlight_width_en);
    assert(eeprom_writes == before + 1);
    finish_eeprom(EEPROM_DEVICE_PAGE3, 514);
}
static void run_offset(int minimum)
{
    unsigned before = eeprom_writes;
    assert(command(0x13) == MB_ENOERR);
    assert_state(0x50);
    for (unsigned i = 0; i < 128; ++i) assert(offset_minmax[i] == 1.0f);
    for (unsigned frame = 0; frame < 100 && start_offset_cal; ++frame) {
        avg_minmax_num = offset_avg_num + 1;
        avg_min = 0; avg_max = 0.1f;
        min_max = 0.01f * (1 + fabsf(offset - minimum));
        offset_cal();
    }
    assert_state(0x60);
    assert(!start_offset_cal && offset_phase == 101 && offset == minimum);
    assert(eeprom_writes == before + 1);
    finish_eeprom(EEPROM_DEVICE_PAGE6, 6);
}
static void run_angtab(unsigned pass)
{
    unsigned before = eeprom_writes;
    assert(command(pass == 1 ? 0x11 : 0x12) == MB_ENOERR);
    assert_state(pass == 1 ? 0x10 : 0x30);
    /* Let the real algorithm consume the start flag and reset its arrays. */
    s_l1 = s_l2 = 0;
    angtab_cal();
    assert(!start_calibrate && auto_cal == (int)pass);
    assert(command(0x13) == MB_ETIMEDOUT);
    /* Visit all sectors for both directions. The real direction latch skips
     * adjacent sectors; shift the first sampled sector between sweeps so both
     * parities of the 144-sector synthetic input are exercised. */
    for (unsigned turn = 0; turn < 30 && auto_cal; ++turn) {
        for (int step = 0; step < BIT_TAB_SIZE && auto_cal; ++step) {
            int sampled = (step + (int)turn) % BIT_TAB_SIZE;
            cur_sector = pass == 1 ? sampled : BIT_TAB_SIZE - 1 - sampled;
            lsector = (cur_sector + BIT_TAB_SIZE - 1) % BIT_TAB_SIZE;
            rsector = (cur_sector + 1) % BIT_TAB_SIZE;
            s_l1 = offset - 24.5f;
            pix_dif = 51.0f;
            angtab_cal();
        }
    }
    assert(!auto_cal);
    assert_state(pass == 1 ? 0x20 : 0x40);
    if (pass == 1) {
        assert(pix_rdy_num1 == BIT_TAB_SIZE && eeprom_writes == before);
        assert(live(ENCODER_EEPROM_READY) == 1);
    } else {
        assert(pix_rdy_num2 == BIT_TAB_SIZE && eeprom_writes == before + 1);
        for (unsigned i = 0; i < BIT_TAB_SIZE; ++i)
            assert(fabsf(ang_tab[i] - i * 2.5f) < 0.001f);
        finish_eeprom(EEPROM_DEVICE_PAGE0, BIT_TAB_SIZE * 4 + 2);
    }
}
static void stop_test(uint16_t start)
{
    assert(command(start) == MB_ENOERR);
    /* Consume ang_tab's start flag so STOP is also tested against auto_cal. */
    s_l1 = s_l2 = 0;
    if (start == 0x11 || start == 0x12) angtab_cal();
    cycles_max = 123;
    assert(command(0x10) == MB_ENOERR);
    assert_state(0);
    assert(!start_calibrate && !auto_cal && !start_offset_cal && !start_angk_cal);
    assert(backlight_width_en && cycles_max == 0);
    unsigned before = eeprom_writes;
    for (unsigned frame = 0; frame < 5; ++frame) {
        avg_minmax_num = 100;
        angtab_cal(); offset_cal(); angk_cal();
        assert_state(0);
    }
    assert(eeprom_writes == before); /* No deferred completion after STOP. */
}
int main(void)
{
    assert(live(ENCODER_CAL_VERSION) == ENCODER_CAL_MAGIC);
    assert(command(0x12) == MB_EINVAL);
    run_bufk(); run_offset(64); run_angtab(1); run_angtab(2);
    run_bufk(); run_offset(64); /* Same six commands as the ESP32 master. */
    assert(command(0x10) == MB_ENOERR); assert_state(0);
    assert(eeprom_writes == 5);
    run_offset(61); run_offset(67); /* Both extended search branches. */
    stop_test(0x11);
    run_angtab(1); stop_test(0x12);
    assert(command(0x12) == MB_EINVAL); /* STOP invalidates pass-1 continuation. */
    stop_test(0x13); stop_test(0x14);
    assert(command(0x13) == MB_ENOERR);
    /* Simulate a final-stage I2C already in flight: STOP must not fake completion. */
    i2c3_tx_wp = 0;
    assert(command(0x10) == MB_ENOERR);
    assert_state(0); assert(live(ENCODER_EEPROM_READY) == 0);
    puts("Calibration state integration: Modbus commands, real main.c algorithms, LIVE, STOP OK");
    return 0;
}
