/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  *
  * ПРОЕКТ: считывающая головка абсолютного оптического датчика угла (энкодера).
  * МК: STM32F722RETx (Cortex-M7, 216 МГц), сборка IAR EWARM (каталог EWARM).
  *
  * ФИЗИКА. На валу стоит кодовый диск. Светодиод (подсветка, PA15) просвечивает
  * диск, тень падает на ЛИНЕЙНЫЙ ФОТОПРИЁМНИК TSL1401CL (128 пикселей, шаг 63.5 мкм).
  * Диск разбит на BIT_TAB_SIZE = 144 секторов (360/144 = 2.5 град на сектор).
  * Каждый сектор несёт: РЕПЕР (узкую светлую метку - "startpixel") и 9-битный
  * код сектора. Два соседних репера видны на линейке одновременно и отстоят
  * примерно на SECTOR_SIZE = 51 пиксель.
  *
  * КОНВЕЙЕР ОБРАБОТКИ (один кадр линейки = одна итерация while(1) в main()):
  *   DMA2_Stream3_IRQHandler() -> buf_x0[128]   (сырой кадр АЦП)
  *   copy_data()        -> buf_x1[128]          (вычет тёмнового уровня)
  *   find_startpixel()  -> startpixel1/2        (корреляционный поиск реперов)
  *   mes_piks() x2      -> s_l1, s_l2           (субпиксельные координаты реперов)
  *   find_datablock()   -> pix[9][2]            (выборка 9 бит кода между реперами)
  *   calc_sector()      -> data_byte, rem_sec   (декодирование номера сектора)
  *   err_corr()         -> sector, errorflag    (контроль и коррекция ошибок)
  *   calc_ang()         -> cur_ang_X            (итоговый угол в градусах)
  *   find_avg()/offset_cal()/angk_cal()/angtab_cal() - самокалибровки
  *
  * ТЕРМИНЫ (подробно - в docs/main_c_overview.md и docs/angle_math_and_calibration.md):
  *   ADC/АЦП, DMA, DAC/ЦАП, I2C, EEPROM, GPIO, EXTI, NVIC, DWT,
  *   HAL/LL - см. словарь терминов в docs/main_c_overview.md.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "mb.h"
#include "modbus_board.h"
#include "snapshot_registers.h"
#include "math.h"
#include "arm_math.h"
#include  <stdio.h>
#include  <stdarg.h>
#include  <string.h>



/* Private includes ----------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/





/* Private define ------------------------------------------------------------*/



#define M_PI ((float)3.141592653589793)
#define ABS(X)	((X) > 0 ? (X) : -(X))  


/* ---- Геометрия кодового диска и рабочие уровни сигнала ------------------ */

/* Целевая "полка" яркости самого светлого пикселя в отсчётах 12-битного АЦП
   (0..4095). Используется в dac_ctrl() как уставка АРУ подсветки.            */
#define MAX_LEVEL_PIX           3700
/* Тёмновой (пьедестальный) уровень АЦП: вычитается из каждого пикселя
   в copy_data(), чтобы нулю сигнала соответствовал ноль кода.                */
#define ADC_OFFSET              600

/* Число секторов кодового диска. 360 град / 144 = 2.5 град номинально.
   Размер таблиц ang_tab[], bit_tab[] и всех калибровочных массивов.          */
#define BIT_TAB_SIZE            144
/* Номинальное расстояние между двумя соседними реперами в ПИКСЕЛЯХ линейки.
   Используется, чтобы по найденному реперу предсказать положение второго.    */
#define SECTOR_SIZE             51
/* На сколько битовых ячеек (включая репер и защитные зоны) разбит сектор.
   pix_size = (s_l2 - s_l1) / BITS_PER_SECTOR - ширина одного бита в пикселях.*/
#define BITS_PER_SECTOR         15

/* ---- Тайминги управления фотолинейкой TSL1401CL ------------------------- */

/* Частота тактирования таймеров шины APB1. SYSCLK=216 МГц, APB1 presc = /4 =>
   54 МГц на периферию, но таймеры APB1 тактируются удвоенной частотой = 108 МГц.
   Это множитель "тик таймера -> микросекунда" для TIM4.                      */
#define APB1_TIMER_CLOCK_HZ             108000000UL
/* tqt (qualification time) по даташиту TSL1401CL: минимальная пауза, на которую
   надо остановить T-CLK перед выдачей импульса SI (старт нового кадра).      */
#define TSL1401_TQT_US                  20UL
#define TSL1401_TQT_TIM_TICKS           (APB1_TIMER_CLOCK_HZ / 1000000UL * TSL1401_TQT_US)

/* ---- Окно корреляционного поиска репера --------------------------------- */

/* find_startpixel() перебирает i от STARTPIXEL_MIN до STARTPIXEL_MAX-1.
   Верхняя граница выбрана так, чтобы шаблон, который читает buf_x1[i+69],
   не вышел за массив из 128 пикселей (57+69 = 126 < 128).                    */
#define STARTPIXEL_MIN          6
#define STARTPIXEL_MAX          57

/* Пиксель-"водораздел": если найденный репер правее CENTER_PIXEL, он считается
   ПРАВЫМ репером пары, иначе - ЛЕВЫМ. Нужно, чтобы однозначно достроить
   положение второго репера как startpixel +/- SECTOR_SIZE.                   */
#define CENTER_PIXEL            60




#ifndef M_PI_4
#define M_PI_4 (3.1415926535897932384626433832795/4.0)
#endif
/* Коэффициенты полиномиальной аппроксимации arctan(x) на отрезке [-1, 1]
   видa atan(x) ~ ((A*x^2 + B)*x^2 + C)*x, где C выбран так, чтобы atan(1)=pi/4.
   Используются в Fast2ArcTan(). ВНИМАНИЕ: в текущей версии прошивки
   Fast2ArcTan() нигде не вызывается (осталась от альтернативного алгоритма).  */
#define A 0.0776509570923569
#define B -0.287434475393028
#define C (M_PI_4 - A - B)










/*
  * @brief I2C devices settings
  */
/* Timing register value is computed with the STM32CubeMX Tool,
  * Fast Mode @400kHz with I2CCLK = 216 MHz,
  * rise time = 100ns, fall time = 20ns
  * Timing Value = (uint32_t)0x00A01E5D
  */
#define I2C_TIMING               0x00A01E5D

#define EEPROM_DEVICE           0xA0
#define EEPROM_DEVICE_PAGE0     0xA0
#define EEPROM_DEVICE_PAGE1     0xA2
#define EEPROM_DEVICE_PAGE2     0xA4
#define EEPROM_DEVICE_PAGE3     0xA6
#define EEPROM_DEVICE_PAGE4     0xA8
#define EEPROM_DEVICE_PAGE5     0xAA
#define EEPROM_DEVICE_PAGE6     0xAC
#define EEPROM_DEVICE_PAGE7     0xAE







/* ---- Карта энергонезависимой памяти EEPROM (I2C, шина I2C3) -------------
   EEPROM - Electrically Erasable PROM, микросхема памяти, которая сохраняет
   данные при выключении питания. Здесь в ней лежат результаты калибровок.
   Микросхема адресуется "страницами" по 256 байт: EEPROM_DEVICE_PAGEn -
   это разные 7-битные I2C-адреса одного кристалла (A0,A2,A4,... сдвинутые <<1).
   Каждый блок данных сопровождается CRC16 - контрольной суммой, которая
   проверяется в init_vars() при старте; при несовпадении блок игнорируется и
   остаётся значение "по умолчанию", зашитое в код.                           */
#define	eeprom_len		        2048                                    //length of eeprom data


#define	eeprom_ang_tab                  0x00		                        //eeprom address of ang_tab
#define	eeprom_ang_tab_crc	        BIT_TAB_SIZE*4	                        //eeprom address of ang_tab crc16

#define	eeprom_buf_k	                768	                                //eeprom address of buf_k
#define	eeprom_buf_k_crc	        1280	                                //eeprom address of buf_k crc16

#define	eeprom_offset	                1536	                                //eeprom address of offset
#define	eeprom_offset_crc	        1540	                                //eeprom address of offset crc16
#define	eeprom_lasdac	                1544	                                //eeprom address of lasdac
#define	eeprom_lasdac_crc               1546	                                //eeprom address of lasdac crc16







/* ---- DWT: аппаратный счётчик тактов ядра Cortex-M -----------------------
   DWT (Data Watchpoint and Trace) - отладочный блок ядра. Регистр DWT_CYCCNT
   считает такты процессора. Здесь он используется как профилировщик: счётчик
   сбрасывается в начале кадра (DMA2_Stream3_IRQHandler) и читается в конце
   обработки в main(); cycles_max хранит худший случай. При 216 МГц 1 такт =
   4.63 нс, бюджет на кадр = время считывания линейки (~80 мкс ~ 17000 тактов).
   Доступ идёт прямой записью по абсолютным адресам регистров.                */
/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
/*!< DWT Control register */
#define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)
/*!< DWT Cycle Counter register */
#define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))
/*!< Trace enable bit in DEMCR register */
#define KIN1_TRCENA_BIT              (1UL<<24)


/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */
#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT
/*!< Reset cycle counter */
#define KIN1_ResetCycleCounter() \
  KIN1_DWT_CYCCNT = 0
/*!< Enable cycle counter */
#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
/*!< Disable cycle counter */
#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
/*!< Read cycle counter register */
#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

TIM_HandleTypeDef htim3;

DAC_HandleTypeDef hdac;











//CPU cycles measurement
uint32_t cycles; /* number of cycles */

uint32_t cycles_max;


// I2C

I2C_HandleTypeDef I2c3Handle;           // I2C3 handler declaration
uint8_t i2c3_tx_wp=1;
uint8_t i2c3_err;




/* Теневая копия всего содержимого EEPROM в ОЗУ. Читается целиком один раз в
   init_vars(), а при сохранении калибровок обновляется нужный фрагмент и
   записывается обратно по I2C в прерывании (HAL_I2C_Mem_Write_IT).           */
uint8_t eeprom_buf[eeprom_len];                                                 //eeprom buffer

uint16_t eeprom_crc16;                                                          //eeprom temporary crc16
uint16_t eeprom_crc16_ang_tab;                                                  //eeprom ang_tab crc16
uint16_t eeprom_crc16_buf_k;                                                    //eeprom buf_k crc16
uint16_t eeprom_crc16_offset;                                                   //eeprom offset crc16
uint16_t eeprom_crc16_lasdac;                                                   //eeprom lasdac crc16

uint8_t ang_tab_rdy=0;
uint8_t buf_k_rdy=0;
uint8_t offset_rdy=0;
uint8_t lasdac_rdy=0;




/* ---- ADC1: встроенный датчик температуры кристалла ----------------------
   ADC (Analog-to-Digital Converter) - АЦП. ADC1 в непрерывном режиме оцифровывает
   внутренний температурный датчик, результат через DMA кладётся в
   aADCxConvertedData[0]. Пересчёт в градусы - в main(). aligned(32) нужно,
   потому что у Cortex-M7 включён кэш данных (D-Cache), а инвалидация кэша
   работает строками по 32 байта (SCB_InvalidateDCache_by_Addr).              */
//ADC1
__attribute__((aligned(32))) uint16_t aADCxConvertedData[1];
__attribute__((aligned(4))) float Vsense;
__attribute__((aligned(4))) float V25 = 0.76f;
__attribute__((aligned(4))) float Avg_Slope = 2.5f;
__attribute__((aligned(4))) float Temperature;



/* ---- ADC2: оцифровка видеосигнала фотолинейки ---------------------------
   ADC2 запускается ВНЕШНИМ триггером EXTI line 11 (спад на PB11 - это тактовый
   сигнал линейки T-CLK, заведённый обратно в МК). То есть на каждый такт
   линейки делается ровно одно преобразование - жёсткая синхронизация "такт
   линейки <-> отсчёт АЦП" без участия процессора.
   DMA работает в режиме DOUBLE BUFFER: пока заполняется ADC_VAL, прошлый кадр
   можно читать из ADC_VAL2 и наоборот - кадр не "рвётся" посередине.
   132 = 128 рабочих пикселей + служебные отсчёты в начале (первые 2 отбрасываются,
   см. adc_addr = ADC_VAL + 2).                                               */
//ADC2
__attribute__((aligned(32))) uint16_t ADC_VAL[132] = {0};
__attribute__((aligned(32))) uint16_t ADC_VAL2[132] = {0};
uint16_t *adc_addr;
unsigned char adc_rdy;                                                          // ADC frame ready flag (1 = в buf_x0 лежит свежий кадр)

/* ---- Управление яркостью подсветки (АРУ) --------------------------------
   DAC (Digital-to-Analog Converter) - ЦАП. Исторически яркость лазера задавалась
   напряжением с ЦАП (переменная lasdac). В текущей версии ЦАП инициализируется
   нулём и НЕ используется, а экспозиция регулируется ШИРИНОЙ ИМПУЛЬСА подсветки
   на PA15 (backlight_width_ticks, тики TIM4 по 1/108 мкс).
   backlight_width_en = 0 запрещает автоподстройку (нужно на время калибровки
   buf_k, чтобы яркость не "уползала" в процессе измерений).                  */
//DAC
uint8_t backlight_width_en=1;
int max_level = MAX_LEVEL_PIX;
int adc_offset=ADC_OFFSET;
int max_level_pix = MAX_LEVEL_PIX - ADC_OFFSET;
uint16_t lasdac=700;                                                            // Laser DAC setpoint (initial value)
uint32_t lasdac_counter=0;
uint32_t avg_amaxX_summ=0;
uint32_t avg_amaxX_num=1000;
uint32_t avg_max_data;

#define BACKLIGHT_STATE_IDLE   0
#define BACKLIGHT_STATE_DELAY  1
#define BACKLIGHT_STATE_WIDTH  2
#define BACKLIGHT_WIDTH_MIN_TICKS 2
#define BACKLIGHT_WIDTH_MAX_TICKS 9400

volatile uint16_t backlight_delay_ticks = 1220;
volatile uint16_t backlight_width_ticks = 2500;
volatile uint8_t backlight_timer_state = BACKLIGHT_STATE_IDLE;















// See surrounding code
/* ---- Буферы изображения линейки -----------------------------------------
   buf_x0 - сырой кадр, скопированный из DMA-буфера (0..4095, с пьедесталом);
   buf_x1 - кадр после вычитания adc_offset (рабочий для поиска репера и битов);
   buf_x2 - кадр, умноженный на попиксельные коэффициенты buf_k (для mes_piks);
   buf_x3 - "пиковый детектор" по каждому пикселю за время калибровки buf_k;
   buf_x4, buf_x5 - не используются в текущей версии.                         */
uint16_t buf_x0[128]={0};	                                                // Line sensor pixel buffer (DMA, merged halves)
uint16_t max_data=0;
uint16_t min_data=0;
uint16_t buf_x1[128] = {0};

int16_t tmp1_copy;
int16_t tmp2_copy;
float tmp3_k;


uint16_t max_data2;
uint16_t buf_x4[128] = {0};





//calc algo
float halfsum;
float leftx[128];
float rightx[128];
int leftfind;
int rightfind;



float filt_a = 1, filt_b =3, filt_c =3;





//Config
uint32_t config_state;



//Code reader





/* ---- Кодовая таблица секторов -------------------------------------------
   bit_tab[j] - 9-битный код (0..511), физически нанесённый на сектор с номером j.
   Все 144 значения уникальны, поэтому decode: код -> индекс = номер сектора.
   ВАЖНО: минимальное расстояние Хэмминга по ВСЕЙ таблице равно 1, то есть сам
   код НЕ является помехоустойчивым. Защита от ошибок строится иначе - на
   непрерывности номера сектора во времени (см. err_corr()) и на том, что коды
   СОСЕДНИХ секторов отличаются на 7..9 бит, т.е. одиночный сбой не может
   превратить сектор j в сектор j+-1.                                          */
const uint16_t  bit_tab[BIT_TAB_SIZE] = {                                      //code bit image
197	,
314	,
213	,
298	,
85	,
426	,
84	,
427	,
212	,
299	,
214	,
297	,
210	,
301	,
211	,
300	,
83	,
428	,
82	,
429	,
86	,
425	,
87	,
424	,
215	,
296	,
199	,
312	,
198	,
313	,
206	,
307	,
204	,
51	,
460	,
179	,
332	,
163	,
348	,
167	,
344	,
165	,
346	,
164	,
347	,
180	,
331	,
181	,
330	,
53	,
458	,
52	,
459	,
54	,
457	,
50	,
461	,
178	,
333	,
182	,
329	,
166	,
345	,
230	,
281	,
228	,
283	,
229	,
282	,
101	,
410	,
100	,
411	,
116	,
395	,
117	,
394	,
119	,
408	,
103	,
409	,
102	,
413	,
99	,
412	,
227	,
284	,
231	,
280	,
183	,
328	,
55	,
456	,
311	,
200	,
279	,
232	,
277	,
234	,
405	,
106	,
404	,
107	,
276	,
235	,
278	,
233	,
406	,
105	,
402	,
109	,
403	,
108	,
407	,
104	,
471	,
40	,
467	,
44	,
339	,
172	,
338	,
173	,
466	,
45	,
470	,
41	,
468	,
43	,
469	,
42	,
453	,
58	,
325	,
186	,
341	,
170	,
340	,
171	,
342	,
169	,
326	,
185	,
306	,//327	,
//184	,
//455	,
//56	,
//454	,
};











uint8_t t_cor;
float nbit;

int32_t scor[128];
int32_t scor2[128];

int32_t scor_max1;
int32_t scor_max2;

//int32_t scor2[128];

int32_t scor_t1;
int32_t scor_t2;


int32_t cur_cor;

int32_t max1_cor;
uint8_t max1_pos;
int32_t max2_cor;
uint8_t max2_pos;


uint16_t data_byte;

/* startpixel  - целочисленный индекс пикселя, где корреляция с шаблоном репера
                  максимальна (грубая оценка положения репера);
   startpixel1 - ЛЕВЫЙ репер пары, startpixel2 - ПРАВЫЙ (отличаются на SECTOR_SIZE).
   Субпиксельные уточнения этих же точек - в s_l1 и s_l2.                     */
uint16_t startpixel=0;
uint16_t startpixel1;
uint16_t startpixel2;

uint16_t test1pixel;
uint16_t test2pixel;
uint16_t bck_pixel;

/* Результат выборки 9 информационных бит кода сектора.
   pix[i][0] = max_sdata - отсчёт  ("насколько бит далёк от белого")
   pix[i][1] = отсчёт - min_sdata  ("насколько бит далёк от чёрного")
   Решение: бит = 1, если pix[i][1] > pix[i][0]. Разность |pix[i][1]-pix[i][0]|
   служит МЕРОЙ ДОВЕРИЯ к биту: чем она меньше, тем бит ближе к середине шкалы
   и тем вероятнее, что ошибся именно он (используется в calc_sector/bit_err_corr).
   Размер массива (9 строк) должен совпадать с границами циклов в
   find_datablock() и calc_sector(); маска разряда в bit_err_corr() -
   (0x100 >> badbit_pos). См. docs/angle_math_and_calibration.md, п.5.3.       */
int16_t pix[9][2];



float k_one=1.0;
float k_zero=1.0;


uint16_t t_data=0;
uint16_t max_sdata=0;
uint16_t min_sdata=0;
uint16_t max_ldata=0;
uint16_t min_ldata=0;

float left_f;
float right_f;

float sec_size;
float pix_size;



/* ---- Счётчики контроля ошибок (только для диагностики через отладчик) --
   sec_cnt  - сколько кадров уже обработано (первые 50 - "прогрев", проверка
              непрерывности сектора ещё не включена);
   serrcnt2 - сколько раз запускалась побитовая коррекция;
   serrcnt1 - сколько раз коррекция НЕ помогла (неисправимая ошибка кадра);
   errorflag= 1 означает, что угол в этом кадре считать нельзя.               */
//Sector error control
unsigned sec_cnt=0;
unsigned serrcnt=0;
unsigned serrcnt1=0;
unsigned serrcnt2=0;
unsigned serrcnt3=0;
uint8_t errorflag;

int32_t badbit_pos;
int32_t badbit_value;



int16_t lsec=0;
int16_t rsec=0;
int16_t cursec=0;

int16_t sector=0;
int16_t lsector=0;
int16_t rsector=0;

uint8_t rem_sec;




//BR4

float ampl;
float g1, g0;
float delBR4;

int delta;
float delta_cor[200];
float delBR4;

/* Параметры компенсации систематической субпиксельной ошибки алгоритма BR4
   (см. BR4_C): ошибка имеет вид синусоиды с периодом РОВНО 1 пиксель по дробной
   части координаты. k_comp - амплитуда (в пикселях), phase - фазовый сдвиг.  */
float phase=0;//0.785;
float k_comp=0.01;



float tsl_1;
float tsl_2;
int i_s1;
int i_s2;
float s_l1;
float s_l2;
float s_l3;


float t_l=0;
float t_l1=0;
float t_l2=0;


float m_l1;
float m_l2;
float m_l3;

float df;

float kdf=1;

float buf_x2[128] ={0};
unsigned char max_X0 = 3;                                                       // Index of peak in buf_x2 (sub-pixel search)



unsigned long alg1,alg2;



float buf_x3[128] ={0};

float pix_max;
float pix_min;
float pix_min_max;



float pix_min1_val;
int pix_min1_num;
float pix_min2_val;
int pix_min2_num;
float pix_min3_val;
int pix_min3_num;
float pix_min4_val;
int pix_min4_num;


int16_t buf_k_num;









//Angle calculation

float r_angle;
float l_angle;
float angle_dif;
float pix_dif;
float k_pix;


float r_angle2;
float l_angle2;
float angle_dif2;
float pix_dif2;
float k_pix2;



int old_sector=0;
int cur_sector=0;
int cal_sector=0;

float ts_l1;
float ts_l2;
float ts_l3;


float cur_ang_X;

float cur_ang;


float cur_ang_E;



/* ======================= ПЕРЕМЕННЫЕ САМОКАЛИБРОВКИ =======================
   Калибровка таблицы углов секторов ang_tab[] (функция angtab_cal()).
   Идея: ширина каждого сектора A_j измеряется в пикселях (pix_dif = s_l2-s_l1),
   затем углы границ получаются нормировкой накопленной суммы на полный оборот:
        ang_tab[j] = 360 * (A_0 + ... + A_{j-1}) / (A_0 + ... + A_143).
   Делается ДВА прохода - при вращении в одну и в другую сторону (new_ang_tab1 и
   new_ang_tab2), результаты усредняются, чтобы снять направленно-зависимые
   (гистерезисные/динамические) ошибки.

   pix_dif_num_avg - сколько измерений ширины усредняется на каждый сектор;
   start_calibrate - команда запуска: 1 = проход "вправо", 2 = проход "влево";
   auto_cal        - текущий активный проход (0 = калибровка не идёт).        */
//Calibration

int pix_dif_num_avg=5;
int start_calibrate=0;
int auto_cal=0;

/* Проход 1. pix_dif_sum1[j] - сумма измерений ширины сектора j (в пикселях),
   pix_dif_num1[j] - число накопленных измерений, pix_dif_tab1[j] - среднее A_j,
   pix_rdy_tab1[j] - флаг "сектор j измерен", pix_rdy_num1 - СКОЛЬКО секторов уже
   готово (именно эту переменную удобно читать как ПРОГРЕСС калибровки: 0..144),
   sum_Ai1 - сумма всех A_j (нормировочный коэффициент "пиксели на оборот").   */
float pix_dif_tab1[BIT_TAB_SIZE];
float pix_dif_sum1[BIT_TAB_SIZE];
int pix_dif_num1[BIT_TAB_SIZE];
int pix_rdy_tab1[BIT_TAB_SIZE];
int pix_rdy_num1;
float sum_Ai1;

/* Проход 2 - то же самое при вращении в противоположную сторону.            */
float pix_dif_tab2[BIT_TAB_SIZE];
float pix_dif_sum2[BIT_TAB_SIZE];
int pix_dif_num2[BIT_TAB_SIZE];
int pix_rdy_tab2[BIT_TAB_SIZE];
int pix_rdy_num2;
float sum_Ai2;

float new_ang_tab1[BIT_TAB_SIZE];
float new_ang_tab2[BIT_TAB_SIZE];



float new_ang_tab[BIT_TAB_SIZE];





/* ---- Скользящее окно для оценки КАЧЕСТВА измерения (find_avg) -----------
   avg_buf - кольцевой (сдвиговый) буфер последних 30 значений угла.
   avg_cur = (среднее по 30) - (значение в центре окна). При равномерном вращении
   истинный угол линеен по времени, поэтому среднее симметричного окна равно
   центральному отсчёту, и avg_cur - это ОСТАТОК, т.е. оценка ошибки измерения.
   min_max = (макс. остаток за оборот) - (мин. остаток за оборот) - скалярная
   ЦЕЛЕВАЯ ФУНКЦИЯ, которую минимизируют offset_cal() и angk_cal().           */
float avg_buf[30];
float sum_avg_buf=0;
float avg_ang_X;
float avg_cur;

float min_buf[20];
float sum_avg_min;

float max_buf[20];
float sum_avg_max;


float cur_min;
float avg_min;
float cur_max;
float avg_max;
int avg_minmax_num;


float min_max=0;


int rev_left_cnt=0;
int rev_right_cnt=0;
int rev_en=0;


/* ---- Калибровка offset: поиск "точки съёма" угла на линейке -------------
   offset - координата (в пикселях) той точки линейки, к которой привязывается
   выдаваемый угол; физически это проекция оптической оси. Калибровка - прямой
   перебор целых значений offset с оценкой min_max на каждом.
   offset_avg_num - сколько оборотов усредняется на одну пробу;
   start_offset_cal - флаг запуска, offset_phase - номер шага конечного автомата;
   offset_start/offset_end - начальное окно перебора (63..65),
   offset_snum - насколько окно расширяется влево/вправо, если минимум на краю. */
int offset_avg_num=3;
int start_offset_cal=0;
int offset_phase=0;
int offset_cur=0;
float offset_min[128];
float offset_max[128];
float offset_minmax[128];
float offset_tmp;
int offset_found;
int offset_start=63;
int offset_end=65;
int offset_snum=7;




/* ---- Калибровка buf_k: попиксельная неравномерность чувствительности -----
   У фотолинейки каждый пиксель имеет свою чувствительность (PRNU), плюс есть
   виньетирование и грязь на оптике. buf_k[i] - корректирующий множитель для
   пикселя i: buf_x2[i] = buf_x1[i] * buf_k[i]. Без него субпиксельный алгоритм
   BR4 даёт смещение, зависящее от того, на какие пиксели попал репер.        */
int start_angk_cal=0;
int anglek_phase=0;
int anglek_cur=0;
float anglek_minmax[4];
float anglek_data[4];
float anglek_tmp;
int anglek_found;
int anglek_start=0;
int anglek_end=2;
int bufk_num_start=40;
int bufk_num_end=100;




long s_l1_tab[128];
long s_l2_tab[128];







/* ---- Код состояния головки --------------------------------------------
   Этапы калибровок (0x00..0x80). Раньше код уходил наружу в каждом кадре
   SPI2; внешний SPI-канал удалён, переменная сохранена как внутренний
   статус - пригодится для телеметрии по RS485/ModBus.                   */
unsigned char encoder_state=0;









const unsigned char crc_table[256] = {
0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
0x23,0x7D,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35}
;



/**
  * @brief  Calculate CRC8
  */
//byte - очередной байт, для которого считаем контрольныю сумму
//crc8 - байт самой контрольной суммы
void crc_8_step(unsigned char byte, unsigned char *crc8){
*crc8=crc_table[(*crc8^byte)];
}




static  unsigned int crc_16_table[16]=
  {
    0x0000,0xCC01,0xD801,0x1400,0xF001,0x3C00,0x2800,0xE401,
    0xA001,0x6C00,0x7800,0xB401,0x5000,0x9C01,0x8801,0x4400
  };



/**
  * @brief  Calculate CRC16
  * @param  unsigned char data,  unsigned short crc
  * @retval unsigned short crc
  */
/*-----------------------------------------------------------------------------------*/


unsigned short crc_16_step( unsigned char data, unsigned short crc )
{
  unsigned short r;
    
  // compute checksum of lower four bits of data
  r=crc_16_table[crc&0xF];
  crc=(crc>>4)&0x0FFF;
  crc=crc^r^crc_16_table[data&0xF];
  // now compute checksum of upper four bits of data
  r=crc_16_table[crc&0xF];
  crc=(crc>>4)&0x0FFF;
  crc=crc^r^crc_16_table[(data>>4)&0xF];
    
  return(crc);
}


















/**
  * @brief  Calculate ArcTan
  * @param  unsigned char data,  unsigned short crc
  * @retval unsigned short crc
  */
/*-----------------------------------------------------------------------------------*/

double Fast2ArcTan(double x) {
  double xx = x * x;
  return ((A*xx + B)*xx + C)*x;
}





float buf_x5[128];

int buf_x5_num;













/* Попиксельные корректирующие коэффициенты чувствительности (см. angk_cal()).
   Значения по умолчанию перекрываются содержимым EEPROM в init_vars().
   Элементы 0..20 оставлены равными 1.0: края линейки в расчёте не участвуют.  */
float buf_k[128] ={


1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.1211921	,
1.1004225	,
1.08769679	,
1.07526195	,
1.06344223	,
1.05253339	,
1.05057394	,
1.04441702	,
1.04184616	,
1.03801346	,
1.03484106	,
1.02792954	,
1.02699423	,
1.02203441	,
1.01926553	,
1.01529241	,
1.01377249	,
1.01286268	,
1.00984192	,
1.01165223	,
1.00984192	,
1.01104808	,
1.00653982	,
1.00833833	,
1.00713861	,
1.03105974	,
1.00385416	,
1.00147891	,
1.00207162	,
1.00296211	,
1.0	,
1.00177514	,
1.0008868	,
1.00236821	,
1.00118268	,
1.00266504	,
1.00059104	,
1.00594175	,
1.00266504	,
1.00624072	,
1.00474775	,
1.01407611	,
1.00147891	,
1.01437986	,
1.3368864	,
1.03105974	,
1.00385416	,
1.00683916	,
1.02110982	,
1.14481151	,
1.06982624	,
1.01529241	,
1.01498806	,
1.0162065	,
1.01590156	,
1.02357924	,
1.01590156	,
1.0208019	,
1.01407611	,
1.02203441	,
1.0208019	,
1.43394566	,
1.02543914	,
1.01651156	,
1.00803816	,
1.01437986	,
1.07526195	,
1.0122571	,
1.01014316	,
1.02018678	,
1.01590156	,
1.02481842	,
1.01681685	,
1.01681685	,
1.00624072	,
1.0122571	,
1.01559687	,
1.0257498	,
1.03705978	,
1.02357924	,
1.0208019	,
1.0257498	,
1.01987946	,
1.02512872	,
1.0186522	,
1.02419841	,
1.02018678	,
1.03294694	,
1.02296078	,
1.03168797	,
1.02949226	,
1.04088533	,
1.04280877	,
1.06244111	,
1.05746412	,
1.07185817	,
1.07765758	,
1.10509133	,
1.11052799	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,
1.0	,



};




/* Точка съёма угла на линейке (в пикселях, дробная). Значение по умолчанию 68
   перекрывается значением из EEPROM в init_vars(), если его CRC16 сошлась.
   Подбирается автоматически функцией offset_cal() (команда SPI 0x13).        */
float  offset=68;



/* ---- ГЛАВНАЯ КАЛИБРОВОЧНАЯ ТАБЛИЦА -------------------------------------
   ang_tab[j] - фактический угол (град) начала сектора j, измеренный для
   КОНКРЕТНОГО экземпляра диска. Номинал - j*2.5 град, зашитые здесь значения
   отличаются от номинала в пределах примерно +-0.025 град (+-90 угл.сек) -
   это и есть погрешность нанесения кодовой шкалы, которую снимает калибровка.
   Таблица перезаписывается функцией angtab_cal() и сохраняется в EEPROM.     */
float ang_tab[BIT_TAB_SIZE] = {

0.0	,
2.49463558	,
5.00351524	,
7.5037322	,
1.00117226E+1	,
1.25207882E+1	,
1.50264034E+1	,
1.75337448E+1	,
2.00370369E+1	,
2.25362854E+1	,
2.50418587E+1	,
2.7546318E+1	,
3.00544205E+1	,
3.25529785E+1	,
3.50557861E+1	,
3.75509872E+1	,
4.00589523E+1	,
4.25651398E+1	,
4.50658264E+1	,
4.75716782E+1	,
5.00660477E+1	,
5.25731316E+1	,
5.50650177E+1	,
5.75738869E+1	,
6.00736084E+1	,
6.25698395E+1	,
6.50751953E+1	,
6.75713348E+1	,
7.00786133E+1	,
7.257724E+1	,
7.50781937E+1	,
7.75759277E+1	,
8.00710068E+1	,
8.2572403E+1	,
8.50740204E+1	,
8.75724411E+1	,
9.00669556E+1	,
9.2571846E+1	,
9.5065155E+1	,
9.75699615E+1	,
1.00064926E+2	,
1.02569336E+2	,
1.05065658E+2	,
1.07566025E+2	,
1.10060005E+2	,
1.1256147E+2	,
1.150625E+2	,
1.17557854E+2	,
1.20059105E+2	,
1.22558739E+2	,
1.25061943E+2	,
1.27557922E+2	,
1.3006308E+2	,
1.32553604E+2	,
1.35059952E+2	,
1.37549698E+2	,
1.40052002E+2	,
1.42543472E+2	,
1.45038345E+2	,
1.47533722E+2	,
1.5002951E+2	,
1.52528351E+2	,
1.55022842E+2	,
1.57518585E+2	,
1.60013916E+2	,
1.62512878E+2	,
1.65008789E+2	,
1.67506302E+2	,
1.70005524E+2	,
1.72504791E+2	,
1.75007126E+2	,
1.77500122E+2	,
1.80001862E+2	,
1.8249173E+2	,
1.84998917E+2	,
1.87489838E+2	,
1.89998352E+2	,
1.92492142E+2	,
1.94993896E+2	,
1.97494781E+2	,
1.99995193E+2	,
2.02493713E+2	,
2.04999603E+2	,
2.07492706E+2	,
2.09996002E+2	,
2.12498688E+2	,
2.14991638E+2	,
2.1749797E+2	,
2.19991882E+2	,
2.22495056E+2	,
2.24986328E+2	,
2.27489441E+2	,
2.29989288E+2	,
2.32484985E+2	,
2.34979126E+2	,
2.37476868E+2	,
2.39973419E+2	,
2.42468307E+2	,
2.44963257E+2	,
2.4746637E+2	,
2.49960266E+2	,
2.52463776E+2	,
2.54957123E+2	,
2.57450928E+2	,
2.5995935E+2	,
2.62450684E+2	,
2.64964844E+2	,
2.67470398E+2	,
2.69972168E+2	,
2.7248114E+2	,
2.74980743E+2	,
2.77485779E+2	,
2.79981628E+2	,
2.82487305E+2	,
2.84977417E+2	,
2.87483704E+2	,
2.89969788E+2	,
2.92473083E+2	,
2.94962402E+2	,
2.97461487E+2	,
2.99956116E+2	,
3.02504639E+2	,
3.0499292E+2	,
3.07492493E+2	,
3.09986206E+2	,
3.12487793E+2	,
3.1498291E+2	,
3.17485718E+2	,
3.19977112E+2	,
3.22479736E+2	,
3.24978882E+2	,
3.27485443E+2	,
3.29976624E+2	,
3.32476929E+2	,
3.34975555E+2	,
3.37474396E+2	,
3.39975891E+2	,
3.42479187E+2	,
3.44981079E+2	,
3.47480225E+2	,
3.49985107E+2	,
3.5248584E+2	,
3.54991638E+2	,
3.57492248E+2	,



};















/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_DAC_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_I2C3_Init(void);


void copy_data(void);                                                           //copy TAOS data
void find_startpixel(void);                                                     //search for 1-st startpixel
float mes_piks(uint16_t star_pixel);                                            //calculate subpixel coordinates
void find_datablock(void);                                                      //finding data bits
void calc_sector(void);                                                         //calculate sector number
void err_corr(void);                                                            //error control and correction
void calc_ang();                                                                //calculate angle

float INTEGRAL (float *mas, int centor_int);
float KVADRATURA (float *mas, int centor_int);
float CENTR_OF_MASS (float *mas, int centor_int);
float BR4 (float *mas, int centor_int);
float BR6 (float *mas, int centor_int);
float BR8 (float *mas, int centor_int);
float BR4_F (float *mas, int centor_int);
float BR6_F (float *mas, int centor_int);
float BR4_C (float *mas, int centor_int);    
float calc (float *mas, int centor_int);




void init_vars();
static void Backlight_StartFromSI(void);
static void Backlight_StartTimer(uint16_t ticks);
static void DelayTim4Ticks(uint16_t ticks);




/* Private user code ---------------------------------------------------------*/




/**
  * @brief  Шаг 1 конвейера: предобработка кадра фотолинейки.
  *
  * Что делает:
  *   1) вычитает тёмновой уровень adc_offset из каждого пикселя buf_x0 -> buf_x1
  *      (отрицательные значения обрезаются в 0);
  *   2) находит max_data и min_data по кадру - они нужны корреляционному
  *      детектору репера в find_startpixel() как опорные уровни "белого"/"чёрного";
  *   3) если идёт калибровка buf_k (anglek_phase < 4), ведёт ПИКОВЫЙ ДЕТЕКТОР
  *      buf_x3[i] = max(buf_x3[i], buf_x1[i]*buf_k[i]) - максимальную яркость,
  *      которую пиксель i увидел за всё время прокрутки диска.
  *
  * config_state (состояние перемычки на PB8) определяет ОРИЕНТАЦИЮ головки:
  * при config_state==1 кадр переписывается в обратном порядке (127-i), то есть
  * вторая головка, стоящая зеркально, выдаёт угол в той же системе отсчёта.
  *
  * Цикл идёт шагом 2 (две операции за итерацию) - ручное развёртывание цикла
  * ради скорости: функция вызывается ~12000 раз в секунду.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void copy_data(void) {
  max_data=0;
  min_data=4095;
  if (config_state==0) {
    for (unsigned char i=1;i<127;i+=2) {                                        //copy buffer in straight order
      tmp1_copy = buf_x0[i] - adc_offset;
      if (tmp1_copy<0) tmp1_copy = 0;
      tmp3_k=tmp1_copy*buf_k[i];
      if ( (tmp3_k>buf_x3[i]) && (anglek_phase<4) ) buf_x3[i] = tmp3_k; 
      buf_x1[i] = tmp1_copy; 
      if (tmp1_copy>max_data)       max_data = tmp1_copy;
      if (tmp1_copy<min_data)       min_data = tmp1_copy;
      tmp2_copy = buf_x0[i+1] - adc_offset;
      if (tmp2_copy<0) tmp2_copy = 0;
      tmp3_k=tmp2_copy*buf_k[i+1];
      if ( (tmp3_k>buf_x3[i+1]) && (anglek_phase<4) ) buf_x3[i+1] = tmp3_k; 
      buf_x1[i+1] = tmp2_copy; 
      if (tmp2_copy>max_data)     max_data = tmp2_copy;
      if (tmp2_copy<min_data)     min_data = tmp2_copy;
    }
  }
  else {
    for (unsigned char i=1;i<127;i+=2) {                                        //copy buffer in reverse order
      tmp1_copy = buf_x0[i] - adc_offset;
      if (tmp1_copy<0) tmp1_copy = 0;
      tmp3_k=tmp1_copy*buf_k[127-i];
      if ( (tmp3_k>buf_x3[127-i]) && (anglek_phase<4) ) buf_x3[127-i] = tmp3_k; 
      buf_x1[127-i] = tmp1_copy; 
      if (tmp1_copy>max_data)       max_data = tmp1_copy;
      if (tmp1_copy<min_data)       min_data = tmp1_copy;
      tmp2_copy = buf_x0[i+1] - adc_offset;
      if (tmp2_copy<0) tmp2_copy = 0;
      tmp3_k=tmp2_copy*buf_k[126-i];
      if ( (tmp3_k>buf_x3[126-i]) && (anglek_phase<4) ) buf_x3[126-i] = tmp3_k; 
      buf_x1[126-i] = tmp2_copy; 
      if (tmp2_copy>max_data)     max_data = tmp2_copy;
      if (tmp2_copy<min_data)     min_data = tmp2_copy;
    }
  }

}






/**
  * @brief  Шаг 2 конвейера: поиск РЕПЕРА (startpixel) корреляционным методом.
  *
  * МАТЕМАТИКА. Для каждого сдвига i строится скалярная мера совпадения кадра с
  * бинарным шаблоном репера:
  *     scor[i] = SUM(max_data - buf_x1[i+k]) по "тёмным" позициям шаблона
  *             + SUM(buf_x1[i+k] - min_data) по "светлым" позициям шаблона.
  * Шаблон = 10 тёмных пикселей, 3 светлых (сам репер), пропуск, 5 тёмных;
  * тот же шаблон продублирован со сдвигом +SECTOR_SIZE (+51), то есть ищется
  * сразу ПАРА реперов - это резко снижает вероятность ложного срабатывания на
  * случайном сочетании битов кода. Максимум scor[] и есть положение репера.
  *
  * Выбор между двумя лучшими кандидатами идёт не по самому scor, а по
  * "остроте" пика: scor_max = 2*scor[p] - scor[p-1] - scor[p+1] + scor[p];
  * настоящий репер даёт узкий пик, ложный - размытый.
  *
  * Затем по правилу CENTER_PIXEL достраивается второй репер пары:
  * startpixel1 (левый) и startpixel2 (правый) всегда отстоят на SECTOR_SIZE.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void find_startpixel(void) {
        
  //find main startpixel
  max1_cor=0;
  max2_cor=0;
  for (unsigned char i=STARTPIXEL_MIN;i<STARTPIXEL_MAX;i++) {               //build correlation table for startpixel

    scor[i]=(max_data-buf_x1[i+0])+(max_data-buf_x1[i+1])+(max_data-buf_x1[i+2])+(max_data-buf_x1[i+3])
      +(max_data-buf_x1[i+4])+(max_data-buf_x1[i+5])+(max_data-buf_x1[i+6])+(max_data-buf_x1[i+7])
        +(max_data-buf_x1[i+8])+(max_data-buf_x1[i+9])
          +(buf_x1[i+10]-min_data)+(buf_x1[i+11]-min_data)+(buf_x1[i+12]-min_data)
            +(max_data-buf_x1[i+14])+(max_data-buf_x1[i+15])
              +(max_data-buf_x1[i+16])+(max_data-buf_x1[i+17])+(max_data-buf_x1[i+18])

    +(max_data-buf_x1[i+51])+(max_data-buf_x1[i+52])+(max_data-buf_x1[i+53])+(max_data-buf_x1[i+54])
      +(max_data-buf_x1[i+55])+(max_data-buf_x1[i+56])+(max_data-buf_x1[i+57])+(max_data-buf_x1[i+58])
        +(max_data-buf_x1[i+59])+(max_data-buf_x1[i+60])
          +(buf_x1[i+61]-min_data)+(buf_x1[i+62]-min_data)+(buf_x1[i+63]-min_data)
            +(max_data-buf_x1[i+65])+(max_data-buf_x1[i+66])
              +(max_data-buf_x1[i+67])+(max_data-buf_x1[i+68])+(max_data-buf_x1[i+69]); 

    if(scor[i]>max1_cor) {                                                      //find maximum correlation for startpixel
      max1_cor=scor[i];
      max2_pos=max1_pos;
      max1_pos=i;
    }  
          
  }

  scor_max1=(scor[max1_pos]-scor[max1_pos-1])+(scor[max1_pos]-scor[max1_pos+1])+scor[max1_pos];
  scor_max2=(scor[max2_pos]-scor[max2_pos-1])+(scor[max2_pos]-scor[max2_pos+1])+scor[max2_pos]; 
  if(scor_max1>scor_max2) {
    startpixel=max1_pos;
  }
  else {
    startpixel=max2_pos;
  }


  //find second startpixel
  if(startpixel>CENTER_PIXEL) {                                                 //if main startpixel on the right
    startpixel2=startpixel;                                                     //main startpixel becomes right startpixel
    startpixel1=startpixel-SECTOR_SIZE;                                         //left startpixel canditate for regular sector
  }
  else {
    startpixel1=startpixel;
    startpixel2=startpixel+SECTOR_SIZE;
  }



}






/**
  * @brief  Шаг 3 конвейера: СУБПИКСЕЛЬНАЯ координата репера.
  *
  * Целочисленного номера пикселя мало: 1 пиксель = 2.5/51 = 0.049 град = 176 угл.сек.
  * Поэтому положение светлого пятна репера уточняется до сотых долей пикселя.
  *
  * Порядок действий:
  *   1) в окне [star_pixel+4, star_pixel+16) формируется скорректированный
  *      профиль buf_x2[i] = buf_x1[i] * buf_k[i] (buf_k компенсирует разброс
  *      чувствительности пикселей);
  *   2) находится max_X0 - индекс самого яркого пикселя окна;
  *   3) BR4_C() по 4-6 отсчётам вокруг max_X0 вычисляет дробную координату
  *      центра пятна.
  *
  * @param  star_pixel целочисленная оценка начала репера (startpixel1/startpixel2)
  * @retval субпиксельная координата центра репера в пикселях (тип float)
  */
/******************************************************************************/
    float mes_piks(uint16_t star_pixel){                                                      
      short unsigned amax_X0=0;
      float mes_pix = 0;
                    
      for (unsigned char i=star_pixel+4;i<star_pixel+16;i++) {
           
        buf_x2[i]=buf_x1[i]*buf_k[i];
      
        uint16_t temp_X1 = (int) buf_x2[i];
        if ( temp_X1 > amax_X0 ) { 
          amax_X0=temp_X1;
          max_X0 = i;
        }    
      }
        
      mes_pix = BR4_C (buf_x2, max_X0);

      return (mes_pix);
    }











/**
  * @brief  Линейная интерполяция яркости в ДРОБНОЙ координате pos.
  *
  * y(pos) = buf_x1[floor(pos)] + (buf_x1[ceil(pos)] - buf_x1[floor(pos)]) * {pos}
  * Нужна find_datablock(): центры битовых ячеек почти никогда не попадают точно
  * на целый пиксель, поэтому яркость в них вычисляется, а не берётся из массива.
  *
  * @param  pos дробная координата пикселя, 0..127
  * @retval интерполированное значение яркости (при выходе за границы возвращает 0xFFFF)
  */
/*-----------------------------------------------------------------------------------*/
unsigned short get_pix(float pos) {
  if((pos<0)||(pos>127)) return -1;
  int left=(int)floor(pos);
  int right=(int)ceil(pos);
  float k=pos-left;
  int dif=(int)((buf_x1[right]-buf_x1[left])*k);
  return (dif+buf_x1[left]);

}













/**
  * @brief  Шаг 4 конвейера: выборка 9 информационных бит кода сектора.
  *
  * Опорная сетка строится ОТ ИЗМЕРЕННЫХ реперов, а не от номинала, поэтому
  * она автоматически подстраивается под изменение расстояния до диска,
  * температурное расширение и перекос:
  *     sec_size = s_l2 - s_l1                 (ширина сектора в пикселях)
  *     pix_size = sec_size / BITS_PER_SECTOR  (ширина одной битовой ячейки)
  *     центр первого бита = s_l1 + 3*pix_size, далее шаг pix_size
  *
  * Локальные уровни "белого" (max_sdata) и "чёрного" (min_sdata) берутся не по
  * всему кадру, а по области данных [startpixel1+14, startpixel1+51) - это
  * делает порог решения устойчивым к неравномерности подсветки вдоль линейки.
  *
  * Для каждого бита сохраняются ОБА расстояния до уровней (pix[i][0], pix[i][1]),
  * чтобы дальше можно было оценить достоверность бита, а не только его значение.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void find_datablock(void) {

        float tmp_pos;


        //calc sector size
        sec_size=s_l2-s_l1;
        //calc bit size
        pix_size=sec_size/BITS_PER_SECTOR;


        //find min/max
        max_sdata=0;                                                            //init values
        min_sdata=4095;
        for (int i=14;i<51;i++) {                                               //right side min/max
          t_data=buf_x1[startpixel1+i];
          if (t_data>max_sdata) {
            max_sdata=t_data;
          }
          if (t_data<min_sdata) {
            min_sdata=t_data;
          }
        }

        //get bits
        right_f=s_l1+pix_size*3;                                                //right side bits
        tmp_pos=right_f;
        for (int i=0;i<9;i++) {
          t_data = get_pix(tmp_pos);
          pix[i][0]=max_sdata - t_data;
          pix[i][1]=t_data - min_sdata;
          tmp_pos+=pix_size;
        }  

  
}





/**
  * @brief  Шаг 5 конвейера: декодирование номера сектора.
  *
  * Три действия:
  *   1) ПОИСК САМОГО НЕНАДЁЖНОГО БИТА: минимизируется |pix[i][1] - pix[i][0]|,
  *      то есть ищется отсчёт, ближе всего лежащий к середине между чёрным и
  *      белым. Его индекс -> badbit_pos, запас надёжности -> badbit_value.
  *      Это "мягкое решение" (soft decision), оно используется bit_err_corr().
  *   2) СБОРКА КОДА: бит = 1, если pix[i][1] > pix[i][0]; биты пакуются
  *      старшим вперёд, начиная с маски 0x100 (9 бит) -> data_byte.
  *   3) ПОИСК В ТАБЛИЦЕ bit_tab[]: линейный перебор 144 значений;
  *      rem_sec = найденный индекс (= номер сектора) или 255, если кода нет
  *      в таблице (это признак ошибки чтения).
  *
  * Границы всех трёх циклов равны 9 - числу информационных бит и размеру
  * массива pix[9][2]. Индексация согласована с bit_err_corr(): биту с индексом
  * badbit_pos соответствует маска (0x100 >> badbit_pos).
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void calc_sector(void) {


        //search for possible error bit
        /* Поиск самого ненадёжного бита: минимум |pix[i][1]-pix[i][0]|.
           Границы цикла ДОЛЖНЫ совпадать с числом информационных бит (9),
           которое заполняет find_datablock(), и с размером массива pix[9][2].
           Ранее здесь стояло i<11 (наследие 11-битного варианта кода) - читались
           элементы pix[9] и pix[10] за границей массива, и badbit_pos мог
           указать на несуществующий бит. Исправлено. */
        int tmp1;
        int tmin=1000000;                                                       
        int min=0;
        for (int i=0;i<9;i++) {
          tmp1=pix[i][1]-pix[i][0];
          if (tmp1<0) tmp1=-tmp1;
          if(tmp1<tmin) {
            tmin=tmp1;
            min=i;
          }
        }
        badbit_pos=min;
        badbit_value=tmin;


        //reading code byte
        uint16_t cur_data=0;
        uint16_t bit_pos=0x100;
        for (int i=0;i<9;i++) {
          if (pix[i][1]>pix[i][0]) {
            cur_data=cur_data|bit_pos;
          }
          bit_pos=bit_pos>>1;
        }
        data_byte=cur_data;


        //searching code table
        uint8_t tmp_pos2=255;
        for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
          if (bit_tab[j]==data_byte) {
            tmp_pos2=j;
            break;
          }
        }

        rem_sec=tmp_pos2;

      
}













/**
  * @brief  Фиксация принятого номера сектора и его соседей.
  *
  * cursec = rem_sec, lsec = cursec-1, rsec = cursec+1 (с заворотом по модулю 144).
  * Тройка {lsec, cursec, rsec} - это "окно доверия" для СЛЕДУЮЩЕГО кадра:
  * за один кадр (около 80 мкс) вал физически не может уйти более чем на сектор,
  * поэтому любой номер вне этого окна трактуется как ошибка чтения.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void save_sector(void) {

  cursec=rem_sec;                                                               //save current sector
  if(rem_sec==0) {                                                              //find left sector
    lsec=BIT_TAB_SIZE-1;
  }
  else {
    lsec=rem_sec-1;
  }
  if(rem_sec==BIT_TAB_SIZE-1) {                                                 //find right sector
    rsec=0;
  }
  else {
    rsec=rem_sec+1;
  }

  errorflag=0;                                                                  //clear error flag

}





/**
  * @brief  Коррекция ошибки ВЫБОРА РЕПЕРА (второй уровень защиты).
  *
  * Обнуляет корреляционный пик вокруг забракованного startpixel и ищет
  * следующий по величине максимум scor[], после чего ПОЛНОСТЬЮ переделывает
  * конвейер от mes_piks() до calc_sector() с новым репером.
  *
  * ВНИМАНИЕ: в текущей версии прошивки эта функция НИГДЕ НЕ ВЫЗЫВАЕТСЯ -
  * err_corr() ограничивается побитовой коррекцией. Код оставлен как заготовка.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void startpixel_err_corr(void) {

  scor[startpixel]=0;                                                           //
  scor[startpixel-1]=0;
  scor[startpixel+1]=0;

  max1_cor=0;                                                                   //
  for (unsigned char i=STARTPIXEL_MIN;i<STARTPIXEL_MAX;i++) {
    cur_cor=scor[i];
    if(max1_cor<cur_cor) {
      max1_cor=cur_cor;
      max1_pos=i;
    }
  }
  startpixel=max1_pos;


  //find second startpixel
  if(startpixel>CENTER_PIXEL) {                                                 //if main startpixel on the right
    startpixel2=startpixel;                                                     //main startpixel becomes right startpixel
    startpixel1=startpixel-SECTOR_SIZE;                                         //left startpixel canditate for regular sector
  }
  else {
    startpixel1=startpixel;
    startpixel2=startpixel+SECTOR_SIZE;
  }

  s_l1 = mes_piks (startpixel1);                                                // (440 cycles) calculate 1-st subpixel coordinates

  s_l2 = mes_piks (startpixel2);                                                // (308 cycles) calculate 2-nd subpixel coordinates

  find_datablock();                                                             //finding data bits

  calc_sector();                                                                //calculate sector number

}




/**
  * @brief  Коррекция ОДИНОЧНОЙ битовой ошибки в коде сектора.
  *
  * Алгоритм "flip the least reliable bit": если запас надёжности худшего бита
  * badbit_value меньше порога 1000 отсчётов АЦП, считаем, что ошибся именно он,
  * инвертируем соответствующий разряд data_byte и повторяем поиск в bit_tab[].
  * Это простейший вариант декодирования по мягкому решению (Chase-1).
  *
  * Маска разряда: (0x100 >> badbit_pos) - код 9-битный, старший значащий
  * разряд 0x100 соответствует pix[0] (см. упаковку в calc_sector()).
  * Порог 1000 - эмпирический, зависит от амплитуды сигнала (её удерживает
  * АРУ подсветки около MAX_LEVEL_PIX = 3700, т.е. 1000 ~ 27 % размаха).
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void bit_err_corr(void) {

            if(badbit_value<1000) {                                             //error bit correction

              /* Инверсия самого ненадёжного бита (Chase-1).
                 calc_sector() пакует биты так: pix[0] -> 0x100 (старший),
                 pix[1] -> 0x080, ... pix[8] -> 0x001. Значит маска разряда с
                 индексом badbit_pos равна (0x100 >> badbit_pos).
                 Ранее здесь стоял switch с масками, начинавшимися с 0x400
                 (наследие 11-битного варианта кода): инвертировался разряд,
                 сдвинутый на 2 позиции относительно признанного ненадёжным,
                 из-за чего коррекция почти никогда не срабатывала. Исправлено.
                 Проверка диапазона - защитная: при i<9 в calc_sector()
                 badbit_pos всегда лежит в 0..8.                              */
              if ( (badbit_pos>=0) && (badbit_pos<9) ) {
                data_byte = data_byte ^ (uint16_t)(0x100 >> badbit_pos);
              }

              uint8_t tmp_pos=255;
              for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
                if (bit_tab[j]==data_byte) {
                  tmp_pos=j;
                  break;
                }
              }

              rem_sec=tmp_pos;

          }


}











/**
  * @brief  Шаг 6 конвейера: контроль достоверности и коррекция ошибок.
  *
  * РЕЖИМ ПРОГРЕВА (первые 50 кадров, sec_cnt < 50): истории ещё нет, любой
  * прочитанный номер принимается на веру, errorflag сбрасывается.
  *
  * РАБОЧИЙ РЕЖИМ:
  *   если rem_sec == 255 (кода нет в таблице) ИЛИ rem_sec не попал в окно
  *   доверия {lsec, cursec, rsec}:
  *       serrcnt2++ ; bit_err_corr()   - пробуем исправить худший бит;
  *       если и после этого номер вне окна:
  *           serrcnt1++ ; errorflag = 1 - кадр признан недостоверным,
  *           угол в этом кадре НЕ пересчитывается (см. main()),
  *           но окно доверия всё равно переустанавливается на новый номер,
  *           иначе головка навсегда "залипнет" после реального проскока.
  *   иначе save_sector().
  *
  * Итог: sector / lsector / rsector - то, чем пользуется calc_ang().
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void err_corr(void) {

        
        if ( (sec_cnt<50) ) {                                                   //if less then 50 measurements

          sec_cnt++;
          
          cursec=rem_sec;                                                       //save current sector
          if(rem_sec==0) {                                                      //find left sector
            lsec=BIT_TAB_SIZE-1;
          }
          else {
              lsec=rem_sec-1;
          }
          if(rem_sec==BIT_TAB_SIZE-1) {                                         //find right sector
            rsec=0;
          }
          else {
            rsec=rem_sec+1;
          }  
          errorflag=0;                                                          //clear error flag

      
        }
        else {                                                                  //if more then 50 measurements
     
          if( (rem_sec==255) || ((rem_sec!=cursec)&&(rem_sec!=lsec)&&(rem_sec!=rsec)) ) { //if error found

            serrcnt2++;                                                         //increment bit error counter
            bit_err_corr();                                                     //bit error correction

            if( (rem_sec==255) || ((rem_sec!=cursec)&&(rem_sec!=lsec)&&(rem_sec!=rsec)) ) { //if error found
              serrcnt1++;                                                       //increment unrecoverable error counter
              errorflag=1;                                                      //set error flag

              cursec=rem_sec;                                                   //save current sector
              if(rem_sec==0) {                                                  //find left sector
                lsec=BIT_TAB_SIZE-1;
              }
              else {
                  lsec=rem_sec-1;
              }
              if(rem_sec==BIT_TAB_SIZE-1) {                                     //find right sector
                rsec=0;
              }
              else {
                rsec=rem_sec+1;
              }

            }
            else {
              save_sector();                                                    //save sector
            }

          }
          else {                                                                //if no errors
            save_sector();                                                      //save sector
          }

         }
        
        sector=cursec;
        lsector=lsec;
        rsector=rsec;
        
        
}








/**
  * @brief  Шаг 7 конвейера: ВЫЧИСЛЕНИЕ УГЛА.
  *
  * Дано: номер сектора sector, субпиксельные координаты его границ s_l1 (левая)
  * и s_l2 (правая) на линейке, точка съёма offset, калибровочная таблица ang_tab.
  *
  * Расчёт (линейная интерполяция внутри сектора):
  *     ts_l2    = offset - s_l1            смещение точки съёма от левой границы, пикс.
  *     ts_l1    = offset - s_l2            то же от правой границы, пикс.
  *     pix_dif  = ts_l2 - ts_l1 = s_l2 - s_l1      ширина сектора, пикс.
  *     angle_dif= ang_tab[rsector] - ang_tab[sector]  (+360, если перешли через 0)
  *     k_pix    = angle_dif / pix_dif      МАСШТАБ, град/пиксель (свой для сектора!)
  *     t_l1     = ang_tab[sector] + ts_l2 * k_pix
  *     t_l      = 360 - t_l1               разворот направления отсчёта
  *     cur_ang  = t_l, приведённое к [0, 360)
  *
  * Ключевой момент: k_pix берётся НЕ из номинала 2.5/51, а из реальной ширины
  * конкретного сектора и реальных углов его границ. Поэтому ошибки нанесения
  * шкалы (до +-90 угл.сек) уходят в ang_tab и компенсируются.
  *
  * Результат: cur_ang_X - "сырой" угол текущего кадра. Наружу отдаётся cur_ang_E,
  * который обновляется из cur_ang_X в main() только при errorflag == 0.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void calc_ang() {

// calc_ang() step


  cur_sector=sector;


    ts_l2=-(s_l1-offset);
    ts_l1=-(s_l2-offset);
    pix_dif=ts_l2-ts_l1;
    r_angle=ang_tab[rsector];
    l_angle=ang_tab[sector];
    angle_dif=r_angle-l_angle;
    if (angle_dif<0) angle_dif+=360;
    k_pix=angle_dif/pix_dif;

    t_l1=l_angle+ts_l2*k_pix;


    t_l=360-t_l1;


// calc_ang() step
    if (t_l<0) {
      cur_ang= t_l+360;
    }
    else if (t_l>=360) {
      cur_ang= t_l-360;
    }
    else {
      cur_ang= t_l;
    }

    
    
    cur_ang_X= cur_ang;

}
















/**
  * @brief  BR4 - субпиксельная оценка центра пятна по 4 отсчётам (базовый вариант).
  *
  * Метод: строятся две АНТИСИММЕТРИЧНЫЕ свёртки сигнала с ядром (+1,+1,-1,-1),
  * сдвинутые друг относительно друга на 1 пиксель:
  *     g0 = x[c-2]+x[c-1]-x[c+1]-x[c+2]
  *     g1 = x[c-1]+x[c] - x[c+2]-x[c+3]
  * Для симметричного пятна такая свёртка равна нулю ровно в его центре и почти
  * линейна рядом с ним. Поэтому ноль ищется линейной интерполяцией между g0 и g1:
  *     центр = c + g0 / (g0 - g1)
  * Преимущество перед центром масс: результат не зависит ни от постоянного
  * пьедестала, ни от общей амплитуды сигнала (обе величины сокращаются).
  * Ветвление по условию x[c+1] >= x[c-1] выбирает, с какой стороны от c лежит
  * истинный центр, чтобы интерполяция всегда шла по возрастающему интервалу.
  *
  * В текущей прошивке НЕ вызывается - используется вариант BR4_C.
  *
  * @param  mas        массив отсчётов
  * @param  centor_int индекс максимума
  * @retval дробная координата центра
  */
/*-----------------------------------------------------------------------------------*/
float BR4 (float *mas, int centor_int)
{
  
  if (mas[centor_int+1] >= mas[centor_int-1]) {
    g0 = mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] ;
    g1 = mas[centor_int-1] + mas[centor_int] - mas[centor_int+2] - mas[centor_int+3];
  }
  else {
    g0 = mas[centor_int-3] + mas[centor_int-2] - mas[centor_int] - mas[centor_int+1];
    g1 = mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2];
    centor_int = centor_int - 1;
  }    
  if (g0 == g1) 
    g0++;
 
  delBR4=g0/(g0-g1);
  return centor_int+delBR4;
}




/**
  * @brief  BR4_C - РАБОЧИЙ субпиксельный алгоритм (BR4 + компенсация S-образной ошибки).
  *
  * Любая 4-точечная оценка центра даёт систематическую ошибку, периодическую по
  * дробной части координаты с периодом ровно 1 пиксель (т.н. S-curve / pixel
  * locking - следствие дискретизации непрерывного пятна конечным числом отсчётов).
  * Здесь она моделируется первой гармоникой:
  *     delta = 100 * g0/(g0-g1) + 100      (delta = 0..200 <-> дробная часть -1..+1)
  *     ampl  = k_comp * sin( (delta-100)*pi/50 + phase ) = k_comp * sin(2*pi*frac)
  *     результат = c + g0/(g0-g1) - ampl
  * k_comp = 0.01 пикселя - амплитуда поправки, phase - её фаза.
  * Вызывается через mes_piks(); arm_sin_f32 - быстрый синус из CMSIS-DSP.
  *
  * @param  mas        массив отсчётов (обычно buf_x2 - с учётом buf_k)
  * @param  centor_int индекс максимума
  * @retval дробная координата центра репера
  */
/*-----------------------------------------------------------------------------------*/
float BR4_C (float *mas, int centor_int)
{
   
  delta=0;
  
  if (mas[centor_int+1] >= mas[centor_int-1]) {
    g0 = mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] ;
    g1 = mas[centor_int-1] + mas[centor_int] - mas[centor_int+2] - mas[centor_int+3];
  }
  else {
    g0 = mas[centor_int-3] + mas[centor_int-2] - mas[centor_int] - mas[centor_int+1];
    g1 = mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2];
    centor_int = centor_int - 1;
    delta=-100;
  }    
  if (g0==g1) 
    g0++;
 
 //compensation BR4
  delta+=100*g0/(g0-g1)+100; 
  
//  if(delta>=1 && delta<200) ampl=delta_cor[delta];
//  else ampl=0.;
  if(delta>=1 && delta<200) ampl=k_comp*arm_sin_f32((delta-100)*M_PI/50.+phase);
  else ampl=0;
  
  //ampl=0;

  return centor_int+g0/(g0-g1) - ampl;
}









/*-----------------------------------------------------------------------------------*/


/* АЛЬТЕРНАТИВНЫЕ (НЕ ВЫЗЫВАЕМЫЕ) субпиксельные алгоритмы -------------------
   Оставлены в коде как экспериментальный материал для сравнения точности:
     calc()          - метод половины интеграла (медиана профиля);
     INTEGRAL()      - полиномиальная коррекция интегральных отношений;
     KVADRATURA()    - центр масс по квадратам отсчётов (подавляет хвосты);
     CENTR_OF_MASS() - классический центр масс (чувствителен к пьедесталу);
     BR4_F/BR6/BR6_F/BR8 - те же "балансные" оценки на 6 и 8 отсчётах и с
                           предварительной КИХ-фильтрацией (filt_a/b/c).
   Более широкое ядро = меньше шума, но хуже разрешение близко стоящих пятен. */
float calc (float *mas, int centor_int)
{
  leftx[centor_int-4]=mas[centor_int-4];
  leftx[centor_int-3]=leftx[centor_int-4]+mas[centor_int-3];
  leftx[centor_int-2]=leftx[centor_int-3]+mas[centor_int-2];
  leftx[centor_int-1]=leftx[centor_int-2]+mas[centor_int-1];
  leftx[centor_int]=leftx[centor_int-1]+mas[centor_int];
  leftx[centor_int+1]=leftx[centor_int]+mas[centor_int+1];
  leftx[centor_int+2]=leftx[centor_int+1]+mas[centor_int+2];
  leftx[centor_int+3]=leftx[centor_int+2]+mas[centor_int+3];
  leftx[centor_int+4]=leftx[centor_int+3]+mas[centor_int+4];
  
  halfsum=leftx[centor_int+4]/2;
  
  for (int i=centor_int-4;i<=centor_int+4;i++) {
    if (leftx[i]>halfsum) {
      leftfind=i;
      break;
    }
  }
  return (float)leftfind-(leftx[leftfind]-halfsum)/(leftx[leftfind]-leftx[leftfind-1])+0.5;
}

/**
  * @brief  ?????????? ?????????? ??????? ??????? ???????? 
  * @param
  * @retval
  */
/*-----------------------------------------------------------------------------------*/
float INTEGRAL (float *mas, int centor_int)
{
  float S[7] = 
  {0.0297518919281908, 0.114768212171568, 0.358813823431494, 
  0.641186176568506, 0.885231787828432, 0.970248108071809, 1};
  float a = 26.617, b = 6.1341, c = 0.2545, d = 3.9292; 
  float centor = centor_int;
  float SP[7],SP1[7];
  float SSS = mas[centor_int - 3] + mas[centor_int - 2] + mas[centor_int - 1] 
    + mas[centor_int - 0] + mas[centor_int + 1] + mas[centor_int + 2] + mas[centor_int + 3];
  
  SP[3] = (mas[centor_int - 3] + mas[centor_int - 2] + mas[centor_int - 1] 
           + mas[centor_int - 0] ) / SSS;
  SP1[3] = (mas[centor_int - 0] + mas[centor_int + 1] + mas[centor_int + 2] 
            + mas[centor_int + 3]) / SSS;     
  SP1[4] = (mas[centor_int - 3] + mas[centor_int - 2] + mas[centor_int - 0] + mas[centor_int + 1] ) / SSS;
  
  SP[2] = ( mas[centor_int - 1] + mas[centor_int - 0] + mas[centor_int + 2] + mas[centor_int + 3]) / SSS;
      

  float A0 = S[3] - SP[3];
  float A10 = S[3] - SP1[3];
  float A01 = S[2] - SP[2];
  float A101 = S[4] - SP1[4];
  float H0 = - a * A0 * A0 * A0 * A0 + b * A0 * A0 * A0 - c * A0 * A0 + d * A0;
  float H10 = a * A10 * A10 * A10 * A10 - b * A10 * A10 * A10 + c * A10 * A10 - d * A10;
  float H101 = - a * A101 * A101 * A101 * A101 - b * A101 * A101 * A101 - c * A101 * A101 - d * A101;
  float H01 = a * A01 * A01 * A01 * A01 + b * A01 * A01 * A01 + c * A01 * A01 + d * A01;
    
  return 0.5 + centor + (H0 + H10 + H101 + H01)/4;  
}

/**
  * @brief  ?????????? ?????????? ??????? ?????????? ???????
  * @param
  * @retval
  */
/*-----------------------------------------------------------------------------------*/
float KVADRATURA (float *mas, int centor_int)
{
  float g1 = mas[centor_int] * mas[centor_int];
  float g0 = (centor_int) * mas[centor_int] * mas[centor_int];
  
  for (unsigned char i=1;i<=2;i++) {
      g0 = g0 + (centor_int + i) * mas[centor_int + i] * mas[centor_int + i];
      g0 = g0 + (centor_int - i) * mas[centor_int - i] * mas[centor_int - i];
      
      g1 = g1 + mas[centor_int + i] * mas[centor_int + i];
      g1 = g1 + mas[centor_int - i] * mas[centor_int - i];
  }
  
  return g0 / g1;
}

/**
  * @brief  ?????????? ?????????? ??????? ????? ???? ???????
  * @param
  * @retval
  */
/*-----------------------------------------------------------------------------------*/
float CENTR_OF_MASS (float *mas, int centor_int)
{
  float g1 = mas[centor_int];
  float g0 = (centor_int) * mas[centor_int];
  
  for (unsigned char i=1;i<=2;i++) {
      g0 = g0 + (centor_int + i) * mas[centor_int + i];
      g0 = g0 + (centor_int - i) * mas[centor_int - i];
      
      g1 = g1 + mas[centor_int + i];
      g1 = g1 + mas[centor_int - i];
  }
  
 return g0 / g1;
}

/**
  * @brief  ?????????? ?????????? ???????
  * @param
  * @retval
  */
/*-----------------------------------------------------------------------------------*/
float BR4_F (float *mas, int centor_int)
{
  float ampl;
  float g1, g0;
  delta=0;

  float massi [128]={0};
  for (int i = -3; i <= 3; i++)
  massi[centor_int+i] = filt_a * mas[centor_int-0+i] + filt_b * ( mas[centor_int-1+i] +  mas[centor_int+1+i]) +
      filt_c * (mas[centor_int-2+i] +  mas[centor_int+2+i]);
   
  if (massi[centor_int+1] >= massi[centor_int-1]) {
    g0 = massi[centor_int-2] + massi[centor_int-1] - massi[centor_int+1] - massi[centor_int+2];
    g1 = massi[centor_int-1] + massi[centor_int] - massi[centor_int+2] - massi[centor_int+3];
  }
  else {
    g0 = massi[centor_int-3] + massi[centor_int-2] - massi[centor_int] - massi[centor_int+1];
    g1 = massi[centor_int-2] + massi[centor_int-1] - massi[centor_int+1] - massi[centor_int+2];
    centor_int = centor_int - 1;
  }    
  if (g0==g1) 
    g0++;
  
   //??????????? BR
  delta+=100*g0/(g0-g1)+100; 
  if(delta>=0 && delta<200) ampl=delta_cor[delta];
  else ampl=0.;

  ampl=0;

  return centor_int+g0/(g0-g1) - ampl;
  

}

/*-----------------------------------------------------------------------------------*/
/**
  * @brief  ????? ??????? ??????
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
float BR6 (float *mas, int centor_int)
{
  float g1, g0, ampl;
  delta=0;
  ampl=0;
  
  if (mas[centor_int+1] >= mas[centor_int-1]) {
    g0 = mas[centor_int-3] + mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] - mas[centor_int+3];
    g1 = mas[centor_int-2] + mas[centor_int-1] + mas[centor_int] - mas[centor_int+2] - mas[centor_int+3] - mas[centor_int+4];
  }
  else {
    g0 = mas[centor_int-4] + mas[centor_int-3] + mas[centor_int-2] - mas[centor_int] - mas[centor_int+1] - mas[centor_int+2];
    g1 = mas[centor_int-3] + mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] - mas[centor_int+3];
    centor_int = centor_int - 1;
    delta=-100;
  }    
  if (g0==g1) g0++;

  //??????????? BR
  delta+=100*g0/(g0-g1)+100; 
  if(delta>=0 && delta<200) ampl=delta_cor[delta];
  else ampl=0.;

 //ampl=0.;
  
  return centor_int+g0/(g0-g1)-ampl;
}
/*-----------------------------------------------------------------------------------*/
/**
  * @brief  ????? ??????? ??????
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
float BR6_F (float *mas, int centor_int)
{
  float g1, g0, ampl;
  delta=0;
  ampl=0;
  
  float massi [128]={0};
  for (int i = -4; i <= 4; i++)
  massi[centor_int+i] = filt_a * mas[centor_int-0+i] + filt_b * ( mas[centor_int-1+i] +  mas[centor_int+1+i]) +
      filt_c * (mas[centor_int-2+i] +  mas[centor_int+2+i]);
  
  if (massi[centor_int+1] >= massi[centor_int-1]) {
    g0 = massi[centor_int-3] + massi[centor_int-2] + massi[centor_int-1] - massi[centor_int+1] - massi[centor_int+2] - massi[centor_int+3];
    g1 = massi[centor_int-2] + massi[centor_int-1] + massi[centor_int] - massi[centor_int+2] - massi[centor_int+3] - massi[centor_int+4];
  }
  else {
    g0 = massi[centor_int-4] + massi[centor_int-3] + massi[centor_int-2] - massi[centor_int] - massi[centor_int+1] - massi[centor_int+2];
    g1 = massi[centor_int-3] + massi[centor_int-2] + massi[centor_int-1] - massi[centor_int+1] - massi[centor_int+2] - massi[centor_int+3];
    centor_int = centor_int - 1;
  }    
  if (g0==g1) 
    g0++;

  //??????????? BR
  delta+=100*g0/(g0-g1)+100; 
  if(delta>=0 && delta<200) ampl=delta_cor[delta];
  else ampl=0.;

 //ampl=0.;
  
  return centor_int+g0/(g0-g1)-ampl;
}
/*-----------------------------------------------------------------------------------*/
float BR8 (float *mas, int centor_int)
{
  float g1, g0, ampl;
  delta=0;
  ampl=0;
  
  if (mas[centor_int+1] >= mas[centor_int-1]) {
    g0 = mas[centor_int-4] + mas[centor_int-3] + mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] - mas[centor_int+3] - mas[centor_int+4];
    g1 = mas[centor_int-3] + mas[centor_int-2] + mas[centor_int-1] + mas[centor_int] - mas[centor_int+2] - mas[centor_int+3] - mas[centor_int+4] - mas[centor_int+5];
  }
  else {
    g0 = mas[centor_int-5] + mas[centor_int-4] + mas[centor_int-3] + mas[centor_int-2] - mas[centor_int] - mas[centor_int+1] - mas[centor_int+2] - mas[centor_int+3];
    g1 = mas[centor_int-4] + mas[centor_int-3] + mas[centor_int-2] + mas[centor_int-1] - mas[centor_int+1] - mas[centor_int+2] - mas[centor_int+3] - mas[centor_int+4];
    centor_int = centor_int - 1;
    delta=-100;    
  }    
  if (g0==g1) 
    g0++;
  
  //??????????? BR
  delta+=100*g0/(g0-g1)+100; 
  if(delta>=0 && delta<200) ampl=delta_cor[delta];
  else ampl=0.;

 //ampl=0.;
  
  return centor_int+g0/(g0-g1)-ampl;
}


























/**
  * @brief  Оценка качества измерения: остаток относительно сглаженной траектории.
  *
  * ЗАЧЕМ. Абсолютной эталонной шкалы в системе нет, поэтому критерием качества
  * служит ГЛАДКОСТЬ выдаваемого угла при равномерном вращении вала.
  *
  * КАК. avg_buf - сдвиговый буфер 30 последних углов (avg_buf[0] - свежайший).
  *   avg_ang_X = (1/30) * SUM avg_buf[i]        - среднее по окну
  *   avg_cur   = avg_ang_X - avg_buf[15]        - остаток в центре окна
  * При постоянной скорости истинный угол линеен по времени, среднее линейной
  * функции по симметричному окну равно её значению в центре, значит avg_cur -
  * это ОШИБКА измерения, очищенная от самого движения (ФВЧ-фильтр).
  *
  * За оборот накапливаются экстремумы cur_min / cur_max. Переход через 0/360
  * детектируется по условию "свежий < 0.5 и старый > 359.5" (или наоборот) -
  * это же даёт счётчики оборотов rev_right_cnt / rev_left_cnt и инкремент
  * avg_minmax_num ("прошёл ещё один оборот").
  * Экстремумы усредняются по offset_avg_num оборотам, и получается
  *     min_max = avg_max - avg_min
  * - размах ошибки, ЦЕЛЕВАЯ ФУНКЦИЯ для offset_cal() и angk_cal().
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void find_avg() {        
    avg_buf[29]=avg_buf[28];
    avg_buf[28]=avg_buf[27];
    avg_buf[27]=avg_buf[26];
    avg_buf[26]=avg_buf[25];
    avg_buf[25]=avg_buf[24];
    avg_buf[24]=avg_buf[23];
    avg_buf[23]=avg_buf[22];
    avg_buf[22]=avg_buf[21];
    avg_buf[21]=avg_buf[20];
    avg_buf[20]=avg_buf[19];
    avg_buf[19]=avg_buf[18];
    avg_buf[18]=avg_buf[17];
    avg_buf[17]=avg_buf[16];
    avg_buf[16]=avg_buf[15];
    avg_buf[15]=avg_buf[14];
    avg_buf[14]=avg_buf[13];
    avg_buf[13]=avg_buf[12];
    avg_buf[12]=avg_buf[11];
    avg_buf[11]=avg_buf[10];
    avg_buf[10]=avg_buf[9];
    avg_buf[9]=avg_buf[8];
    avg_buf[8]=avg_buf[7];
    avg_buf[7]=avg_buf[6];
    avg_buf[6]=avg_buf[5];
    avg_buf[5]=avg_buf[4];
    avg_buf[4]=avg_buf[3];
    avg_buf[3]=avg_buf[2];
    avg_buf[2]=avg_buf[1];
    avg_buf[1]=avg_buf[0];
    avg_buf[0]=cur_ang_X;

    
    if ((cur_ang_X>0.5)&&(cur_ang_X<359.5)) {
    
      rev_en=1;
      
      sum_avg_buf = 0;
      for (int i=0;i<30;i++) {
        sum_avg_buf+=avg_buf[i];
      }
      avg_ang_X=sum_avg_buf/30;
    
      avg_cur=avg_ang_X-avg_buf[15];


        if ( (cur_min>avg_cur)&&(avg_cur>0)) {
          cur_min=avg_cur;
        }
        
        if ( (cur_max<avg_cur)&&(avg_cur<0.1)) {
          cur_max=avg_cur;
        }
        
    
    
    
    
    }
    else if ((avg_buf[0]<0.5)&&(avg_buf[29]>359.5)&&(rev_en==1) ) {
      
      rev_en=0;
      
      rev_right_cnt++;


      
      avg_minmax_num++;
      
      
      sum_avg_min = 0;
      for (int i=offset_avg_num-1;i>0;i--) {
        min_buf[i]=min_buf[i-1];
        sum_avg_min+=min_buf[i];
      }
      min_buf[0]=cur_min;
      sum_avg_min+=min_buf[0];
      avg_min=sum_avg_min/offset_avg_num;
      
      cur_min=avg_cur;
      
      
      sum_avg_max = 0;
      for (int i=offset_avg_num-1;i>0;i--) {
        max_buf[i]=max_buf[i-1];
        sum_avg_max+=max_buf[i];
      }
      max_buf[0]=cur_max;
      sum_avg_max+=max_buf[0];
      avg_max=sum_avg_max/offset_avg_num;

      cur_max=avg_cur;
      

      min_max=avg_max-avg_min;
      
      

      
    }
    

    else if ((avg_buf[29]<0.5)&&(avg_buf[0]>359.5)&&(rev_en==1) ) {
      
      rev_en=0;
      
      rev_left_cnt++;

      

      avg_minmax_num++;
      
      
      sum_avg_min = 0;
      for (int i=offset_avg_num-1;i>0;i--) {
        min_buf[i]=min_buf[i-1];
        sum_avg_min+=min_buf[i];
      }
      min_buf[0]=cur_min;
      sum_avg_min+=min_buf[0];
      avg_min=sum_avg_min/offset_avg_num;
      
      cur_min=avg_cur;
      
      
      sum_avg_max = 0;
      for (int i=offset_avg_num-1;i>0;i--) {
        max_buf[i]=max_buf[i-1];
        sum_avg_max+=max_buf[i];
      }
      max_buf[0]=cur_max;
      sum_avg_max+=max_buf[0];
      avg_max=sum_avg_max/offset_avg_num;

      cur_max=avg_cur;
      

      min_max=avg_max-avg_min;
      
      
      
    }

}
    











/**
  * @brief  САМОКАЛИБРОВКА ТАБЛИЦЫ УГЛОВ СЕКТОРОВ ang_tab[] (метод замыкания оборота).
  *
  * ИДЕЯ. Ширину сектора j можно измерить локально и очень точно - это расстояние
  * между его реперами A_j = s_l2 - s_l1 (переменная pix_dif, пиксели). Абсолютный
  * масштаб "пиксели -> градусы" неизвестен, но известно УСЛОВИЕ ЗАМЫКАНИЯ: сумма
  * всех 144 секторов равна ровно 360 градусов. Отсюда:
  *     ang_tab[0] = 0
  *     ang_tab[j] = ang_tab[j-1] + 360 * A_(j-1) / SUM(A_k, k=0..143)
  * Никакого внешнего эталона угла не требуется - это классическая self-calibration
  * круговой шкалы (принцип полной окружности / closure method).
  *
  * ЗАПУСК И ХОД (два прохода; внешний командный канал удалён вместе с SPI,
 *   запуск будет переведён на RS485/ModBus, флаги те же):
 *   start_calibrate=1, encoder_state=0x10 : проход "вправо";
  *           оператор вращает вал в одну сторону, пока не будут измерены все 144
  *           сектора; по завершении auto_cal=0 и encoder_state=0x20;
 *   start_calibrate=2, encoder_state=0x30 : проход "влево";
  *           по завершении таблицы усредняются
  *               new_ang_tab[j] = (new_ang_tab1[j] + new_ang_tab2[j]) / 2,
  *           копируются в ang_tab, пишутся в EEPROM (стр. 0, адрес 0, 576 байт +
  *           CRC16), encoder_state=0x40;
 *   аварийная остановка: start_calibrate=0, auto_cal=0, encoder_state=0x00.
  *
  * УСЛОВИЕ ВЗЯТИЯ ОТСЧЁТА (ключевая строка if ниже):
  *     (cur_sector != old_sector) И ( s_l1 в [offset-25, offset-24]
  *                                 ИЛИ s_l2 в [offset+24, offset+25] )
  * То есть отсчёт берётся не в произвольный момент, а когда сектор стоит
  * СИММЕТРИЧНО относительно точки съёма (половина сектора = 51/2 = 25.5 пикс).
  * Это убирает зависимость измеренной ширины от положения сектора в поле зрения
  * (дисторсия оптики, расфокусировка по краям линейки). Защёлка old_sector
  * (= rsector в проходе 1, = lsector в проходе 2) задаёт НАПРАВЛЕНИЕ обхода и
  * не даёт многократно засчитать один и тот же сектор.
  *
  * НАКОПЛЕНИЕ. На каждый сектор набирается pix_dif_num_avg = 5 измерений, они
  * усредняются в pix_dif_tab[j], сектор помечается готовым (pix_rdy_tab[j]=1),
  * счётчик готовых pix_rdy_num растёт от 0 до BIT_TAB_SIZE=144.
  * КОНТРОЛЬ ХОДА калибровки: читать pix_rdy_num1 / pix_rdy_num2 (отладчик/SWD)
  * и encoder_state. Пока pix_rdy_num < 144 - продолжать вращение.
  *
  * ВАЖНО: pix_dif берётся из глобальной переменной, вычисленной в calc_ang(),
  * и НЕ зависит от старого содержимого ang_tab - калибровка не "тянет" за собой
  * предыдущую ошибку и может запускаться с любой начальной таблицей.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void angtab_cal(){

  uint16_t crc16;

  if (start_calibrate>0) {
    auto_cal=start_calibrate;
    start_calibrate=0;

    //
    if (auto_cal==1) {
      for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
        pix_rdy_tab1[j]=0;
        pix_dif_sum1[j]=0;
        pix_dif_num1[j]=0;
      }
      pix_rdy_num1=0;
      for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
        new_ang_tab1[j]=0;
      }
    }
    else if (auto_cal==2) {

      for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
        pix_rdy_tab2[j]=0;
        pix_dif_sum2[j]=0;
        pix_dif_num2[j]=0;
      }
      pix_rdy_num2=0;


      for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
        new_ang_tab2[j]=0;
      }
    }
    
    
  }    

  
  if (auto_cal==1) {

    /* Триггер отсчёта: сектор сменился И один из его реперов стоит ровно в
       полусекторе (51/2 = 25.5 пикс) от точки съёма offset - то есть сектор
       расположен симметрично в поле зрения. Всегда одинаковая геометрия
       измерения => систематика оптики не попадает в результат.               */
    if (  (cur_sector!=old_sector)&& ( ((s_l1>=(offset-25) )&&(s_l1<=(offset-24)))||(((s_l2>=(offset+24))&&(s_l2<=(offset+25)))) )  ) {

      old_sector=rsector;                                                       //защёлка направления обхода (проход 1)
      cal_sector=cur_sector;


        if ( (pix_dif_num1[cal_sector]>=pix_dif_num_avg) ) {

          if ( pix_rdy_tab1[cal_sector]==0 ) {

            pix_dif_tab1[cal_sector]=pix_dif_sum1[cal_sector]/pix_dif_num1[cal_sector];
            pix_rdy_tab1[cal_sector]=1;
            pix_rdy_num1++;
            
            if(pix_rdy_num1==BIT_TAB_SIZE) {

              float tmp_sum=0;
              for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
                tmp_sum+=pix_dif_tab1[j];
              }
              /* ЗАМЫКАНИЕ ОБОРОТА: сумма всех измеренных ширин секторов
                 приравнивается к 360 град, и углы границ получаются как
                 нарастающий итог нормированных ширин. Внешний эталон не нужен. */
              sum_Ai1=tmp_sum;
              new_ang_tab1[0]=0;
              for (unsigned char j=1;j<BIT_TAB_SIZE;j++) {
                new_ang_tab1[j]=new_ang_tab1[j-1]+pix_dif_tab1[j-1]/sum_Ai1*360;

              }
              auto_cal=0;                                                       //проход 1 завершён
                
              encoder_state=0x20;                                               //статус "ждём команду 0x12 (проход 2)"
              
            }
            
          }
          
        }
        else {

          if ( pix_rdy_tab1[cal_sector]==0 ) {


            pix_dif_sum1[cal_sector]+=pix_dif;
            pix_dif_num1[cal_sector]++;

          }


        }
        
          
        
          
    }
    

  }      
    



  else if (auto_cal==2) {

    if (  (cur_sector!=old_sector)&& ( ((s_l1>=(offset-25) )&&(s_l1<=(offset-24)))||(((s_l2>=(offset+24))&&(s_l2<=(offset+25)))) )  ) {

      old_sector=lsector;  
      cal_sector=cur_sector;

        if ( (pix_dif_num2[cal_sector]>=pix_dif_num_avg) ) {

          if ( pix_rdy_tab2[cal_sector]==0 ) {

            pix_dif_tab2[cal_sector]=pix_dif_sum2[cal_sector]/pix_dif_num2[cal_sector];
            pix_rdy_tab2[cal_sector]=1;
            pix_rdy_num2++;
            
            if(pix_rdy_num2==BIT_TAB_SIZE) {

              float tmp_sum=0;
              for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
                tmp_sum+=pix_dif_tab2[j];
              }
              sum_Ai2=tmp_sum;
              new_ang_tab2[0]=0;
              for (unsigned char j=1;j<BIT_TAB_SIZE;j++) {
                new_ang_tab2[j]=new_ang_tab2[j-1]+pix_dif_tab2[j-1]/sum_Ai2*360;
              }
                
              /* Усреднение двух проходов (вращение в разные стороны) снимает
                 направленно-зависимую составляющую ошибки: люфт, гистерезис
                 механики и динамическое запаздывание обработки.              */
              for (unsigned char j=0;j<BIT_TAB_SIZE;j++) {
                new_ang_tab[j]=(new_ang_tab1[j]+new_ang_tab2[j])/2;
              }

              memcpy(ang_tab,new_ang_tab,BIT_TAB_SIZE*4);                       //use found ang_tab

              memcpy(&eeprom_buf[eeprom_ang_tab],&ang_tab,BIT_TAB_SIZE*4);      //copy ang_tab to write buffer
              crc16=0xFFFF;                                                     //CRC16 calc
              for (int i=eeprom_ang_tab;i<eeprom_ang_tab+BIT_TAB_SIZE*4;i++) {
                crc16 = crc_16_step (eeprom_buf[i],crc16);
              }
              eeprom_buf[eeprom_ang_tab_crc+0]=crc16&0xff;
              eeprom_buf[eeprom_ang_tab_crc+1]=crc16>>8;
              i2c3_tx_wp=0;                                                     //tx_wp flag reset
              LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);                     //WP disable
              HAL_I2C_Mem_Write_IT(&I2c3Handle, EEPROM_DEVICE_PAGE0, eeprom_ang_tab&0xFF, I2C_MEMADD_SIZE_8BIT, &eeprom_buf[eeprom_ang_tab], BIT_TAB_SIZE*4+2); //write ang_tab to FLASH

              encoder_state=0x40;                                               //set "ang_tab calibration ready" status

              auto_cal=0;                                                       //reset ang_tab calibration flag
                
              
            }
            
          }
          
        }
        else {

          if ( pix_rdy_tab2[cal_sector]==0 ) {

            pix_dif_sum2[cal_sector]+=pix_dif;
            pix_dif_num2[cal_sector]++;

          }


        }
        
          
    }
    

  }      
    






  


}  
  
  












/**
  * @brief  САМОКАЛИБРОВКА offset - поиск точки съёма угла на линейке.
  *
  * Задача одномерной минимизации: найти целое offset, при котором размах
  * остаточной ошибки min_max (см. find_avg) минимален.
  *
  * Реализована конечным автоматом по offset_phase; на каждую пробу тратится
  * offset_avg_num (=3) полных оборота вала:
  *   0   - применить offset = offset_cur;
  *   1   - дождаться avg_minmax_num > 3, записать min_max в offset_minmax[],
  *         сдвинуться к следующему значению до offset_end; затем найти минимум
  *         в окне [offset_start, offset_end] = [63, 65];
  *         если минимум на ЛЕВОМ краю (<64) - расширять окно влево (фазы 2/3)
  *         до offset_start-offset_snum = 56;
  *         если на ПРАВОМ (>64) - вправо (фазы 4/5) до offset_end+offset_snum = 72;
  *         если ровно 64 - сразу на фазу 100;
  *   2/3, 4/5 - те же "применить и измерить" при расширении окна;
  *   100 - offset = offset_found, запись 4 байт + CRC16 в EEPROM (стр. 6, адрес
  *         1536), encoder_state = 0x60, start_offset_cal = 0, переход на 101;
  *   101 - калибровка завершена, ничего не делать.
  *
  * Запуск - установка флагов (обнуляются offset_phase, avg_minmax_num,
  * массив offset_minmax[], encoder_state = 0x50; ранее - команда SPI 0x13,
 * внешний командный канал удалён).
  * Время работы: примерно (число проб) * 3 оборота, то есть вал надо вращать
  * равномерно всё это время.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void offset_cal() {    

  uint16_t crc16;
    
  if (start_offset_cal>0) {
      
   switch (offset_phase)  {
    
    case 0: 
      
      offset=offset_cur;
    
      offset_phase=1;
    
      break;
    
      
    case 1: 
      
      if (avg_minmax_num>offset_avg_num) {


        if ((offset_cur>=offset_start)&&(offset_cur<=offset_end)){
          
          offset_min[offset_cur]=avg_min;
          offset_max[offset_cur]=avg_max;
          offset_minmax[offset_cur]=min_max;
        
        }
        
        avg_minmax_num=0;

        
        
        if (offset_cur<offset_end) {
          
          offset_cur++;
          offset_phase=0;
          
        }
        else {
         

          offset_tmp=1;
          offset_found=offset_start;
          for (int i=offset_start;i<=offset_end;i++) {
            if (offset_minmax[i]<offset_tmp) {
              offset_found=i;
              offset_tmp=offset_minmax[i];
            }
            
          }
          


          if (offset_found<64) {

            offset_cur=offset_found;
            offset_cur--;
            offset_phase=2;

          }

          else if(offset_found>64) {

            offset_cur=offset_found;
            offset_cur++;
            offset_phase=4;

          }

          else {

            offset_phase=100;


          }

          
        }
        
        
      }
      
      break;

      


    case 2: 
      
      offset=offset_cur;
    
      offset_phase=3;
    
      break;


    case 3:

     
      if (avg_minmax_num>offset_avg_num) {
       
        offset_min[offset_cur]=avg_min;
        offset_max[offset_cur]=avg_max;
        offset_minmax[offset_cur]=min_max;
        
        avg_minmax_num=0;
        
        if (offset_cur>offset_start-offset_snum) {
          
          offset_cur--;
          offset_phase=2;
          
        }
        else {

          offset_tmp=1;
          offset_found=offset_start-offset_snum;
          for (int i=offset_start-offset_snum;i<=offset_end;i++) {
            if (offset_minmax[i]<offset_tmp) {
              offset_found=i;
              offset_tmp=offset_minmax[i];
            }
            
          }

          offset_phase=100;

          
        }
        
        
      }
      
      break;



    case 4: 
      
      offset=offset_cur;
    
      offset_phase=5;
    
      break;


    case 5:

      if (avg_minmax_num>offset_avg_num) {
       
        offset_min[offset_cur]=avg_min;
        offset_max[offset_cur]=avg_max;
        offset_minmax[offset_cur]=min_max;
        
        avg_minmax_num=0;
        
        if (offset_cur<offset_end+offset_snum) {
          
          offset_cur++;
          offset_phase=4;
          
        }
        else {

          offset_tmp=1;
          offset_found=offset_start;
          for (int i=offset_start;i<=offset_end+offset_snum;i++) {
            if (offset_minmax[i]<offset_tmp) {
              offset_found=i;
              offset_tmp=offset_minmax[i];
            }
            
          }

          offset_phase=100;

          
        }
        
        
      }
      
      break;


   



    case 100: 
        offset=offset_found;                                                    //use found offset

        memcpy(&eeprom_buf[eeprom_offset],&offset,4);                           //copy offset to write buffer
        crc16=0xFFFF;                                                           //CRC16 calc
        for (int i=eeprom_offset;i<eeprom_offset+4;i++) {
          crc16 = crc_16_step (eeprom_buf[i],crc16);
        }
        eeprom_buf[eeprom_offset_crc+0]=crc16&0xff;
        eeprom_buf[eeprom_offset_crc+1]=crc16>>8;
        i2c3_tx_wp=0;                                                           //tx_wp flag reset
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);                           //WP disable
        HAL_I2C_Mem_Write_IT(&I2c3Handle, EEPROM_DEVICE_PAGE6, eeprom_offset&0xFF, I2C_MEMADD_SIZE_8BIT, &eeprom_buf[eeprom_offset], 6); //write offset to FLASH

        encoder_state=0x60;                                                     //set "offset calibration ready" status
        offset_phase=101;

        start_offset_cal=0;                                                     //reset offset calibration flag

        break;


    case 101: 
        break;
   
    



    

     default:
        break;
  }
    
    
    
}  
    
    
}    
    
    





/**
  * @brief  САМОКАЛИБРОВКА buf_k[] - выравнивание чувствительности пикселей.
  *
  * ЗАЧЕМ. Разброс чувствительности пикселей линейки (PRNU), виньетирование
  * оптики и загрязнения делают профиль одного и того же репера разным в разных
  * местах линейки, а BR4 чувствителен к асимметрии профиля - появляется
  * периодическая ошибка угла. buf_k[i] выравнивает отклик пикселей.
  *
  * Фазы (anglek_phase), запуск установкой флагов (ранее - команда SPI 0x14,
 * encoder_state = 0x70; внешний командный канал удалён):
  *   0   - сбросить buf_k[15..119] = 1, начать накопление. Параллельно
  *         copy_data() заполняет buf_x3[i] пиковой яркостью пикселя i
  *         (пиковый детектор работает, пока anglek_phase < 4);
  *   1   - после 3 оборотов (все пиксели успели "увидеть" репер) выровнять
  *         отклик: buf_k[i] = pix_max / buf_x3[i]. Заодно найти 4 самых
  *         "слабых" пикселя pix_min1..4_num в зоне 30..110;
  *   2   - итеративное уточнение: повторять buf_k[i] *= pix_max/buf_x3[i],
  *         пока размах pix_min_max = pix_max - pix_min не станет < 100 отсчётов;
  *   3/4, 5/6, 7/8 - для трёх самых слабых пикселей (pix_min1..3_num) прямым
  *         перебором трёх значений усиления (шаг *1.04) выбирается то, при
  *         котором min_max (размах ошибки угла за оборот) минимален;
  *   100 - запись buf_k (128 float = 512 байт) + CRC16 в EEPROM (стр. 3,
  *         адрес 768), encoder_state = 0x80, backlight_width_en = 1
  *         (АРУ подсветки снова разрешена), start_angk_cal = 0;
  *   101 - завершено.
  *
  * Команда 0x14 перед стартом обнуляет buf_x3[] и ЗАПРЕЩАЕТ АРУ подсветки
  * (backlight_width_en = 0) - иначе пиковый детектор мерил бы меняющуюся яркость.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void angk_cal() {    

  uint16_t crc16;
    
  if (start_angk_cal>0) {
      
   switch (anglek_phase)  {


    case 0: 

      for (int i=15;i<120;i++) {
        buf_k[i]=1;
      }
      avg_minmax_num=0;
      anglek_phase=1;
    
      break;



    case 1: 
      
      if (avg_minmax_num>offset_avg_num) {

        pix_max=0;
        pix_min=4095;
        for (int i=16;i<120;i++) {
          if (buf_x3[i]>pix_max) {
            pix_max=buf_x3[i];
          }
          if (buf_x3[i]<pix_min) {
            pix_min=buf_x3[i];
          }
        }
        pix_min_max=pix_max-pix_min;


        for (int i=16;i<120;i++) {
          buf_k[i]=pix_max/buf_x3[i];
        }




        pix_min1_val=4096;
        for (int i=30;i<110;i++) {
          if (buf_x3[i]<pix_min1_val) {
            pix_min1_val=buf_x3[i];
            pix_min1_num=i;
          }
        }

        pix_min2_val=4096;
        for (int i=30;i<110;i++) {
          if ( (buf_x3[i]<pix_min2_val) && (i!=pix_min1_num) ) {
            pix_min2_val=buf_x3[i];
            pix_min2_num=i;
          }
        }

        pix_min3_val=4096;
        for (int i=30;i<110;i++) {
          if ( (buf_x3[i]<pix_min3_val) && (i!=pix_min1_num) && (i!=pix_min2_num) ) {
            pix_min3_val=buf_x3[i];
            pix_min3_num=i;
          }
        }

        pix_min4_val=4096;
        for (int i=30;i<110;i++) {
          if ( (buf_x3[i]<pix_min4_val) && (i!=pix_min1_num) && (i!=pix_min2_num) && (i!=pix_min3_num) ) {
            pix_min4_val=buf_x3[i];
            pix_min4_num=i;
          }
        }



        avg_minmax_num=0;
        anglek_phase=2;
           
    
      }
      
      break;




    case 2: 

      if (avg_minmax_num>offset_avg_num) {


        pix_max=0;
        pix_min=4095;
        for (int i=16;i<120;i++) {
          if (buf_x3[i]>pix_max) {
            pix_max=buf_x3[i];
          }
          if (buf_x3[i]<pix_min) {
            pix_min=buf_x3[i];
          }
        }
        pix_min_max=pix_max-pix_min;


        if (pix_min_max<100) {
          anglek_phase=3;
        }
        else {

          for (int i=16;i<120;i++) {
            buf_k[i]=buf_k[i]*(pix_max/buf_x3[i]);
          }

          avg_minmax_num=0;
          anglek_phase=2;
        }


      }

      

      break;



    case 3: 
      
      avg_minmax_num=0;
      for (int i=anglek_start;i<=anglek_end;i++) {
        anglek_minmax[i]=1;
      }
      anglek_cur=anglek_start;

      buf_k_num=pix_min1_num;

      anglek_data[anglek_start]=buf_k[buf_k_num];
      for (int i=anglek_start+1;i<anglek_end+1;i++) {
        anglek_data[i]=anglek_data[i-1]*1.04;
      }

      buf_k[buf_k_num]=anglek_data[anglek_cur];

      anglek_phase++;

      break;
   


    case 4: 
      
      if (avg_minmax_num>offset_avg_num) {
        

        if ((anglek_cur>=anglek_start)&&(anglek_cur<=anglek_end)){
          
          anglek_minmax[anglek_cur]=min_max;
          
        }
      
        avg_minmax_num=0;

        if (anglek_cur<anglek_end) {
          
          anglek_cur++;

          buf_k[buf_k_num]=anglek_data[anglek_cur];

          
        }
        else {

          anglek_tmp=1;
          anglek_found=anglek_start;
          for (int i=anglek_start;i<=anglek_end;i++) {
            if (anglek_minmax[i]<anglek_tmp) {
              anglek_found=i;
              anglek_tmp=anglek_minmax[i];
            }
            
          }
     
          buf_k[buf_k_num]=anglek_data[anglek_found];                             //use found angle_k
      
          anglek_phase++;
          
        }


      }
      
      break;






    case 5: 
      
      avg_minmax_num=0;
      for (int i=anglek_start;i<=anglek_end;i++) {
        anglek_minmax[i]=1;
      }
      anglek_cur=anglek_start;

      buf_k_num=pix_min2_num;

      anglek_data[anglek_start]=buf_k[buf_k_num];
      for (int i=anglek_start+1;i<anglek_end+1;i++) {
        anglek_data[i]=anglek_data[i-1]*1.04;
      }

      buf_k[buf_k_num]=anglek_data[anglek_cur];

      anglek_phase++;

      break;


    case 6: 
      
      if (avg_minmax_num>offset_avg_num) {
        

        if ((anglek_cur>=anglek_start)&&(anglek_cur<=anglek_end)){
          
          anglek_minmax[anglek_cur]=min_max;
          
        }
      
        avg_minmax_num=0;

        if (anglek_cur<anglek_end) {
          
          anglek_cur++;

          buf_k[buf_k_num]=anglek_data[anglek_cur];

          
        }
        else {

          anglek_tmp=1;
          anglek_found=anglek_start;
          for (int i=anglek_start;i<=anglek_end;i++) {
            if (anglek_minmax[i]<anglek_tmp) {
              anglek_found=i;
              anglek_tmp=anglek_minmax[i];
            }
            
          }
     
          buf_k[buf_k_num]=anglek_data[anglek_found];                             //use found angle_k
      
          anglek_phase++;


          
        }


      }
      
      break;








    case 7: 
      
      avg_minmax_num=0;
      for (int i=anglek_start;i<=anglek_end;i++) {
        anglek_minmax[i]=1;
      }
      anglek_cur=anglek_start;

      buf_k_num=pix_min3_num;

      anglek_data[anglek_start]=buf_k[buf_k_num];
      for (int i=anglek_start+1;i<anglek_end+1;i++) {
        anglek_data[i]=anglek_data[i-1]*1.04;
      }

      buf_k[buf_k_num]=anglek_data[anglek_cur];

      anglek_phase++;

      break;



    case 8: 
      
      if (avg_minmax_num>offset_avg_num) {
        

        if ((anglek_cur>=anglek_start)&&(anglek_cur<=anglek_end)){
          
          anglek_minmax[anglek_cur]=min_max;
          
        }
      
        avg_minmax_num=0;

        if (anglek_cur<anglek_end) {
          
          anglek_cur++;

          buf_k[buf_k_num]=anglek_data[anglek_cur];

          
        }
        else {

          anglek_tmp=1;
          anglek_found=anglek_start;
          for (int i=anglek_start;i<=anglek_end;i++) {
            if (anglek_minmax[i]<anglek_tmp) {
              anglek_found=i;
              anglek_tmp=anglek_minmax[i];
            }
            
          }
     
          buf_k[buf_k_num]=anglek_data[anglek_found];                             //use found angle_k
      
          anglek_phase++;
          anglek_phase=100;

          
        }


      }
      
      break;



    case 100: 

        memcpy(&eeprom_buf[eeprom_buf_k],&buf_k,512);                           //copy buf_k to write buffer
        crc16=0xFFFF;                                                           //CRC16 calc
        for (int i=eeprom_buf_k;i<eeprom_buf_k+512;i++) {
          crc16 = crc_16_step (eeprom_buf[i],crc16);
        }
        eeprom_buf[eeprom_buf_k_crc+0]=crc16&0xff;
        eeprom_buf[eeprom_buf_k_crc+1]=crc16>>8;
        i2c3_tx_wp=0;                                                           //tx_wp flag reset
        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);                           //WP disable
        HAL_I2C_Mem_Write_IT(&I2c3Handle, EEPROM_DEVICE_PAGE3, eeprom_buf_k&0xFF, I2C_MEMADD_SIZE_8BIT, &eeprom_buf[eeprom_buf_k], 514); //write buf_k to FLASH

        encoder_state=0x80;                                                     //set "angle_k calibration ready" status
        backlight_width_en=1;
        anglek_phase=101;

        start_angk_cal=0;                                                       //reset angle_k calibration flag

        break;


    case 101: 
        break;
   
    

     default:
        break;
  }
    
    
    
}  
    
    
}    
    













/**
  * @brief  АРУ подсветки: удержание амплитуды сигнала в рабочем диапазоне АЦП.
  *
  * Медленный интегральный регулятор (+-1 тик за 1000 кадров, около 12 раз в
  * секунду), управляемая величина - ДЛИТЕЛЬНОСТЬ импульса светодиода
  * backlight_width_ticks (тики TIM4, 1 тик = 1/108 мкс), то есть экспозиция:
  *     avg_max_data = среднее max_data за avg_amaxX_num = 1000 кадров
  *     avg_max_data < max_level_pix - 200  -> ширина++  (темно)
  *     avg_max_data > max_level_pix        -> ширина--  (светло)
  * Ограничения BACKLIGHT_WIDTH_MIN_TICKS..MAX (2..9400 тиков, т.е. ~0.02..87 мкс).
  * Заодно подстраивается тёмновой уровень: adc_offset = avg_max_data / 5.
  * Медленность регулятора намеренная: он не должен реагировать на смену сектора,
  * только на загрязнение оптики, старение светодиода и температуру.
  *
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void dac_ctrl() {

   adc_offset=avg_max_data/5;                                                   //ADC offset calculation
   max_level_pix = max_level - adc_offset;                                      //max_level_pix calculation
        
   lasdac_counter++;
   avg_amaxX_summ += max_data;
    
    if(lasdac_counter>=avg_amaxX_num) {      
      lasdac_counter=0;    
      avg_max_data = avg_amaxX_summ/avg_amaxX_num;
      if (avg_max_data < max_level_pix - 200) {
        if (backlight_width_ticks < BACKLIGHT_WIDTH_MAX_TICKS) {
          if (backlight_width_en) backlight_width_ticks++;
        }
      }
      else if (avg_max_data > max_level_pix) {
        if (backlight_width_ticks > BACKLIGHT_WIDTH_MIN_TICKS) {
          if (backlight_width_en) backlight_width_ticks--;
        }
      }
      avg_amaxX_summ = 0;
  }

  
}























/**
  * @brief  Блокирующая задержка на заданное число тиков TIM4 (1 тик = 1/108 мкс).
  *
  * Используется только в прерывании DMA2_Stream3_IRQHandler для выдержки tqt
  * перед импульсом SI фотолинейки. Прерывание по UPDATE на время задержки
  * запрещается, чтобы не сработал обработчик подсветки, и восстанавливается в конце.
  * Ожидание активным опросом флага (polling) - 20 мкс, заводить планировщик дороже.
  *
  * @param  ticks длительность задержки в тиках TIM4
  * @retval None
  */
static void DelayTim4Ticks(uint16_t ticks)
{
  if (ticks == 0) ticks = 1;

  LL_TIM_DisableCounter(TIM4);
  LL_TIM_DisableIT_UPDATE(TIM4);
  LL_TIM_SetCounter(TIM4, ticks);
  LL_TIM_ClearFlag_UPDATE(TIM4);
  LL_TIM_EnableCounter(TIM4);

  while (LL_TIM_IsActiveFlag_UPDATE(TIM4) == 0) {
  }

  LL_TIM_DisableCounter(TIM4);
  LL_TIM_ClearFlag_UPDATE(TIM4);
  LL_TIM_EnableIT_UPDATE(TIM4);
}

static void Backlight_StartTimer(uint16_t ticks)
{
  if (ticks == 0) ticks = 1;

  LL_TIM_DisableCounter(TIM4);
  LL_TIM_SetCounter(TIM4, ticks);
  LL_TIM_ClearFlag_UPDATE(TIM4);
  LL_TIM_EnableCounter(TIM4);
}

/**
  * @brief  Запуск одновибратора подсветки, привязанного к началу кадра (импульсу SI).
  *
  * Формирует на PA15 импульс: сначала пауза backlight_delay_ticks, затем вспышка
  * длительностью backlight_width_ticks. Обе выдержки отсчитывает TIM4 в режиме
  * вычитающего счёта с прерыванием по UPDATE (см. TIM4_IRQHandler и автомат
  * backlight_timer_state: IDLE -> DELAY -> WIDTH -> IDLE).
  * Привязка вспышки к SI делает экспозицию одинаковой для всех пикселей кадра.
  */
static void Backlight_StartFromSI(void)
{
  LL_TIM_DisableCounter(TIM4);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);

  if (backlight_width_ticks == 0) {
    backlight_timer_state = BACKLIGHT_STATE_IDLE;
    return;
  }

  if (backlight_delay_ticks == 0) {
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
    backlight_timer_state = BACKLIGHT_STATE_WIDTH;
    Backlight_StartTimer(backlight_width_ticks);
  } else {
    backlight_timer_state = BACKLIGHT_STATE_DELAY;
    Backlight_StartTimer(backlight_delay_ticks);
  }
}
/**
  * @brief  Загрузка калибровок из EEPROM и начальная инициализация переменных.
  *
 * Порядок:
 *   1) ОДНО блокирующее чтение всей EEPROM (2048 байт) в eeprom_buf[];
 *   2) для каждого из четырёх блоков (ang_tab, buf_k, offset, lasdac)
 *      пересчитывается CRC16 и сравнивается с хранимой рядом контрольной суммой.
 *      Совпала - блок копируется в рабочую переменную и ставится флаг *_rdy = 1.
 *      Не совпала - блок молча игнорируется и остаётся значение по умолчанию,
 *      зашитое в код. Это защита от чтения "мусора" после сбоя записи;
 *   3) чтение перемычки конфигурации PB8 -> config_state (ориентация головки);
 *   4) заполнение таблицы delta_cor[] - табулированной синусоиды компенсации
 *      субпиксельной ошибки (используется неиспользуемыми вариантами BR6/BR8).
  *
  * @retval None
  */
void init_vars() {



  //EEPROM vars init
  HAL_I2C_Mem_Read(&I2c3Handle, EEPROM_DEVICE_PAGE0, 0, I2C_MEMADD_SIZE_8BIT, &eeprom_buf[0], eeprom_len, 1000);

  //Init ang_tab
  eeprom_crc16=0xFFFF;                                                          //CRC16 calc
  for (int i=eeprom_ang_tab;i<eeprom_ang_tab+BIT_TAB_SIZE*4;i++) {
    eeprom_crc16 = crc_16_step (eeprom_buf[i],eeprom_crc16);
  }
  eeprom_crc16_ang_tab=eeprom_buf[eeprom_ang_tab_crc] | (eeprom_buf[eeprom_ang_tab_crc+1]<<8);
  if (eeprom_crc16==eeprom_crc16_ang_tab) {
    memcpy(&ang_tab,&eeprom_buf[eeprom_ang_tab],BIT_TAB_SIZE*4);
    ang_tab_rdy=1;
  }

  //Init buf_k
  eeprom_crc16=0xFFFF;                                                          //CRC16 calc
  for (int i=eeprom_buf_k;i<eeprom_buf_k+512;i++) {
    eeprom_crc16 = crc_16_step (eeprom_buf[i],eeprom_crc16);
  }
  eeprom_crc16_buf_k=eeprom_buf[eeprom_buf_k_crc] | (eeprom_buf[eeprom_buf_k_crc+1]<<8);
  if (eeprom_crc16==eeprom_crc16_buf_k) {
    memcpy(&buf_k,&eeprom_buf[eeprom_buf_k],512);
    buf_k_rdy=1;
  }

  //Init offset
  eeprom_crc16=0xFFFF;                                                          //CRC16 calc
  for (int i=eeprom_offset;i<eeprom_offset+4;i++) {
    eeprom_crc16 = crc_16_step (eeprom_buf[i],eeprom_crc16);
  }
  eeprom_crc16_offset=eeprom_buf[eeprom_offset_crc] | (eeprom_buf[eeprom_offset_crc+1]<<8);
  if (eeprom_crc16==eeprom_crc16_offset) {
    memcpy(&offset,&eeprom_buf[eeprom_offset],4);
    offset_rdy=1;
  }

  //Init lasdac
  eeprom_crc16=0xFFFF;                                                          //CRC16 calc
  for (int i=eeprom_lasdac;i<eeprom_lasdac+2;i++) {
    eeprom_crc16 = crc_16_step (eeprom_buf[i],eeprom_crc16);
  }
  eeprom_crc16_lasdac=eeprom_buf[eeprom_lasdac_crc] | (eeprom_buf[eeprom_lasdac_crc+1]<<8);
  if (eeprom_crc16==eeprom_crc16_lasdac) {
    memcpy(&lasdac,&eeprom_buf[eeprom_lasdac],2);
    lasdac_rdy=1;
  }

  //read config
  if (LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_8)) {
    config_state=1;
  }
  else {
    config_state=0;
  }


  //compensation BR4
  phase=0;
  for (unsigned char j=0;j<200;j++) {
   delta_cor[j]=0.01*sin((j-100)*3.1415926/50.+phase);
  }


}

  







/**
  * @brief  Точка входа. Инициализация и бесконечный рабочий цикл.
  *
  * Архитектура программы - СУПЕРЦИКЛ без ОС:
  *   - прерывания выполняют только жёсткое реальное время (тактирование линейки,
  *     перекладывание кадра из DMA);
  *   - вся арифметика идёт в while(1) по флагу adc_rdy, выставленному из
  *     DMA2_Stream3_IRQHandler. Один проход цикла = один кадр линейки (~80 мкс).
  *
  * Порядок инициализации важен:
  *   HAL_Init -> SystemClock_Config (216 МГц от HSE 25 МГц через PLL) ->
  *   включение кэшей I-Cache/D-Cache (ускорение Cortex-M7; из-за D-Cache дальше
  *   в коде встречаются SCB_InvalidateDCache_by_Addr / SCB_CleanDCache_by_Addr -
  *   DMA пишет в ОЗУ мимо кэша, и кэш надо синхронизировать вручную) ->
  *   MX_*_Init (периферия) -> init_vars (калибровки из EEPROM) ->
  *   включение счётчика тактов DWT -> __enable_irq().
  *
  * Угол обновляется в cur_ang_E только если errorflag == 0; иначе наружу
  * продолжает отдаваться последнее достоверное значение.
  *
  * @retval int (не возвращается)
  */
int main(void)
{

  // MCU Configuration--------------------------------------------------------

  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();

  // Configure the system clock
  SystemClock_Config();

  // Enable I-Cache
  SCB_EnableICache();
  SCB_EnableDCache();

  // Initialize all configured peripherals
  MX_GPIO_Init();
  MX_DAC_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C3_Init();

  init_vars();                                                                  // init variables

  /* Инициализировать Modbus RTU slave: индивидуальный адрес головки, USART1,
     115200/8E1; библиотека подключит IRQ USART1 и таймер паузы TIM2. */
  if (eMBInit(MB_RTU, MB_SLAVE_ADDRESS, 1, MB_BAUD, MB_PAR_EVEN, 1) != MB_ENOERR ||
      eMBEnable() != MB_ENOERR) Error_Handler();

  KIN1_InitCycleCounter();                                                      // enable DWT hardware
  KIN1_EnableCycleCounter();

  __enable_irq();                                                               // enable interrupts
  cycles_max=0;

  // Infinite loop
  while (1)
  {
    /* Non-blocking FreeModbus foreground state machine. It handles FC04
       STATUS/DATA and FC06 SNAP; UART and t3.5 interrupts only queue events. */
    ModbusSlave_Poll();

    /* Флаг выставляется из DMA2_Stream3_IRQHandler: в buf_x0 лежит свежий
       кадр линейки. Весь расчёт ниже должен уложиться в период кадра (~80 мкс),
       контроль - по счётчику тактов DWT (cycles / cycles_max).              */
    if (adc_rdy == 1) {                                                         // ADC frame ready flag

      copy_data();                                                              // (??? cycles) copy TAOS data

      find_startpixel();                                                        // (??? cycles) find 1-st startpixel

      s_l1 = mes_piks (startpixel1);                                            // (440 cycles) calculate 1-st subpixel coordinates

      s_l2 = mes_piks (startpixel2);                                            // (308 cycles) calculate 2-nd subpixel coordinates

      find_datablock();                                                         // (??? cycles) finding data bits

      calc_sector();                                                            // (??? cycles) calculate sector number

      err_corr();                                                               // (??? cycles) error control and correction

      /* Угол и все калибровки считаются ТОЛЬКО по достоверному кадру.
         При errorflag==1 наружу продолжает отдаваться прежнее cur_ang_E.     */
      if (errorflag==0) {

        calc_ang();                                                             // (??? cycles) calculate angle

        find_avg();                                                             // average calculation

        offset_cal();                                                           // offset calibration

        angk_cal();                                                             // angk calibration
        
        angtab_cal();                                                           // angtab calibration

        cur_ang_E=cur_ang_X;

      }


      /* Обновить текущий результат только после окончания расчёта кадра.
         При errorflag!=0 сохранённый старый угол НЕ считается валидным SNAP. */
      snapshot_publish(cur_ang_E, (uint16_t)sector, errorflag,
                       encoder_state, HAL_GetTick());

      dac_ctrl();                                                               // DAC ctrl


      SCB_InvalidateDCache_by_Addr((uint32_t *)aADCxConvertedData,((2+31)/32)*32);//force to let update caches again with memory content to see the changes
      Vsense=(aADCxConvertedData[0]*3.3f)/4095.0f;                              // calculate Temperature
      Temperature=((Vsense-V25)*1000/Avg_Slope)+25.0f;

      /* Publish a coherent, fully calculated live frame. A later broadcast
         SNAP copies this record (including buf_x0) to a frozen Modbus frame. */
      ModbusSlave_Publish(cur_ang_E, sector, data_byte, startpixel1, startpixel2,
                          errorflag, Temperature, buf_x0);

      cycles = KIN1_GetCycleCounter();                                          // get cycle counter
      KIN1_DisableCycleCounter();                                               // disable counting if not used any more
      if (cycles>cycles_max) cycles_max=cycles;                                 // find maximum cycle counter

      adc_rdy=0;                                                                // ADC frame ready flag

    }

    /* Неблокирующий разбор событий Modbus; обработчик SNAP фиксирует последний кадр. */
    if (eMBPoll() != MB_ENOERR) Error_Handler();

  }

}

/**
  * @brief  Конфигурация тактирования: 216 МГц - максимум для STM32F722.
  *
  * HSE (внешний кварц) 25 МГц -> PLL: /M=25 -> 1 МГц, *N=432 -> 432 МГц,
  * /P=2 -> SYSCLK = 216 МГц. AHB /1 = 216 МГц, APB1 /4 = 54 МГц (таймеры 108 МГц),
  * APB2 /2 = 108 МГц (таймеры 216 МГц).
  * На такой частоте обязательны 7 тактов задержки Flash (LATENCY_7), режим
  * питания Scale 1 и Over-drive - иначе ядро не запустится стабильно.
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_7)
  {
  Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_PWR_EnableOverDriveMode();
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 432, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(216000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(216000000);
}








/**
  * @brief  ADC1 + DMA2_Stream0: непрерывное измерение температуры кристалла.
  *
  * Канал LL_ADC_CHANNEL_TEMPSENSOR - внутренний термодатчик МК. Разрешение 12 бит,
  * время выборки 480 тактов (у внутренних каналов большое выходное сопротивление).
  * Запуск программный + ContinuousMode, DMA в кольцевом режиме бесконечно
  * обновляет aADCxConvertedData[0] - процессор просто читает актуальное значение.
  * Температура нужна как служебный параметр для телеметрии (внешний интерфейс
 * в разработке) и как
  * косвенный признак теплового дрейфа геометрии.
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
//## Configuration of DMA ##################################################
  // Enable the peripheral clock of DMA 
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
  // Configure the DMA transfer
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_0, LL_DMA_CHANNEL_0);
  LL_DMA_ConfigTransfer(DMA2,
                        LL_DMA_STREAM_0,
                        LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                        LL_DMA_MODE_CIRCULAR              |                     //  - DMA transfer in circular mode to match with ADC configuration:
                        LL_DMA_PERIPH_NOINCREMENT         |                     //  - DMA transfer from ADC without address increment.
                        LL_DMA_MEMORY_INCREMENT           |                     //  - DMA transfer to memory with address increment.
                        LL_DMA_PDATAALIGN_HALFWORD        |                     //  - DMA transfer from ADC by half-word to match with ADC configuration
                        LL_DMA_MDATAALIGN_HALFWORD        |                     //  - DMA transfer to memory by half-word to match with ADC conversion data
                        LL_DMA_PRIORITY_LOW               );
  // Set DMA transfer addresses of source and destination
  LL_DMA_ConfigAddresses(DMA2,
                        LL_DMA_STREAM_0,
                         LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                         (uint32_t)&aADCxConvertedData[0],
                         LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  // Set DMA transfer size
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_0, 1);
  //## Activation of DMA #####################################################
  // Enable the DMA transfer
  LL_DMA_EnableStream(DMA2,LL_DMA_STREAM_0);
  
  /*## Configuration of ADC1 ##################################################*/
  /* Enable ADC clock (core clock) */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
  /* Note: Call of the functions below are commented because they are       */
  /*       useless in this example:                                         */
  /*       setting corresponding to default configuration from reset state. */
    
  /* Set ADC clock (conversion clock) common to several ADC instances */
  LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
  /*## Configuration of ADC hierarchical scope: multimode ####################*/
  /* Set ADC measurement path to internal channels */
  LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR);  
  /*## Configuration of ADC hierarchical scope: ADC instance #################*/
  /* Set ADC data resolution */
  LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
  /* Set ADC conversion data alignment */
  LL_ADC_SetResolution(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
  /* Set Set ADC sequencers scan mode, for all ADC groups                   */
  /* (group regular, group injected).                                       */
  LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_DISABLE);
  // Set ADC group regular trigger source
  LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
  // Set ADC group regular continuous mode
  LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);
  // Set ADC group regular conversion data transfer
  LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
  // Set ADC group regular sequencer length and scan direction
  LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
  // Set ADC group regular sequence: channel on the selected sequence rank.
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
  /*## Configuration of ADC hierarchical scope: channels #####################*/
  /* Set ADC channels sampling time */
  /* Note: Considering interruption occurring after each ADC group          */
  /*       regular sequence conversions                                     */
  /*       (IT from DMA transfer complete),                                 */
  /*       select sampling time and ADC clock with sufficient               */
  /*       duration to not create an overhead situation in IRQHandler.      */

  /* Note: Set long sampling time due to internal channels (VrefInt,        */
  /*       temperature sensor) constraints.                                 */
  /*       Refer to description of function                                 */
  /*       "LL_ADC_SetChannelSamplingTime()".                               */
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_480CYCLES);
  // Enable the ADC1 instance.
  LL_ADC_Enable(ADC1);
  LL_ADC_REG_StartConversionSWStart(ADC1);
 }





/**
  * @brief  ADC2 + DMA2_Stream3: оцифровка видеосигнала фотолинейки TSL1401CL.
  *
  * Вход: PA2 = ADC2_IN2 (аналоговый выход AO линейки).
  * Триггер: LL_ADC_REG_TRIG_EXT_EXTI_LINE11 по СПАДУ - линия EXTI11 заведена на
  * PB11, куда подан тактовый сигнал линейки T-CLK. Каждый такт линейки = ровно
  * одно преобразование, джиттер определяется только аппаратурой.
  * Время выборки всего 3 такта АЦП - источник низкоомный, а темп высокий.
  * DMA: кольцевой режим + DOUBLE BUFFER (ADC_VAL / ADC_VAL2), 132 полуслова на
  * кадр, высший приоритет потока, прерывание по Transfer Complete с приоритетом
  * NVIC 0 (самый высокий) - обработчик должен успеть выдать SI до следующего кадра.
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{
  /* (1) Enable the clock of DMA2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

  /* (2) Configure the DMA functionnal parameters */
  LL_DMA_EnableDoubleBufferMode(DMA2, LL_DMA_STREAM_3);
  LL_DMA_SetMemory1Address(DMA2, LL_DMA_STREAM_3, (uint32_t)&ADC_VAL2[0]);
  LL_DMA_SetCurrentTargetMem(DMA2, LL_DMA_STREAM_3, LL_DMA_CURRENTTARGETMEM0);
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_3, LL_DMA_CHANNEL_1);
  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_3, LL_DMA_PRIORITY_VERYHIGH);
  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MODE_CIRCULAR);
  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_HALFWORD);
  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_HALFWORD);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_3, 132);
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_3, (uint32_t)&ADC_VAL[0]);
  LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_3, (uint32_t) &ADC2->DR);
  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_3);
  LL_DMA_SetMemoryBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_MBURST_SINGLE);
  LL_DMA_SetPeriphBurstxfer(DMA2, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);

  /* (3) Configure NVIC for DMA transfer complete/error interrupts */
  LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_3);
  NVIC_SetPriority(DMA2_Stream3_IRQn, 0);
  NVIC_EnableIRQ(DMA2_Stream3_IRQn);

  /* Start the DMA transfer */
  LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_3);
 

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
  
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**ADC2 GPIO Configuration  
  PA2   ------> ADC2_IN2 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Common config 
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
  LL_ADC_Init(ADC2, &ADC_InitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_EXTI_LINE11;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
  LL_ADC_REG_Init(ADC2, &ADC_REG_InitStruct);
  LL_ADC_REG_SetFlagEndOfConversion(ADC2, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
  LL_ADC_DisableIT_EOCS(ADC2);
  ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC2), &ADC_CommonInitStruct);
  LL_ADC_REG_StartConversionExtTrig(ADC2, LL_ADC_REG_TRIG_EXT_FALLING);

  // Configure Regular Channel 
  LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_2);
  LL_ADC_SetChannelSamplingTime(ADC2, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_3CYCLES);


  LL_ADC_Enable(ADC2);
}






/**
  * @brief  TIM3 CH1 (PC6): генератор тактового сигнала T-CLK для фотолинейки.
  *
  * Period = 32, предделитель 0, тактирование таймеров APB1 = 108 МГц:
  *   частота обновления = 108 МГц / 33 = 3.27 МГц,
  *   режим OCMODE_TOGGLE переключает вывод раз за период =>
  *   T-CLK = 1.64 МГц (предел TSL1401CL - 8 МГц).
  * Полный кадр 128 пикселей = около 78 мкс, то есть примерно 12800 кадров/с.
  * Тот же сигнал заведён на PB11 и служит триггером ADC2, а в обработчике кадра
  * состояние PC6 опрашивается напрямую для привязки импульса SI к фронту такта.
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 32;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  HAL_TIM_Base_Init(&htim3);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);


  HAL_TIM_OC_Init(&htim3);


  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_DISABLE;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;

  HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  HAL_TIM_OC_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);



  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOC_CLK_ENABLE();
  //TIM3 GPIO Configuration    
  //PC6     ------> TIM3_CH1 
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


  // Enable counter
  HAL_TIM_Base_Start(&htim3);
  // Enable output channel 1
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);



}






/**
  * @brief  TIM4: одновибратор для задержки и длительности вспышки подсветки (PA15).
  *
  * Вычитающий счёт, предделитель 0 (1 тик = 1/108 мкс = 9.26 нс), автоперезагрузка
  * 0xFFFF. Счётчик заряжается нужным числом тиков и по достижении нуля выдаёт
  * прерывание UPDATE -> TIM4_IRQHandler. Приоритет NVIC 1 - ниже кадра линейки.
  * Этот же таймер используется для блокирующей выдержки tqt (DelayTim4Ticks).
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

  LL_TIM_DisableCounter(TIM4);
  LL_TIM_SetPrescaler(TIM4, 0);
  LL_TIM_SetCounterMode(TIM4, LL_TIM_COUNTERMODE_DOWN);
  LL_TIM_SetAutoReload(TIM4, 0xFFFF);
  LL_TIM_SetCounter(TIM4, 0);
  LL_TIM_ClearFlag_UPDATE(TIM4);
  LL_TIM_EnableIT_UPDATE(TIM4);

  NVIC_SetPriority(TIM4_IRQn, 1);
  NVIC_EnableIRQ(TIM4_IRQn);
}
/**
  * @brief  DAC (ЦАП) канал 1 -> PA4: аналоговая уставка тока лазера/подсветки.
  *
  * Инициализируется нулём и в текущей версии прошивки НЕ перестраивается:
  * регулировка экспозиции делается шириной импульса подсветки (см. dac_ctrl).
  * Переменная lasdac читается из EEPROM, но в управление не заведена.
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig = {0};

  /** DAC Initialization 
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config 
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_DAC_Start(&hdac,DAC_CHANNEL_1);

  HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,0);

}










/**
  * @brief  I2C3: шина к микросхеме EEPROM с калибровками.
  *
  * I2C (Inter-Integrated Circuit) - двухпроводная шина (SCL - такт, SDA - данные)
  * с адресацией устройств. Здесь STM32 - мастер, EEPROM - ведомый.
  * I2C_TIMING = 0x00A01E5D - предрассчитанные в STM32CubeMX тайминги для Fast Mode
  * 400 кГц при I2CCLK = 216 МГц; дополнительно включается Fast Mode Plus (до 1 МГц).
  * Запись калибровок идёт в НЕБЛОКИРУЮЩЕМ режиме (HAL_I2C_Mem_Write_IT), чтобы не
  * сорвать реальное время кадра; о завершении сообщает HAL_I2C_MemTxCpltCallback.
  * Вывод PA9 - сигнал WP (Write Protect) микросхемы: аппаратная защита от случайной
  * записи, снимается только на время сохранения и сразу возвращается.
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  //##-1- Configure the I2C3 peripheral ######################################
  I2c3Handle.Instance             = I2C3;
  I2c3Handle.Init.Timing          = I2C_TIMING;
  I2c3Handle.Init.OwnAddress1     = 0x00;
  I2c3Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2c3Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2c3Handle.Init.OwnAddress2     = 0x00;
  I2c3Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2c3Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  i2c3_err=HAL_I2C_Init(&I2c3Handle);
  // Enable Fast Mode Plus FM+ on I2C3
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C3);

}













/**
  * @brief  Настройка выводов общего назначения (GPIO) и внешнего прерывания EXTI.
  *
  * GPIO - General Purpose Input/Output. Карта используемых ног:
  *   PB11 - вход, EXTI line 11 по спаду: аппаратный триггер запуска ADC2
  *          (сюда заведён тактовый сигнал фотолинейки T-CLK);
  *   PC7  - выход T-SI: импульс начала кадра фотолинейки;
  *   PA15 - выход "подсветка": импульс светодиода, длительность = экспозиция;
  *   PB8  - вход "конфигурация": перемычка, задающая номер/ориентацию головки
  *          (читается в init_vars -> config_state);
  *   PA9  - выход WP (Write Protect) микросхемы EEPROM, по умолчанию 1 (защита
  *          включена), сбрасывается только на время записи калибровок.
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

  // GPIO Ports Clock Enable
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);


  //PORTB11 - EXTI
  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE11);
  EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_11;
  EXTI_InitStruct.LineCommand = ENABLE;
  EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
  LL_EXTI_Init(&EXTI_InitStruct);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_NO);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT);


  //PORTC7 - T-SI
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(GPIOC, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(GPIOC, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);
  LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_7);

  //PORTA15 - TSL1401CL backlight enable
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_15, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);

  //PORTB8 - config
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_INPUT);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);



  //PORTA9 - EEPROM WP
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);


}






/**
  * @brief  СЕРДЦЕ СИСТЕМЫ: обработчик завершения кадра фотолинейки.
  *
  * Вызывается, когда DMA закончил перекладывать 132 отсчёта АЦП. Делает:
  *   1) перезапускает счётчик тактов DWT (начало отсчёта времени обработки кадра);
  *   2) __disable_irq() - дальше идёт критичная по времени последовательность
  *      формирования кадра, прерывать её нельзя;
  *   3) инвалидирует кэш данных для ADC_VAL/ADC_VAL2 - DMA писал в ОЗУ напрямую,
  *      и без инвалидации ядро Cortex-M7 прочитало бы устаревшие данные из D-Cache;
  *   4) по опросу PC6 (выход T-CLK) ловит нужный фронт такта, останавливает TIM3,
  *      выдерживает паузу tqt = 20 мкс (требование даташита TSL1401CL),
  *      поднимает SI (PC7), запускает одновибратор подсветки, снова пускает TIM3
  *      и снимает SI через один такт - это и есть штатный старт нового кадра;
  *   5) выбирает тот буфер двойной буферизации, который СЕЙЧАС не заполняется,
  *      и копирует из него 128 пикселей (256 байт) в buf_x0, пропустив 2 служебных
  *      отсчёта; выставляет adc_rdy = 1 - сигнал для while(1) в main().
  *      Если предыдущий кадр ещё не обработан (adc_rdy == 1), новый просто
  *      отбрасывается - система не копит задержку.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */
  if(LL_DMA_IsActiveFlag_TC3(DMA2) == 1)
  {
    LL_DMA_ClearFlag_TC3(DMA2);

    KIN1_ResetCycleCounter();                                                   // reset cycle counter
    KIN1_EnableCycleCounter();                                                  // start counting

    __disable_irq ();

    SCB_InvalidateDCache_by_Addr((uint32_t *)ADC_VAL,((132*2+31)/32)*32);//force to let update caches again with memory content to see the changes
    SCB_InvalidateDCache_by_Addr((uint32_t *)ADC_VAL2,((132*2+31)/32)*32);//force to let update caches again with memory content to see the changes

    while (!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6)) {
    }
  
    while (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6)) {
    }

    LL_TIM_DisableCounter(TIM3);                                                 // hold T-CLK low for TSL1401 tqt
    DelayTim4Ticks((uint16_t)TSL1401_TQT_TIM_TICKS);

    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);
    Backlight_StartFromSI();
    LL_TIM_EnableCounter(TIM3);

    while (!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_6)) {
    }

    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_7);

    // copy data to buffer
    if (LL_DMA_GetCurrentTargetMem(DMA2, LL_DMA_STREAM_3)==LL_DMA_CURRENTTARGETMEM0) {
      adc_addr=ADC_VAL+2;
    }
    else {
      adc_addr=ADC_VAL2+2;
    }
    if (adc_rdy == 0) {
      memcpy(buf_x0,adc_addr,256);                                                
      adc_rdy=1;
    }

   __enable_irq (); 


  }

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  

}







/**
  * @brief  Автомат одновибратора подсветки (вывод PA15).
  *
  * Состояние DELAY: истекла пауза от SI -> включить светодиод и перезапустить
  * TIM4 на backlight_width_ticks (состояние WIDTH).
  * Состояние WIDTH: истекла длительность вспышки -> выключить светодиод, IDLE.
  */
void TIM4_IRQHandler(void)
{
  if(LL_TIM_IsActiveFlag_UPDATE(TIM4) == 1) {
    LL_TIM_ClearFlag_UPDATE(TIM4);
    LL_TIM_DisableCounter(TIM4);

    if (backlight_timer_state == BACKLIGHT_STATE_DELAY) {
      LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
      backlight_timer_state = BACKLIGHT_STATE_WIDTH;
      Backlight_StartTimer(backlight_width_ticks);
    } else {
      LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
      backlight_timer_state = BACKLIGHT_STATE_IDLE;
    }
  }
}
/**
  * @brief I2C MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

 if (hi2c==&I2c3Handle) {

    //##-1- Enable peripherals and GPIO Clocks #################################
    // Enable GPIO TX/RX clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    // Enable I2C3 clock
    __HAL_RCC_I2C3_CLK_ENABLE(); 

    //##-2- Configure peripheral GPIO ##########################################  
    // I2C SDA GPIO pin configuration 
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    // I2C SCL GPIO pin configuration 
    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //##-3- Configure the NVIC for I2C ########################################
    // NVIC for I2Cx
    HAL_NVIC_SetPriority(I2C3_ER_IRQn, 0, 3);
    HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);     
    HAL_NVIC_SetPriority(I2C3_EV_IRQn, 0, 4);
    HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);


  }

}

/**
  * @brief I2C MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{

  if (hi2c==&I2c3Handle) {

    //##-1- Reset peripherals ##################################################
    __HAL_RCC_I2C3_FORCE_RESET();
    __HAL_RCC_I2C3_RELEASE_RESET();

    //##-2- Disable peripherals and GPIO Clocks #################################
    // Configure I2C SDA as alternate function
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);
    // Configure I2C SCL as alternate function
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
  
    //##-3- Disable the NVIC for I2C ##########################################
    HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
    HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);


  }


}





/**
  * @brief  Колбэк завершения фоновой записи в EEPROM.
  *
  * Вызывается из прерывания I2C, когда HAL_I2C_Mem_Write_IT закончил передачу.
  * Возвращает аппаратную защиту записи: WP (PA9) = 1, и поднимает флаг
  * i2c3_tx_wp = 1 ("запись завершена, EEPROM снова защищена").
  */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{

  if (I2cHandle==&I2c3Handle) {
    i2c3_tx_wp=1;
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_9);                                 //WP enable
  }


}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{

  if (I2cHandle==&I2c3Handle) {




  }


}



/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  /** Error_Handler() function is called when error occurs.
    * 1- When Slave don't acknowledge it's address, Master restarts communication.
    * 2- When Master don't acknowledge the last data transferred, Slave don't care in this example.
    */
  if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
  {


  }
}



/**
  * @brief  This function handles I2C event interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C data transmission
  */
void I2C3_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&I2c3Handle);
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is redefined in "main.h" and related to I2C error
  */
void I2C3_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&I2c3Handle);
}










/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
