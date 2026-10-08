/*
 * Исполнение самокалибровки по Modbus RTU: приём команд, защита, живой статус.
 *
 * Что здесь происходит и почему это безопасно:
 *
 * 1) Все функции вызываются ИЗ ОСНОВНОГО ЦИКЛА (eMBPoll -> eMBRegHoldingCB /
 *    eMBRegInputCB), в том же контексте, где живут конечные автоматы main.c.
 *    Команда поэтому только взводит флаги, а не лезет в прерывание АЦП: жёсткое
 *    реальное время кадра не срывается, и гонки за переменные нет — их пишем мы
 *    и main loop, оба по очереди.
 *
 * 2) Коды команд (0x10…0x14) и их эффекты повторяют удалённый SPI-обработчик
 *    (историческая справка: Src/main.c до коммита 6890091, spi_recv_process()).
 *    Добавлено только то, чего в SPI-канале не было: ключ-затвор, проверка
 *    занятости и незавершённой записи EEPROM, откат прерванной калибровки.
 *
 * 3) EEPROM. Завершённый автомат сам пишет результат (6…578 байт + CRC16),
 *    снимая аппаратную защиту WP. Значит запуск новой калибровки поверх
 *    незаписанного результата испортит память, а прерванная «на середине»
 *    калибровка оставляет в RAM недописанные таблицы. Отсюда MB_CAL_ERR_EEPROM
 *    (не пишем, пока i2c3_tx_wp == 0) и снимок offset/buf_k для отката.
 *
 * src/main.c не изменён: доступ к его переменным идёт через Inc/encoder_vars.h.
 */
#include "modbus_calib.h"
#include "encoder_vars.h"
#include "snapshot_registers.h" /* snapshot_publish_count() */
#include <string.h>

#define UDEG_PER_DEG 1000000.0      /* масштаб угла для передачи целыми словами */
#define MB_CAL_OFFSET_MIN 30.0f     /* физический смысл offset'а имеет только     */
#define MB_CAL_OFFSET_MAX 90.0f     /* в середине активной части линейки          */

#define ROLLBACK_NONE  0u
#define ROLLBACK_OFFSET 1u
#define ROLLBACK_BUFK   2u

/* Снимок предыдущего опроса — нужен, чтобы DIAG показывал приращения, а не
 * абсолютные значения: «прогресс встал» видно только относительно того, что
 * оператор читал секунду назад. */
typedef struct {
    uint32_t frames;
    int right, left;
    int rdy1, rdy2;
    int ophase, kphase;
    int revs;
    unsigned serr1;
    float minmax;
    uint8_t valid;
} diag_prev_t;

typedef struct {
    uint8_t armed;                    /* ключ CAL_KEY взведён                     */
    uint8_t relax, keep;              /* флаги CAL_CFG                            */
    float offset_saved;               /* состояние до пуска offset_cal            */
    float bufk_saved[ENCODER_PIXELS]; /* состояние до пуска angk_cal              */
    uint8_t rollback;                 /* что откатывать при 0x10 (ROLLBACK_*)     */
    uint8_t page_table;               /* CAL_PAGE[15:8]                           */
    uint16_t page_base;               /* CAL_PAGE[7:0], прижатый к концу таблицы  */
    uint16_t last_cmd, last_result;
    uint16_t cmd_count, rej_count;
    diag_prev_t prev;
} mb_cal_ctx_t;

static mb_cal_ctx_t ctx;

/* ------------------------------ вспомогательные ---------------------------- */

static int angtab_running(void)
{
    return start_calibrate != 0 || encoder_state == MB_CAL_STATE_ANGTAB_1 ||
           encoder_state == MB_CAL_STATE_ANGTAB_2;
}
static int offset_running(void)
{
    return start_offset_cal != 0 || encoder_state == MB_CAL_STATE_OFFSET;
}
static int angk_running(void)
{
    return start_angk_cal != 0 || encoder_state == MB_CAL_STATE_BUFK;
}
static int cal_busy(void)
{
    return angtab_running() || offset_running() || angk_running();
}

/* Таблица углов секторов имеет смысл только при заведомо известной точке съёма:
 * либо она принята из EEPROM, либо offset_cal только что завершилась. */
static int offset_is_known(void)
{
    return offset_rdy != 0 || encoder_state == MB_CAL_STATE_OFFSET_RDY;
}

static uint16_t table_len(uint8_t table)
{
    switch (table) {
    case CAL_T_ANG_TAB: case CAL_T_PIXDIF1: case CAL_T_PIXDIF2: return BIT_TAB_SIZE;
    case CAL_T_BUF_K: case CAL_T_OFFSET_MINMAX: return ENCODER_PIXELS;
    default: return 0;
    }
}

static const float *table_ptr(uint8_t table)
{
    switch (table) {
    case CAL_T_ANG_TAB: return &ang_tab[0];
    case CAL_T_BUF_K: return &buf_k[0];
    case CAL_T_PIXDIF1: return &pix_dif_tab1[0];
    case CAL_T_PIXDIF2: return &pix_dif_tab2[0];
    case CAL_T_OFFSET_MINMAX: return &offset_minmax[0];
    default: return (const float *)0;
    }
}

/* Завершение этапа, которого мы дождались, отключает откат: результат уже и в
 * EEPROM, и в RAM, возвращать «как было» нельзя. */
static void forget_finished(void)
{
    if (encoder_state == MB_CAL_STATE_OFFSET_RDY) ctx.rollback &= (uint8_t)~ROLLBACK_OFFSET;
    if (encoder_state == MB_CAL_STATE_BUFK_RDY) ctx.rollback &= (uint8_t)~ROLLBACK_BUFK;
    if (!cal_busy() && encoder_state == MB_CAL_STATE_IDLE) ctx.rollback = ROLLBACK_NONE;
}

static void apply_rollback(void)
{
    if (ctx.keep) return;
    if (ctx.rollback & ROLLBACK_OFFSET) offset = ctx.offset_saved;
    if (ctx.rollback & ROLLBACK_BUFK)
        memcpy(&buf_k[0], &ctx.bufk_saved[0], sizeof ctx.bufk_saved);
    ctx.rollback = ROLLBACK_NONE;
}

static uint16_t clamp_u16(double v)
{
    if (!(v == v) || v < 0.0) return 0u;
    if (v > 65535.0) return 0xFFFFu;
    return (uint16_t)(v + 0.5);
}

static int16_t clamp_i16(double v)
{
    if (!(v == v)) return 0;
    if (v > 32767.0) return 32767;
    if (v < -32768.0) return (int16_t)-32768;
    return (int16_t)(v < 0.0 ? v - 0.5 : v + 0.5);
}

static uint32_t udeg_of(float deg)
{
    /* NaN не проходит ни одно сравнение с самим собой, поэтому проверка
     * «>= 0 && < 360» отсекает и мусор, и кадр на переходе через 360. */
    if (!(deg >= 0.0f) || !(deg < 360.0f)) return 0u;
    return (uint32_t)((double)deg * UDEG_PER_DEG + 0.5);
}

static int32_t sudeg_of(float deg)
{
    double v;
    if (!(deg == deg) || deg > 3600.0f || deg < -3600.0f) return 0;
    v = (double)deg * UDEG_PER_DEG;
    return (v < 0.0) ? (int32_t)(v - 0.5) : (int32_t)(v + 0.5);
}

/* -------------------------------- запись ---------------------------------- */

static int known_cmd(uint16_t code)
{
    switch (code) {
    case MB_CAL_CMD_ANGTAB_RIGHT: case MB_CAL_CMD_ANGTAB_LEFT:
    case MB_CAL_CMD_OFFSET: case MB_CAL_CMD_BUFK: return 1;
    default: return 0;
    }
}

static mb_cal_result_t accept_start(uint16_t code)
{
    /* Неизвестный код проверяется до всего остального: «мусор в шине» и
     * «забыли ключ» — разные отказы, и первый из них ключом не лечится. */
    if (!known_cmd(code)) return MB_CAL_ERR_VALUE;
    if (!ctx.armed) return MB_CAL_ERR_LOCKED;
    if (i2c3_tx_wp == 0) return MB_CAL_ERR_EEPROM;
    if (cal_busy()) return MB_CAL_ERR_BUSY;

    if (code == MB_CAL_CMD_ANGTAB_RIGHT || code == MB_CAL_CMD_ANGTAB_LEFT) {
        /* Равномерность проверки не зависит от RELAX: она защищает EEPROM от
         * записи результата, полученного при заведомо бессмысленном offset. */
        if (!(offset >= MB_CAL_OFFSET_MIN && offset <= MB_CAL_OFFSET_MAX))
            return MB_CAL_ERR_PREREQ;
        if (!ctx.relax && !offset_is_known()) return MB_CAL_ERR_PREREQ;
    }
    if (code == MB_CAL_CMD_ANGTAB_LEFT && !ctx.relax &&
        encoder_state != MB_CAL_STATE_ANGTAB_1_RDY)
        return MB_CAL_ERR_PREREQ; /* усреднение с пустой таблицей прохода 1 */

    ctx.armed = 0; /* one-shot: ключ гаснет сразу, повтор кадра не запустит всё снова */

    switch (code) {
    case MB_CAL_CMD_ANGTAB_RIGHT:
        start_calibrate = 1;
        encoder_state = MB_CAL_STATE_ANGTAB_1;
        break;
    case MB_CAL_CMD_ANGTAB_LEFT:
        start_calibrate = 2;
        encoder_state = MB_CAL_STATE_ANGTAB_2;
        break;
    case MB_CAL_CMD_OFFSET:
        ctx.offset_saved = offset;
        ctx.rollback |= ROLLBACK_OFFSET;
        start_offset_cal = 1;
        avg_minmax_num = 0;
        offset_phase = 0;
        for (int i = 0; i < ENCODER_PIXELS; i++) offset_minmax[i] = 1.0f;
        offset_cur = offset_start;
        encoder_state = MB_CAL_STATE_OFFSET;
        break;
    case MB_CAL_CMD_BUFK:
        memcpy(&ctx.bufk_saved[0], &buf_k[0], sizeof ctx.bufk_saved);
        ctx.rollback |= ROLLBACK_BUFK;
        start_angk_cal = 1;
        avg_minmax_num = 0;
        for (int i = 0; i < ENCODER_PIXELS; i++) buf_x3[i] = 0.0f;
        anglek_phase = 0;
        backlight_width_en = 0; /* пиковый детектор не должен видеть АРУ */
        encoder_state = MB_CAL_STATE_BUFK;
        break;
    default:
        return MB_CAL_ERR_VALUE; /* сюда не доходим: known_cmd() выше */
    }
    return MB_CAL_OK;
}

static void do_stop(void)
{
    start_calibrate = 0;
    auto_cal = 0;
    start_offset_cal = 0;
    start_angk_cal = 0;
    backlight_width_en = 1;
    cycles_max = 0;
    encoder_state = MB_CAL_STATE_IDLE;
    apply_rollback();
}

mb_cal_result_t mb_cal_write(uint16_t address, uint16_t value)
{
    mb_cal_result_t r = MB_CAL_OK;

    forget_finished();

    switch (address) {
    case MB_H_REG_CAL_PAGE: {
        uint8_t table = MB_CAL_PAGE_TABLE(value);
        uint16_t len, limit;
        if (table >= CAL_T_COUNT) return MB_CAL_ERR_VALUE;
        len = table_len(table);
        limit = (len > MB_CAL_WINDOW_FLOATS) ? (uint16_t)(len - MB_CAL_WINDOW_FLOATS) : 0u;
        ctx.page_table = table;
        ctx.page_base = (MB_CAL_PAGE_INDEX(value) > limit)
                            ? limit
                            : MB_CAL_PAGE_INDEX(value);
        break;
    }
    case MB_H_REG_CAL_CFG:
        ctx.relax = (uint8_t)((value & MB_CAL_CFG_RELAX) != 0);
        ctx.keep = (uint8_t)((value & MB_CAL_CFG_KEEP) != 0);
        break;
    case MB_H_REG_CAL_KEY:
        ctx.armed = (uint8_t)(value == MB_CAL_KEY_UNLOCK);
        break;
    case MB_H_REG_CAL_CMD:
        if (value == MB_CAL_CMD_STOP) {
            /* Стоп — единственная команда без ключа и без проверок: аварийный
             * выход не должен запираться собственной защитой. */
            do_stop();
        } else {
            r = accept_start((uint16_t)(value & 0x00FFu));
        }
        ctx.last_cmd = value;
        break;
    default:
        return MB_CAL_ERR_REG;
    }

    ctx.last_result = (uint16_t)r;
    if (r == MB_CAL_OK) ctx.cmd_count++;
    else ctx.rej_count++;
    return r;
}

/* -------------------------------- чтение ---------------------------------- */

static uint16_t build_flags(void)
{
    uint16_t f = 0;
    if (start_calibrate != 0) f |= (uint16_t)1u << CAL_F_CAL_PENDING;
    if (offset_running()) f |= (uint16_t)1u << CAL_F_OFFSET_RUN;
    if (angk_running()) f |= (uint16_t)1u << CAL_F_ANGK_RUN;
    if (rev_en) f |= (uint16_t)1u << CAL_F_REV_EN;
    if (backlight_width_en) f |= (uint16_t)1u << CAL_F_AGC_ON;
    if (i2c3_tx_wp) f |= (uint16_t)1u << CAL_F_EEPROM_IDLE;
    if (errorflag) f |= (uint16_t)1u << CAL_F_ERROR;
    if (ctx.armed) f |= (uint16_t)1u << CAL_F_ARMED;
    if (ctx.relax) f |= (uint16_t)1u << CAL_F_RELAX;
    if (ctx.keep) f |= (uint16_t)1u << CAL_F_KEEP;
    if (ang_tab_rdy) f |= (uint16_t)1u << CAL_F_ANG_TAB_RDY;
    if (buf_k_rdy) f |= (uint16_t)1u << CAL_F_BUF_K_RDY;
    if (offset_rdy) f |= (uint16_t)1u << CAL_F_OFFSET_RDY;
    if (lasdac_rdy) f |= (uint16_t)1u << CAL_F_LASDAC_RDY;
    if (cal_busy()) f |= (uint16_t)1u << CAL_F_BUSY;
    if (encoder_state == MB_CAL_STATE_ANGTAB_1_RDY) f |= (uint16_t)1u << CAL_F_WAIT_PASS2;
    return f;
}

static uint16_t build_diag(uint32_t frames)
{
    uint16_t d = 0;
    int moving_right, moving_left = 0;

    if (ctx.prev.valid) {
        if ((int32_t)(frames - ctx.prev.frames) <= 0) d |= (uint16_t)1u << CAL_D_NO_FRAMES;
        moving_right = (rev_right_cnt - ctx.prev.right) > 0;
        moving_left = (rev_left_cnt - ctx.prev.left) > 0;
        if (moving_right) d |= (uint16_t)1u << CAL_D_MOVING_RIGHT;
        if (moving_left) d |= (uint16_t)1u << CAL_D_MOVING_LEFT;
        /* Проход 1 требует роста угла, проход 2 — убывания. Повинился не туда —
         * видно сразу, до того как оператор потратит обороты впустую. */
        if (auto_cal == 1 && moving_left && !moving_right)
            d |= (uint16_t)1u << CAL_D_WRONG_DIRECTION;
        if (auto_cal == 2 && moving_right && !moving_left)
            d |= (uint16_t)1u << CAL_D_WRONG_DIRECTION;
        if (angtab_running() && pix_rdy_num1 == ctx.prev.rdy1 &&
            pix_rdy_num2 == ctx.prev.rdy2)
            d |= (uint16_t)1u << CAL_D_ANGTAB_STALLED;
        if (offset_running() && offset_phase < 100 && offset_phase == ctx.prev.ophase)
            d |= (uint16_t)1u << CAL_D_OFFSET_STALLED;
        if (angk_running() && anglek_phase < 100 && anglek_phase == ctx.prev.kphase)
            d |= (uint16_t)1u << CAL_D_ANGK_STALLED;
        if (min_max > ctx.prev.minmax) d |= (uint16_t)1u << CAL_D_MINMAX_UP;
        if (serrcnt1 != ctx.prev.serr1) d |= (uint16_t)1u << CAL_D_SERR_GROWING;
        if ((offset_running() || angk_running()) && avg_minmax_num == ctx.prev.revs)
            d |= (uint16_t)1u << CAL_D_NO_REVS;
    }
    /* 72 = измерен каждый второй сектор — классический признак неверного
     * направления (docs §9.4). Считается независимо от опросов. */
    if (angtab_running() && (pix_rdy_num1 == BIT_TAB_SIZE / 2 ||
                             pix_rdy_num2 == BIT_TAB_SIZE / 2))
        d |= (uint16_t)1u << CAL_D_EVERY_OTHER;

    ctx.prev.frames = frames;
    ctx.prev.right = rev_right_cnt;
    ctx.prev.left = rev_left_cnt;
    ctx.prev.rdy1 = pix_rdy_num1;
    ctx.prev.rdy2 = pix_rdy_num2;
    ctx.prev.ophase = offset_phase;
    ctx.prev.kphase = anglek_phase;
    ctx.prev.revs = avg_minmax_num;
    ctx.prev.serr1 = serrcnt1;
    ctx.prev.minmax = min_max;
    ctx.prev.valid = 1;
    return d;
}

static uint16_t live_reg(uint16_t index)
{
    switch (index) {
    case CAL_R_BLOCK_ID - MB_CAL_INPUT_FIRST: return MB_CAL_BLOCK_ID_VALUE;
    case CAL_R_STATE - MB_CAL_INPUT_FIRST: return (uint16_t)encoder_state;
    case CAL_R_AUTO_CAL - MB_CAL_INPUT_FIRST: return (uint16_t)auto_cal;
    case CAL_R_RDY1 - MB_CAL_INPUT_FIRST: return clamp_u16(pix_rdy_num1);
    case CAL_R_RDY2 - MB_CAL_INPUT_FIRST: return clamp_u16(pix_rdy_num2);
    case CAL_R_OFFSET_PHASE - MB_CAL_INPUT_FIRST: return clamp_u16(offset_phase);
    case CAL_R_OFFSET_CUR - MB_CAL_INPUT_FIRST: return clamp_u16(offset_cur);
    case CAL_R_OFFSET_FOUND - MB_CAL_INPUT_FIRST: return clamp_u16(offset_found);
    case CAL_R_OFFSET_X256 - MB_CAL_INPUT_FIRST: return clamp_u16((double)offset * 256.0);
    case CAL_R_ANGLEK_PHASE - MB_CAL_INPUT_FIRST: return clamp_u16(anglek_phase);
    case CAL_R_ANGLEK_CUR - MB_CAL_INPUT_FIRST: return clamp_u16(anglek_cur);
    case CAL_R_PIXMINMAX - MB_CAL_INPUT_FIRST: return (uint16_t)clamp_i16(pix_min_max);
    case CAL_R_MINMAX_HI - MB_CAL_INPUT_FIRST:
        return (uint16_t)((uint32_t)sudeg_of(min_max) >> 16);
    case CAL_R_MINMAX_LO - MB_CAL_INPUT_FIRST:
        return (uint16_t)(uint32_t)sudeg_of(min_max);
    case CAL_R_REVS - MB_CAL_INPUT_FIRST: return clamp_u16(avg_minmax_num);
    case CAL_R_REV_RIGHT - MB_CAL_INPUT_FIRST: return (uint16_t)rev_right_cnt;
    case CAL_R_REV_LEFT - MB_CAL_INPUT_FIRST: return (uint16_t)rev_left_cnt;
    case CAL_R_SERR1 - MB_CAL_INPUT_FIRST: return (uint16_t)serrcnt1;
    case CAL_R_SERR2 - MB_CAL_INPUT_FIRST: return (uint16_t)serrcnt2;
    case CAL_R_SECTOR - MB_CAL_INPUT_FIRST: return (uint16_t)sector;
    case CAL_R_ANGLE_HI - MB_CAL_INPUT_FIRST: return (uint16_t)(udeg_of(cur_ang_E) >> 16);
    case CAL_R_ANGLE_LO - MB_CAL_INPUT_FIRST: return (uint16_t)udeg_of(cur_ang_E);
    case CAL_R_FLAGS - MB_CAL_INPUT_FIRST: return build_flags();
    case CAL_R_DIAG - MB_CAL_INPUT_FIRST: return build_diag(snapshot_publish_count());
    case CAL_R_LAST_CMD - MB_CAL_INPUT_FIRST: return ctx.last_cmd;
    case CAL_R_LAST_RESULT - MB_CAL_INPUT_FIRST: return ctx.last_result;
    case CAL_R_CMD_COUNT - MB_CAL_INPUT_FIRST: return ctx.cmd_count;
    case CAL_R_REJ_COUNT - MB_CAL_INPUT_FIRST: return ctx.rej_count;
    case CAL_R_CYCLES_HI - MB_CAL_INPUT_FIRST: return (uint16_t)(cycles_max >> 16);
    case CAL_R_CYCLES_LO - MB_CAL_INPUT_FIRST: return (uint16_t)cycles_max;
    case CAL_R_BACKLIGHT - MB_CAL_INPUT_FIRST: return backlight_width_ticks;
    case CAL_R_TEMP - MB_CAL_INPUT_FIRST: return (uint16_t)clamp_i16((double)Temperature * 10.0);
    case CAL_R_PIX_AVG - MB_CAL_INPUT_FIRST: return clamp_u16(pix_dif_num_avg);
    case CAL_R_OFFSET_AVG - MB_CAL_INPUT_FIRST: return clamp_u16(offset_avg_num);
    case CAL_R_SAMPLE_HI - MB_CAL_INPUT_FIRST:
        return (uint16_t)(snapshot_publish_count() >> 16);
    case CAL_R_SAMPLE_LO - MB_CAL_INPUT_FIRST:
        return (uint16_t)snapshot_publish_count();
    default: return 0u;
    }
}

static uint16_t window_word(uint16_t index)
{
    const float *table = table_ptr(ctx.page_table);
    uint16_t limit = table_len(ctx.page_table);
    uint16_t i;
    uint32_t bits;
    float value;

    if (table == 0 || index >= MB_CAL_WINDOW_COUNT) return 0u;
    i = (uint16_t)(index / 2);
    if (i >= MB_CAL_WINDOW_FLOATS) return 0u;
    if ((uint32_t)ctx.page_base + i >= limit) return 0u;

    value = table[ctx.page_base + i];
    (void)memcpy(&bits, &value, sizeof bits);
    /* Порядок Modbus: сначала старшее слово регистра, потом младшее. */
    return (index & 1u) ? (uint16_t)bits : (uint16_t)(bits >> 16);
}

uint16_t mb_cal_read(uint16_t address)
{
    if (address < MB_CAL_INPUT_FIRST) return 0u; /* снимок — не к нам */
    if (address < CAL_R_INPUT_END)
        return live_reg((uint16_t)(address - MB_CAL_INPUT_FIRST));
    if (address <= MB_CAL_WINDOW_LAST)
        return window_word((uint16_t)(address - MB_CAL_WINDOW_FIRST));
    return 0u;
}

void mb_cal_reset(void)
{
    (void)memset(&ctx, 0, sizeof ctx);
}
