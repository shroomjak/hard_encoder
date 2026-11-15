/*
 * Заглушки глобальных переменных и процедур калибровки для хостовых тестов (UTF-8).
 * Подключаются run_snapshot_test.sh, run_calibration_test.sh и run_rtu_test.sh
 * вместо платформозависимого Src/main.c.
 */
#include <stdint.h>

int start_calibrate, auto_cal, start_offset_cal, offset_phase;
int start_angk_cal, anglek_phase, offset_cur, offset_start = 63, offset_end = 65;
int avg_minmax_num, old_sector;
float offset_minmax[128], buf_x3[128];
unsigned char encoder_state;
uint32_t cycles_max;
uint8_t backlight_width_en = 1, i2c3_tx_wp = 1;

unsigned stub_angk_calls, stub_offset_calls, stub_angtab_calls;

void angk_cal(void);
void offset_cal(void);
void angtab_cal(void);

void angk_cal(void) { ++stub_angk_calls; }
void offset_cal(void) { ++stub_offset_calls; }
void angtab_cal(void) { ++stub_angtab_calls; }
