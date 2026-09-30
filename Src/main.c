/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "math.h"
#include "arm_math.h"
#include  <stdio.h>
#include  <stdarg.h>
#include  <string.h>
#include  <stdint.h>
#include  <stddef.h>

#include "cmd_defs.h"
#include <standardflash.h>


/* Private includes ----------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/


typedef  void (*pFunction)(void);



/* Private define ------------------------------------------------------------*/



#define M_PI ((float)3.141592653589793)
#define ABS(X)	((X) > 0 ? (X) : -(X))  


#define MAX_LEVEL_PIX           3700
#define ADC_OFFSET              600

#define BIT_TAB_SIZE            144
#define SECTOR_SIZE             51
#define BITS_PER_SECTOR         15

#define APB1_TIMER_CLOCK_HZ             108000000UL
#define TSL1401_TQT_US                  20UL
#define TSL1401_TQT_TIM_TICKS           (APB1_TIMER_CLOCK_HZ / 1000000UL * TSL1401_TQT_US)

#define STARTPIXEL_MIN          6
#define STARTPIXEL_MAX          57

#define CENTER_PIXEL            60




#ifndef M_PI_4
#define M_PI_4 (3.1415926535897932384626433832795/4.0)
#endif
#define A 0.0776509570923569
#define B -0.287434475393028
#define C (M_PI_4 - A - B)






#define FLASH_START_ADDRESS  ((uint32_t)0x08010000)                             //FLASH start address

#define RS485_PORT      GPIOB
#define RS485_DE_PIN    LL_GPIO_PIN_5
#define RS485_TX_PIN    LL_GPIO_PIN_6
#define RS485_RX_PIN    LL_GPIO_PIN_7
#define RS485_USART     USART1
#define RS485_USART_IRQn USART1_IRQn

/*
 * Адрес датчика на шине RS-485 (Modbus-подобный протокол SNAP/STATUS/READ).
 * На каждый физический датчик прошивка собирается со своим значением ID
 * (1..254, 0 и 255 зарезервированы). Проще всего задать через define
 * проекта (-DRS485_DEVICE_ID=2 в настройках сборки для второго датчика),
 * чтобы не поддерживать несколько веток кода. В дальнейшем адрес можно
 * читать во время выполнения (DIP-переключатель/резисторные делители на
 * свободных GPIO, либо значение, хранимое в EEPROM), тогда это define
 * станет только значением по умолчанию.
 */
#ifndef RS485_DEVICE_ID
#define RS485_DEVICE_ID   1U
#endif

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







#define	eeprom_len		        2048                                    //length of eeprom data


#define	eeprom_ang_tab                  0x00		                        //eeprom address of ang_tab
#define	eeprom_ang_tab_crc	        BIT_TAB_SIZE*4	                        //eeprom address of ang_tab crc16

#define	eeprom_buf_k	                768	                                //eeprom address of buf_k
#define	eeprom_buf_k_crc	        1280	                                //eeprom address of buf_k crc16

#define	eeprom_offset	                1536	                                //eeprom address of offset
#define	eeprom_offset_crc	        1540	                                //eeprom address of offset crc16
#define	eeprom_lasdac	                1544	                                //eeprom address of lasdac
#define	eeprom_lasdac_crc               1546	                                //eeprom address of lasdac crc16







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


/* CRC handler declaration */
CRC_HandleTypeDef   CrcHandle;





// RNG
RNG_HandleTypeDef RNG_Handle;

uint16_t rng_value;



//CPU cycles measurement
uint32_t cycles; /* number of cycles */

uint32_t cycles_max;


// I2C

I2C_HandleTypeDef I2c3Handle;           // I2C3 handler declaration
uint8_t i2c3_tx_wp=1;
uint8_t i2c3_err;




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




//ADC1
__attribute__((aligned(32))) uint16_t aADCxConvertedData[1];
__attribute__((aligned(4))) float Vsense;
__attribute__((aligned(4))) float V25 = 0.76f;
__attribute__((aligned(4))) float Avg_Slope = 2.5f;
__attribute__((aligned(4))) float Temperature;



//ADC2
__attribute__((aligned(32))) uint16_t ADC_VAL[132] = {0};
__attribute__((aligned(32))) uint16_t ADC_VAL2[132] = {0};
uint16_t *adc_addr;
unsigned char adc_rdy;                                                          // ADC frame ready flag

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


volatile uint32_t adc_frame_count = 0U;
volatile uint32_t adc_frame_rate = 0U;
volatile uint32_t angle_update_count = 0U;
volatile uint32_t angle_update_rate = 0U;
volatile uint32_t rate_t0 = 0U;

/*
 * ---------------------------------------------------------------------
 * Конвейер измерений для протокола SNAP/STATUS/READ по RS-485.
 * ---------------------------------------------------------------------
 * АЦП -> расчёт -> calculating -> published -> (SNAP) -> frozen
 *
 *  - calculating: рабочие (промежуточные) поля текущего кадра, пишутся
 *    только в главном цикле, во время самого расчёта угла.
 *  - published:   последний ПОЛНОСТЬЮ посчитанный кадр. Обновляется одним
 *    присваиванием структуры сразу после того, как calculating собран
 *    целиком, поэтому нет промежуточного состояния "наполовину новый кадр".
 *  - frozen + frozen_seq/frozen_ok: неизменный снимок published,
 *    сделанный в момент прихода широковещательной команды SNAP,seq.
 *    Именно frozen отдаётся в ответ на READ,id,seq, поэтому пока идёт
 *    выдача данных за кадр seq, АЦП и алгоритм совершенно свободно
 *    продолжают считать следующие кадры в calculating/published — они
 *    друг другу не мешают.
 *
 * Все эти структуры трогает только main() (сама обработка АЦП и разбор
 * команд RS-485 выполняются в общем bare-metal цикле без ОС), поэтому
 * присваивание структуры целиком атомарно с точки зрения гонок: копия
 * никогда не попадёт в разрыв между чтением ADC ISR и обработкой команды -
 * ISR USART1 только складывает байты в кольцевой буфер, но не трогает
 * calculating/published/frozen.
 */
typedef struct {
    int32_t  angle_mdeg;   /* угол в миллиградусах, [0..359999], -1 если сектор не определён */
    int16_t  sector;       /* номер сектора или -1, если невалидно */
    uint8_t  valid;        /* 1 - измерение достоверно (errorflag==0 и сектора в диапазоне) */
    uint8_t  state;        /* последнее значение encoder_state (диагностика) */
    uint32_t frame_no;     /* локальный, никогда не обнуляемый счётчик кадров этой головки */
} Measurement;

static Measurement calculating;      /* собирается для текущего кадра */
static Measurement published;        /* последний полностью готовый кадр */
static Measurement frozen;           /* неизменный снимок для READ */
static uint32_t    frozen_seq;       /* какому seq соответствует frozen */
static uint8_t     frozen_ok;        /* 1, если SNAP с frozen_seq уже обработан */
static uint32_t    measurement_frame_no; /* свободно бегущий счётчик кадров головки */

//FLASH


// Instantiate the arrays needed for testing purposes.
// dataWrite is used when sending data that will be flashed to the device.
static uint8_t dataWrite[MAXIMUM_BUFFER_SIZE+8] = {0};
// dataRead is used as a buffer for received data. Read data will be stored here.
static uint8_t dataRead[MAXIMUM_BUFFER_SIZE+8] = {0};


uint8_t boot_state;

pFunction Jump_To_Application;
uint32_t JumpAddress;





//software update
static uint8_t sec_buffer[4096];
uint16_t prog_crc;
uint16_t prog_len;
uint32_t prog_addr;
uint32_t prog_data;
uint32_t prog_header;
uint32_t prog_len_cnt;
uint32_t prog_cur;
uint16_t prog_offset=0;
uint16_t prog_crc_cur;
uint8_t prog_errcode;
uint32_t update_len;
uint32_t update_crc;
uint32_t update_time;
uint32_t update_Checksum;
uint16_t update_version=11;










// See surrounding code
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

uint16_t startpixel=0;
uint16_t startpixel1;
uint16_t startpixel2;

uint16_t test1pixel;
uint16_t test2pixel;
uint16_t bck_pixel;

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



//Calibration

int pix_dif_num_avg=5;
int start_calibrate=0;
int auto_cal=0;

float pix_dif_tab1[BIT_TAB_SIZE];
float pix_dif_sum1[BIT_TAB_SIZE];
int pix_dif_num1[BIT_TAB_SIZE];
int pix_rdy_tab1[BIT_TAB_SIZE];
int pix_rdy_num1;
float sum_Ai1;

float pix_dif_tab2[BIT_TAB_SIZE];
float pix_dif_sum2[BIT_TAB_SIZE];
int pix_dif_num2[BIT_TAB_SIZE];
int pix_rdy_tab2[BIT_TAB_SIZE];
int pix_rdy_num2;
float sum_Ai2;

float new_ang_tab1[BIT_TAB_SIZE];
float new_ang_tab2[BIT_TAB_SIZE];



float new_ang_tab[BIT_TAB_SIZE];





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







//SPI
__attribute__((aligned(32))) unsigned short aTxBuffer[SPI_TxBufSize];
__attribute__((aligned(32))) unsigned short aRxBuffer[SPI_RxBufSize];
__attribute__((aligned(32))) unsigned char TxBuffer[SPI_TxBufSize*2];
uint32_t spi_send_cnt=0;
uint8_t recv_num;
uint8_t recv_num_prev;
uint32_t spi_badframes=0;
uint32_t spi_goodframes=0;
uint32_t spi_lostframes=0;

uint32_t *spi_recv_p2;
uint32_t spi_recv_crc32;
__IO uint32_t spi_recv_CRCValue = 0;
uint32_t *spi_send_p2;
__IO uint32_t spi_send_CRCValue;

static REM_BUF spi_buf1;
static REM_BUF spi_buf2;

uint8_t s_buf[16];                                                              //SPI command buffer


unsigned char command=0; 
unsigned char encoder_state=0;


unsigned char SloCom=0; 









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
//byte - пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ, пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
//crc8 - пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅ
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





const unsigned short Crc16Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};


//  Init  : 0xFFFF
//  Revert: false
//  XorOut: 0x0000
//  Check : 0x29B1 ("123456789")
//  MaxLen: 4095 
//*----------------------------------------------------------------------------
unsigned short Crc16_step( unsigned char data, unsigned short crc )
{
   crc = (crc << 8) ^ Crc16Table[(crc >> 8) ^ data];
   return crc;
}













static const uint32_t crc32_table[0x100] = {
  0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD, 
  0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD, 
  0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D, 
  0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95, 0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 
  0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 
  0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA, 
  0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A, 
  0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A, 
  0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB, 0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53, 
  0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 
  0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3, 
  0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3, 
  0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24, 
  0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC, 0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 
  0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 
  0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4, 
};


uint32_t CalcCRC32(uint8_t * pData, uint32_t DataLength)
{
    uint32_t Checksum = 0xFFFFFFFF;
    for(unsigned int i=0; i < DataLength; i++)
    {
        uint8_t top = (uint8_t)(Checksum >> 24);
        top ^= pData[i];
        Checksum = (Checksum << 8) ^ crc32_table[top];
    }
    return Checksum;
}



uint32_t CalcCRC32_step(uint8_t pData, uint32_t Checksum)
{
  uint8_t top = (uint8_t)(Checksum >> 24);
  top ^= pData;
  Checksum = (Checksum << 8) ^ crc32_table[top];
  return Checksum;
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








//FLASH variables





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




float  offset=68;



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
static void MX_CRC_Init(void);
static void MX_RNG_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C3_Init(void);

static void MX_RS485_USART1_Init(void);
static void RS485_Write(const uint8_t *data, size_t len);
static void RS485_SendMeasurement(void);
static void RS485_PollCommands(void);
static void RS485_HandleLine(const char *line);
static void RS485_OnSnap(uint32_t seq);
static void RS485_OnStatus(uint8_t id, uint32_t seq);
static void RS485_OnRead(uint8_t id, uint32_t seq);

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

void Send_SPI(unsigned char *trm_buff);
void Recv_SPI();
void spi_recv_process (unsigned char *recv_buff);
int spi_SendBuf1(unsigned char *data, unsigned char len);
int spi_SendBuf2(unsigned char *data, unsigned char len);
void spi_SendExec();
void spi_FreeAll();

uint8_t ProgCheckStm32();
uint8_t ProgBlock();
uint8_t CalcUpdateChecksum();
uint8_t ProgFlashHeader();
void ReadFlashHeader();
void SloComProcess();

void Flash_Read_Data (uint32_t StartPageAddress, void *Data, uint16_t numberofwords);
uint32_t Flash_Write_Data (uint32_t StartPageAddress, void *Data, uint16_t numberofwords);

void init_vars();
static void Backlight_StartFromSI(void);
static void Backlight_StartTimer(uint16_t ticks);
static void DelayTim4Ticks(uint16_t ticks);




/* Private user code ---------------------------------------------------------*/




/**
  * Brief for Fast2ArcTan()
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
  * Brief for copy_data()
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
  * @brief  calculate subpixel coordinates
  * @param
  * @retval
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
  * @brief  calc precise value of pixel
  * @param  none
  * @retval none
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
  * @brief  ????? ??????? ?????????????????? ????? ? ?????? ?? ??????
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
  * @brief  calculate sector number
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void calc_sector(void)
{
    int tmp1;
    int tmin = 1000000;
    int min = 0;

    for (int i = 0; i < 9; i++) {
        tmp1 = pix[i][1] - pix[i][0];
        if (tmp1 < 0) {
            tmp1 = -tmp1;
        }

        if (tmp1 < tmin) {
            tmin = tmp1;
            min = i;
        }
    }

    badbit_pos = min;
    badbit_value = tmin;

    uint16_t cur_data = 0;
    uint16_t bit_pos = 0x100;

    for (int i = 0; i < 9; i++) {
        if (pix[i][1] > pix[i][0]) {
            cur_data |= bit_pos;
        }
        bit_pos >>= 1;
    }

    data_byte = cur_data;

    uint8_t tmp_pos2 = 255;
    for (uint8_t j = 0; j < BIT_TAB_SIZE; j++) {
        if (bit_tab[j] == data_byte) {
            tmp_pos2 = j;
            break;
        }
    }

    rem_sec = tmp_pos2;
}













/**
  * @brief  save sector number
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
  * @brief  startpixel error correction
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
  * @brief  bit error correction
  * @param  none
  * @retval none
  */
/*-----------------------------------------------------------------------------------*/
void bit_err_corr(void) {

            if(badbit_value<1000) {                                             //error bit correction
              switch (badbit_pos) {
                case 0:
                  data_byte=data_byte^0x400;
                  break;
                case 1:
                  data_byte=data_byte^0x200;
                  break;
                case 2:
                  data_byte=data_byte^0x100;
                  break;
                case 3:
                  data_byte=data_byte^0x80;
                  break;
                case 4:
                  data_byte=data_byte^0x40;
                  break;
                case 5:
                  data_byte=data_byte^0x20;
                  break;
                case 6:
                  data_byte=data_byte^0x10;
                  break;
                case 7:
                  data_byte=data_byte^0x08;
                  break;
                case 8:
                  data_byte=data_byte^0x04;
                  break;
                case 9:
                  data_byte=data_byte^0x02;
                  break;
                default:
                  data_byte=data_byte^0x01;
                  break;
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
  * @brief  error control and correction
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
  * @brief  Angle calculation
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
  * Brief for if()
  * @param  none
  * @retval none
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
  * @brief  ?????????? ???/????
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
  * @brief  angtab calibration
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

    if (  (cur_sector!=old_sector)&& ( ((s_l1>=(offset-25) )&&(s_l1<=(offset-24)))||(((s_l2>=(offset+24))&&(s_l2<=(offset+25)))) )  ) {

      old_sector=rsector;  
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
              sum_Ai1=tmp_sum;
              new_ang_tab1[0]=0;
              for (unsigned char j=1;j<BIT_TAB_SIZE;j++) {
                new_ang_tab1[j]=new_ang_tab1[j-1]+pix_dif_tab1[j-1]/sum_Ai1*360;

              }
              auto_cal=0;
                
              encoder_state=0x20;
              
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

              encoder_state=0x40;                                               //set "ang_tab calibration ready" SPI status

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
  * @brief  offset calibration
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

        encoder_state=0x60;                                                     //set "offset calibration ready" SPI status
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
  * @brief  Angle koef calibration
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

        encoder_state=0x80;                                                     //set "angle_k calibration ready" SPI status
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
  * @brief  This function configures WWDG
  * @param  None
  * @retval None
  */
void Configure_WWDG(void)
{
  /* Enable the peripheral clock of DBG register (uncomment for debug purpose) */
  /*LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_WWDG_STOP); */
  
  /* Enable the peripheral clock WWDG */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_WWDG);

  /* Configure WWDG */
  /* (1) set prescaler to have a rollover each about ~2s */
  /* (2) set window value to same value (~2s) as downcounter in order to ba able to refresh the WWDG almost immediately */
  /* (3) Refresh WWDG before activate it */
  /* (4) Activate WWDG */
  LL_WWDG_SetPrescaler(WWDG, LL_WWDG_PRESCALER_8); /* (1) */
  LL_WWDG_SetWindow(WWDG,0x7E);                    /* (2) */
  LL_WWDG_SetCounter(WWDG, 0X7E);                  /* (3) */
  LL_WWDG_Enable(WWDG);                            /* (4) */
}

/**
  * @brief  This function check if the system has resumed from WWDG reset
  * @param  None
  * @retval None
  */
void Check_WWDG_Reset(void)
{
  if (LL_RCC_IsActiveFlag_WWDGRST())
  {
    /* clear WWDG reset flag */
    LL_RCC_ClearResetFlags();
  }
}







/**
  * Brief for angk_cal()
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














void SloComProcess() {

    if(SloCom>0)
      switch (SloCom)  {

        //prog end block
        case spi_ProgEndBlock:
          prog_errcode=ProgBlock();                                             //Program block to flash
          s_buf[0]=spi_ProgEndBlock;                                            //response code
          s_buf[6]=prog_errcode;                                                //error code
          spi_SendBuf2(&s_buf[0],14);                                           //send response
          SloCom=0;                                                             //reset SloCom
          break;

        //prog update header
        case spi_ProgUpdateHeader:
          prog_errcode=CalcUpdateChecksum();                                    //check update crc32
          if (prog_errcode==0) {                                                //if crc32 ok
            prog_errcode=ProgFlashHeader();                                     // prog header to flash
          }
          s_buf[0]=spi_ProgUpdateHeader;                                        //response code
          s_buf[6]=prog_errcode;                                                //error code
          spi_SendBuf2(&s_buf[0],14);                                           //send response
          SloCom=0;                                                             //reset SloCom
          break;

        //prog update check
        case spi_ProgUpdateCheck:
          ReadFlashHeader();                                                    //read flash header
          prog_errcode=CalcUpdateChecksum();                                    //check update crc32
          if (prog_errcode==0) {                                                //if crc32 ok
            prog_errcode=ProgCheckStm32();                                      //check FLASH update
          }
          s_buf[0]=spi_ProgUpdateCheck;                                         //response code
          memcpy(&s_buf[6],&update_len,4);                                      //update length
          s_buf[10]=prog_errcode;                                               //error code
          spi_SendBuf2(&s_buf[0],14);                                           //send response
          SloCom=0;                                                             //reset SloCom
          break;

        //execute boot command
        case spi_ProgExecBoot:
          SloCom=0;                                                             //reset SloCom
          boot_state=0;
          LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_9);                         //WP disable
          HAL_I2C_Mem_Write(&I2c3Handle, EEPROM_DEVICE_PAGE7, 0xF0, I2C_MEMADD_SIZE_8BIT, &boot_state, 1, 1000); //write boot state to EEPROM
          Check_WWDG_Reset();
          Configure_WWDG();
          break;


        default:
          break;

    }



}






uint8_t ProgBlock() {

  uint8_t errcode;                                                              //error code

  prog_crc_cur=0xffff;                                                          //init block crc16
  for (int i=0;i<prog_len;i++) {                                                //block crc16 calculation
    prog_crc_cur=Crc16_step(sec_buffer[i], prog_crc_cur);                       //calc block crc16
  }
  if(prog_crc==prog_crc_cur) {                                                  //if crc16 is good
    prog_data=prog_addr+4096;                                                   //calc data address
    standardflashWriteEnable();                                                 //Write enable the device
    standardflashBlockErase4K(prog_data);                                       //Block erase 4K block
    standardflashWaitOnReady();                                                 //Wait on ready
    standardflashWriteEnable();                                                 //Write enable the device

    prog_cur=0;                                                                 //clear cur address
    prog_len_cnt=prog_len;                                                      //set length counter
    while (prog_len_cnt>=256) {
      standardflashWriteEnable();                                               //Write enable the device
      standardflashBytePageProgram(prog_data+prog_cur, sec_buffer+prog_cur, 256);//Program the device
      standardflashWaitOnReady();                                               //Wait on ready
      prog_cur+=256;                                                            //Modify addr
      prog_len_cnt-=256;                                                        //Modify counter
    }
    if (prog_len_cnt>0) {
      standardflashWriteEnable();                                               //Write enable the device
      standardflashBytePageProgram(prog_data+prog_cur, sec_buffer+prog_cur, prog_len_cnt);//Program the device
      standardflashWaitOnReady();                                               //Wait on ready
    }

    standardflashReadArrayLowFreq(prog_data, dataRead, prog_len);               //Now read back the data in order to confirm that erase/program/read all
    if(compareByteArrays(sec_buffer, dataRead, prog_len)) {
      errcode=0;                                                                // set no error code
    }
    else {
      errcode=2;                                                                // set flash error code
    }

  }
  else {                                                                        //if crc16 is bad
    errcode=1;                                                                  // set crc16 error code
  }

  return (errcode);

}





uint8_t CalcUpdateChecksum() {

  uint8_t errcode;                                                              //error code

  update_Checksum=0xFFFFFFFF;                                                   //init crc32
  prog_data=4096;                                                               //calc data address
  prog_cur=0;                                                                   //clear current data address
  prog_len_cnt=update_len;                                                      //init length counter
  while (prog_len_cnt>=4096) {
    standardflashReadArrayLowFreq(prog_data+prog_cur, dataRead, 4096);          //read data buffer
    for (int i=0;i<4096;i++) {
      update_Checksum=CalcCRC32_step(dataRead[i], update_Checksum);             //calc crc32
    }
    prog_cur+=4096;                                                             //Modify addr
    prog_len_cnt-=4096;                                                         //Modify counter
  }
  if (prog_len_cnt>0) {
    standardflashReadArrayLowFreq(prog_data+prog_cur, dataRead, prog_len_cnt);  //read data buffer
    for (int i=0;i<prog_len_cnt;i++) {
      update_Checksum=CalcCRC32_step(dataRead[i], update_Checksum);             //calc crc32
    }
  }

  if (update_crc==update_Checksum) {                                            //if crc32 ok
    errcode=0;                                                                  // set no error code
  }
  else {                                                                        //if crc32 not ok
    errcode=3;                                                                  // set update crc32 error code
  }

  return (errcode);

}





uint8_t ProgFlashHeader() {

  uint8_t errcode;                                                              //error code

  prog_data=0;                                                                  //header address
  standardflashWriteEnable();                                                   //Write enable the device
  standardflashBlockErase4K(prog_data);                                         //Block erase 4K block
  standardflashWaitOnReady();                                                   //Wait on ready
  standardflashWriteEnable();                                                   //Write enable the device
  memcpy(&dataWrite[0],&update_len,4);                                          //write update length
  memcpy(&dataWrite[4],&update_crc,4);                                          //write update crc32
  memcpy(&dataWrite[8],&update_time,4);                                         //write update time
  standardflashWriteEnable();                                                   //Write enable the device
  standardflashBytePageProgram(prog_data, dataWrite, 12);                       //Program the device
  standardflashWaitOnReady();                                                   //Wait on ready
  standardflashReadArrayLowFreq(prog_data, dataRead, 12);                       //read back header
  if(compareByteArrays(dataWrite, dataRead, 12)) {
    errcode=0;                                                                  // set no error code
  }
  else {
    errcode=4;                                                                  // set header flash error code
  }

  return (errcode);

}





void ReadFlashHeader() {

  prog_data=0;                                                                  //header address
  standardflashReadArrayLowFreq(prog_data, dataRead, 12);                       //read back header
  memcpy(&update_len,&dataRead[0],4);                                           //read update length
  memcpy(&update_crc,&dataRead[4],4);                                           //read update crc32
  memcpy(&update_time,&dataRead[8],4);                                          //read update time

}





uint8_t ProgCheckStm32() {

  uint8_t errcode;                                                              //error code
  uint32_t CurPageAddress;

  prog_data=0;                                                                  //header address
  standardflashReadArrayLowFreq(prog_data, dataRead, 12);                       //read back header
  memcpy(&update_len,&dataRead[0],4);                                           //read update length
  memcpy(&update_crc,&dataRead[4],4);                                           //read update crc32
  memcpy(&update_time,&dataRead[8],4);                                          //read update time

  update_Checksum=0xFFFFFFFF;                                                   //init crc32
  prog_data=4096;                                                               //calc data address
  prog_cur=0;                                                                   //clear current data address
  prog_len_cnt=update_len;                                                      //init length counter
  CurPageAddress=FLASH_START_ADDRESS;                                           //init FLASH page address
  if(prog_len_cnt>0x70000) prog_len_cnt=0x70000;                                //limit length counter

  while (prog_len_cnt>=4096) {
    standardflashReadArrayLowFreq(prog_data+prog_cur, dataRead, 4096);          //read data buffer

    Flash_Read_Data (CurPageAddress, &dataWrite, 1024);                         //read FLASH buffer
    for (int i=0;i<4096;i++) {
      update_Checksum=CalcCRC32_step(dataWrite[i], update_Checksum);            //calc crc32
    }

    if (memcmp(dataRead,dataWrite,4096)!=0) {
      return 101;
    }

    CurPageAddress+=4096;                                                       //Modify FLASH addr
    prog_cur+=4096;                                                             //Modify addr
    prog_len_cnt-=4096;                                                         //Modify counter
  }
  if (prog_len_cnt>0) {
    standardflashReadArrayLowFreq(prog_data+prog_cur, dataRead, prog_len_cnt);  //read data buffer

    Flash_Read_Data (CurPageAddress, &dataWrite, 1024);                         //read FLASH buffer

    if (memcmp(dataRead,dataWrite,prog_len_cnt)!=0) {
      return 101;
    }

    for (int i=0;i<prog_len_cnt;i++) {
      update_Checksum=CalcCRC32_step(dataWrite[i], update_Checksum);            //calc crc32
    }

  }

  if (update_crc==update_Checksum) {                                            //if crc32 ok
    errcode=0;                                                                  // set no error code
  }
  else {                                                                        //if crc32 not ok
    errcode=100;                                                                // set update crc32 error code
  }

  return (errcode);

}





void Flash_Read_Data (uint32_t StartPageAddress, void *Data, uint16_t numberofwords)
{
  uint32_t *RxBuf = Data;
  while (numberofwords--) {

    *RxBuf = *(__IO uint32_t *)StartPageAddress;
    StartPageAddress += 4;
    RxBuf++;
  }
}









/**
  * @brief Variables initialization
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
void init_vars() {

  //init buffers
  spi_FreeAll();


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

  prog_errcode=ProgCheckStm32();                                                //check FLASH update

  //compensation BR4
  phase=0;
  for (unsigned char j=0;j<200;j++) {
   delta_cor[j]=0.01*sin((j-100)*3.1415926/50.+phase);
  }


}

  
static void MX_RS485_USART1_Init(void)
{
    LL_USART_InitTypeDef usart;

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    LL_GPIO_SetPinMode(RS485_PORT, RS485_DE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(RS485_PORT, RS485_DE_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(RS485_PORT, RS485_DE_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(RS485_PORT, RS485_DE_PIN, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(RS485_PORT, RS485_DE_PIN);

    LL_GPIO_SetPinMode(RS485_PORT, RS485_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(RS485_PORT, RS485_TX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(RS485_PORT, RS485_TX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(RS485_PORT, RS485_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(RS485_PORT, RS485_TX_PIN, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(RS485_PORT, RS485_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(RS485_PORT, RS485_RX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(RS485_PORT, RS485_RX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(RS485_PORT, RS485_RX_PIN, LL_GPIO_PULL_UP);

    LL_USART_StructInit(&usart);

    /*
     * ВАЖНО: скорость должна совпадать со скоростью мастера ESP32
     * (RS_BAUD в скетче). 9600 было наследием старого "теста", для
     * протокола SNAP/STATUS/READ с таймаутом 30 мс на слово нужен
     * запас по времени, поэтому переведено на 115200, как в скетче
     * мастера. Меняйте оба конца одновременно.
     */
    usart.BaudRate = 115200;
    usart.DataWidth = LL_USART_DATAWIDTH_8B;
    usart.StopBits = LL_USART_STOPBITS_1;
    usart.Parity = LL_USART_PARITY_NONE;
    usart.TransferDirection = LL_USART_DIRECTION_TX_RX;
    usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    usart.OverSampling = LL_USART_OVERSAMPLING_16;

    LL_USART_Disable(RS485_USART);
    LL_USART_Init(RS485_USART, &usart);
    LL_USART_ConfigAsyncMode(RS485_USART);
    LL_USART_Enable(RS485_USART);

    /* Приём команд мастера ведётся по прерыванию, чтобы не терять байты,
     * пока main() занят расчётом угла по кадру АЦП. */
    LL_USART_EnableIT_RXNE(RS485_USART);
    NVIC_SetPriority(RS485_USART_IRQn, 5);
    NVIC_EnableIRQ(RS485_USART_IRQn);
}


static void RS485_Write(const uint8_t *data, size_t len)
{
    if (data == NULL || len == 0) {
        return;
    }

    LL_GPIO_SetOutputPin(RS485_PORT, RS485_DE_PIN);
    //HAL_Delay(10);  // ?????? ??? ???????????

    for (size_t i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(RS485_USART)) {}
        LL_USART_TransmitData8(RS485_USART, data[i]);
    }

    while (!LL_USART_IsActiveFlag_TC(RS485_USART)) {}

    //HAL_Delay(10);  // ?????? ??? ???????????
    LL_GPIO_ResetOutputPin(RS485_PORT, RS485_DE_PIN);
}

static void RS485_Test(void)
{
    static const uint8_t test[] = "RS485 OK\r\n";
    RS485_Write(test, sizeof(test) - 1U);
}

static void RS485_SendMeasurement(void)
{
    char line[80];
    uint8_t valid;
    int n;

    valid = (errorflag == 0) &&
            (sector >= 0) && (sector < BIT_TAB_SIZE) &&
            (rsector >= 0) && (rsector < BIT_TAB_SIZE);

    n = snprintf(line, sizeof(line),
                 "A=%.6f;S=%d;V=%u;ST=%02X\r\n",
                 (double)cur_ang_E,
                 valid ? sector : -1,
                 valid,
                 encoder_state);

    if ((n > 0) && ((size_t)n < sizeof(line))) {
        RS485_Write((const uint8_t *)line, (size_t)n);
    }
}

/*
 * ---------------------------------------------------------------------
 * Приём и разбор команд SNAP/STATUS/READ по RS-485.
 * ---------------------------------------------------------------------
 * Формат строк ASCII, разделитель полей ',', конец строки '\n' (перед
 * ним допускается необязательный '\r', как и в текущем формате DATA):
 *
 *   S,<seq>                       SNAP:  широковещательная команда всем
 *                                        датчикам. Ответа НЕ предполагает.
 *   T,<id>,<seq>                  STATUS: адресный запрос готовности.
 *                                        Ответ ACK,<id>,<seq>,frame=<N>
 *                                        либо NAK,<id>,<seq>.
 *   R,<id>,<seq>                  READ:  адресный запрос данных.
 *                                        Ответ D,<id>,<seq>,angle,sector,
 *                                        valid,state,frame (см. формат,
 *                                        который уже понимает parseData()
 *                                        в скетче мастера).
 *
 * <id> сравнивается с RS485_DEVICE_ID; команды с чужим id молча
 * игнорируются (в шину при этом ничего не передаётся - иначе на
 * многоточечной линии возникнет коллизия с ответом адресованного
 * датчика).
 */

#define RS485_RXBUF_SIZE 128U /* степень двойки! */

static volatile uint8_t  rs485_rxbuf[RS485_RXBUF_SIZE];
static volatile uint16_t rs485_rx_head = 0U;
static volatile uint16_t rs485_rx_tail = 0U;
static volatile uint8_t  rs485_rx_overflow = 0U;

/* Вызывается из USART1_IRQHandler(). Делает минимум работы -
 * только складывает байт в кольцевой буфер. */
static void RS485_RxByteFromISR(uint8_t byte)
{
    uint16_t next = (uint16_t)((rs485_rx_head + 1U) & (RS485_RXBUF_SIZE - 1U));

    if (next != rs485_rx_tail) {
        rs485_rxbuf[rs485_rx_head] = byte;
        rs485_rx_head = next;
    } else {
        rs485_rx_overflow = 1U; /* буфер переполнен, байт потерян */
    }
}

/* SNAP,seq: сделать снимок published -> frozen. Broadcast, без ответа. */
static void RS485_OnSnap(uint32_t seq)
{
    frozen = published;   /* копия структуры одним присваиванием (атомарно
                            * в рамках bare-metal main loop) */
    frozen_seq = seq;
    frozen_ok = 1U;        /* именно "снимок сделан", а не "угол валиден" -
                            * достоверность самого измерения отражена в
                            * frozen.valid и уходит в DATA отдельно */
}

/* STATUS,id,seq: сообщить, готов ли снимок с меткой seq. */
static void RS485_OnStatus(uint8_t id, uint32_t seq)
{
    char line[48];
    int n;

    if (id != (uint8_t)RS485_DEVICE_ID) {
        return; /* команда не к нам - в шину не отвечаем */
    }

    if (frozen_ok && (frozen_seq == seq)) {
        n = snprintf(line, sizeof(line), "ACK,%u,%lu,frame=%lu\r\n",
                     (unsigned)RS485_DEVICE_ID,
                     (unsigned long)seq,
                     (unsigned long)frozen.frame_no);
    } else {
        /* Явный отрицательный ответ быстрее таймаута на мастере:
         * например, SNAP ещё не был обработан этим датчиком. */
        n = snprintf(line, sizeof(line), "NAK,%u,%lu\r\n",
                     (unsigned)RS485_DEVICE_ID,
                     (unsigned long)seq);
    }

    if ((n > 0) && ((size_t)n < sizeof(line))) {
        RS485_Write((const uint8_t *)line, (size_t)n);
    }
}

/* READ,id,seq: отдать данные зафиксированного кадра seq. */
static void RS485_OnRead(uint8_t id, uint32_t seq)
{
    char line[80];
    int n;

    if (id != (uint8_t)RS485_DEVICE_ID) {
        return;
    }

    if (!(frozen_ok && (frozen_seq == seq))) {
        /* Нет данных под этим seq (READ раньше STATUS/SNAP, или мастер
         * ошибся) - молчим, мастер получит таймаут и повторит цикл. */
        return;
    }

    n = snprintf(line, sizeof(line), "D,%u,%lu,%ld,%d,%u,%02X,%lu\r\n",
                 (unsigned)RS485_DEVICE_ID,
                 (unsigned long)seq,
                 (long)frozen.angle_mdeg,
                 (int)frozen.sector,
                 (unsigned)frozen.valid,
                 (unsigned)frozen.state,
                 (unsigned long)frozen.frame_no);

    if ((n > 0) && ((size_t)n < sizeof(line))) {
        RS485_Write((const uint8_t *)line, (size_t)n);
    }
}

/* Разбор одной собранной строки команды и вызов соответствующего
 * обработчика. */
static void RS485_HandleLine(const char *line)
{
    unsigned id = 0U;
    unsigned long seq = 0UL;

    if ((line[0] == 'S') && (line[1] == ',')) {
        if (sscanf(line, "S,%lu", &seq) == 1) {
            RS485_OnSnap((uint32_t)seq);
        }
        return;
    }

    if ((line[0] == 'T') && (line[1] == ',')) {
        if (sscanf(line, "T,%u,%lu", &id, &seq) == 2) {
            RS485_OnStatus((uint8_t)id, (uint32_t)seq);
        }
        return;
    }

    if ((line[0] == 'R') && (line[1] == ',')) {
        if (sscanf(line, "R,%u,%lu", &id, &seq) == 2) {
            RS485_OnRead((uint8_t)id, (uint32_t)seq);
        }
        return;
    }

    /* неизвестная команда - молча игнорируем */
}

/* Выбирает готовые байты из кольцевого буфера, собирает строки по '\n'
 * (с необязательным ведущим '\r') и передаёт их в RS485_HandleLine().
 * Вызывается из главного цикла на КАЖДОЙ итерации (не только когда
 * adc_rdy==1), чтобы задержка ответа не зависела от текущей фазы
 * обработки кадра АЦП. */
static void RS485_PollCommands(void)
{
    static char   line[64];
    static size_t line_len = 0U;

    if (rs485_rx_overflow) {
        rs485_rx_overflow = 0U; /* сюда можно добавить счётчик диагностики */
    }

    while (rs485_rx_tail != rs485_rx_head) {
        uint8_t byte = rs485_rxbuf[rs485_rx_tail];
        rs485_rx_tail = (uint16_t)((rs485_rx_tail + 1U) & (RS485_RXBUF_SIZE - 1U));

        if (byte == (uint8_t)'\r') {
            continue;
        }

        if (byte == (uint8_t)'\n') {
            line[line_len] = '\0';
            if (line_len > 0U) {
                RS485_HandleLine(line);
            }
            line_len = 0U;
            continue;
        }

        if ((line_len + 1U) < sizeof(line)) {
            line[line_len++] = (char)byte;
        } else {
            /* строка длиннее ожидаемого - явно мусор, отбрасываем её
             * целиком, ждём следующий '\n' */
            line_len = 0U;
        }
    }
}


/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  // MCU Configuration--------------------------------------------------------

  HAL_Init();

  SystemClock_Config();
  HAL_ResumeTick();

  SCB_EnableICache();
  SCB_EnableDCache();

  MX_GPIO_Init();
  MX_RS485_USART1_Init();
  MX_DAC_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  MX_CRC_Init();
  MX_RNG_Init();
  MX_I2C3_Init();

  init_vars();

  KIN1_InitCycleCounter();
  KIN1_EnableCycleCounter();

  __enable_irq();
  cycles_max = 0;

  rate_t0 = HAL_GetTick();
  
  while (1)
  {
    /*
     * Разбор команд RS-485 (SNAP/STATUS/READ) выполняется на КАЖДОЙ
     * итерации главного цикла, а не только внутри блока adc_rdy==1.
     * Иначе ответ на STATUS/READ будет задержан на длительность
     * обработки кадра АЦП, и мастер будет чаще упираться в таймаут.
     */
    RS485_PollCommands();

    if (adc_rdy == 1)
    {
      adc_frame_count++;
      copy_data();

      find_startpixel();

      s_l1 = mes_piks(startpixel1);
      s_l2 = mes_piks(startpixel2);

      find_datablock();

      calc_sector();

      err_corr();

      if (errorflag == 0)
      {
        calc_ang();

        find_avg();

        offset_cal();

        angk_cal();

        angtab_cal();

        cur_ang_E = cur_ang_X;
        angle_update_count++;
      }

      /*
       * Публикация кадра для протокола SNAP/STATUS/READ.
       * calculating заполняется целиком и только потом одним
       * присваиванием фиксируется в published, поэтому published
       * всегда содержит консистентный набор полей одного и того же
       * кадра (не бывает "угол от нового кадра + sector от старого").
       */
      measurement_frame_no++;

      calculating.valid = (errorflag == 0) &&
                           (sector >= 0) && (sector < BIT_TAB_SIZE) &&
                           (rsector >= 0) && (rsector < BIT_TAB_SIZE);
      calculating.sector = calculating.valid ? (int16_t)sector : (int16_t)-1;
      /*
       * ВНИМАНИЕ: пример парсера DATA на мастере (parseData()) жёстко
       * требует 0 <= angle < 360000 и отбрасывает всю строку при
       * нарушении, даже если valid=0. Поэтому при невалидном измерении
       * сюда пишется ПОСЛЕДНИЙ известный угол (из published), а не -1 -
       * иначе мастер будет считать саму строку DATA битой, а не просто
       * получит valid=0. Разбор valid/sector остаётся источником истины
       * о достоверности данных.
       */
      calculating.angle_mdeg = calculating.valid
                                    ? (int32_t)lroundf(cur_ang_E * 1000.0f)
                                    : published.angle_mdeg;
      calculating.state = encoder_state;
      calculating.frame_no = measurement_frame_no;

      published = calculating;

      /* Периодическая безадресная рассылка RS485_SendMeasurement()
       * отключена: на шине с несколькими датчиками несколько
       * передатчиков RS-485, включающихся одновременно каждые 100 мс,
       * гарантированно приводят к коллизиям. Ответы теперь отдаются
       * только адресно, из RS485_OnStatus()/RS485_OnRead(), которые
       * вызываются из RS485_PollCommands() выше. Для одиночной отладки
       * на столе можно временно раскомментировать следующий вызов, но
       * только пока на линии физически один датчик. */
      /* RS485_SendMeasurement(); */

      spi_SendExec();

      dac_ctrl();

      SloComProcess();

      SCB_InvalidateDCache_by_Addr(
          (uint32_t *)aADCxConvertedData,
          ((2 + 31) / 32) * 32);

      Vsense = (aADCxConvertedData[0] * 3.3f) / 4095.0f;
      Temperature = ((Vsense - V25) * 1000.0f / Avg_Slope) + 25.0f;

      cycles = KIN1_GetCycleCounter();
      KIN1_DisableCycleCounter();

      if (cycles > cycles_max)
      {
        cycles_max = cycles;
      }

      adc_rdy = 0;
    }
    if ((HAL_GetTick() - rate_t0) >= 1000U)
    {
        rate_t0 += 1000U;

        adc_frame_rate = adc_frame_count;
        angle_update_rate = angle_update_count;

        adc_frame_count = 0U;
        angle_update_count = 0U;
    }
  }
}

/*
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    HAL_ResumeTick();

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    // ?? ???????? MX_RS485_USART1_Init()
    // ?? ???????? MX_GPIO_Init(), ???? ??? ??????? PB5/PB6

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_5, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_5, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_LOW);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_LOW);
  
    dbg_systick_ctrl = SysTick->CTRL;
    dbg_systick_load = SysTick->LOAD;
    dbg_systick_val  = SysTick->VAL;
    dbg_vtor         = SCB->VTOR;
    dbg_primask      = __get_PRIMASK();
    dbg_ipsr         = __get_IPSR();
    
    while (1)
    {
        // ????? ???????????, D = 1
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
        LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
        HAL_Delay(1000);

        // ?????????? ???????? ???????, D = 0
        LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
        HAL_Delay(1000);
    }
}
*/

/**
  * @brief System Clock Configuration
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
  * @brief ADC1 Initialization Function
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
  * @brief ADC2 Initialization Function
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
  * @brief TIM3 Initialization Function
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
  * @brief TIM4 Initialization Function for PA15 backlight one-shot timing
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
  * @brief DAC Initialization Function
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
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

 //##-1- Configure the CRC peripheral #######################################
  CrcHandle.Instance = CRC;
  // The default polynomial is used
  CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
  // The default init value is used
  CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
  // The input data are not inverted
  CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
  // The output data are not inverted
  CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  // The input data are 32-bit long words
  CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;
  // Initialization CRC
  HAL_CRC_Init(&CrcHandle);

}



/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  //Enable RNG peripheral clock
  __HAL_RCC_RNG_CLK_ENABLE();
  //Initialize RNG
  RNG_Handle.Instance = RNG;
  HAL_RNG_Init(&RNG_Handle);

}





/**
  * @brief I2C3 Initialization Function
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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */


static void MX_SPI1_Init(void)
{

  LL_SPI_DeInit(SPI1);

  //SPI1 initialization
  LL_SPI_InitTypeDef SPI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
  
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /**SPI1 GPIO Configuration  
  PC4    ------> SPI1_NSS
  PA5    ------> SPI1_SCK
  PA6    ------> SPI1_MISO
  PA7    ------> SPI1_MOSI 
  */

  //SPI1_NSS
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_4);

  //SPI1_SCK
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //SPI1_MISO
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  //SPI1_MOSI
  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // SPI1 parameter configuration
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI1, &SPI_InitStruct);

  LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
  LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);

  LL_SPI_Enable(SPI1);                                                          // Enable SPI1

};





/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  // Peripheral clock enable
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5); 
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2); 

  /**SPI2 GPIO Configuration  
  PB12   ------> SPI2_NSS
  PB13   ------> SPI2_SCK
  PB14   ------> SPI2_MISO
  PB15   ------> SPI2_MOSI 
  */
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_12, LL_GPIO_PULL_UP);
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  LL_TIM_DeInit(TIM5);                                                          // Deinit timer
  LL_TIM_SetCounterMode(TIM5, LL_TIM_COUNTERMODE_DOWN);                         // Set the timer counter counting mode
  LL_TIM_SetPrescaler(TIM5, 0);                                                 // Set the pre-scaler value
  NVIC_SetPriority(TIM5_IRQn, 5);                                               // Set TIM5 global Interrupt priority
  NVIC_EnableIRQ(TIM5_IRQn);                                                    // Enable TIM5 global Interrupt
  LL_TIM_EnableIT_UPDATE(TIM5);                                                 // Enable update interrupt (UIE)
 
  LL_TIM_DeInit(TIM2);                                                          // Deinit timer
  LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_DOWN);                         // Set the timer counter counting mode
  LL_TIM_SetPrescaler(TIM2, 0);                                                 // Set the pre-scaler value
  NVIC_SetPriority(TIM2_IRQn, 5);                                               // Set TIM5 global Interrupt priority
  NVIC_EnableIRQ(TIM2_IRQn);                                                    // Enable TIM2 global Interrupt
  LL_TIM_EnableIT_UPDATE(TIM2);                                                 // Enable update interrupt (UIE)


  LL_SPI_InitTypeDef SPI_InitStruct = {0};
  LL_SPI_DeInit(SPI2);
  // SPI2 parameter configuration
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI2, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableDMAReq_TX(SPI2);                                                 //Enable SPI DMA TX Requsts
  LL_SPI_EnableDMAReq_RX(SPI2);                                                 //Enable SPI DMA RX Requsts


  // SPI2_TX Init
  LL_DMA_DeInit(DMA1, LL_DMA_STREAM_4);
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_0);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_4, LL_DMA_PRIORITY_MEDIUM);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_4, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_4, LL_DMA_PDATAALIGN_HALFWORD);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_4, LL_DMA_MDATAALIGN_HALFWORD);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, SPI_TxBufSize);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_4, (uint32_t) &aTxBuffer );
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_4, (uint32_t) &(SPI2->DR) );
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_4);
  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_MBURST_SINGLE);
  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_4, LL_DMA_PBURST_SINGLE);
  // Configure NVIC for DMA transfer complete/error interrupts
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_4);
  NVIC_SetPriority(DMA1_Stream4_IRQn, 7);
  NVIC_EnableIRQ(DMA1_Stream4_IRQn);

  // SPI2_RX Init
  LL_DMA_DeInit(DMA1, LL_DMA_STREAM_3);
  LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_0);
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_3, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
  LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_3, LL_DMA_PRIORITY_MEDIUM);
  LL_DMA_SetMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_HALFWORD);
  LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_HALFWORD);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_3, (uint32_t) &aRxBuffer );
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_3, (uint32_t) &(SPI2->DR) );
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, SPI_RxBufSize);
  LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_3);
  LL_DMA_SetMemoryBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_MBURST_SINGLE);
  LL_DMA_SetPeriphBurstxfer(DMA1, LL_DMA_STREAM_3, LL_DMA_PBURST_SINGLE);
  // Configure NVIC for DMA transfer complete/error interrupts
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);
  NVIC_SetPriority(DMA1_Stream3_IRQn, 6);
  NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}




// @brief  This function handles DMA1_STREAM4 (SPI2 TC) interrupt request.
// @param  None
// @retval : None
void DMA1_Stream4_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC4(DMA1) == 1) {                                      //If Stream 4 transfer complete flag
    LL_DMA_ClearFlag_TC4(DMA1);                                                 // Clear Stream 4 transfer complete flag
    LL_DMA_DisableStream(DMA1,LL_DMA_STREAM_4);                                 // Disable the DMA transfer
    spi_send_cnt++;                                                             // increment sent frames counter
    LL_TIM_SetCounter(TIM5, 1000);                                              // Set the timer counter value
    LL_TIM_EnableCounter(TIM5);                                                 // Enable timer counter
  }
}




// @brief  This function handles DMA1_STREAM3 (SPI2 RC) interrupt request.
// @param  None
// @retval : None
void DMA1_Stream3_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC3(DMA1) == 1) {                                      //If Stream 3 transfer complete flag
    LL_DMA_ClearFlag_TC3(DMA1);                                                 // Clear DMA1 Stream 3 transfer complete flag
    LL_DMA_DisableStream(DMA1,LL_DMA_STREAM_3);                                 // Disable the DMA1 Stream3 transfer
    SCB_InvalidateDCache_by_Addr((uint32_t *)aRxBuffer,((SPI_RxBufSize*2+31)/32)*32);//force to let update caches again with memory content to see the changes
    Recv_SPI();                                                                 // process received frame
  }
}



/**
* @brief  This function handles TIM5 interrupt.
* @param  None
* @retval None
*/
void TIM5_IRQHandler(void)
{
  if(LL_TIM_IsActiveFlag_UPDATE(TIM5) == 1) {                                   //If UPDATE interrupt is pending
    LL_TIM_ClearFlag_UPDATE(TIM5);                                              // Clear the update interrupt flag
    LL_TIM_DisableCounter(TIM5);                                                // Disable counter
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);                                // NSS = 1
  }
}





// @brief  Recv_SPI(). This function handles rx buffer
// @param  None 
// @retval None 
//-----------------------------------------------------------------------------------
void Recv_SPI() {
  recv_num_prev++;                                                              //increment last recv frame number
  recv_num=(aRxBuffer[1]&0xff);                                                 //frame number
  spi_recv_p2 = (uint32_t*)&aRxBuffer[2];                                       //recv buffer address
  __disable_irq ();                                                             //disable IRQ
  LL_CRC_ResetCRCCalculationUnit(CRC);                                          //init crc32 calc
  LL_CRC_FeedData32(CRC, aRxBuffer[1]);                                         //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_recv_p2++);                                       //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_recv_p2++);                                       //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_recv_p2++);                                       //add data to crc32
  spi_recv_CRCValue = ~LL_CRC_ReadData32(CRC);                                  //read calculated crc32
  __enable_irq ();                                                              //enable IRQ
  spi_recv_crc32=*spi_recv_p2++;                                                //read frame crc32
  if(spi_recv_CRCValue == spi_recv_crc32) {                                     //if frame crc correct
    if (recv_num_prev!=recv_num) {                                              // if frame num not ok
      spi_lostframes++;                                                         //  increment lost frames counter
    }
    else {                                                                      // if frame num ok
      spi_recv_process ((void*)&aRxBuffer[1]);                                  //  process received frame
      spi_goodframes++;                                                         //  increment good frames counter
    }
  }
  else {                                                                        //if frame crc bad
    spi_badframes++;                                                            // increment error frames counter
    MX_SPI2_Init();                                                             // reinit SPI
  }
  recv_num_prev=recv_num;                                                       //store last recv frame number
  memset(&aRxBuffer[0],0,SPI_RxBufSize*2);                                      //clear recv buffer
}





/*******************************************************************************
* Function Name  : Send_SPI
* Description    : This function handles start of transmiting packet
* Input          : trm_buff
* Output         : None
* Return         : None
*******************************************************************************/
void Send_SPI(unsigned char *trm_buff) {
  aTxBuffer[0]=(0x55<<8)|(spi_send_cnt&0xff);                                   //preamble and frame number
  memcpy(&aTxBuffer[1],trm_buff,14);                                            //copy data to send buffer
  spi_send_p2 = (uint32_t*)&aTxBuffer[0];                                       //init send buffer address
  __disable_irq ();                                                             //disable IRQ
  LL_CRC_ResetCRCCalculationUnit(CRC);                                          //init crc32 calc
  LL_CRC_FeedData32(CRC, *spi_send_p2++);                                       //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_send_p2++);                                       //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_send_p2++);                                       //add data to crc32
  LL_CRC_FeedData32(CRC, *spi_send_p2++);                                       //add data to crc32
  spi_send_CRCValue = ~LL_CRC_ReadData32(CRC);                                  //read calculated crc32
   __enable_irq ();                                                             //enable IRQ
  *spi_send_p2++=spi_send_CRCValue;                                             //write calculated crc32 to send buffer
  rng_value=HAL_RNG_GetRandomNumber(&RNG_Handle)&0x00000FFF;                    //get random
  LL_TIM_SetCounter(TIM2, 10+(1000+rng_value)*config_state);                    //Set the timer counter value
  LL_TIM_EnableCounter(TIM2);                                                   //Enable timer counter
}




/**
* @brief  This function handles TIM2 interrupt.
* @param  None
* @retval None
*/
void TIM2_IRQHandler(void)
{
  if(LL_TIM_IsActiveFlag_UPDATE(TIM2) == 1) {                                   //If UPDATE interrupt is pending
    LL_TIM_ClearFlag_UPDATE(TIM2);                                              // Clear the update interrupt flag
    LL_TIM_DisableCounter(TIM2);                                                // Disable counter
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_12);                              // NSS = 0
    LL_SPI_Enable(SPI2);                                                        // Enable SPI2
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_4, (uint32_t) &aTxBuffer );     // Set TX Memory address
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, SPI_TxBufSize);                 // Set TX Number of data to transfer
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_3, (uint32_t) &aRxBuffer );     // Set RX Memory address
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, SPI_RxBufSize);                 // Set RX Number of data to transfer
    SCB_CleanDCache_by_Addr((uint32_t *)aTxBuffer, ((SPI_TxBufSize*2+31)/32)*32 );// force to let update the memory with cache content
    LL_DMA_EnableStream(DMA1,LL_DMA_STREAM_4);                                  // Start the TX DMA transfer
    LL_DMA_EnableStream(DMA1,LL_DMA_STREAM_3);                                  // Start the RX DMA transfer
  }
}





/**
  * @brief  spi_recv_process
  * @param  *recv_buff
  * @retval None 
  */
/*-----------------------------------------------------------------------------------*/
void spi_recv_process (unsigned char *recv_buff) {

  recv_buff++;
  command = recv_buff[0];                                                       //command
  
  switch (command)  {


      //read BadFrames command
      case spi_ReadBadFrames:
        s_buf[0]=spi_ReadBadFrames;
        memcpy(&s_buf[10],&spi_badframes,4);
        spi_SendBuf2(&s_buf[0],14);
        break;

      //read temperature command
      case spi_ReadTemperature:
        s_buf[0]=spi_ReadTemperature;                                           //response code
        memcpy(&s_buf[6],&Temperature,4);                                       //read temperature
        spi_SendBuf2(&s_buf[0],14);                                             //send response
        break;


      //prog start block
      case spi_ProgStartBlock:
        memset(&sec_buffer[0],0xff,4096);                                       //clear block buffer
        s_buf[0]=spi_ProgStartBlock;                                            //response code
        spi_SendBuf1(&s_buf[0],14);                                             //send response
        break;

      //prog send data
      case spi_ProgSendData:
        memcpy(&prog_offset,&recv_buff[1],2);                                   //read buffer offset
        if (prog_offset>4092) prog_offset=4092;                                 //limit offset
        memcpy(&sec_buffer[prog_offset],&recv_buff[3],4);                       //copy buffer fragment
        s_buf[0]=spi_ProgSendData;                                              //response code
        memcpy(&s_buf[6],&recv_buff[1],6);                                      //response data
        spi_SendBuf1(&s_buf[0],14);                                             //send response
        break;

      //prog end block
      case spi_ProgEndBlock:
        memcpy(&prog_crc,&recv_buff[1],2);                                      //read block crc16
        memcpy(&prog_len,&recv_buff[3],2);                                      //read block length
        memcpy(&prog_addr,&recv_buff[5],4);                                     //read block address
        SloCom=spi_ProgEndBlock;
        break;

      //prog update header
      case spi_ProgUpdateHeader:
        memcpy(&update_len,&recv_buff[1],4);                                    //read update length
        memcpy(&update_crc,&recv_buff[5],4);                                    //read update crc32
        memcpy(&update_time,&recv_buff[9],4);                                   //read update time
        SloCom=spi_ProgUpdateHeader;
        break;

      //prog update check
      case spi_ProgUpdateCheck:
        SloCom=spi_ProgUpdateCheck;
        break;

      //prog update check2
      case spi_ProgUpdateCheck2:
        s_buf[0]=spi_ProgUpdateCheck2;                                          //response code
        memcpy(&s_buf[6],&update_crc,4);                                        //update crc
        memcpy(&s_buf[10],&update_time,4);                                      //update time
        spi_SendBuf1(&s_buf[0],14);                                             //send response
        break;

      //execute boot command
      case spi_ProgExecBoot:
        s_buf[0]=spi_ProgExecBoot;                                              //response code
        s_buf[6]=0;                                                             //error code
        spi_SendBuf1(&s_buf[0],14);                                             //send response
        SloCom=spi_ProgExecBoot;
        break;

      //check soft version command
      case spi_ProgGetVersion:
        s_buf[0]=spi_ProgGetVersion;                                            //response code
        memcpy(&s_buf[6],&update_version,2);                                    //update version
        memcpy(&s_buf[8],&prog_errcode,1);                                      //read errcode
        spi_SendBuf2(&s_buf[0],14);                                             //send response
        break;




      //stop calibration
      case 0x10:
        start_calibrate=0;
        auto_cal=0;
        start_offset_cal=0;
        cycles_max=0;
        encoder_state=0x00;
        break;
          
      //angle calibration right
      case 0x11:
        start_calibrate=1;
        encoder_state=0x10;
        break;

      //angle calibration left
      case 0x12:
        start_calibrate=2;
        encoder_state=0x30;
        break;


      //offset calibration
      case 0x13:
        start_offset_cal=1;
        avg_minmax_num=0;
        offset_phase=0;
        for (int i=0;i<=127;i++) {
          offset_minmax[i]=1;
        }
        offset_cur=offset_start;
        encoder_state=0x50;
        break;
        
      //bufk calibration
      case 0x14:
        start_angk_cal=1;
        avg_minmax_num=0;
        for (int i=0;i<128;i++) buf_x3[i]=0;
        anglek_phase=0;
        backlight_width_en=0;
        encoder_state=0x70;
        break;
     
        
      default:
        break;

    }




}








/**
  * @brief  Free all buffers SPI
  * @param  None
  * @retval None
  */

void spi_FreeAll() {
  
  spi_buf1.used=0;
  spi_buf2.used=0;
  
} 





/**
  * @brief  This function puts data to send buffer SPI
  * @param  data: pointer to data
  *         len:  length of data
    * @retval 0: data sent to buffer
  *         -1: error occurred
  */

int spi_SendBuf1(unsigned char *data, unsigned char len) {

  if ( (len>16) || (len<1)) {                            //error - invalid length
    return -2;
  }
  
  if (spi_buf1.used==0) {
    spi_buf1.used=1;
    spi_buf1.len=len;
    memcpy(&spi_buf1.data[0], data, len);
    return 0;                                           //data sent to buffer
  }

  return -1;                                            //error - no free buffer
 
}



/**
  * @brief  This function puts data to send buffer SPI
  * @param  data: pointer to data
  *         len:  length of data
    * @retval 0: data sent to buffer
  *         -1: error occurred
  */

int spi_SendBuf2(unsigned char *data, unsigned char len) {

  if ( (len>16) || (len<1)) {                            //error - invalid length
    return -2;
  }
  
  if (spi_buf2.used==0) {
    spi_buf2.used=1;
    spi_buf2.len=len;
    memcpy(&spi_buf2.data[0], data, len);
    return 0;                                           //data sent to buffer
  }

  return -1;                                            //error - no free buffer
 
}





/**
  * @brief  Send and free one data buffer SPI
  * @param  None
  * @retval None
  */

void spi_SendExec() {
    
  if (spi_buf1.used==1) {
    spi_buf1.used=0;
    spi_buf1.data[1]=encoder_state;                                             //state
    memcpy(&spi_buf1.data[2],&cur_ang_E,4);                                     //angle
    Send_SPI(&spi_buf1.data[0]);
  }
  else if (spi_buf2.used==1) {
    spi_buf2.used=0;
    spi_buf2.data[1]=encoder_state;                                             //state
    memcpy(&spi_buf2.data[2],&cur_ang_E,4);                                     //angle
    Send_SPI(&spi_buf2.data[0]);
  }
  else {
    TxBuffer[0]=spi_ReadStatus;                                                 //command
    TxBuffer[1]=encoder_state;                                                  //state
    memcpy(&TxBuffer[2],&cur_ang_E,4);                                          //angle
    Send_SPI(TxBuffer);
  }
  
}








/**
  * @brief GPIO Initialization Function
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
  * @brief This function handles DMA2 stream3 global interrupt.
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
  * @brief  This function handles USART1 (RS-485) global interrupt.
  *         Делает минимум работы: перекладывает принятый байт в кольцевой
  *         буфер, а сама разборка команд идёт из главного цикла
  *         (RS485_PollCommands()). Так интервал занятости прерывания не
  *         зависит от длины/содержимого команды.
  * @retval None
  */
void USART1_IRQHandler(void)
{
  if (LL_USART_IsActiveFlag_RXNE(RS485_USART) && LL_USART_IsEnabledIT_RXNE(RS485_USART)) {
    uint8_t byte = LL_USART_ReceiveData8(RS485_USART);
    RS485_RxByteFromISR(byte);
  }

  /* Ошибки линии (обрыв/помеха/переполнение) не должны "подвешивать"
   * приём - сбрасываем флаги и продолжаем, поврежденная строка будет
   * просто отброшена в RS485_PollCommands() при получении '\n'. */
  if (LL_USART_IsActiveFlag_ORE(RS485_USART)) {
    LL_USART_ClearFlag_ORE(RS485_USART);
  }
  if (LL_USART_IsActiveFlag_NE(RS485_USART)) {
    LL_USART_ClearFlag_NE(RS485_USART);
  }
  if (LL_USART_IsActiveFlag_FE(RS485_USART)) {
    LL_USART_ClearFlag_FE(RS485_USART);
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
