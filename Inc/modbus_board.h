#ifndef MODBUS_BOARD_H
#define MODBUS_BOARD_H

#define MB_SLAVE_ADDRESS 1U  /* каждой головке свой адрес 1..247; 0 — вещание */

#if MB_SLAVE_ADDRESS < 1 || MB_SLAVE_ADDRESS > 247
#error "MB_SLAVE_ADDRESS must be unique on the bus and in [1,247]"
#endif
#define MB_BAUD 115200U
#define MB_TX_PIN LL_GPIO_PIN_6
#define MB_RX_PIN LL_GPIO_PIN_7
#define MB_DE_PIN LL_GPIO_PIN_5
#define MB_NRE_PIN LL_GPIO_PIN_4
#define MB_TX_PORT GPIOB
#define MB_RX_PORT GPIOB
#define MB_DE_PORT GPIOB
#define MB_NRE_PORT GPIOB
#endif
