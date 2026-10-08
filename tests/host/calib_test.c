/*
 * Хостовый тест командного канала самокалибровки.
 *
 * Проверяет то, что нельзя проверить «глазами» по коду: порядок отказов,
 * одноразовость ключа, откат прерванной калибровки, живое зеркало переменных
 * main.c и постраничную выгрузку таблиц. Плюс адресная разметка на уровне
 * обработчиков FreeModbus (eMBRegHoldingCB/eMBRegInputCB), где живёт сдвиг +1.
 *
 * Сборка — tests/host/run_calib_test.sh.
 */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "encoder_vars.h"
#include "modbus_calib.h"
#include "snapshot_registers.h"

/* Обработчики из Src/modbus_registers.c (тот же код, что использует прошивка). */
#include "mb.h"
extern eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count,
                                    eMBRegisterMode mode);
extern eMBErrorCode eMBRegInputCB(UCHAR *buf, USHORT address, USHORT count);

static mb_cal_result_t wr(uint16_t addr, uint16_t value)
{
    return mb_cal_write(addr, value);
}

static mb_cal_result_t arm_and_send(uint16_t cmd)
{
    mb_cal_result_t r = wr(MB_H_REG_CAL_KEY, MB_CAL_KEY_UNLOCK);
    if (r != MB_CAL_OK) return r;
    return wr(MB_H_REG_CAL_CMD, cmd);
}

/* Чтение блока калибровки так, как его делает FC04: адрес на проводе. */
static uint16_t rd(uint16_t addr)
{
    return mb_cal_read(addr);
}

static uint16_t flag(uint16_t bit)
{
    return (uint16_t)(rd(CAL_R_FLAGS) >> bit & 1u);
}
/* DIAG перечитывать нельзя: каждое чтение обновляет базу приращений, поэтому
 * биты проверяются по ОДНОМУ снимку слова. Первый же опрос только задаёт базу. */
static uint16_t diag_once(void)
{
    return rd(CAL_R_DIAG);
}

static uint32_t u32_of(uint16_t hi, uint16_t lo)
{
    return ((uint32_t)hi << 16) | lo;
}

static float word_pair_to_float(uint16_t hi, uint16_t lo)
{
    uint32_t bits = ((uint32_t)hi << 16) | lo;
    float f;
    memcpy(&f, &bits, sizeof f);
    return f;
}

/* Отправить кадр FC06 так, как это делает стек: адрес на проводе + 1. */
static eMBErrorCode fc06(uint16_t wire_addr, uint16_t value)
{
    UCHAR bytes[2] = { (UCHAR)(value >> 8), (UCHAR)(value & 0xFF) };
    return eMBRegHoldingCB(bytes, (USHORT)(wire_addr + 1), 1, MB_REG_WRITE);
}

/* FC04: как стек — адрес на проводе + 1; на выходе 2*count байт (ст. первым). */
static eMBErrorCode fc04(uint16_t wire_addr, uint16_t count, uint16_t *out)
{
    UCHAR raw[2 * MB_CAL_INPUT_TOTAL];
    eMBErrorCode e = eMBRegInputCB(raw, (USHORT)(wire_addr + 1), count);
    if (e == MB_ENOERR && out != NULL)
        for (uint16_t i = 0; i < count; ++i)
            out[i] = (uint16_t)((raw[2 * i] << 8) | raw[2 * i + 1]);
    return e;
}

static void reset_world(void)
{
    mb_cal_reset();
    encoder_state = 0;
    start_calibrate = 0;
    auto_cal = 0;
    start_offset_cal = 0;
    start_angk_cal = 0;
    pix_rdy_num1 = 0;
    pix_rdy_num2 = 0;
    pix_min_max = 400.0f; /* в init_vars он не инициализирован — фиксируем сами */
    min_max = 0.0f;
    offset = 68.0f;
    offset_phase = 0;
    offset_cur = 0;
    offset_found = 0;
    anglek_phase = 0;
    anglek_cur = 0;
    avg_minmax_num = 0;
    rev_en = 0;
    rev_left_cnt = 0;
    rev_right_cnt = 0;
    serrcnt1 = 0;
    serrcnt2 = 0;
    errorflag = 0;
    sector = 0;
    cur_ang_E = 0.0f;
    cycles_max = 0;
    backlight_width_en = 1;
    backlight_width_ticks = 2500;
    i2c3_tx_wp = 1;
    ang_tab_rdy = 0;
    buf_k_rdy = 0;
    offset_rdy = 0;
    lasdac_rdy = 0;
    for (int i = 0; i < BIT_TAB_SIZE; ++i) {
        ang_tab[i] = (float)i + 0.25f;
        pix_dif_tab1[i] = 0.0f;
        pix_dif_tab2[i] = 0.0f;
    }
    for (int i = 0; i < ENCODER_PIXELS; ++i) {
        buf_k[i] = 1.0f;
        buf_x3[i] = 0.0f;
        offset_minmax[i] = 0.0f;
    }
}

static void test_lock(void)
{
    reset_world();
    /* Неизвестный код — VALUE, и даже без ключа (мусор в шине ≠ забытый ключ). */
    assert(wr(MB_H_REG_CAL_CMD, 0x15) == MB_CAL_ERR_VALUE);
    assert(rd(CAL_R_LAST_RESULT) == MB_CAL_ERR_VALUE);
    assert(rd(CAL_R_LAST_CMD) == 0x15);
    assert(rd(CAL_R_REJ_COUNT) == 1);
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_ERR_LOCKED);

    /* Ключ не взведён → LOCKED, и состояние main.c не тронуто. */
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_BUFK) == MB_CAL_ERR_LOCKED);
    assert(start_angk_cal == 0 && encoder_state == 0);
    assert(rd(CAL_R_LAST_RESULT) == MB_CAL_ERR_LOCKED);
    assert(rd(CAL_R_CMD_COUNT) == 0 && rd(CAL_R_REJ_COUNT) == 3);

    /* Неверный ключ не взводит; правильный — взводит ровно на одну команду. */
    assert(wr(MB_H_REG_CAL_KEY, 0xCA1Au) == MB_CAL_OK);
    assert(flag(CAL_F_ARMED) == 0);
    assert(wr(MB_H_REG_CAL_KEY, MB_CAL_KEY_UNLOCK) == MB_CAL_OK);
    assert(flag(CAL_F_ARMED) == 1);

    offset_rdy = 1; /* иначе 0x11 отобьётся пререквизитом offset */
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_OK);
    assert(flag(CAL_F_ARMED) == 0);
    assert(encoder_state == 0x10 && start_calibrate == 1);

    /* Ключ сгорел: вторая команда без новой записи ключа невозможна. */
    encoder_state = 0;
    start_calibrate = 0;
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_BUFK) == MB_CAL_ERR_LOCKED);

    /* Стоп не требует ни ключа, ни свободных автоматов. */
    start_angk_cal = 1;
    backlight_width_en = 0;
    anglek_phase = 2;
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(start_angk_cal == 0 && backlight_width_en == 1 && anglek_phase == 2);
    printf("  lock/one-shot key, stop without key ... ok\n");
}

static void test_guards(void)
{
    reset_world();
    /* EEPROM ещё пишет предыдущий блок → пуск нельзя. */
    i2c3_tx_wp = 0;
    assert(arm_and_send(MB_CAL_CMD_BUFK) == MB_CAL_ERR_EEPROM);
    i2c3_tx_wp = 1;

    /* Идёт angk_cal → пуск offset_cal нельзя (занято). */
    assert(arm_and_send(MB_CAL_CMD_BUFK) == MB_CAL_OK);
    assert(rd(CAL_R_FLAGS) & (1u << CAL_F_BUSY));
    assert(arm_and_send(MB_CAL_CMD_OFFSET) == MB_CAL_ERR_BUSY);
    assert(start_offset_cal == 0);
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(!(rd(CAL_R_FLAGS) & (1u << CAL_F_BUSY)));

    /* ang_tab при неизвестном offset → PREREQ; с RELAX — проходит. */
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_ERR_PREREQ);
    assert(wr(MB_H_REG_CAL_CFG, MB_CAL_CFG_RELAX) == MB_CAL_OK);
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_OK);
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(wr(MB_H_REG_CAL_CFG, 0) == MB_CAL_OK);

    /* Физически невозможный offset не проходит даже с RELAX:RELAX снимает
     * только «порядковые» проверки, а не защиту от мусора в EEPROM. */
    assert(wr(MB_H_REG_CAL_CFG, MB_CAL_CFG_RELAX) == MB_CAL_OK);
    offset = 3.0f;
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_ERR_PREREQ);
    offset = 68.0f;
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_RIGHT) == MB_CAL_OK);
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(wr(MB_H_REG_CAL_CFG, 0) == MB_CAL_OK);

    /* Проход 2 без завершённого прохода 1 → PREREQ. */
    offset_rdy = 1;
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_LEFT) == MB_CAL_ERR_PREREQ);
    encoder_state = MB_CAL_STATE_ANGTAB_1_RDY;
    assert(arm_and_send(MB_CAL_CMD_ANGTAB_LEFT) == MB_CAL_OK);
    assert(start_calibrate == 2 && encoder_state == 0x30);
    printf("  busy/eeprom/prereq guards ... ok\n");
}

static void test_start_effects(void)
{
    reset_world();
    /* 0x13: как удалённый SPI-обработчик — свой стартовый набор. */
    for (int i = 0; i < ENCODER_PIXELS; ++i) offset_minmax[i] = 7.0f;
    offset_cur = 0;
    offset_phase = 4;
    avg_minmax_num = 11;
    assert(arm_and_send(MB_CAL_CMD_OFFSET) == MB_CAL_OK);
    assert(start_offset_cal == 1 && encoder_state == 0x50);
    assert(offset_phase == 0 && offset_cur == offset_start && avg_minmax_num == 0);
    assert(offset_minmax[0] == 1.0f && offset_minmax[127] == 1.0f);

    /* 0x14: пиковый детектор обнулён, АРУ запрещена. */
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    buf_x3[5] = 99.0f;
    backlight_width_en = 1;
    anglek_phase = 7;
    assert(arm_and_send(MB_CAL_CMD_BUFK) == MB_CAL_OK);
    assert(start_angk_cal == 1 && encoder_state == 0x70);
    assert(backlight_width_en == 0 && anglek_phase == 0 && buf_x3[5] == 0.0f);
    assert(flag(CAL_F_AGC_ON) == 0);
    printf("  start effects of 0x13/0x14 ... ok\n");
}

static void test_rollback(void)
{
    reset_world();
    buf_k[3] = 1.5f;
    assert(arm_and_send(MB_CAL_CMD_BUFK) == MB_CAL_OK);
    /* Автомат успел поправить buf_k и снять АРУ. */
    buf_k[3] = 2.0f;
    pix_min_max = 10.0f;
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(buf_k[3] == 1.5f);
    assert(backlight_width_en == 1 && encoder_state == 0x00);

    /* KEEP — оставить недописанный результат в RAM. */
    assert(wr(MB_H_REG_CAL_CFG, MB_CAL_CFG_KEEP) == MB_CAL_OK);
    assert(arm_and_send(MB_CAL_CMD_BUFK) == MB_CAL_OK);
    buf_k[3] = 3.0f;
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(buf_k[3] == 3.0f);
    assert(wr(MB_H_REG_CAL_CFG, 0) == MB_CAL_OK);

    /* offset_cal: откат точки съёма. */
    offset = 68.0f;
    assert(arm_and_send(MB_CAL_CMD_OFFSET) == MB_CAL_OK);
    offset = 71.5f;
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(offset == 68.0f);

    /* А завершившуюся калибровку стоп откатывать НЕ должен. */
    assert(arm_and_send(MB_CAL_CMD_OFFSET) == MB_CAL_OK);
    offset = 71.5f;
    encoder_state = MB_CAL_STATE_OFFSET_RDY; /* автомат дописал блок */
    assert(wr(MB_H_REG_CAL_CMD, MB_CAL_CMD_STOP) == MB_CAL_OK);
    assert(offset == 71.5f);
    printf("  abort/rollback semantics ... ok\n");
}

static void test_window(void)
{
    reset_world();
    ang_tab[0] = -12.5f;
    ang_tab[15] = 0.5f;
    ang_tab[128] = 3.0f;

    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(CAL_T_ANG_TAB << 8 | 0)) == MB_CAL_OK);
    assert(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST), rd(MB_CAL_WINDOW_FIRST + 1)) == -12.5f);
    assert(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST + 30), rd(MB_CAL_WINDOW_FIRST + 31)) == 0.5f);

    /* buf_k, страница с 5-го элемента */
    buf_k[5] = 1.25f;
    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(CAL_T_BUF_K << 8 | 5)) == MB_CAL_OK);
    assert(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST), rd(MB_CAL_WINDOW_FIRST + 1)) == 1.25f);

    /* Индекс прижимается к концу таблицы: за границы выйти нельзя. */
    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(CAL_T_ANG_TAB << 8 | 250)) == MB_CAL_OK);
    assert(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST), rd(MB_CAL_WINDOW_FIRST + 1)) == ang_tab[128]);

    /* pix_dif_tab1/2 и offset_minmax доступны; неизвестная таблица — VALUE. */
    pix_dif_tab1[0] = 51.0f;
    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(CAL_T_PIXDIF1 << 8 | 0)) == MB_CAL_OK);
    assert(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST), rd(MB_CAL_WINDOW_FIRST + 1)) == 51.0f);
    offset_minmax[7] = 0.000001f;
    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(CAL_T_OFFSET_MINMAX << 8 | 7)) == MB_CAL_OK);
    assert(fabsf(word_pair_to_float(rd(MB_CAL_WINDOW_FIRST), rd(MB_CAL_WINDOW_FIRST + 1)) -
                 0.000001f) < 1e-12f);
    assert(wr(MB_H_REG_CAL_PAGE, (uint16_t)(9u << 8)) == MB_CAL_ERR_VALUE);
    printf("  paged float window ... ok\n");
}

static void test_live(void)
{
    reset_world();
    assert(rd(CAL_R_BLOCK_ID) == MB_CAL_BLOCK_ID_VALUE);
    /* Границы блока: 36 живых регистров, сразу за ними 32 слова окна. */
    assert(MB_CAL_INPUT_COUNT == 36);
    assert(MB_CAL_WINDOW_FIRST == MB_CAL_INPUT_FIRST + MB_CAL_INPUT_COUNT);
    assert(MB_CAL_WINDOW_LAST == MB_CAL_INPUT_TOTAL - 1);
    assert(MB_CAL_INPUT_TOTAL == 79);
    assert(rd(MB_CAL_WINDOW_LAST + 1) == 0); /* за картой — тишина, не вылет */

    pix_rdy_num1 = 37;
    offset = 68.5f;
    min_max = 0.000020f; /* 20 микроградусов */
    cur_ang_E = 90.5f;
    Temperature = 27.3f;
    cycles_max = 12345;
    encoder_state = 0x20;
    assert(rd(CAL_R_RDY1) == 37);
    assert(rd(CAL_R_OFFSET_X256) == (uint16_t)(68.5f * 256.0f + 0.5f));
    assert(u32_of(rd(CAL_R_MINMAX_HI), rd(CAL_R_MINMAX_LO)) == 20u);
    assert(u32_of(rd(CAL_R_ANGLE_HI), rd(CAL_R_ANGLE_LO)) == 90500000u);
    assert(rd(CAL_R_TEMP) == 273);
    assert(u32_of(rd(CAL_R_CYCLES_HI), rd(CAL_R_CYCLES_LO)) == 12345u);
    assert(flag(CAL_F_WAIT_PASS2) == 1);

    /* Отрицательная метрика проходит как знаковое число. */
    min_max = -0.000003f;
    assert(u32_of(rd(CAL_R_MINMAX_HI), rd(CAL_R_MINMAX_LO)) == 0xFFFFFFFDu);

    /* Живой счётчик кадров: published snapshot-счётчик растёт без SNAP. */
    uint32_t before = u32_of(rd(CAL_R_SAMPLE_HI), rd(CAL_R_SAMPLE_LO));
    snapshot_publish(10.0f, 5, 0, 0x00, 1000);
    assert(u32_of(rd(CAL_R_SAMPLE_HI), rd(CAL_R_SAMPLE_LO)) == before + 1);

    /* DIAG — приращения к прошлому чтению: направление по счётчикам оборотов. */
    (void)diag_once(); /* прайм */
    snapshot_publish(11.0f, 5, 0, 0x00, 1001);
    rev_right_cnt += 1;
    auto_cal = 1; /* проход 1 требует роста угла — всё верно */
    uint16_t d = diag_once();
    assert((d & (1u << CAL_D_MOVING_RIGHT)) != 0);
    assert((d & (1u << CAL_D_MOVING_LEFT)) == 0);
    assert((d & (1u << CAL_D_WRONG_DIRECTION)) == 0);
    assert((d & (1u << CAL_D_NO_FRAMES)) == 0);

    snapshot_publish(12.0f, 5, 0, 0x00, 1002);
    rev_left_cnt += 2;
    d = diag_once();
    assert((d & (1u << CAL_D_MOVING_LEFT)) != 0);

    /* Крутим не туда: тот же ход, но уже проход 2. */
    snapshot_publish(13.0f, 5, 0, 0x00, 1003);
    rev_right_cnt += 1;
    auto_cal = 2;
    d = diag_once();
    assert((d & (1u << CAL_D_WRONG_DIRECTION)) != 0);

    /* Ни одного нового кадра между опросами → NO_FRAMES. */
    d = diag_once();
    assert((d & (1u << CAL_D_NO_FRAMES)) != 0);
    auto_cal = 0;

    /* Классический признак неверного направления: застыло на 72. */
    reset_world();
    start_calibrate = 1;
    pix_rdy_num1 = BIT_TAB_SIZE / 2;
    assert((diag_once() & (1u << CAL_D_EVERY_OTHER)) != 0);
    pix_rdy_num1 = 3;
    assert((diag_once() & (1u << CAL_D_EVERY_OTHER)) == 0);
    printf("  live mirror and diagnostics ... ok\n");
}

static void test_handlers(void)
{
    uint16_t regs[MB_CAL_INPUT_TOTAL];
    reset_world();

    /* SNAP по-прежнему на нулевом адресе, команды — на 1…4. */
    assert(fc06(MB_H_REG_SNAP, 42) == MB_ENOERR);
    uint16_t snap[2];
    assert(fc04(0, 2, snap) == MB_ENOERR);
    assert(snap[0] == 42);

    /* Неизвестный holding-адрес → 0x02 (Illegal Data Address). */
    assert(fc06(9, 1) == MB_ENOREG);

    /* Отказ команды → 0x04 через FC06 (mb_cal_write → MB_EIO). */
    assert(fc06(MB_H_REG_CAL_CMD, MB_CAL_CMD_OFFSET) == MB_EIO);
    /* …и то же самое видно в LAST_RESULT. */
    assert(rd(CAL_R_LAST_RESULT) == MB_CAL_ERR_LOCKED);

    /* Занятость/EEPROM → 0x06 */
    i2c3_tx_wp = 0;
    assert(fc06(MB_H_REG_CAL_KEY, MB_CAL_KEY_UNLOCK) == MB_ENOERR);
    assert(fc06(MB_H_REG_CAL_CMD, MB_CAL_CMD_OFFSET) == MB_ETIMEDOUT);
    i2c3_tx_wp = 1;

    /* FC04: весь блок разом и проверка границ. */
    assert(fc04(MB_CAL_INPUT_FIRST, MB_CAL_INPUT_COUNT, regs) == MB_ENOERR);
    assert(regs[0] == MB_CAL_BLOCK_ID_VALUE);
    assert(fc04(0, MB_CAL_INPUT_TOTAL, regs) == MB_ENOERR);
    assert(regs[CAL_R_BLOCK_ID] == MB_CAL_BLOCK_ID_VALUE);
    assert(fc04(0, (uint16_t)(MB_CAL_INPUT_TOTAL + 1), regs) == MB_ENOREG);
    assert(fc04((uint16_t)(MB_CAL_INPUT_TOTAL - 4), 8, regs) == MB_ENOREG);
    printf("  modbus handler routing and exceptions ... ok\n");
}

int main(void)
{
    printf("Modbus self-calibration command channel:\n");
    test_lock();
    test_guards();
    test_start_effects();
    test_rollback();
    test_window();
    test_live();
    test_handlers();
    printf("OK\n");
    return 0;
}
