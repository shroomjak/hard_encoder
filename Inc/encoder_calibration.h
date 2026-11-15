/* Командный интерфейс и диспетчер автоматов калибровки STM32.
 * Не зависит от SPI, HAL или Modbus. */
#ifndef ENCODER_CALIBRATION_H
#define ENCODER_CALIBRATION_H
#include <stdint.h>

typedef enum {
    ENCODER_CAL_OK,
    ENCODER_CAL_INVALID,
    ENCODER_CAL_BUSY
} encoder_cal_result_t;

/* 0x10 STOP, 0x11/0x12 ang_tab (проходы 1/2), 0x13 offset, 0x14 buf_k.
 * OK означает принятие команды, НЕ завершение алгоритма/записи EEPROM.
 * INVALID/BUSY не меняют состояние. Только последовательный вызов из main loop. */
encoder_cal_result_t encoder_calibration_command(uint16_t command);

/* Единственная точка вызова шага калибровки из суперцикла main() на достоверном
 * кадре (errorflag == 0). Гарантирует взаимное исключение автоматов angk_cal(),
 * offset_cal() и angtab_cal(); при обнаружении коллизии флагов (одновременной
 * активации двух и более процедур, например через отладчик SWD) принудительно
 * сбрасывает все автоматы в безопасное состояние STOP (0x10). */
void encoder_calibration_step(void);

#endif
