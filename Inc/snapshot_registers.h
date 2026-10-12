#ifndef SNAPSHOT_REGISTERS_H
#define SNAPSHOT_REGISTERS_H
#include <stdint.h>
/* Адреса входных регистров FC04 на линии отсчитываются от нуля.
 * Угол — uint32 в микроградусах (1° = 1000000), регистры 2–3.
 * У 32-битных значений первым идёт старший 16-битный регистр. */
enum { SNAP_SEQ, SNAP_READY, SNAP_ANGLE_HI, SNAP_ANGLE_LO, SNAP_SECTOR,
       SNAP_ERROR, SNAP_ENCODER_STATE, SNAP_SAMPLE_HI, SNAP_SAMPLE_LO,
       SNAP_TIME_HI, SNAP_TIME_LO, SNAP_REG_COUNT };
/* Вызывать после обработки каждого кадра АЦП в main(), но не из IRQ DMA. */
void snapshot_publish(float angle_deg, uint16_t sector, uint8_t error,
                      uint8_t encoder_state, uint32_t tick_ms);
/* Запись FC06 в holding-регистр 0: зафиксировать последний готовый кадр. */
void snapshot_latch(uint16_t seq);
/* Прочитать один регистр ранее зафиксированного снимка. */
uint16_t snapshot_register(uint16_t address);
/* Счётчик обработанных кадров АЦП на текущий момент (не счётчик снимка):
 * нужен живому блоку калибровки, чтобы отличать «прогресс встал» от
 * «кадров вообще нет». См. Src/modbus_calib.c и docs/calibration_modbus.md. */
uint32_t snapshot_publish_count(void);
#endif
