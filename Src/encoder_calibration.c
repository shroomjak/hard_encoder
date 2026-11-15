/*
 * Единый модуль управления автоматами калибровки STM32 (без зависимости от Modbus/SPI).
 *
 * Архитектура разделения ответственности:
 *   1) Последовательность полного цикла калибровки задаётся ТОЛЬКО в одном месте —
 *      в автомате мастера ESP32 (esp32_master/main/workflow.c):
 *          0x14 (angk_cal: buf_k) -> 0x13 (offset_cal: offset) ->
 *          0x11 (angtab_cal: проход 1) -> 0x12 (angtab_cal: проход 2) -> 0x10 (STOP).
 *   2) Ведомая головка STM32 исполняет ровно ОДНУ атомарную операцию за раз.
 *      Запуск и остановка проходят через encoder_calibration_command(), а
 *      покадровое продвижение активного автомата — исключительно через
 *      encoder_calibration_step().
 *   3) Одновременная работа нескольких автоматов запрещена: при старте проверяется
 *      отсутствие активных процедур и завершение фоновой записи EEPROM (i2c3_tx_wp),
 *      а в encoder_calibration_step() контролируется инвариант «активен не более чем
 *      один автомат» (защита от ручной модификации флагов через SWD).
 */
#include "encoder_calibration.h"
#include <string.h>

/* Переменные и процедуры калибровки из Src/main.c. */
extern int start_calibrate, auto_cal, start_offset_cal, offset_phase;
extern int start_angk_cal, anglek_phase, offset_cur, offset_start;
extern int avg_minmax_num, old_sector;
extern float offset_minmax[128], buf_x3[128];
extern unsigned char encoder_state;
extern uint8_t backlight_width_en, i2c3_tx_wp;
extern uint32_t cycles_max;

extern void angk_cal(void);
extern void offset_cal(void);
extern void angtab_cal(void);

static void reset_all_calibration_machines(void)
{
    /* STOP останавливает ВСЕ автоматы (включая buf_k), паркует фазы на 101
     * и возвращает разрешение АРУ подсветки. Уже запущенную фоновую транзакцию
     * I2C EEPROM и коэффициенты в ОЗУ не откатывает. */
    start_calibrate = 0;
    auto_cal = 0;
    start_offset_cal = 0;
    start_angk_cal = 0;
    offset_phase = 101;
    anglek_phase = 101;
    backlight_width_en = 1;
    cycles_max = 0;
    encoder_state = 0;
}

encoder_cal_result_t encoder_calibration_command(uint16_t command)
{
    if (command == 0x10U) {
        reset_all_calibration_machines();
        return ENCODER_CAL_OK;
    }
    if (command < 0x11U || command > 0x14U) {
        return ENCODER_CAL_INVALID;
    }
    if (start_calibrate || auto_cal || start_offset_cal || start_angk_cal ||
        !i2c3_tx_wp) {
        return ENCODER_CAL_BUSY;
    }
    /* Второй проход ang_tab допустим строго после завершения первого (0x20),
     * без промежуточного STOP или запуска другой калибровки. */
    if (command == 0x12U && encoder_state != 0x20U) {
        return ENCODER_CAL_INVALID;
    }

    switch (command) {
    case 0x11U:
    case 0x12U:
        /* Массивы накопления прохода очищает angtab_cal() при приёме start_calibrate;
         * old_sector = -1 снимает защёлку сектора перед первым валидным отсчётом. */
        old_sector = -1;
        start_calibrate = (command == 0x11U) ? 1 : 2;
        encoder_state = (command == 0x11U) ? 0x10U : 0x30U;
        break;

    case 0x13U:
        /* Поиск offset стартует с начала настроенного окна [offset_start, offset_end];
         * буфер целевой функции инициализируется заведомо большим значением 1.0f град. */
        offset_cur = offset_start;
        offset_phase = 0;
        avg_minmax_num = 0;
        for (unsigned i = 0U; i < 128U; ++i) {
            offset_minmax[i] = 1.0f;
        }
        encoder_state = 0x50U;
        start_offset_cal = 1;
        break;

    case 0x14U:
        /* Пиковый детектор buf_x3 должен копить амплитуду при фиксированной яркости
         * светодиода: обнуляем старые пики и блокируем подстройку АРУ подсветки. */
        anglek_phase = 0;
        avg_minmax_num = 0;
        memset(buf_x3, 0, sizeof buf_x3);
        backlight_width_en = 0;
        encoder_state = 0x70U;
        start_angk_cal = 1;
        break;
    }
    return ENCODER_CAL_OK;
}

void encoder_calibration_step(void)
{
    const unsigned angk_active = (start_angk_cal > 0) ? 1U : 0U;
    const unsigned offset_active = (start_offset_cal > 0) ? 1U : 0U;
    const unsigned angtab_active = (start_calibrate > 0 || auto_cal > 0) ? 1U : 0U;
    const unsigned active_count = angk_active + offset_active + angtab_active;

    if (active_count == 0U) {
        return;
    }

    /* Защита от коллизий: два автомата никогда не должны исполняться одновременно,
     * так как они разделяют счётчик оборотов avg_minmax_num, меняют взаимозависимые
     * параметры (buf_k -> offset -> ang_tab) и пишут в одну шину I2C3 EEPROM. */
    if (active_count > 1U) {
        reset_all_calibration_machines();
        return;
    }

    /* Порядок веток отражает физическую зависимость параметров:
     * 1) buf_k (0x14) -> 2) offset (0x13) -> 3) ang_tab (0x11, 0x12). */
    if (angk_active) {
        angk_cal();
    } else if (offset_active) {
        offset_cal();
    } else {
        angtab_cal();
    }
}
