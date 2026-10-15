/*
 * Две копии результата декодирования угла для одной головки энкодера.
 *
 * latest  — последний ПОЛНОСТЬЮ обработанный кадр фотолинейки, обновляется
 *           функцией snapshot_publish() в Src/main.c после calc_sector(),
 *           err_corr() и, если нет ошибок, calc_ang(). Это НЕ DMA-буфер.
 * frozen  — независимая копия latest на момент обработки команды SNAP;
 *           до следующего SNAP именно frozen видят STATUS и READ.
 *
 * Публикация, фиксация и чтение вызываются в одном контексте основного
 * цикла (FreeModbus обслуживается eMBPoll()), поэтому memcpy не требуется
 * защищать от прерывания АЦП. ADC IRQ меняет buf_x0/adc_rdy, но не эти
 * структуры. Фиксация происходит при обработке broadcast, а не строго
 * одновременно с экспозицией нескольких головок: см. docs/modbus_rtu.md.
 */
#include "snapshot_registers.h"
#include <string.h>

/* Микроградусы позволяют передавать угол без двусмысленного формата float.
 * sample и tick_ms — счётчик обработанных кадров и локальный HAL_GetTick().
 * valid не передаётся напрямую: он преобразуется в frozen_ready.
 * error/encoder_state сохраняются даже у неудачного кадра для диагностики. */
typedef struct {
    uint32_t angle_udeg, sample, tick_ms;
    uint16_t sector;
    uint8_t error, encoder_state, valid;
} frame_t;
static frame_t latest, frozen;
static uint16_t frozen_seq;
static uint8_t frozen_ready;

/*
 * Вызов один раз после каждого обработанного кадра АЦП.
 * angle_deg = cur_ang_E, sector/error/encoder_state из существующего
 * алгоритма; tick_ms = HAL_GetTick(). При ошибке декодирования в main.c
 * cur_ang_E сохраняет старый угол — запрещаем считать его новым валидным
 * снимком. NaN также не проходит сравнения с границами [0,360).
 * Поле angle_udeg при невалидном кадре может остаться прежним: master
 * обязан сперва проверить ready, а не интерпретировать такой угол.
 */
void snapshot_publish(float angle_deg, uint16_t sector, uint8_t error,
                      uint8_t encoder_state, uint32_t tick_ms)
{
    latest.valid = !error && sector < 144 &&
                   (angle_deg >= 0.0f) && (angle_deg < 360.0f);
    /* Умножаем в double: float на больших углах теряет единицы
     * микроградусов уже при масштабировании. Точность исходного angle_deg
     * остаётся float; формат передачи сам по себе её не увеличивает. */
    if (latest.valid)
        latest.angle_udeg = (uint32_t)((double)angle_deg * 1000000.0 + 0.5);
    latest.sector = sector;
    latest.error = error;
    latest.encoder_state = encoder_state;
    latest.tick_ms = tick_ms;
    latest.sample++;
}

/*
 * SNAP(seq): скопировать состояние одного кадра, включая метаданные.
 * Если для этого seq уже зафиксирован валидный кадр, повтор вещательной
 * команды не заменяет его более новым: STATUS и READ останутся согласованы.
 * При отсутствии или ошибке кадра сохраняем seq, но выдаём ready=0;
 * повтор того же seq после появления корректного кадра может исправить
 * неудачную фиксацию. Broadcast не имеет ответной квитанции Modbus.
 */
void snapshot_latch(uint16_t seq)
{
    if (frozen_ready && frozen_seq == seq) return;
    memcpy(&frozen, &latest, sizeof frozen);
    frozen_seq = seq;
    frozen_ready = frozen.valid && frozen.sample != 0;
}

/*
 * Вернуть один регистр FC04 по адресу НА ПРОВОДЕ (нумерация с нуля).
 * Слова 32-битных величин упорядочены как старшее -> младшее; раскладка
 * каждого слова на два байта выполняется в modbus_registers.c.
 * Допустимые адреса заранее проверяет eMBRegInputCB().
 */
uint16_t snapshot_register(uint16_t address)
{
    switch (address) {
    case SNAP_SEQ: return frozen_seq;               /* номер SNAP */
    case SNAP_READY: return frozen_ready;           /* можно ли принимать ACK */
    case SNAP_ANGLE_HI: return (uint16_t)(frozen.angle_udeg >> 16);
    case SNAP_ANGLE_LO: return (uint16_t)frozen.angle_udeg;
    case SNAP_SECTOR: return frozen.sector;
    case SNAP_ERROR: return frozen.error;
    case SNAP_ENCODER_STATE: return frozen.encoder_state;
    case SNAP_SAMPLE_HI: return (uint16_t)(frozen.sample >> 16);
    case SNAP_SAMPLE_LO: return (uint16_t)frozen.sample;
    case SNAP_TIME_HI: return (uint16_t)(frozen.tick_ms >> 16);
    case SNAP_TIME_LO: return (uint16_t)frozen.tick_ms;
    default: return 0;
    }
}
