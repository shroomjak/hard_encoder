#ifndef ENCODER_VARS_H
#define ENCODER_VARS_H
/*
 * Мост к глобальным переменным самокалибровки, которые ОПРЕДЕЛЕНЫ в Src/main.c.
 *
 * Смысл заголовка один: внешний интерфейс (Src/modbus_calib.c) должен трогать
 * ровно те же объекты, что трогает отладчик в окне Live Watch, и при этом сам
 * Src/main.c оставаться нетронутым. Поэтому main.c сюда ничего не включает,
 * а мы объявляем эти имена extern'ами.
 *
 * Объявления обязаны совпадать с определениями в main.c по имени, базовому
 * типу и размеру массива (128 и BIT_TAB_SIZE считаются равными, см. тесты).
 * Расхождение — это не предупреждение компилятора (единицы translation
 * разнесены), а скрытая ошибка, поэтому согласованность проверяет
 * tests/encoder_vars_test.py, запускаемый вместе с остальными тестами.
 *
 * Инклюдить <stdint.h> не нужно: типы uint*_t приходят из main.h/HAL.
 */

#define BIT_TAB_SIZE 144   /* число секторов диска = длина ang_tab, main.c */
#define ENCODER_PIXELS 128 /* число пикселей TSL1401CL = длина buf_k       */

/* --- общий ход автоматов ------------------------------------------------- */
extern unsigned char encoder_state;   /* код этапа, см. docs §8                  */
extern int start_calibrate;           /* 1 = ang_tab проход 1, 2 = проход 2      */
extern int auto_cal;                  /* то же, но уже прочитанное angtab_cal()   */

/* --- angtab_cal: таблица углов секторов ---------------------------------- */
extern int pix_rdy_num1;              /* прогресс прохода 1, 0…144               */
extern int pix_rdy_num2;              /* прогресс прохода 2, 0…144               */
extern float ang_tab[BIT_TAB_SIZE];    /* таблица углов секторов, градусы         */
extern float pix_dif_tab1[BIT_TAB_SIZE]; /* средние ширины секторов, проход 1     */
extern float pix_dif_tab2[BIT_TAB_SIZE]; /* средние ширины секторов, проход 2     */
extern int pix_dif_num_avg;            /* глубина усреднения на сектор            */

/* --- offset_cal: точка съёма угла ---------------------------------------- */
extern float offset;                   /* выборка относительно репера, пиксели    */
extern int offset_avg_num;             /* оборотов на одну пробу                  */
extern int start_offset_cal;           /* запрос калибровки offset                */
extern int offset_phase;               /* фаза автомата (0…5, 100/101 финал)     */
extern int offset_cur;                 /* текущая проба                           */
extern float offset_minmax[ENCODER_PIXELS]; /* метрика по каждой пробе           */
extern int offset_found;               /* итог: лучшая проба                      */
extern int offset_start;               /* начало перебора                         */
extern int offset_end;                 /* конец перебора                           */

/* --- angk_cal: чувствительность пикселей --------------------------------- */
extern int start_angk_cal;             /* запрос калибровки buf_k                 */
extern int anglek_phase;               /* фаза автомата (0…8, 100/101 финал)     */
extern int anglek_cur;                 /* номер кандидата усиления                */
extern float buf_k[ENCODER_PIXELS];    /* поправочные коэффициенты пикселей       */
extern float buf_x3[ENCODER_PIXELS];   /* пиковые значения яркости по пикселям    */
extern float pix_min_max;              /* разброс пиков, отсчёты АЦП              */

/* --- метрика качества и счётчики ----------------------------------------- */
extern float min_max;                  /* размах остатка угла за оборот, градусы  */
extern int avg_minmax_num;             /* сколько оборотов вошло в текущую пробу  */
extern int rev_left_cnt;               /* переходы через 0° «влево»               */
extern int rev_right_cnt;              /* переходы через 0° «вправо»              */
extern int rev_en;                     /* детектор перехода взведён               */
extern unsigned serrcnt1;              /* неисправимые ошибки декодирования       */
extern unsigned serrcnt2;              /* сработавшие побитовые коррекции         */
extern int16_t sector;                 /* текущий сектор                          */
extern float cur_ang_E;                /* достоверный угол, градусы               */
extern uint8_t errorflag;              /* кадр бракован                           */
extern uint32_t cycles_max;            /* время обработки кадра в тактах          */
extern float Temperature;              /* температура кристалла, °C               */
extern volatile uint16_t backlight_width_ticks; /* экспозиция подсветки, тики    */
extern uint8_t backlight_width_en;     /* 1 = АРУ подсветки разрешена             */

/* --- признаки принятых из EEPROM блоков и состояние записи ---------------- */
extern uint8_t ang_tab_rdy;            /* ang_tab загружена и CRC16 сошлась       */
extern uint8_t buf_k_rdy;              /* buf_k загружена                         */
extern uint8_t offset_rdy;             /* offset загружен                         */
extern uint8_t lasdac_rdy;             /* lasdac загружен                         */
extern uint8_t i2c3_tx_wp;             /* 0 = запись в EEPROM ещё не завершена    */

#endif /* ENCODER_VARS_H */
