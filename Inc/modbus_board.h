#ifndef MODBUS_BOARD_H
#define MODBUS_BOARD_H

#define MB_SLAVE_ADDRESS 2U  /* каждой головке свой адрес 1..247; 0 — вещание */

#if MB_SLAVE_ADDRESS < 1 || MB_SLAVE_ADDRESS > 247
#error "MB_SLAVE_ADDRESS must be unique on the bus and in [1,247]"
#endif
#define MB_BAUD 230400U
#define MB_TX_PIN LL_GPIO_PIN_6
#define MB_RX_PIN LL_GPIO_PIN_7
#define MB_DE_PIN LL_GPIO_PIN_5
#define MB_NRE_PIN LL_GPIO_PIN_4
#define MB_TX_PORT GPIOB
#define MB_RX_PORT GPIOB
#define MB_DE_PORT GPIOB
#define MB_NRE_PORT GPIOB

#if defined(USART1) && defined(DWT)
extern volatile uint32_t modbus_rxne_probe_cyccnt;
extern volatile uint8_t modbus_rxne_probe_valid;

/* Беспрырывный захват метки DWT->CYCCNT в момент появления байта в USART1->RDR
 * при закрытых прерываниях (внутри 22.5-мкс обработчика линейки DMA2_Stream3_IRQHandler).
 * Не читает RDR, поэтому аппаратный флаг RXNE сохраняется для штатного USART1_IRQHandler,
 * а 25-мкс «полка» квантования в age_us полностью исчезает (разрешение ~0.05..0.7 мкс). */
static inline void modbus_poll_rx_cyccnt(void)
{
    if (!modbus_rxne_probe_valid &&
        ((USART1->ISR & USART_ISR_RXNE) != 0U) &&
        ((USART1->CR1 & USART_CR1_RXNEIE) != 0U)) {
        modbus_rxne_probe_cyccnt = DWT->CYCCNT;
        modbus_rxne_probe_valid = 1U;
    }
}
#else
static inline void modbus_poll_rx_cyccnt(void) {}
#endif

#endif
