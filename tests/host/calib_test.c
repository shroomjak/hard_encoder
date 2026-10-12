/*
 * Хостовый тест регистра канала самокалибровки: что именно делает запись кода
 * команды, что видит оператор в блоке FC04 и как выгружаются таблицы.
 *
 * Сборка и запуск: sh tests/host/run_calib_test.sh
 */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "encoder_vars.h"
#include "modbus_calib.h"
#include "snapshot_registers.h"
#include "mb.h"

extern eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count,
                                   eMBRegisterMode mode);
extern eMBErrorCode eMBRegInputCB(UCHAR *buf, USHORT address, USHORT count);

static uint16_t rd(uint16_t addr) { return mb_cal_read(addr); }

static eMBErrorCode fc06(uint16_t wire_addr, uint16_t value)
{
    UCHAR b[2] = { (UCHAR)(value >> 8), (UCHAR)(value & 0xFF) };
    return eMBRegHoldingCB(b, (USHORT)(wire_addr + 1), 1, MB_REG_WRITE);
}

static eMBErrorCode fc04(uint16_t wire_addr, uint16_t count, uint16_t *out)
{
    UCHAR raw[2 * MB_CAL_TOTAL];
    eMBErrorCode e = eMBRegInputCB(raw, (USHORT)(wire_addr + 1), count);
    if (e == MB_ENOERR)
        for (uint16_t i = 0; i < count; ++i)
            out[i] = (uint16_t)((raw[2 * i] << 8) | raw[2 * i + 1]);
    return e;
}

static float window_float(uint16_t k)
{
    uint32_t bits = ((uint32_t)rd(MB_CAL_WINDOW + 2 * k) << 16) | rd(MB_CAL_WINDOW + 2 * k + 1);
    float f;
    (void)memcpy(&f, &bits, sizeof f);
    return f;
}

static void world(float offset_value)
{
    encoder_state = 0;
    start_calibrate = 0;
    auto_cal = 0;
    start_offset_cal = 0;
    start_angk_cal = 0;
    cycles_max = 0;
    backlight_width_en = 1;
    pix_rdy_num1 = 0;
    pix_rdy_num2 = 0;
    offset = offset_value;
    offset_phase = 0;
    offset_cur = 0;
    offset_found = 0;
    offset_start = 63;
    anglek_phase = 0;
    pix_min_max = 0;
    min_max = 0;
    avg_minmax_num = 0;
    rev_right_cnt = 0;
    rev_left_cnt = 0;
    serrcnt1 = 0;
    cur_ang_E = 0;
    ang_tab_rdy = 0;
    buf_k_rdy = 0;
    offset_rdy = 0;
    for (int i = 0; i < BIT_TAB_SIZE; i++) ang_tab[i] = (float)i + 0.25f;
    for (int i = 0; i < ENCODER_PIXELS; i++) {
        buf_k[i] = 1.0f;
        buf_x3[i] = 0.0f;
        offset_minmax[i] = 0.0f;
    }
}

/* Запись кода = ровно те же флаги, что взводит автомат при запуске. */
static void test_commands(void)
{
    world(68.0f);

    assert(mb_cal_write(MB_H_CAL_CMD, MB_CAL_BUFK) == 0);
    assert(start_angk_cal == 1 && encoder_state == 0x70);
    assert(backlight_width_en == 0);        /* АРУ выключена на время измерений */
    assert(buf_x3[0] == 0.0f && avg_minmax_num == 0);

    /* Повтор возможен всегда: ничего не заперто. Пока идёт angk_cal,
     * запускаем offset_cal — команды независимы, как и при запуске флагами. */
    assert(mb_cal_write(MB_H_CAL_CMD, MB_CAL_OFFSET) == 0);
    assert(start_offset_cal == 1 && encoder_state == 0x50);
    assert(offset_phase == 0 && offset_cur == offset_start && avg_minmax_num == 0);
    assert(offset_minmax[0] == 1.0f && offset_minmax[127] == 1.0f);

    assert(mb_cal_write(MB_H_CAL_CMD, MB_CAL_ANGTAB_RIGHT) == 0);
    assert(start_calibrate == 1 && encoder_state == 0x10);

    world(68.0f);
    assert(mb_cal_write(MB_H_CAL_CMD, MB_CAL_ANGTAB_LEFT) == 0);
    assert(start_calibrate == 2 && encoder_state == 0x30);

    /* Недопустимый код ничего не меняет. */
    int before = encoder_state;
    assert(mb_cal_write(MB_H_CAL_CMD, 0x15) == -2);
    assert(mb_cal_write(MB_H_CAL_CMD, 0x00) == -2);
    assert(encoder_state == before);
    assert(mb_cal_write(9, 1) == -1);

    /* Останов снимает всё, включая запуск angk_cal, и возвращает АРУ. */
    start_angk_cal = 1;
    backlight_width_en = 0;
    anglek_phase = 3;
    cycles_max = 12345;
    assert(mb_cal_write(MB_H_CAL_CMD, MB_CAL_STOP) == 0);
    assert(encoder_state == 0x00 && start_calibrate == 0 && auto_cal == 0);
    assert(start_offset_cal == 0 && start_angk_cal == 0);
    assert(backlight_width_en == 1 && cycles_max == 0);
    /* Пресечённая по полукону фаза остаётся как есть: калибровку повторяют
     * заново, а незаписанный результат приходит из EEPROM. */
    assert(anglek_phase == 3);
    printf("  команды: запуск, повтор, останов ... ok\n");
}

/* Живое зеркало: значения должны приходить из переменных, а не из снимка. */
static void test_status(void)
{
    uint16_t regs[MB_CAL_TOTAL];
    world(68.5f);
    encoder_state = 0x50;
    pix_rdy_num1 = 37;
    pix_rdy_num2 = 144;
    offset_phase = 2;
    offset_cur = 65;
    offset_found = 64;
    anglek_phase = 100;
    pix_min_max = -12.4f;
    min_max = 0.000020f;
    cur_ang_E = 90.5f;
    avg_minmax_num = 4;
    rev_right_cnt = 11;
    rev_left_cnt = 2;
    serrcnt1 = 5;
    i2c3_tx_wp = 1;                 /* EEPROM защищён: записей нет */
    ang_tab_rdy = 1;
    buf_k_rdy = 1;
    offset_rdy = 0;

    assert(fc04(MB_CAL_FIRST, MB_R_EEPROM - MB_CAL_FIRST + 1, regs) == MB_ENOERR);
    assert(regs[MB_R_STATE - MB_CAL_FIRST] == 0x50);
    assert(regs[MB_R_RDY1 - MB_CAL_FIRST] == 37);
    assert(regs[MB_R_RDY2 - MB_CAL_FIRST] == 144);
    assert(regs[MB_R_OFFSET_PHASE - MB_CAL_FIRST] == 2);
    assert(regs[MB_R_OFFSET_CUR - MB_CAL_FIRST] == 65);
    assert(regs[MB_R_OFFSET_FOUND - MB_CAL_FIRST] == 64);
    assert(regs[MB_R_ANGK_PHASE - MB_CAL_FIRST] == 100);
    assert(regs[MB_R_REVS - MB_CAL_FIRST] == 4);
    assert(regs[MB_R_OFFSET_X256 - MB_CAL_FIRST] == 17536); /* 68,5 * 256 */
    assert((int16_t)regs[MB_R_PIX_MIN_MAX - MB_CAL_FIRST] == -12);
    assert(((uint32_t)regs[MB_R_MINMAX_HI - MB_CAL_FIRST] << 16 |
            regs[MB_R_MINMAX_LO - MB_CAL_FIRST]) == 20u);
    assert(((uint32_t)regs[MB_R_ANGLE_HI - MB_CAL_FIRST] << 16 |
            regs[MB_R_ANGLE_LO - MB_CAL_FIRST]) == 90500000u);
    assert(regs[MB_R_REV_RIGHT - MB_CAL_FIRST] == 11);
    assert(regs[MB_R_REV_LEFT - MB_CAL_FIRST] == 2);
    assert(regs[MB_R_SERR1 - MB_CAL_FIRST] == 5);
    assert(regs[MB_R_EEPROM - MB_CAL_FIRST] ==
           ((1u << MB_E_WRITTEN) | (1u << MB_E_ANGTAB) | (1u << MB_E_BUFK)));
    i2c3_tx_wp = 0;   /* запись в EEPROM идёт — бит 0 снят */
    assert(fc04(MB_R_EEPROM, 1, regs) == MB_ENOERR);
    assert(regs[0] == ((1u << MB_E_ANGTAB) | (1u << MB_E_BUFK)));
    i2c3_tx_wp = 1;

    /* min_max со знаком минус читается как отрицательное число. */
    min_max = -0.000003f;
    assert(rd(MB_R_MINMAX_HI) == 0xFFFF && (int32_t)((uint32_t)0xFFFF0000u | rd(MB_R_MINMAX_LO)) == -3);

    /* Кадры АЦП: счётчик живой, растёт без SNAP. */
    uint32_t frames = ((uint32_t)rd(MB_R_FRAMES_HI) << 16) | rd(MB_R_FRAMES_LO);
    snapshot_publish(10.0f, 5, 0, 0x00, 1);
    assert((((uint32_t)rd(MB_R_FRAMES_HI) << 16) | rd(MB_R_FRAMES_LO)) == frames + 1);

    /* Вне карты — молчание, а не чтение за массивом. */
    assert(rd(MB_CAL_FIRST - 1) == 0 && rd(MB_CAL_TOTAL) == 0);
    printf("  живой статус: 11…%d ... ok\n", MB_R_EEPROM);
}

static void test_window(void)
{
    world(68.0f);
    ang_tab[0] = -12.5f;
    ang_tab[15] = 0.5f;
    ang_tab[128] = 3.0f;
    buf_k[5] = 1.25f;

    assert(mb_cal_write(MB_H_CAL_PAGE, MB_CAL_T_ANGTAB << 8) == 0);
    assert(window_float(0) == -12.5f);
    assert(window_float(15) == 0.5f);

    assert(mb_cal_write(MB_H_CAL_PAGE, (MB_CAL_T_BUFK << 8) | 5) == 0);
    assert(window_float(0) == 1.25f);

    /* Индекс прижимается к концу таблицы: 250 для ang_tab -> 128. */
    assert(mb_cal_write(MB_H_CAL_PAGE, (MB_CAL_T_ANGTAB << 8) | 250) == 0);
    assert(window_float(0) == ang_tab[128]);
    assert(window_float(15) == ang_tab[143]);

    assert(mb_cal_write(MB_H_CAL_PAGE, (9u << 8)) == -2);
    printf("  выгрузка таблиц постранично ... ok\n");
}

static void test_bus(void)
{
    uint16_t regs[MB_CAL_TOTAL];
    world(68.0f);

    /* SNAP остался на нулевом адресе, команда — на единице. */
    assert(fc06(MB_H_SNAP, 42) == MB_ENOERR);
    uint16_t snap[2];
    assert(fc04(0, 2, snap) == MB_ENOERR && snap[0] == 42);

    assert(fc06(MB_H_CAL_CMD, 0x77) == MB_EIO);       /* исключение 0x04 */
    assert(fc06(3, 0) == MB_ENOREG);                   /* исключение 0x02 */
    assert(fc06(MB_H_CAL_PAGE, 25u) == MB_ENOERR);
    assert(fc06(MB_H_CAL_CMD, MB_CAL_BUFK) == MB_ENOERR);

    /* Границы FC04: вся карта целиком проходит, на регистр больше — отказ. */
    assert(fc04(0, MB_CAL_TOTAL, regs) == MB_ENOERR);
    assert(regs[MB_R_STATE - 0] == 0x70);
    assert(fc04(0, MB_CAL_TOTAL + 1, regs) == MB_ENOREG);
    assert(fc04((uint16_t)(MB_CAL_TOTAL - 2), 3, regs) == MB_ENOREG);
    /* Больше одного регистра за раз не пишем: FC16 в сборке выключен. */
    UCHAR two[2] = { 0, 0 };
    assert(eMBRegHoldingCB(two, 1, 2, MB_REG_WRITE) == MB_ENOREG);
    printf("  адреса и исключения на шине ... ok\n");
}

int main(void)
{
    printf("Канал калибровки по Modbus RTU:\n");
    test_commands();
    test_status();
    test_window();
    test_bus();
    printf("OK\n");
    return 0;
}
