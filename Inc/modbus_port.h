/*
 * STM32F722 RS-485 / FreeModbus port configuration.
 *
 * Defaults are a conflict-free proposal for this repository:
 *   USART3_TX PC10 -> DI (D) of the RS-485 transceiver
 *   USART3_RX PC11 <- RO (R) of the RS-485 transceiver
 *   PB14           -> tied DE + /RE direction input (1=transmit, 0=receive)
 *
 * Confirm these three nets against the PCB schematic before flashing.  Override
 * the macros in the project preprocessor settings or edit this file if another
 * USART/pin was routed.  Never assign the debug connector's TCK/TMS/SWO/reset
 * pins to this interface.
 */
#ifndef HARD_ENCODER_MODBUS_PORT_H
#define HARD_ENCODER_MODBUS_PORT_H

#include "main.h"

#ifndef MODBUS_SLAVE_ADDRESS
#define MODBUS_SLAVE_ADDRESS             1U
#endif

#ifndef MODBUS_BAUDRATE
#define MODBUS_BAUDRATE                  115200UL
#endif

#ifndef MODBUS_USART_INSTANCE
#define MODBUS_USART_INSTANCE            USART3
#endif
#ifndef MODBUS_USART_IRQN
#define MODBUS_USART_IRQN                USART3_IRQn
#endif
#ifndef MODBUS_USART_IRQ_HANDLER
#define MODBUS_USART_IRQ_HANDLER         USART3_IRQHandler
#endif
#ifndef MODBUS_USART_CLOCK_ENABLE
#define MODBUS_USART_CLOCK_ENABLE()      LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#endif
#ifndef MODBUS_USART_PCLK_HZ
#define MODBUS_USART_PCLK_HZ()           HAL_RCC_GetPCLK1Freq()
#endif

#ifndef MODBUS_USART_GPIO_PORT
#define MODBUS_USART_GPIO_PORT           GPIOC
#endif
#ifndef MODBUS_USART_TX_PIN
#define MODBUS_USART_TX_PIN              LL_GPIO_PIN_10
#endif
#ifndef MODBUS_USART_RX_PIN
#define MODBUS_USART_RX_PIN              LL_GPIO_PIN_11
#endif
#ifndef MODBUS_USART_AF
#define MODBUS_USART_AF                  LL_GPIO_AF_7
#endif
#ifndef MODBUS_USART_GPIO_CLOCK_ENABLE
#define MODBUS_USART_GPIO_CLOCK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)
#endif

#ifndef MODBUS_RS485_DE_GPIO_PORT
#define MODBUS_RS485_DE_GPIO_PORT        GPIOB
#endif
#ifndef MODBUS_RS485_DE_PIN
#define MODBUS_RS485_DE_PIN              LL_GPIO_PIN_14
#endif
#ifndef MODBUS_RS485_DE_GPIO_CLOCK_ENABLE
#define MODBUS_RS485_DE_GPIO_CLOCK_ENABLE() LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB)
#endif
/* Set to 0 only if the board inserts an inverter in the RE/DE signal. */
#ifndef MODBUS_RS485_DE_ACTIVE_HIGH
#define MODBUS_RS485_DE_ACTIVE_HIGH      1U
#endif

#endif /* HARD_ENCODER_MODBUS_PORT_H */
