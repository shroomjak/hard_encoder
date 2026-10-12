#ifndef ENCODER_VARS_H
#define ENCODER_VARS_H
/*
 * Переменные самокалибровки, определённые в Src/main.c.
 *
 * Src/main.c этот заголовок не включает, поэтому объявление здесь и определение
 * там компилятор не сверит. Расхождение поймает tests/encoder_vars_test.py —
 * его надо запускать после правки имён, типов или размеров в main.c.
 * main.c сохранён в CP1251, новые файлы — в UTF-8 (.editorconfig).
 */
#include <stdint.h>

#define BIT_TAB_SIZE   144 /* секторов диска; длина ang_tab — main.c  */
#define ENCODER_PIXELS 128 /* пикселей линейки; длина buf_k — main.c  */

/* запуск и ход автоматов */
extern unsigned char encoder_state;    /* код этапа калибровки                    */
extern int start_calibrate;            /* 1 — проход 1 ang_tab, 2 — проход 2      */
extern int auto_cal;                   /* то же, прочитанное angtab_cal()          */
extern int start_offset_cal;           /* запуск offset_cal                         */
extern int start_angk_cal;             /* запуск angk_cal                           */
extern uint32_t cycles_max;            /* снятый автоматом счётчик времени кадра   */
extern uint8_t backlight_width_en;     /* 1 — АРУ подсветки разрешена              */

/* ang_tab */
extern int pix_rdy_num1;               /* прогресс прохода 1, 0…144                */
extern int pix_rdy_num2;               /* прогресс прохода 2, 0…144                */
extern float ang_tab[BIT_TAB_SIZE];    /* углы секторов, градусы                   */

/* offset */
extern float offset;                   /* точка съёма угла, пиксели                */
extern int offset_phase;               /* фаза offset_cal                          */
extern int offset_cur;                 /* текущая проба                            */
extern int offset_found;               /* итог перебора                            */
extern int offset_start;               /* с какой пробы начинается перебор         */
extern float offset_minmax[ENCODER_PIXELS]; /* метрика по пробам                   */

/* buf_k */
extern float buf_k[ENCODER_PIXELS];    /* коэффициенты пикселей                    */
extern float buf_x3[ENCODER_PIXELS];   /* пиковая яркость по пикселям              */
extern int anglek_phase;               /* фаза angk_cal                             */
extern float pix_min_max;              /* разброс пиков, отсчёты АЦП               */

/* метрика и счётчики */
extern float min_max;                  /* размах остатка угла за оборот, градусы   */
extern int avg_minmax_num;             /* оборотов учтено в текущей пробе          */
extern int rev_right_cnt;              /* оборотов «вправо»                        */
extern int rev_left_cnt;               /* оборотов «влево»                         */
extern unsigned serrcnt1;              /* кадров с неисправимой ошибкой            */
extern float cur_ang_E;                /* текущий достоверный угол, градусы         */

/* EEPROM */
extern uint8_t i2c3_tx_wp;             /* 1 —EEPROM защищён, 0 — запись идёт */
extern uint8_t ang_tab_rdy;            /* 1 — блок ang_tab принят из EEPROM         */
extern uint8_t buf_k_rdy;              /* 1 — блок buf_k принят из EEPROM           */
extern uint8_t offset_rdy;             /* 1 — offset принят из EEPROM               */

#endif /* ENCODER_VARS_H */
