#ifndef SNAPSHOT_REGISTERS_H
#define SNAPSHOT_REGISTERS_H
#include <stdint.h>

/* Адреса входных регистров FC04 на линии отсчитываются от нуля.
 * Угол — uint32 в микроградусах (1° = 1000000), регистры 2–3.
 * Регистры 9–10 (SNAP_TIME_HI/LO) хранят знаковый int32 возраст экспозиции
 * в мкс (t_SNAP_rx - t_exp_mid по DWT 216 МГц) при вызове snapshot_publish_timed(),
 * либо локальный tick_ms при вызове совместимой функции snapshot_publish().
 * У 32-битных значений первым идёт старший 16-битный регистр. */
enum { SNAP_SEQ, SNAP_READY, SNAP_ANGLE_HI, SNAP_ANGLE_LO, SNAP_SECTOR,
       SNAP_ERROR, SNAP_ENCODER_STATE, SNAP_SAMPLE_HI, SNAP_SAMPLE_LO,
       SNAP_TIME_HI, SNAP_TIME_LO, SNAP_REG_COUNT };

/* Совместимая публикация кадра с миллисекундной меткой tick_ms. */
void snapshot_publish(float angle_deg, uint16_t sector, uint8_t error,
                      uint8_t encoder_state, uint32_t tick_ms);

/* Детерминированная публикация кадра с тактовой меткой середины оптической
 * экспозиции exp_cyccnt (свободно бегущий счётчик DWT->CYCCNT, 216 МГц).
 * Не блокирует прерывания (двойная буферизация latest_slots[2]). */
void snapshot_publish_timed(float angle_deg, uint16_t sector, uint8_t error,
                            uint8_t encoder_state, uint32_t exp_cyccnt);

/* Зафиксировать тактовую метку DWT последнего байта принятого RTU-кадра
 * (вызывается из обработчика таймера t3.5 перед разбором кадра). */
void snapshot_set_rx_cyccnt(uint32_t rx_cyccnt);

/* Явная фиксация снимка с известной меткой приёма SNAP и частотой таймера. */
void snapshot_latch_timed(uint16_t seq, uint32_t snap_rx_cyccnt, uint32_t ticks_per_us);

/* Запись FC06 в holding-регистр 0 (или быстрый перехват в ISR t3.5):
 * зафиксировать последний готовый кадр с расчётом точного возраста age_us. */
void snapshot_latch(uint16_t seq);

/* Прочитать один регистр ранее зафиксированного снимка. */
uint16_t snapshot_register(uint16_t address);

#endif
