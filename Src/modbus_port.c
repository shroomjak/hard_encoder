/*
 * Адаптация стека FreeModbus к аппаратуре STM32F722RE.
 *
 * FreeModbus (ThirdParty/FreeModbus) реализует Modbus RTU: адресацию,
 * кадрирование, CRC16 и функции протокола. Этот файл реализует только
 * требуемые стеком операции: очередь событий, USART1, управление направлением
 * RS485 и однократный таймер тишины TIM2. Таблица регистров находится в
 * modbus_registers.c; расшифровка результатов энкодера — в main.c.
 *
 * Тракт: ESP32 TX -> A/B -> преобразователь RS485 -> USART1 RX -> IRQ
 * -> FreeModbus -> eMBPoll() в основном цикле. Ответ идёт обратным путём.
 * Пример назначения выводов находится в Inc/modbus_board.h: его обязательно
 * сверить с разводкой реальной платы ДО прошивки. Контакты отладчика
 * SWD/JTAG этим портом не используются.
 */
#include "main.h"
#include "modbus_board.h"
#include "mb.h"
#include "mbport.h"

/* Символ, считанный из RDR обработчиком USART1; FreeModbus сразу забирает его
 * через xMBPortSerialGetByte(), поэтому здесь достаточно одного байта. */
static volatile CHAR rx_byte;
/* TXE сообщает лишь об опустевшем регистре передачи. Пока tx_finishing=1,
 * последний байт ещё может передаваться; DE сбрасывается только по TC. */
static volatile BOOL tx_finishing;

/* События рождаются в прерываниях UART/TIM2, а разбираются eMBPoll() из main.
 * Кольцевая очередь содержит максимум EVENT_CAPACITY-1 событий; отдельные
 * EV_FRAME_RECEIVED и EV_EXECUTE нельзя затирать друг другом. При переполнении
 * Post возвращает FALSE (стек теряет событие): контролировать это при отладке
 * высоких нагрузок/длительных блокировок основного цикла. */
#define EVENT_CAPACITY 8
static volatile eMBEventType events[EVENT_CAPACITY];
static volatile uint8_t event_head, event_tail;

/* Сброс очереди при eMBInit(); успешно даже при пустой очереди. */
BOOL xMBPortEventInit(void)
{
    event_head = event_tail = 0;
    return TRUE;
}

/* Поместить событие без ожидания. PRIMASK сохраняется и восстанавливается:
 * этот же код вызывается как из главного цикла, так и из обработчика IRQ. */
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

/* Забрать одно событие для eMBPoll(); FALSE означает отсутствие работы. */
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

/* DE=0: выход драйвера отключён. /RE=0: приёмник включён. Только адресуемая
 * головка вправе активировать DE, иначе на общей шине возникнет коллизия. */
static void transceiver_rx(void)
{
    LL_GPIO_ResetOutputPin(MB_DE_PORT, MB_DE_PIN);
    LL_GPIO_ResetOutputPin(MB_NRE_PORT, MB_NRE_PIN);
}

/* DE=1: передатчик RS485 включён. /RE=1: приёмник выключен, чтобы не
 * принимать собственный ответ как новый входящий запрос. */
static void transceiver_tx(void)
{
    LL_GPIO_SetOutputPin(MB_NRE_PORT, MB_NRE_PIN);
    LL_GPIO_SetOutputPin(MB_DE_PORT, MB_DE_PIN);
}

/*
 * Инициализация аппаратного последовательного порта для FreeModbus.
 * Параметры передаются из eMBInit() в main.c; при несовпадении с режимом
 * 115200/8E1 или выборе другого USART возвращаем FALSE, а не запускаем
 * связь с незаметно отличающимися настройками. На STM32 бит чётности
 * входит в длину слова: 9B + EVEN == 8 бит данных + бит чётности.
 * Никакого DMA UART здесь нет: RXNE/TXE обрабатываются прерываниями.
 */
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
    /* Ниже по приоритету, чем DMA2_Stream3 (0) и TIM4 (1) из main.c. */
    NVIC_SetPriority(USART1_IRQn, 2);
    NVIC_EnableIRQ(USART1_IRQn);
    return TRUE;
}

/* Переключение режима по запросу автомата RTU.
 * Если TX=TRUE, включаем DE и разрешаем TXE — стек начнёт отдавать байты.
 * Если стек отключил TX, последняя посылка ещё может находиться в сдвиговом
 * регистре USART. До TC сохраняем DE=1 и RXNE выключенным. Только в IRQ
 * по TC освобождаем RS485 и возвращаемся к приёму. */
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

/* Синхронные операции одного байта для обработчиков конечного автомата RTU. */
BOOL xMBPortSerialGetByte(CHAR *byte) { *byte = rx_byte; return TRUE; }
BOOL xMBPortSerialPutByte(CHAR byte)
{
    LL_USART_TransmitData8(USART1, (uint8_t)byte);
    return TRUE;
}

/* Таймер межкадровой паузы RTU: аргумент FreeModbus в единицах 50 мкс.
 * При скорости >19200 бод библиотека запрашивает 35 * 50 = 1750 мкс.
 * В данной конфигурации APB1 делится на 4, поэтому такт TIM2 равен
 * 2 * PCLK1; делим его до 1 МГц и отсчитываем timeout_50us * 50 мкс.
 * TIM2 не занят измерениями энкодера (TIM3/TIM4 применяются в main.c). */
BOOL xMBPortTimersInit(USHORT timeout_50us)
{
    uint32_t tim_clk = HAL_RCC_GetPCLK1Freq() * 2U;
    if (!timeout_50us || tim_clk % 1000000U) return FALSE;
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetPrescaler(TIM2, tim_clk / 1000000U - 1U);
    LL_TIM_SetAutoReload(TIM2, timeout_50us * 50U - 1U);
    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2); /* загрузить PSC до первого кадра */
    LL_TIM_ClearFlag_UPDATE(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
    return TRUE;
}

/* Вызов при получении очередного байта сбрасывает отсчёт паузы. */
void vMBPortTimersEnable(void)
{
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);
}
void vMBPortTimersDisable(void) { LL_TIM_DisableCounter(TIM2); LL_TIM_ClearFlag_UPDATE(TIM2); }

/*
 * Обработчик USART1: RXNE передаёт байт в конечный автомат FreeModbus,
 * TXE запрашивает следующий байт ответа. После отправки последнего байта
 * стек отключает TXE; по отдельному флагу TC отпускаем DE на общей шине.
 * Аппаратные флаги ORE/FE/NE/PE очищаются, но отдельной проверки PE при
 * приёме здесь нет: правильность данных проверяет CRC кадра FreeModbus.
 * При диагностике помех эти ошибки стоит дополнительно считать/учитывать.
 */
void USART1_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
    if (LL_USART_IsActiveFlag_FE(USART1)) LL_USART_ClearFlag_FE(USART1);
    if (LL_USART_IsActiveFlag_NE(USART1)) LL_USART_ClearFlag_NE(USART1);
    if (LL_USART_IsActiveFlag_PE(USART1)) LL_USART_ClearFlag_PE(USART1);
    if (LL_USART_IsEnabledIT_RXNE(USART1) && LL_USART_IsActiveFlag_RXNE(USART1)) {
        rx_byte = (CHAR)LL_USART_ReceiveData8(USART1);
        pxMBFrameCBByteReceived();
    }
    if (LL_USART_IsEnabledIT_TXE(USART1) && LL_USART_IsActiveFlag_TXE(USART1))
        pxMBFrameCBTransmitterEmpty();
    if (tx_finishing && LL_USART_IsActiveFlag_TC(USART1)) {
        LL_USART_DisableIT_TC(USART1);
        LL_USART_ClearFlag_TC(USART1);
        tx_finishing = FALSE;
        /* Не передавать в Modbus возможное локальное эхо собственной посылки. */
        if (LL_USART_IsActiveFlag_RXNE(USART1))
            (void)LL_USART_ReceiveData8(USART1);
        transceiver_rx();
        LL_USART_EnableIT_RXNE(USART1);
    }
}

/* По истечении тишины t3.5 FreeModbus считает кадр законченным и помещает
 * событие в очередь. Разбор запроса и создание ответа происходят позднее,
 * вне прерывания, при очередном eMBPoll() из Src/main.c. */
void TIM2_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
        LL_TIM_DisableCounter(TIM2);
        pxMBPortCBTimerExpired();
    }
}
