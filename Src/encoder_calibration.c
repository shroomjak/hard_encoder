/*
 * Команды автоматов калибровки STM32, без SPI и без зависимости от Modbus.
 * Источник переноса: Src/main.c, spi_recv_process(), case 0x10..0x14,
 * commit 4151ab14a18038c5da68aab7dd27da33447cbb08 (строки 5599..5640).
 * Перенесены действия из C-кода, а не из описания алгоритмов.
 *
 * Отличия от исторического обработчика намеренные: проверка занятости/EEPROM,
 * запрет прохода 2 до состояния 0x20, сброс old_sector перед ang_tab;
 * STOP также останавливает buf_k, паркует фазы и возвращает АРУ подсветки.
 * Вызывать только из суперцикла: eMBPoll() и алгоритмы main.c не параллельны.
 * Команда лишь готовит флаги/буферы, не ждёт вращения или записи EEPROM.
 * Состояния завершения 20/40/60/80 выставляют сами алгоритмы в main.c.
 */
#include "encoder_calibration.h"
#include <string.h>

/* Переменные существующих алгоритмов main.c; типы совпадают с определениями. */
extern int start_calibrate, auto_cal, start_offset_cal, offset_phase;
extern int start_angk_cal, anglek_phase, offset_cur, offset_start;
extern int avg_minmax_num, old_sector;
extern float offset_minmax[128], buf_x3[128];
extern unsigned char encoder_state;
extern uint8_t backlight_width_en, i2c3_tx_wp;
extern uint32_t cycles_max;

encoder_cal_result_t encoder_calibration_command(uint16_t command)
{
    if (command == 0x10) {
        /* STOP останавливает ВСЕ автоматы, в том числе buf_k.
         * Не отменяет начатую I2C-запись и не откатывает коэффициенты в RAM. */
        start_calibrate = auto_cal = start_offset_cal = start_angk_cal = 0;
        offset_phase = anglek_phase = 101;
        backlight_width_en = 1;
        cycles_max = 0;
        encoder_state = 0;
        return ENCODER_CAL_OK;
    }
    if (command < 0x11 || command > 0x14) return ENCODER_CAL_INVALID;
    if (start_calibrate || auto_cal || start_offset_cal || start_angk_cal ||
        !i2c3_tx_wp) return ENCODER_CAL_BUSY;
    /* Второй проход допустим только после первого, без промежуточного STOP. */
    if (command == 0x12 && encoder_state != 0x20) return ENCODER_CAL_INVALID;
    switch (command) {
    case 0x11:
    case 0x12:
        /* Отсчёты нового прохода инициализирует angtab_cal(); -1 снимает
         * защёлку сектора, чтобы первый подходящий отсчёт мог быть принят. */
        old_sector = -1;
        start_calibrate = command == 0x11 ? 1 : 2;
        encoder_state = command == 0x11 ? 0x10 : 0x30;
        break;
    case 0x13:
        /* Как в spi_recv_process() исходного main.c: сохраняем настроенное
         * окно offset_start/offset_end, новый поиск начинаем с offset_start.
         * Именно float 1, не ноль и не memset(..., 1, ...). */
        offset_cur = offset_start;
        offset_phase = avg_minmax_num = 0;
        for (unsigned i = 0; i < 128; ++i) offset_minmax[i] = 1.0f;
        encoder_state = 0x50;
        start_offset_cal = 1;
        break;
    case 0x14:
        /* Пиковый детектор должен накопить новую яркость при фиксированной
         * подсветке. Старые пики и работающая АРУ исказили бы новые buf_k. */
        anglek_phase = avg_minmax_num = 0;
        memset(buf_x3, 0, sizeof buf_x3);
        backlight_width_en = 0;
        encoder_state = 0x70;
        start_angk_cal = 1;
        break;
    }
    return ENCODER_CAL_OK;
}
