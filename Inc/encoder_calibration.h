/* Командный интерфейс автоматов STM32. Не зависит от SPI, HAL или Modbus. */
#ifndef ENCODER_CALIBRATION_H
#define ENCODER_CALIBRATION_H
#include <stdint.h>

typedef enum {
    ENCODER_CAL_OK,
    ENCODER_CAL_INVALID,
    ENCODER_CAL_BUSY
} encoder_cal_result_t;

/* 0x10 STOP, 0x11/12 ang_tab, 0x13 offset, 0x14 buf_k.
 * OK означает принятие команды, НЕ завершение алгоритма/записи EEPROM.
 * INVALID/BUSY не меняют состояние. Только последовательный вызов из main loop. */
encoder_cal_result_t encoder_calibration_command(uint16_t command);
#endif
