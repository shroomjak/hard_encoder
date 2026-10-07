/*
 * STM32F722 bare-metal port layer for the BSD-licensed FreeModbus RTU slave.
 * The protocol state machine lives in Middlewares/Third_Party/FreeModbus;
 * this file only binds it to USART3, RS-485 direction GPIO and TIM7.
 */
#include "modbus_port.h"
#include "mb.h"
#include "mbport.h"

#define MODBUS_EVENT_QUEUE_LEN 4U
#define MODBUS_TIMER_TICK_HZ   20000UL /* one tick = 50 us */

static volatile eMBEventType modbus_event_queue[MODBUS_EVENT_QUEUE_LEN];
static volatile uint8_t modbus_event_head;
static volatile uint8_t modbus_event_tail;

/* True from first TXE until the USART reports the final stop bit (TC). */
static volatile BOOL modbus_tx_active;
static volatile BOOL modbus_rx_after_tx;
static USHORT modbus_t35_ticks;

static uint32_t ModbusPort_EnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static void ModbusPort_ExitCritical(uint32_t primask)
{
    if (primask == 0U) {
        __enable_irq();
    }
}

static void ModbusPort_SetDriverTransmit(BOOL transmit)
{
#if MODBUS_RS485_DE_ACTIVE_HIGH
    if (transmit != FALSE) {
        LL_GPIO_SetOutputPin(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN);
    } else {
        LL_GPIO_ResetOutputPin(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN);
    }
#else
    if (transmit != FALSE) {
        LL_GPIO_ResetOutputPin(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN);
    } else {
        LL_GPIO_SetOutputPin(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN);
    }
#endif
}

static void ModbusPort_SetAlternateFunction(GPIO_TypeDef *port, uint32_t pin, uint32_t alternate)
{
    if ((pin & 0xFF00U) != 0U) {
        LL_GPIO_SetAFPin_8_15(port, pin, alternate);
    } else {
        LL_GPIO_SetAFPin_0_7(port, pin, alternate);
    }
}

/* ---------------------------- event port -------------------------------- */
BOOL xMBPortEventInit(void)
{
    uint32_t primask = ModbusPort_EnterCritical();
    modbus_event_head = 0U;
    modbus_event_tail = 0U;
    ModbusPort_ExitCritical(primask);
    return TRUE;
}

BOOL xMBPortEventPost(eMBEventType event)
{
    uint8_t next;
    uint32_t primask = ModbusPort_EnterCritical();

    next = (uint8_t)((modbus_event_head + 1U) % MODBUS_EVENT_QUEUE_LEN);
    if (next == modbus_event_tail) {
        /* A full queue means foreground code has not serviced the stack. */
        ModbusPort_ExitCritical(primask);
        return FALSE;
    }

    modbus_event_queue[modbus_event_head] = event;
    modbus_event_head = next;
    ModbusPort_ExitCritical(primask);
    return TRUE;
}

BOOL xMBPortEventGet(eMBEventType *event)
{
    uint32_t primask;

    if (event == 0) {
        return FALSE;
    }

    primask = ModbusPort_EnterCritical();
    if (modbus_event_tail == modbus_event_head) {
        ModbusPort_ExitCritical(primask);
        return FALSE;
    }

    *event = modbus_event_queue[modbus_event_tail];
    modbus_event_tail = (uint8_t)((modbus_event_tail + 1U) % MODBUS_EVENT_QUEUE_LEN);
    ModbusPort_ExitCritical(primask);
    return TRUE;
}

/* --------------------------- serial port --------------------------------- */
BOOL xMBPortSerialInit(UCHAR port, ULONG baudrate, UCHAR data_bits,
                       eMBParity parity, UCHAR stop_bits)
{
    uint32_t parity_mode;
    uint32_t word_length;
    uint32_t stop_mode;

    (void)port;
    if (data_bits != 8U) {
        return FALSE;
    }

    if (parity == MB_PAR_NONE) {
        word_length = LL_USART_DATAWIDTH_8B;
        parity_mode = LL_USART_PARITY_NONE;
    } else {
        /* STM32 counts its parity bit in the word length: 8E1 is 9B + E. */
        word_length = LL_USART_DATAWIDTH_9B;
        parity_mode = (parity == MB_PAR_ODD) ? LL_USART_PARITY_ODD : LL_USART_PARITY_EVEN;
    }
    stop_mode = (stop_bits == 2U) ? LL_USART_STOPBITS_2 : LL_USART_STOPBITS_1;

    MODBUS_USART_CLOCK_ENABLE();
    MODBUS_USART_GPIO_CLOCK_ENABLE();
    MODBUS_RS485_DE_GPIO_CLOCK_ENABLE();

    LL_GPIO_SetPinMode(MODBUS_USART_GPIO_PORT, MODBUS_USART_TX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinOutputType(MODBUS_USART_GPIO_PORT, MODBUS_USART_TX_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(MODBUS_USART_GPIO_PORT, MODBUS_USART_TX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(MODBUS_USART_GPIO_PORT, MODBUS_USART_TX_PIN, LL_GPIO_PULL_NO);
    ModbusPort_SetAlternateFunction(MODBUS_USART_GPIO_PORT, MODBUS_USART_TX_PIN, MODBUS_USART_AF);

    LL_GPIO_SetPinMode(MODBUS_USART_GPIO_PORT, MODBUS_USART_RX_PIN, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinSpeed(MODBUS_USART_GPIO_PORT, MODBUS_USART_RX_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(MODBUS_USART_GPIO_PORT, MODBUS_USART_RX_PIN, LL_GPIO_PULL_NO);
    ModbusPort_SetAlternateFunction(MODBUS_USART_GPIO_PORT, MODBUS_USART_RX_PIN, MODBUS_USART_AF);

    LL_GPIO_SetPinMode(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(MODBUS_RS485_DE_GPIO_PORT, MODBUS_RS485_DE_PIN, LL_GPIO_PULL_NO);
    ModbusPort_SetDriverTransmit(FALSE); /* receiver mode is the safe reset state */

    LL_USART_Disable(MODBUS_USART_INSTANCE);
    LL_USART_SetTransferDirection(MODBUS_USART_INSTANCE, LL_USART_DIRECTION_TX_RX);
    LL_USART_ConfigCharacter(MODBUS_USART_INSTANCE, word_length, parity_mode, stop_mode);
    LL_USART_SetOverSampling(MODBUS_USART_INSTANCE, LL_USART_OVERSAMPLING_16);
    LL_USART_SetHWFlowCtrl(MODBUS_USART_INSTANCE, LL_USART_HWCONTROL_NONE);
    LL_USART_ConfigAsyncMode(MODBUS_USART_INSTANCE);
    LL_USART_SetBaudRate(MODBUS_USART_INSTANCE, MODBUS_USART_PCLK_HZ(),
                         LL_USART_OVERSAMPLING_16, baudrate);

    LL_USART_DisableIT_RXNE(MODBUS_USART_INSTANCE);
    LL_USART_DisableIT_TXE(MODBUS_USART_INSTANCE);
    LL_USART_DisableIT_TC(MODBUS_USART_INSTANCE);
    LL_USART_ClearFlag_PE(MODBUS_USART_INSTANCE);
    LL_USART_ClearFlag_FE(MODBUS_USART_INSTANCE);
    LL_USART_ClearFlag_NE(MODBUS_USART_INSTANCE);
    LL_USART_ClearFlag_ORE(MODBUS_USART_INSTANCE);
    LL_USART_ClearFlag_TC(MODBUS_USART_INSTANCE);
    LL_USART_Enable(MODBUS_USART_INSTANCE);

    modbus_tx_active = FALSE;
    modbus_rx_after_tx = FALSE;
    NVIC_SetPriority(MODBUS_USART_IRQN, 2U);
    NVIC_EnableIRQ(MODBUS_USART_IRQN);
    return TRUE;
}

void vMBPortSerialEnable(BOOL rx_enable, BOOL tx_enable)
{
    if ((rx_enable == FALSE) && (tx_enable == FALSE)) {
        LL_USART_DisableIT_RXNE(MODBUS_USART_INSTANCE);
        LL_USART_DisableIT_TXE(MODBUS_USART_INSTANCE);
        LL_USART_DisableIT_TC(MODBUS_USART_INSTANCE);
        modbus_tx_active = FALSE;
        modbus_rx_after_tx = FALSE;
        ModbusPort_SetDriverTransmit(FALSE);
        return;
    }

    if (tx_enable != FALSE) {
        /* DE must lead the first start bit; TXE is then allowed to feed TDR. */
        modbus_tx_active = TRUE;
        modbus_rx_after_tx = FALSE;
        LL_USART_DisableIT_RXNE(MODBUS_USART_INSTANCE);
        LL_USART_DisableIT_TC(MODBUS_USART_INSTANCE);
        LL_USART_ClearFlag_TC(MODBUS_USART_INSTANCE);
        ModbusPort_SetDriverTransmit(TRUE);
        LL_USART_EnableIT_TXE(MODBUS_USART_INSTANCE);
        return;
    }

    /* FreeModbus has loaded the last byte.  TXE is too early to drop DE:
       wait for TC, otherwise the last stop bit can be truncated. */
    LL_USART_DisableIT_TXE(MODBUS_USART_INSTANCE);
    if (modbus_tx_active != FALSE) {
        modbus_rx_after_tx = rx_enable;
        LL_USART_DisableIT_RXNE(MODBUS_USART_INSTANCE);
        LL_USART_EnableIT_TC(MODBUS_USART_INSTANCE);
    } else {
        ModbusPort_SetDriverTransmit(FALSE);
        if (rx_enable != FALSE) {
            LL_USART_EnableIT_RXNE(MODBUS_USART_INSTANCE);
        } else {
            LL_USART_DisableIT_RXNE(MODBUS_USART_INSTANCE);
        }
    }
}

BOOL xMBPortSerialGetByte(CHAR *byte)
{
    if (byte == 0) {
        return FALSE;
    }
    *byte = (CHAR)LL_USART_ReceiveData8(MODBUS_USART_INSTANCE);
    return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR byte)
{
    LL_USART_TransmitData8(MODBUS_USART_INSTANCE, (uint8_t)byte);
    return TRUE;
}

void MODBUS_USART_IRQ_HANDLER(void)
{
    uint32_t status = MODBUS_USART_INSTANCE->ISR;

    if ((status & USART_ISR_PE) != 0U) {
        LL_USART_ClearFlag_PE(MODBUS_USART_INSTANCE);
    }
    if ((status & USART_ISR_FE) != 0U) {
        LL_USART_ClearFlag_FE(MODBUS_USART_INSTANCE);
    }
    if ((status & USART_ISR_NE) != 0U) {
        LL_USART_ClearFlag_NE(MODBUS_USART_INSTANCE);
    }
    if ((status & USART_ISR_ORE) != 0U) {
        LL_USART_ClearFlag_ORE(MODBUS_USART_INSTANCE);
    }

    if ((LL_USART_IsActiveFlag_RXNE(MODBUS_USART_INSTANCE) != 0U)
        && (LL_USART_IsEnabledIT_RXNE(MODBUS_USART_INSTANCE) != 0U)
        && (pxMBFrameCBByteReceived != 0)) {
        (void)pxMBFrameCBByteReceived();
    }

    if ((LL_USART_IsActiveFlag_TXE(MODBUS_USART_INSTANCE) != 0U)
        && (LL_USART_IsEnabledIT_TXE(MODBUS_USART_INSTANCE) != 0U)
        && (pxMBFrameCBTransmitterEmpty != 0)) {
        (void)pxMBFrameCBTransmitterEmpty();
    }

    if ((LL_USART_IsActiveFlag_TC(MODBUS_USART_INSTANCE) != 0U)
        && (LL_USART_IsEnabledIT_TC(MODBUS_USART_INSTANCE) != 0U)) {
        LL_USART_ClearFlag_TC(MODBUS_USART_INSTANCE);
        LL_USART_DisableIT_TC(MODBUS_USART_INSTANCE);
        modbus_tx_active = FALSE;
        ModbusPort_SetDriverTransmit(FALSE);
        if (modbus_rx_after_tx != FALSE) {
            LL_USART_EnableIT_RXNE(MODBUS_USART_INSTANCE);
        }
    }
}

/* ------------------------- t3.5 timer port ------------------------------- */
BOOL xMBPortTimersInit(USHORT timeout_50us)
{
    uint32_t prescaler;

    modbus_t35_ticks = (timeout_50us == 0U) ? 1U : timeout_50us;
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
    LL_TIM_DisableCounter(TIM7);

    /* APB1 timer clock is 108 MHz in SystemClock_Config(); use 20 kHz. */
    prescaler = (HAL_RCC_GetPCLK1Freq() * 2UL) / MODBUS_TIMER_TICK_HZ;
    if (prescaler == 0U) {
        return FALSE;
    }
    LL_TIM_SetPrescaler(TIM7, prescaler - 1U);
    LL_TIM_SetCounterMode(TIM7, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM7, 0U);
    LL_TIM_SetCounter(TIM7, 0U);
    LL_TIM_GenerateEvent_UPDATE(TIM7);
    LL_TIM_ClearFlag_UPDATE(TIM7);
    LL_TIM_EnableIT_UPDATE(TIM7);

    NVIC_SetPriority(TIM7_IRQn, 3U);
    NVIC_EnableIRQ(TIM7_IRQn);
    return TRUE;
}

void vMBPortTimersEnable(void)
{
    LL_TIM_DisableCounter(TIM7);
    LL_TIM_SetAutoReload(TIM7, (uint32_t)modbus_t35_ticks - 1U);
    LL_TIM_SetCounter(TIM7, 0U);
    LL_TIM_GenerateEvent_UPDATE(TIM7);
    LL_TIM_ClearFlag_UPDATE(TIM7);
    LL_TIM_EnableCounter(TIM7);
}

void vMBPortTimersDisable(void)
{
    LL_TIM_DisableCounter(TIM7);
    LL_TIM_ClearFlag_UPDATE(TIM7);
}

void TIM7_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7) != 0U) {
        LL_TIM_ClearFlag_UPDATE(TIM7);
        LL_TIM_DisableCounter(TIM7);
        if (pxMBPortCBTimerExpired != 0) {
            (void)pxMBPortCBTimerExpired();
        }
    }
}
