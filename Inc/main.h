/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_dac.h"
#include "stm32f7xx_hal_rng.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_dac.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_ll_tim.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_rng.h"
#include "stm32f7xx_ll_wwdg.h"
#include "stm32f7xx.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/


//------------------------- REM_BUF --------------------------
typedef  struct  rem_buf {
    unsigned char used;
    unsigned char data[16];                         /*                                                 */
    unsigned char len;                               /*                                                */
} REM_BUF;                                        



/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */



#define SPI_RxBufSize  10
#define SPI_TxBufSize  10





#define spi_ReadStatus		        0x25		                        //read status command
#define spi_ReadBadFrames	        0x61		                        //read badframes command
#define spi_ReadTemperature	        0x62		                        //read temperature command

#define spi_ProgSendData	        0x71		                        //prog send data
#define spi_ProgStartBlock	        0x72		                        //prog start block
#define spi_ProgEndBlock	        0x73		                        //prog end block
#define spi_ProgUpdateHeader	        0x74		                        //prog update header
#define spi_ProgUpdateCheck	        0x75		                        //prog update check
#define spi_ProgUpdateCheck2	        0x76		                        //prog update check2
#define spi_ProgFlash   	        0x77		                        //program update to FLASH
#define spi_ProgExecWork    	        0x78		                        //execute updated soft command
#define spi_ProgGetVersion              0x79		                        //check soft version command
#define spi_ProgExecBoot    	        0x7a		                        //execute boot soft command



/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);



void eeprom_init();
uint8_t eeprom_read_byte(uint16_t eeprom_address);
void eeprom_write_byte(uint16_t eeprom_address, unsigned char value);


/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DAC_L_Pin LL_GPIO_PIN_4
#define DAC_L_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
