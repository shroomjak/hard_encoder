/* Definitions matching the existing main.c; algorithms/EEPROM are not simulated. */
#include <stdint.h>
int start_calibrate, auto_cal, start_offset_cal, offset_phase;
int start_angk_cal, anglek_phase, offset_cur, offset_start = 63, offset_end = 65;
int avg_minmax_num, old_sector;
float offset_minmax[128], buf_x3[128];
unsigned char encoder_state;
uint8_t backlight_width_en = 1, i2c3_tx_wp = 1;
