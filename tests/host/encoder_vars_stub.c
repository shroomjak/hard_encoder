/*
 * Определения переменных, которые в прошивке живут в Src/main.c: хостовые
 * тесты собираются без него. Значения по умолчанию повторяют состояние после
 * init_vars(). Тестам, которым нужен другой мир, достаточно дописать своё.
 *
 * Типы обязаны совпадать с Inc/encoder_vars.h и Src/main.c — это проверяет
 * tests/encoder_vars_test.py.
 */
#include <stdint.h>
#include "encoder_vars.h"

unsigned char encoder_state = 0;
int start_calibrate = 0;
int auto_cal = 0;
int start_offset_cal = 0;
int start_angk_cal = 0;
uint32_t cycles_max = 0;
uint8_t backlight_width_en = 1;

int pix_rdy_num1 = 0;
int pix_rdy_num2 = 0;
float ang_tab[BIT_TAB_SIZE];

float offset = 68;
int offset_phase = 0;
int offset_cur = 0;
int offset_found = 0;
int offset_start = 63;
float offset_minmax[ENCODER_PIXELS];

float buf_k[ENCODER_PIXELS];
float buf_x3[ENCODER_PIXELS];
int anglek_phase = 0;
float pix_min_max = 0;

float min_max = 0;
int avg_minmax_num = 0;
int rev_right_cnt = 0;
int rev_left_cnt = 0;
unsigned serrcnt1 = 0;
float cur_ang_E = 0;

uint8_t i2c3_tx_wp = 1;
uint8_t ang_tab_rdy = 0;
uint8_t buf_k_rdy = 0;
uint8_t offset_rdy = 0;
