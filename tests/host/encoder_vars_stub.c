/*
 * Определения переменных, которые в прошивке живут в Src/main.c.
 * Хостовые тесты (calib_test.c, snapshot_test.c, rtu_test.c) собираются без
 * main.c — HAL, регистры и тактирование им не нужны, но Linker'у нужны сами
 * объекты. Значения по умолчанию повторяют состояние после init_vars() и
 * объявлений main.c; тест, которому нужен другой мир, дописывает его сам.
 *
 * Типы обязаны совпадать с Inc/encoder_vars.h и Src/main.c — расхождение
 * поймает tests/encoder_vars_test.py.
 */
#include <stdint.h>
#include "encoder_vars.h"

unsigned char encoder_state = 0;
int start_calibrate = 0;
int auto_cal = 0;

int pix_rdy_num1 = 0;
int pix_rdy_num2 = 0;
float ang_tab[BIT_TAB_SIZE];
float pix_dif_tab1[BIT_TAB_SIZE];
float pix_dif_tab2[BIT_TAB_SIZE];
int pix_dif_num_avg = 5;

float offset = 68;
int offset_avg_num = 3;
int start_offset_cal = 0;
int offset_phase = 0;
int offset_cur = 0;
float offset_minmax[ENCODER_PIXELS];
int offset_found = 0;
int offset_start = 63;
int offset_end = 65;

int start_angk_cal = 0;
int anglek_phase = 0;
int anglek_cur = 0;
float buf_k[ENCODER_PIXELS];
float buf_x3[ENCODER_PIXELS];
float pix_min_max = 0;

float min_max = 0;
int avg_minmax_num = 0;
int rev_left_cnt = 0;
int rev_right_cnt = 0;
int rev_en = 0;
unsigned serrcnt1 = 0;
unsigned serrcnt2 = 0;
int16_t sector = 0;
float cur_ang_E = 0;
uint8_t errorflag = 0;
uint32_t cycles_max = 0;
float Temperature = 25.0f;
volatile uint16_t backlight_width_ticks = 2500;
uint8_t backlight_width_en = 1;

uint8_t ang_tab_rdy = 0;
uint8_t buf_k_rdy = 0;
uint8_t offset_rdy = 0;
uint8_t lasdac_rdy = 0;
uint8_t i2c3_tx_wp = 1;
