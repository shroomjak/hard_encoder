/*
 * Запуск и контроль самокалибровки по Modbus RTU.
 *
 * Тело команд повторяет то, что делают автоматы при запуске из отладчика
 * (исторически те же коды приходили по SPI, см. Src/main.c до коммита 6890091):
 * запись только взводит флаги, шаг автомата делает main loop. Поэтому обработчик
 * вызывается из eMBPoll() в основном цикле, а не из прерывания, и жёсткое
 * реальное время кадра не трогает.
 *
 * src/main.c не изменяется: его переменные объявлены в Inc/encoder_vars.h.
 * Карта регистров и процедура: Inc/modbus_calib.h, docs/calibration_modbus.md.
 */
#include "modbus_calib.h"
#include "encoder_vars.h"
#include "snapshot_registers.h"   /* snapshot_publish_count() */

#include <string.h>

/* Выбранная страница выгрузки (CAL_PAGE). */
static uint8_t page_table;
static uint16_t page_index;

static uint16_t table_len(uint8_t table)
{
    return (table == MB_CAL_T_ANGTAB) ? (uint16_t)BIT_TAB_SIZE      /* ang_tab  */
                                     : (uint16_t)ENCODER_PIXELS;    /* buf_k    */
}

static const float *table_ptr(uint8_t table)
{
    if (table == MB_CAL_T_ANGTAB) return &ang_tab[0];
    if (table == MB_CAL_T_BUFK) return &buf_k[0];
    return &ang_tab[0];               /* код таблицы проверен при записи CAL_PAGE */
}

/* Угол в микроградусы. NaN и выход за [0,360) дают 0: такой кадр не готов. */
static uint32_t udeg_of(float deg)
{
    if (!(deg >= 0.0f) || !(deg < 360.0f)) return 0u;
    return (uint32_t)((double)deg * 1000000.0 + 0.5);
}

/* Метрика min_max тоже в микроградусы, но со знаком: она из остатка, не из угла. */
static int32_t sudeg_of(float deg)
{
    double v;
    if (!(deg == deg) || deg > 3600.0f || deg < -3600.0f) return 0;
    v = (double)deg * 1000000.0;
    return (v < 0.0) ? (int32_t)(v - 0.5) : (int32_t)(v + 0.5);
}

static uint16_t u16_of(double v)
{
    if (!(v == v) || v < 0.0) return 0u;
    if (v > 65535.0) return 0xFFFFu;
    return (uint16_t)(v + 0.5);
}

/* ------------------------------- запись, FC06 ------------------------------- */

int mb_cal_write(uint16_t address, uint16_t value)
{
    if (address == MB_H_CAL_PAGE) {
        uint8_t table = (uint8_t)(value >> 8);
        uint16_t first = (uint16_t)(value & 0x00FFu);
        uint16_t limit;
        if (table >= MB_CAL_T_COUNT) return -2;
        /* Начало окна прижимается к концу таблицы — за границу выйти нельзя. */
        limit = (uint16_t)(table_len(table) - MB_CAL_WINDOW_FLOATS);
        page_table = table;
        page_index = (first > limit) ? limit : first;
        return 0;
    }
    if (address != MB_H_CAL_CMD) return -1;

    switch (value) {
    case MB_CAL_STOP:
        start_calibrate = 0;
        auto_cal = 0;
        start_offset_cal = 0;
        start_angk_cal = 0;      /* иначе прерванная angk_cal продолжится сама */
        backlight_width_en = 1;  /* и АРУ подсветки остаётся выключенной       */
        cycles_max = 0;
        encoder_state = 0x00;
        break;
    case MB_CAL_ANGTAB_RIGHT:
        start_calibrate = 1;
        encoder_state = 0x10;
        break;
    case MB_CAL_ANGTAB_LEFT:
        start_calibrate = 2;
        encoder_state = 0x30;
        break;
    case MB_CAL_OFFSET:
        start_offset_cal = 1;
        avg_minmax_num = 0;
        offset_phase = 0;
        for (int i = 0; i < ENCODER_PIXELS; i++) offset_minmax[i] = 1.0f;
        offset_cur = offset_start;
        encoder_state = 0x50;
        break;
    case MB_CAL_BUFK:
        start_angk_cal = 1;
        avg_minmax_num = 0;
        for (int i = 0; i < ENCODER_PIXELS; i++) buf_x3[i] = 0.0f;
        anglek_phase = 0;
        backlight_width_en = 0;  /* пиковый детектор не должен видеть АРУ      */
        encoder_state = 0x70;
        break;
    default:
        return -2;
    }
    return 0;
}

/* -------------------------------- чтение, FC04 ------------------------------- */

static uint16_t window_word(uint16_t index)
{
    uint32_t bits;
    float value;
    uint16_t i = (uint16_t)(index / 2);
    const float *table = table_ptr(page_table);
    uint16_t limit = table_len(page_table);

    if (index >= 2 * MB_CAL_WINDOW_FLOATS || (uint32_t)page_index + i >= limit) return 0u;
    value = table[page_index + i];
    (void)memcpy(&bits, &value, sizeof bits);
    /* Сначала старшее слово регистра, как везде в Modbus. */
    return (index & 1u) ? (uint16_t)bits : (uint16_t)(bits >> 16);
}

uint16_t mb_cal_read(uint16_t address)
{
    uint32_t frames = snapshot_publish_count();
    uint32_t minmax;
    uint32_t angle;

    if (address < MB_CAL_FIRST || address >= MB_CAL_TOTAL) return 0u;
    if (address >= MB_CAL_WINDOW) return window_word((uint16_t)(address - MB_CAL_WINDOW));

    minmax = (uint32_t)sudeg_of(min_max);
    angle = udeg_of(cur_ang_E);

    switch (address) {
    case MB_R_STATE:        return (uint16_t)encoder_state;
    case MB_R_RDY1:         return u16_of(pix_rdy_num1);
    case MB_R_RDY2:         return u16_of(pix_rdy_num2);
    case MB_R_OFFSET_PHASE: return u16_of(offset_phase);
    case MB_R_OFFSET_CUR:   return u16_of(offset_cur);
    case MB_R_OFFSET_FOUND: return u16_of(offset_found);
    case MB_R_OFFSET_X256:  return u16_of((double)offset * 256.0);
    case MB_R_ANGK_PHASE:   return u16_of(anglek_phase);
    case MB_R_PIX_MIN_MAX:  return (uint16_t)(int16_t)(pix_min_max == pix_min_max
                                               ? (pix_min_max < 0.0f ? pix_min_max - 0.5f
                                                                    : pix_min_max + 0.5f)
                                               : 0.0f);
    case MB_R_MINMAX_HI:    return (uint16_t)(minmax >> 16);
    case MB_R_MINMAX_LO:    return (uint16_t)minmax;
    case MB_R_REVS:         return u16_of(avg_minmax_num);
    case MB_R_REV_RIGHT:    return u16_of(rev_right_cnt);
    case MB_R_REV_LEFT:     return u16_of(rev_left_cnt);
    case MB_R_SERR1:        return u16_of(serrcnt1);
    case MB_R_ANGLE_HI:     return (uint16_t)(angle >> 16);
    case MB_R_ANGLE_LO:     return (uint16_t)angle;
    case MB_R_FRAMES_HI:    return (uint16_t)(frames >> 16);
    case MB_R_FRAMES_LO:    return (uint16_t)frames;
    case MB_R_EEPROM: {
        uint16_t bits = 0u;
        if (i2c3_tx_wp) bits |= (1u << MB_E_WRITTEN);   /* 0 — запись идёт */
        if (ang_tab_rdy) bits |= (1u << MB_E_ANGTAB);
        if (buf_k_rdy) bits |= (1u << MB_E_BUFK);
        if (offset_rdy) bits |= (1u << MB_E_OFFSET);
        return bits;
    }
    default: return 0u;
    }
}
