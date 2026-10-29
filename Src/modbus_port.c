/*
 * Адаптация стека FreeModbus к аппаратуре STM32F722RE.
 *
 * FreeModbus (ThirdParty/FreeModbus) реализует Modbus RTU: адресацию,
 * кадрирование, CRC16 и функции протокола. Этот файл реализует только
 * требуемые стеком операции: очередь событий, USART1, управление направлением
 * RS485, однократный таймер тишины TIM2 и захват тактовой метки DWT->CYCCNT
 * последнего байта входящего кадра для точного расчёта возраста экспозиции.
 */
#include "main.h"
#include "modbus_board.h"
#include "mb.h"
#include "mbport.h"
#include "snapshot_registers.h"

/* Символ, считанный из RDR обработчиком USART1; FreeModbus сразу забирает его
 * через xMBPortSerialGetByte(), поэтому здесь достаточно одного байта. */
static volatile CHAR rx_byte;
/* TXE сообщает лишь об опустевшем регистре передачи. Пока tx_finishing=1,
 * последний байт ещё может передаваться; DE сбрасывается только по TC. */
static volatile BOOL tx_finishing;

/* Тактовая метка DWT->CYCCNT (216 МГц) прихода последнего байта по USART1 RX
 * и счётчик аппаратных ошибок приёма (ORE/FE/NE/PE) для диагностики по SWD. */
static volatile uint32_t usart1_last_rx_cyccnt;
volatile uint32_t modbus_uart_error_count;

/* События рождаются в прерываниях UART/TIM2, а разбираются eMBPoll() из main. */
#define EVENT_CAPACITY 8
static volatile eMBEventType events[EVENT_CAPACITY];
static volatile uint8_t event_head, event_tail;

BOOL xMBPortEventInit(void)
{
    event_head = event_tail = 0;
    return TRUE;
}

BOOL xMBPortEventPost(eMBEventType event)
{
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    uint8_t next = (uint8_t)((event_head + 1) % EVENT_CAPACITY);
    if (next == event_tail) { __set_PRIMASK(saved); return FALSE; }
    events[event_head] = event;
    event_head = next;
    __set_PRIMASK(saved);
    return TRUE;
}

BOOL xMBPortEventGet(eMBEventType *event)
{
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    if (event_head == event_tail) { __set_PRIMASK(saved); return FALSE; }
    *event = events[event_tail];
    event_tail = (uint8_t)((event_tail + 1) % EVENT_CAPACITY);
    __set_PRIMASK(saved);
    return TRUE;
}

static void transceiver_rx(void)
{
    LL_GPIO_ResetOutputPin(MB_DE_PORT, MB_DE_PIN);
    LL_GPIO_ResetOutputPin(MB_NRE_PORT, MB_NRE_PIN);
}

static void transceiver_tx(void)
{
    LL_GPIO_SetOutputPin(MB_NRE_PORT, MB_NRE_PIN);
    LL_GPIO_SetOutputPin(MB_DE_PORT, MB_DE_PIN);
}

BOOL xMBPortSerialInit(UCHAR port, ULONG baud, UCHAR data_bits,
                       eMBParity parity, UCHAR stop_bits)
{
    if (port != 1 || baud != MB_BAUD || data_bits != 8 ||
        parity != MB_PAR_EVEN || stop_bits != 1) return FALSE;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_USART_Disable(USART1);
    LL_GPIO_SetAFPin_0_7(MB_TX_PORT, MB_TX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetAFPin_0_7(MB_RX_PORT, MB_RX_PIN, LL_GPIO_AF_7);
    LL_GPIO_SetPinMode(MB_TX_PORT, MB_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(MB_RX_PORT, MB_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(MB_TX_PORT, MB_TX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(MB_TX_PORT, MB_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(MB_RX_PORT, MB_RX_PIN, LL_GPIO_PULL_UP);
    LL_GPIO_ResetOutputPin(MB_DE_PORT, MB_DE_PIN);
    LL_GPIO_ResetOutputPin(MB_NRE_PORT, MB_NRE_PIN);
    LL_GPIO_SetPinMode(MB_DE_PORT, MB_DE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinMode(MB_NRE_PORT, MB_NRE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(MB_DE_PORT, MB_DE_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinOutputType(MB_NRE_PORT, MB_NRE_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    transceiver_rx();
    LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetDataWidth(USART1, LL_USART_DATAWIDTH_9B);
    LL_USART_SetParity(USART1, LL_USART_PARITY_EVEN);
    LL_USART_SetStopBitsLength(USART1, LL_USART_STOPBITS_1);
    LL_USART_SetBaudRate(USART1, HAL_RCC_GetPCLK2Freq(), LL_USART_OVERSAMPLING_16, baud);
    LL_USART_Enable(USART1);
    /* Высший приоритет (0) короткому обработчику байтов USART1 (~80 тактов):
     * исключает потерю байтов (ORE) во время 20-мкс выдержки tqt в DMA2_Stream3 (приоритет 1)
     * и фиксирует момент прихода последнего байта SNAP с джиттером < 0.8 мкс. */
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
    return TRUE;
}

void vMBPortSerialEnable(BOOL rx, BOOL tx)
{
    LL_USART_DisableIT_RXNE(USART1);
    LL_USART_DisableIT_TXE(USART1);
    if (tx) {
        tx_finishing = FALSE;
        LL_USART_DisableIT_TC(USART1);
        LL_USART_ClearFlag_TC(USART1);
        transceiver_tx();
        LL_USART_EnableIT_TXE(USART1);
    } else if (LL_GPIO_IsOutputPinSet(MB_DE_PORT, MB_DE_PIN)) {
        tx_finishing = TRUE;
        LL_USART_EnableIT_TC(USART1);
    } else if (rx) {
        LL_USART_EnableIT_RXNE(USART1);
    }
}

BOOL xMBPortSerialGetByte(CHAR *byte) { *byte = rx_byte; return TRUE; }
BOOL xMBPortSerialPutByte(CHAR byte)
{
    LL_USART_TransmitData8(USART1, (uint8_t)byte);
    return TRUE;
}

/* Таймер межкадровой паузы RTU: аргумент FreeModbus в единицах 50 мкс.
 * При скорости 230400 бод библиотека запрашивает 4 * 50 = 200 мкс. */
BOOL xMBPortTimersInit(USHORT timeout_50us)
{
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq() * 2U;
    if (!timeout_50us || tim_clk % 1000000U) return FALSE;
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetPrescaler(TIM2, tim_clk / 1000000U - 1U);
    LL_TIM_SetAutoReload(TIM2, timeout_50us * 50U - 1U);
    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
    return TRUE;
}

void vMBPortTimersEnable(void)
{
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
}
void vMBPortTimersDisable(void) { LL_TIM_DisableCounter(TIM2); LL_TIM_ClearFlag_UPDATE(TIM2); }

void USART1_IRQHandler(void)
{
    uint32_t err = 0U;
    if (LL_USART_IsActiveFlag_ORE(USART1)) { LL_USART_ClearFlag_ORE(USART1); err = 1U; }
    if (LL_USART_IsActiveFlag_FE(USART1))  { LL_USART_ClearFlag_FE(USART1);  err = 1U; }
    if (LL_USART_IsActiveFlag_NE(USART1))  { LL_USART_ClearFlag_NE(USART1);  err = 1U; }
    if (LL_USART_IsActiveFlag_PE(USART1))  { LL_USART_ClearFlag_PE(USART1);  err = 1U; }
    if (err) modbus_uart_error_count++;

    if (LL_USART_IsEnabledIT_RXNE(USART1) && LL_USART_IsActiveFlag_RXNE(USART1)) {
        usart1_last_rx_cyccnt = DWT->CYCCNT;
        rx_byte = (CHAR)LL_USART_ReceiveData8(USART1);
        pxMBFrameCBByteReceived();
    }
    if (LL_USART_IsEnabledIT_TXE(USART1) && LL_USART_IsActiveFlag_TXE(USART1))
        pxMBFrameCBTransmitterEmpty();
    if (tx_finishing && LL_USART_IsActiveFlag_TC(USART1)) {
        LL_USART_DisableIT_TC(USART1);
        LL_USART_ClearFlag_TC(USART1);
        tx_finishing = FALSE;
        if (LL_USART_IsActiveFlag_RXNE(USART1))
            (void)LL_USART_ReceiveData8(USART1);
        transceiver_rx();
        LL_USART_EnableIT_RXNE(USART1);
    }
}

void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        LL_TIM_DisableCounter(TIM2);
        snapshot_set_rx_cyccnt(usart1_last_rx_cyccnt);
        pxMBPortCBTimerExpired();
    }
}
