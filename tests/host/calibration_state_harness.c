/* Harness for tests/calibration_state_test.py, not a standalone translation unit. */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mb.h"
#include "encoder_protocol.h"
#include "encoder_calibration.h"

static float arm_sin_f32(float x) { return sinf(x); }

uint16_t buf_x1[128];

/* EXTRACTED_DECLARATIONS */
unsigned char encoder_state;
uint8_t i2c3_tx_wp = 1, backlight_width_en = 1;
uint32_t cycles_max;
int old_sector, cur_sector, cal_sector;
int16_t lsector, rsector;
float pix_dif; /* offset itself is extracted from Src/main.c with its real type. */
float ang_tab[BIT_TAB_SIZE], buf_k[128];
uint8_t eeprom_buf[eeprom_len];
static uint8_t eeprom_chip[eeprom_len]; /* What init_vars() reads back after a "reboot". */
static int I2c3Handle;
#define GPIOA 0
#define GPIOB 1
#define LL_GPIO_PIN_8 8
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
static int HAL_I2C_Mem_Read(int *handle, uint16_t device, uint16_t address,
                            int size, uint8_t *data, uint16_t length, uint32_t timeout)
{
    assert(handle == &I2c3Handle && device == EEPROM_DEVICE_PAGE0 && address == 0);
    assert(size == I2C_MEMADD_SIZE_8BIT && data == eeprom_buf && length == eeprom_len);
    (void)timeout;
    memcpy(data, eeprom_chip, length);
    return 0;
}
static uint32_t LL_GPIO_IsInputPinSet(int port, int pin)
{
    assert(port == GPIOB && pin == LL_GPIO_PIN_8);
    return 0; /* config_state = 0: direct pixel order. */
}

/* Forward prototypes for -Wmissing-prototypes when compiling extracted C functions. */
unsigned short crc_16_step(unsigned char data, unsigned short crc);
float BR4_C(float *mas, int centor_int);
float mes_piks(uint16_t star_pixel);
void angtab_cal(void);
void offset_eeprom_pack(uint8_t value, uint8_t *image);
uint8_t offset_eeprom_unpack(const uint8_t *image, uint8_t *value);
void offset_cal(void);
void angk_cal(void);
void init_vars(void);

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

static void run_bufk_uniform(void)
{
    unsigned before = eeprom_writes;
    assert(command(0x14) == MB_ENOERR);
    assert_state(0x70);
    assert(!backlight_width_en && anglek_phase == 0);
    for (unsigned i = 0; i < 128; ++i) assert(buf_x3[i] == 0.0f);

    for (unsigned frame = 0; frame < 100 && start_angk_cal; ++frame) {
        for (unsigned i = 1; i <= 126; ++i) buf_x3[i] = 2000.0f * buf_k[i];
        avg_minmax_num = offset_avg_num + 1;
        min_max = 0.01f;
        encoder_calibration_step();
    }
    assert_state(0x80);
    assert(!start_angk_cal && anglek_phase == 101 && backlight_width_en);
    assert(eeprom_writes == before + 1);
    finish_eeprom(EEPROM_DEVICE_PAGE3, 514);
}

/* Verify that angk_cal() calibrates ALL working pixels [6..126] (including
 * previously skipped 6..15 and 120..126), keeps borders 0..5 and 127 at 1.0f,
 * clears buf_x3 between Phase 1 and Phase 2, guards zero peaks against Inf/NaN,
 * terminates Phase 2 in bounded iterations, and removes edge distortion in BR4_C(). */
static void test_angk_pixel_range_and_br4_edges(void)
{
    float sens[128];
    for (int i = 0; i < 128; ++i) {
        /* Strong smooth vignetting toward edges 6..20 and 115..126 (down to 0.78). */
        float d = ((float)i - 64.0f) / 62.0f;
        sens[i] = 1.0f - 0.22f * d * d;
    }
    buf_k[0] = 9.9f;
    buf_k[5] = 8.8f;
    buf_k[127] = 7.7f;

    unsigned before = eeprom_writes;
    assert(command(0x14) == MB_ENOERR);

    /* Phase 0: resets all buf_k[0..127] = 1.0f and clears buf_x3. */
    encoder_calibration_step();
    assert(anglek_phase == 1);
    for (int i = 0; i < 128; ++i) {
        assert(buf_k[i] == 1.0f && buf_x3[i] == 0.0f);
    }

    /* Phase 1: direct single-pass equalization ("as is", no iterative phase 2..8). */
    for (int i = 1; i <= 126; ++i) {
        buf_x3[i] = 2500.0f * sens[i];
    }
    avg_minmax_num = offset_avg_num + 1;
    encoder_calibration_step();
    assert(anglek_phase == 100);

    /* Phase 100: immediately writes EEPROM and transitions to 0x80 (3 steps total!). */
    encoder_calibration_step();
    assert_state(0x80);
    assert(!start_angk_cal && anglek_phase == 101 && backlight_width_en);
    assert(eeprom_writes == before + 1);
    finish_eeprom(EEPROM_DEVICE_PAGE3, 514);

    /* Borders 0..5 and 127 must remain 1.0f; ALL working pixels 6..126 must be equalized! */
    for (int i = 0; i < 6; ++i) assert(buf_k[i] == 1.0f);
    assert(buf_k[127] == 1.0f);
    const float target = 2500.0f * sens[64] * buf_k[64];
    for (int i = 6; i <= 126; ++i) {
        assert(isfinite(buf_k[i]));
        if (i != 64) assert(buf_k[i] > 1.0f);
        float eq = 2500.0f * sens[i] * buf_k[i];
        assert(fabsf(eq - target) < 2.0f);
    }

    /* Test subpixel accuracy of real mes_piks() + BR4_C() at BOTH sensor edges:
     *   1) Left fiducial at startpixel1 = 6 with true spot center at pixel 14.5
     *      (kernel spans pixels 12..17, straddling old uncalibrated boundary 15/16!);
     *   2) Right fiducial at startpixel2 = 107 with true spot center at pixel 120.5
     *      (kernel spans pixels 118..123, straddling old uncalibrated boundary 119/120!). */
    const uint16_t stars[2] = {6U, 107U};
    const float centers[2] = {14.5f, 120.5f};
    k_comp = 0.0f; /* Pure symmetric spot center check */
    for (unsigned t = 0; t < 2; ++t) {
        memset(buf_x1, 0, sizeof buf_x1);
        memset(buf_x2, 0, sizeof buf_x2);
        for (int i = 6; i <= 126; ++i) {
            float dx = (float)i - centers[t];
            float ideal = 2600.0f * expf(-0.5f * dx * dx / (1.35f * 1.35f));
            buf_x1[i] = (uint16_t)(ideal * sens[i] + 0.5f);
        }
        float est = mes_piks(stars[t]);
        assert(fabsf(est - centers[t]) < 0.003f);
    }
    k_comp = 0.01f;

    /* Test dead-pixel / zero-peak protection AND bounded Phase-2 iteration limit. */
    before = eeprom_writes;
    assert(command(0x14) == MB_ENOERR);
    for (unsigned step = 0; step < 40 && start_angk_cal; ++step) {
        for (int i = 6; i <= 126; ++i) {
            /* Force persistent >100 spread in Phase 2 plus a dead pixel at i=42. */
            buf_x3[i] = (i == 42) ? 0.0f : ((i == 50) ? 1500.0f : 2200.0f);
        }
        avg_minmax_num = offset_avg_num + 1;
        min_max = 0.01f;
        encoder_calibration_step();
    }
    assert_state(0x80);
    assert(buf_k[42] == 1.0f);
    for (int i = 0; i < 128; ++i) {
        assert(isfinite(buf_k[i]) && buf_k[i] >= ANGK_GAIN_MIN && buf_k[i] <= ANGK_GAIN_MAX);
    }
    finish_eeprom(EEPROM_DEVICE_PAGE3, 514);
}

static uint16_t block_crc(const uint8_t *data, unsigned length)
{
    uint16_t crc = 0xFFFFU;
    while (length--) crc = crc_16_step(*data++, crc);
    return crc;
}

static void run_offset(int minimum)
{
    unsigned before = eeprom_writes;
    memset(&eeprom_buf[eeprom_offset], 0xEE, 6); /* Stale bytes must be overwritten. */
    assert(command(0x13) == MB_ENOERR);
    assert_state(0x50);
    for (unsigned i = 0; i < 128; ++i) assert(offset_minmax[i] == 1.0f);
    for (unsigned frame = 0; frame < 100 && start_offset_cal; ++frame) {
        avg_minmax_num = offset_avg_num + 1;
        avg_min = 0; avg_max = 0.1f;
        min_max = 0.01f * (float)(1 + abs((int)offset - minimum));
        encoder_calibration_step();
    }
    assert_state(0x60);
    assert(!start_offset_cal && offset_phase == 101 && offset == minimum);
    assert(eeprom_writes == before + 1);
    /* EEPROM image: byte 0 = offset, bytes 1..3 = 0, then CRC16 over these 4 bytes. */
    const uint8_t *image = &eeprom_buf[eeprom_offset];
    assert(image[0] == minimum && image[1] == 0 && image[2] == 0 && image[3] == 0);
    const uint16_t crc = block_crc(image, 4);
    assert(eeprom_buf[eeprom_offset_crc] == (uint8_t)crc &&
           eeprom_buf[eeprom_offset_crc + 1] == (uint8_t)(crc >> 8));
    finish_eeprom(EEPROM_DEVICE_PAGE6, 6);
}

/* Power cycle: the chip keeps the given 4-byte offset image, RAM starts from defaults. */
static void reboot_with_offset_image(const uint8_t image[4], int valid_crc)
{
    memcpy(eeprom_chip, eeprom_buf, sizeof eeprom_chip);
    memcpy(&eeprom_chip[eeprom_offset], image, 4);
    uint16_t crc = block_crc(image, 4);
    if (!valid_crc) crc ^= 0x5A5AU;
    eeprom_chip[eeprom_offset_crc] = (uint8_t)crc;
    eeprom_chip[eeprom_offset_crc + 1] = (uint8_t)(crc >> 8);
    memset(eeprom_buf, 0, sizeof eeprom_buf);
    offset = 64U;
    ang_tab_rdy = buf_k_rdy = offset_rdy = lasdac_rdy = 0;
    init_vars();
}

/* offset is a uint8_t pixel index; its EEPROM block stays 4 bytes + CRC16.
 * init_vars() must restore it without touching neighbouring RAM (a raw 4-byte
 * memcpy on the 1-byte variable fails this build via -Werror) and must keep
 * heads calibrated by earlier firmware usable. */
static void test_offset_eeprom_image(void)
{
    float saved_ang_tab[BIT_TAB_SIZE], saved_buf_k[128];
    memcpy(saved_ang_tab, ang_tab, sizeof saved_ang_tab);
    memcpy(saved_buf_k, buf_k, sizeof saved_buf_k);

    /* Round trip of what offset_cal() has just stored, together with buf_k and ang_tab. */
    const uint8_t stored = offset;
    uint8_t image[4];
    memcpy(image, &eeprom_buf[eeprom_offset], 4);
    memset(ang_tab, 0, sizeof ang_tab);
    memset(buf_k, 0, sizeof buf_k);
    reboot_with_offset_image(image, 1);
    assert(offset_rdy == 1 && offset == stored);
    assert(ang_tab_rdy == 1 && !memcmp(ang_tab, saved_ang_tab, sizeof ang_tab));
    assert(buf_k_rdy == 1 && !memcmp(buf_k, saved_buf_k, sizeof buf_k));

    /* Current format written by offset_eeprom_pack(). */
    memset(image, 0xEE, sizeof image);
    offset_eeprom_pack(67U, image);
    assert(image[0] == 67U && image[1] == 0 && image[2] == 0 && image[3] == 0);
    reboot_with_offset_image(image, 1);
    assert(offset_rdy == 1 && offset == 67U);

    /* Interim firmware: byte 0 = offset, bytes 1..3 = whatever followed it in RAM. */
    const uint8_t interim[4] = {61U, 0xA5U, 0x3CU, 0xFFU};
    reboot_with_offset_image(interim, 1);
    assert(offset_rdy == 1 && offset == 61U);

    /* Original firmware: IEEE-754 float with an integer value (low byte is 0). */
    for (int pixel = 56; pixel <= 72; ++pixel) {
        const float legacy = (float)pixel;
        memcpy(image, &legacy, 4);
        assert(image[0] == 0);
        reboot_with_offset_image(image, 1);
        assert(offset_rdy == 1 && offset == pixel);
    }

    /* Rejected images leave the compiled-in default and offset_rdy = 0. */
    const float bad_floats[] = {0.0f, 63.5f, 200.0f, -64.0f, (float)NAN};
    for (unsigned i = 0; i < sizeof bad_floats / sizeof bad_floats[0]; ++i) {
        memcpy(image, &bad_floats[i], 4);
        reboot_with_offset_image(image, 1);
        assert(offset_rdy == 0 && offset == 64U);
    }
    const uint8_t blank[4] = {0xFFU, 0xFFU, 0xFFU, 0xFFU}; /* Erased EEPROM cell. */
    reboot_with_offset_image(blank, 1);
    assert(offset_rdy == 0 && offset == 64U);
    offset_eeprom_pack(67U, image);
    reboot_with_offset_image(image, 0); /* Valid payload, wrong CRC16. */
    assert(offset_rdy == 0 && offset == 64U);

    /* Leave the head as calibrated before this test. */
    offset_eeprom_pack(stored, image);
    reboot_with_offset_image(image, 1);
    assert(offset_rdy == 1 && offset == stored);
}

static void run_angtab_ex(unsigned pass, int increasing_sector_dir)
{
    unsigned before = eeprom_writes;
    assert(command(pass == 1 ? 0x11 : 0x12) == MB_ENOERR);
    assert_state(pass == 1 ? 0x10 : 0x30);
    s_l1 = s_l2 = 0;
    encoder_calibration_step();
    assert(!start_calibrate && auto_cal == (int)pass);
    assert(command(0x13) == MB_ETIMEDOUT);
    for (unsigned turn = 0; turn < 8 && auto_cal; ++turn) {
        for (int step = 0; step < BIT_TAB_SIZE && auto_cal; ++step) {
            cur_sector = increasing_sector_dir ? step : (BIT_TAB_SIZE - 1 - step);
            lsector = (int16_t)((cur_sector + BIT_TAB_SIZE - 1) % BIT_TAB_SIZE);
            rsector = (int16_t)((cur_sector + 1) % BIT_TAB_SIZE);
            s_l1 = offset - 25.5f;
            s_l2 = offset + 25.5f;
            pix_dif = 50.8f;
            encoder_calibration_step();
            /* Second consecutive 103 us frame inside the same strobe must not double-count! */
            encoder_calibration_step();
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
            assert(fabsf(ang_tab[i] - (float)i * 2.5f) < 0.001f);
        finish_eeprom(EEPROM_DEVICE_PAGE0, BIT_TAB_SIZE * 4 + 2);
    }
}

static void run_angtab(unsigned pass)
{
    run_angtab_ex(pass, pass == 1 ? 0 : 1);
}

static void stop_test(uint16_t start)
{
    assert(command(start) == MB_ENOERR);
    s_l1 = s_l2 = 0;
    if (start == 0x11 || start == 0x12) encoder_calibration_step();
    cycles_max = 123;
    assert(command(0x10) == MB_ENOERR);
    assert_state(0);
    assert(!start_calibrate && !auto_cal && !start_offset_cal && !start_angk_cal);
    assert(backlight_width_en && cycles_max == 0);
    unsigned before = eeprom_writes;
    for (unsigned frame = 0; frame < 5; ++frame) {
        avg_minmax_num = 100;
        encoder_calibration_step();
        assert_state(0);
    }
    assert(eeprom_writes == before); /* No deferred completion after STOP. */
}

int main(void)
{
    assert(live(ENCODER_CAL_VERSION) == ENCODER_CAL_MAGIC);
    assert(command(0x12) == MB_EINVAL);

    /* Exact four-stage sequence executed by ESP32 master: 0x14 -> 0x13 -> 0x11 -> 0x12 -> 0x10. */
    run_bufk_uniform();
    run_offset(64);
    run_angtab(1);
    run_angtab(2);
    assert(command(0x10) == MB_ENOERR);
    assert_state(0);
    assert(eeprom_writes == 3);

    /* Full pixel range [6..126], single-pass angk_cal, BR4_C edge accuracy, NaN/Inf guard. */
    test_angk_pixel_range_and_br4_edges();

    /* Verify angtab_cal with opposite initial shaft direction as well. */
    run_angtab_ex(1, 1);
    run_angtab_ex(2, 0);
    assert(command(0x10) == MB_ENOERR);
    assert_state(0);

    /* Both extended search branches & custom window interior minimum. */
    run_offset(61);
    run_offset(67);
    offset_start = 61; offset_end = 66;
    run_offset(63);
    offset_start = 63; offset_end = 65;

    /* uint8_t offset: EEPROM image, reboot round trip, images of earlier firmware. */
    test_offset_eeprom_image();

    stop_test(0x11);
    run_angtab(1);
    stop_test(0x12);
    assert(command(0x12) == MB_EINVAL); /* STOP invalidates pass-1 continuation. */
    stop_test(0x13);
    stop_test(0x14);

    /* Simulate simultaneous SWD flag collision: dispatcher must abort all machines safely. */
    assert(command(0x14) == MB_ENOERR);
    start_offset_cal = 1;
    encoder_calibration_step();
    assert_state(0);
    assert(!start_angk_cal && !start_offset_cal && backlight_width_en == 1);

    assert(command(0x13) == MB_ENOERR);
    /* Simulate a final-stage I2C already in flight: STOP must not fake completion. */
    i2c3_tx_wp = 0;
    assert(command(0x10) == MB_ENOERR);
    assert_state(0);
    assert(live(ENCODER_EEPROM_READY) == 0);
    puts("Calibration state integration: 4-stage flow, [6..126] buf_k, BR4_C edges, "
         "offset EEPROM image, LIVE, STOP OK");
    return 0;
}
