/*
 * Заглушки глобальных переменных калибровки для хостовых тестов (UTF-8).
 * Не самостоятельный тест: подключаются run_snapshot_test.sh,
 * run_calibration_test.sh и run_rtu_test.sh вместо базового Src/main.c.
 * Имена и типы совпадают с main.c, поэтому можно линковать настоящий
 * modbus_registers.c без STM32/HAL. Изменение флагов проверяет сам тест;
 * ни вращение, ни работа алгоритмов, ни запись EEPROM здесь не моделируются.
 */
#include <stdint.h>
int start_calibrate, auto_cal, start_offset_cal, offset_phase;
int start_angk_cal, anglek_phase, offset_cur, offset_start = 63, offset_end = 65;
int avg_minmax_num, old_sector;
float offset_minmax[128], buf_x3[128];
unsigned char encoder_state;
uint32_t cycles_max;
uint8_t backlight_width_en = 1, i2c3_tx_wp = 1;
