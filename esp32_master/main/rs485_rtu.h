/*
 * Прямой детерминированный драйвер Modbus RTU Master поверх RS-485 (UART2, 230400 8E1).
 *
 * Зачем вместо esp-modbus:
 *  - Стек esp-modbus создаёт 4 фоновые задачи FreeRTOS, мьютексы, программные
 *    таймеры ESP_TIMER_TASK и принудительный 150 мс cooldown при любом таймауте,
 *    а также сбрасывает RX-буфер между вызовом send_request и реальной передачей,
 *    внося нестабильную задержку перед выходом байтов SNAP на физическую линию.
 *  - Данный модуль работает напрямую с аппаратным контроллером UART2 ESP32 в режиме
 *    UART_MODE_RS485_HALF_DUPLEX (аппаратное управление DE/RE по RTS без участия ПО),
 *    фиксирует метку времени t_us строго по окончании стоп-бита кадра SNAP на линии
 *    (uart_wait_tx_done) и выдерживает детерминированные паузы t3.5 (300/250 мкс).
 */
#ifndef RS485_RTU_H
#define RS485_RTU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef RS485_RTU_HOST_ONLY
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_TIMEOUT 0x107
#define ESP_ERR_INVALID_RESPONSE 0x108
#define ESP_ERR_INVALID_CRC 0x109
#else
#include "esp_err.h"
#endif

#define RS485_BAUD_RATE          230400U
/* Выдержка после broadcast SNAP: STM32 t3.5 = 200 мкс + ~1.5 мкс защёлка в TIM2 ISR. */
#define RS485_SNAP_SETTLE_US     300U
/* Межкадровая пауза между ответом одной головки и запросом к следующей (> 200 мкс). */
#define RS485_INTERFRAME_GAP_US  250U
/* Таймаут ответа головки в рабочем цикле poll (~250 мкс в норме -> 3.5 мс с запасом >10x). */
#define RS485_POLL_TIMEOUT_US    3500U
/* Таймаут ответа при сканировании / калибровке. */
#define RS485_CMD_TIMEOUT_US     8000U

/* Чистые функции формирования/разбора RTU-кадров (доступны в host-тестах без ESP-IDF). */
uint16_t rs485_crc16(const uint8_t *data, size_t len);
size_t rs485_build_fc04(uint8_t id, uint16_t first_reg, uint16_t reg_count, uint8_t out_frame[8]);
size_t rs485_build_fc06(uint8_t id, uint16_t reg_addr, uint16_t value, uint8_t out_frame[8]);
bool rs485_parse_fc04_reply(const uint8_t *buf, size_t len, uint8_t expected_id,
                            uint16_t reg_count, uint16_t *out_words);
bool rs485_parse_fc06_reply(const uint8_t *buf, size_t len, const uint8_t req_frame[8]);

#ifndef RS485_RTU_HOST_ONLY
/* Инициализировать UART2 (RS-485 Half-Duplex, 230400 8E1) с привязкой IRQ к текущему ядру (Core0). */
esp_err_t rs485_rtu_init(int uart_num, int tx_gpio, int rx_gpio, int rts_gpio, uint32_t baud_rate);

/* Отправить широковещательный SNAP (FC06 addr=0 reg=0 val=seq), дождаться физического
 * выхода стоп-бита на линию RS-485, зафиксировать *wire_edge_us и выдержать паузу
 * RS485_SNAP_SETTLE_US, за которую все STM32 защёлкнут снимок в TIM2_IRQHandler. */
esp_err_t rs485_send_snap(uint16_t seq, uint64_t *wire_edge_us, uint32_t *snap_total_us);

/* Прочитать reg_count входных регистров (FC04) у головки id. */
esp_err_t rs485_read_input_regs(uint8_t id, uint16_t first_reg, uint16_t reg_count,
                                uint16_t *out_words, uint32_t timeout_us);

/* Записать один holding-регистр (FC06); для id == 0 (broadcast) ответ не читается. */
esp_err_t rs485_write_single_reg(uint8_t id, uint16_t reg_addr, uint16_t value,
                                 uint32_t timeout_us);

/* Точная межкадровая пауза шины RS-485 в микросекундах. */
void rs485_interframe_gap(uint32_t gap_us);
#endif

#endif /* RS485_RTU_H */
