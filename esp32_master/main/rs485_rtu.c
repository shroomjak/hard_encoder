/*
 * Реализация прямого детерминированного драйвера Modbus RTU Master (RS-485).
 */
#include "rs485_rtu.h"
#include <string.h>

uint16_t rs485_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFU;
    for (size_t i = 0; i < len; ++i) {
        crc ^= (uint16_t)data[i];
        for (unsigned bit = 0; bit < 8U; ++bit) {
            if (crc & 1U) {
                crc = (uint16_t)((crc >> 1) ^ 0xA001U);
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

size_t rs485_build_fc04(uint8_t id, uint16_t first_reg, uint16_t reg_count, uint8_t out_frame[8])
{
    out_frame[0] = id;
    out_frame[1] = 0x04U;
    out_frame[2] = (uint8_t)(first_reg >> 8);
    out_frame[3] = (uint8_t)(first_reg & 0xFFU);
    out_frame[4] = (uint8_t)(reg_count >> 8);
    out_frame[5] = (uint8_t)(reg_count & 0xFFU);
    const uint16_t crc = rs485_crc16(out_frame, 6U);
    out_frame[6] = (uint8_t)(crc & 0xFFU);
    out_frame[7] = (uint8_t)(crc >> 8);
    return 8U;
}

size_t rs485_build_fc06(uint8_t id, uint16_t reg_addr, uint16_t value, uint8_t out_frame[8])
{
    out_frame[0] = id;
    out_frame[1] = 0x06U;
    out_frame[2] = (uint8_t)(reg_addr >> 8);
    out_frame[3] = (uint8_t)(reg_addr & 0xFFU);
    out_frame[4] = (uint8_t)(value >> 8);
    out_frame[5] = (uint8_t)(value & 0xFFU);
    const uint16_t crc = rs485_crc16(out_frame, 6U);
    out_frame[6] = (uint8_t)(crc & 0xFFU);
    out_frame[7] = (uint8_t)(crc >> 8);
    return 8U;
}

bool rs485_parse_fc04_reply(const uint8_t *buf, size_t len, uint8_t expected_id,
                            uint16_t reg_count, uint16_t *out_words)
{
    const size_t byte_count = (size_t)reg_count * 2U;
    const size_t expected_len = 5U + byte_count;
    if (!buf || !out_words || reg_count == 0U || reg_count > 125U || len != expected_len) {
        return false;
    }
    if (buf[0] != expected_id || buf[1] != 0x04U || buf[2] != (uint8_t)byte_count) {
        return false;
    }
    const uint16_t rx_crc = (uint16_t)buf[expected_len - 2U] |
                            ((uint16_t)buf[expected_len - 1U] << 8);
    if (rs485_crc16(buf, expected_len - 2U) != rx_crc) {
        return false;
    }
    for (uint16_t i = 0; i < reg_count; ++i) {
        const size_t off = 3U + (size_t)i * 2U;
        out_words[i] = ((uint16_t)buf[off] << 8) | (uint16_t)buf[off + 1U];
    }
    return true;
}

bool rs485_parse_fc06_reply(const uint8_t *buf, size_t len, const uint8_t req_frame[8])
{
    if (!buf || !req_frame || len != 8U) {
        return false;
    }
    return memcmp(buf, req_frame, 8U) == 0;
}

#ifndef RS485_RTU_HOST_ONLY
#include "driver/uart.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static uart_port_t s_uart = UART_NUM_2;

esp_err_t rs485_rtu_init(int uart_num, int tx_gpio, int rx_gpio, int rts_gpio, uint32_t baud_rate)
{
    s_uart = (uart_port_t)uart_num;
    const uart_config_t cfg = {
        .baud_rate = (int)baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };

    /* Вызывается строго из задачи encoder_bus на Core 0 -> прерывание UART2 привязывается к Core 0.
     * Флаг ESP_INTR_FLAG_IRAM передаём только если в sdkconfig включён CONFIG_UART_ISR_IN_IRAM,
     * иначе драйвер ESP-IDF вернёт ESP_ERR_INVALID_ARG при старте со старым sdkconfig. */
#if defined(CONFIG_UART_ISR_IN_IRAM) && CONFIG_UART_ISR_IN_IRAM
    const int intr_flags = ESP_INTR_FLAG_IRAM;
#else
    const int intr_flags = 0;
#endif
    esp_err_t err = uart_driver_install(s_uart, 512, 0, 0, NULL, intr_flags);
    if (err != ESP_OK) return err;
    err = uart_param_config(s_uart, &cfg);
    if (err != ESP_OK) return err;
    err = uart_set_pin(s_uart, tx_gpio, rx_gpio, rts_gpio, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) return err;
    err = uart_set_mode(s_uart, UART_MODE_RS485_HALF_DUPLEX);
    if (err != ESP_OK) return err;

    /* Порог RX FIFO = 27 байт (ровно длина ответа FC04 на 11 регистров) и таймаут 2 символа
     * (~95 мкс на 230400 бод вместо стандартных 10 символов = 477 мкс): пробуждает чтение
     * сразу по приходу кадра без паразитной задержки ~380 мкс на каждую головку. */
    (void)uart_set_rx_full_threshold(s_uart, 27);
    (void)uart_set_rx_timeout(s_uart, 2);
    return ESP_OK;
}

void rs485_interframe_gap(uint32_t gap_us)
{
    if (gap_us > 0U) {
        esp_rom_delay_us(gap_us);
    }
}

static esp_err_t rs485_read_exact_or_exc(uint8_t expected_id, uint8_t expected_fc,
                                         uint8_t *buf, size_t expected_len,
                                         size_t *out_len, uint32_t timeout_us)
{
    const uint64_t deadline_us = (uint64_t)esp_timer_get_time() + (uint64_t)timeout_us;
    size_t got = 0U;

    while (got < expected_len) {
        const uint64_t now_us = (uint64_t)esp_timer_get_time();
        if (now_us >= deadline_us) {
            break;
        }
        const uint64_t rem_us = deadline_us - now_us;
        TickType_t wait_ticks = pdMS_TO_TICKS((uint32_t)((rem_us + 999ULL) / 1000ULL));
        if (wait_ticks == 0) {
            wait_ticks = 1;
        }
        const int r = uart_read_bytes(s_uart, buf + got, expected_len - got, wait_ticks);
        if (r > 0) {
            got += (size_t)r;
            /* Короткий кадр исключения Modbus RTU (5 байт: id, fc|0x80, code, crc_lo, crc_hi) */
            if (got >= 5U && buf[0] == expected_id && buf[1] == (uint8_t)(expected_fc | 0x80U)) {
                *out_len = got;
                return ESP_ERR_INVALID_RESPONSE;
            }
        }
    }

    *out_len = got;
    return (got == expected_len) ? ESP_OK : ESP_ERR_TIMEOUT;
}

esp_err_t rs485_send_snap(uint16_t seq, uint64_t *wire_edge_us, uint32_t *snap_total_us)
{
    uint8_t frame[8];
    rs485_build_fc06(0U, 0U, seq, frame);

    (void)uart_flush_input(s_uart);
    const uint64_t start_us = (uint64_t)esp_timer_get_time();

    if (uart_write_bytes(s_uart, frame, sizeof frame) != (int)sizeof frame) {
        return ESP_FAIL;
    }
    esp_err_t err = uart_wait_tx_done(s_uart, pdMS_TO_TICKS(5));
    const uint64_t done_us = (uint64_t)esp_timer_get_time();
    if (wire_edge_us) {
        *wire_edge_us = done_us;
    }
    if (err != ESP_OK) {
        if (snap_total_us) {
            *snap_total_us = (uint32_t)(done_us - start_us);
        }
        return err;
    }

    /* Даём всем головкам STM32 гарантированно завершить t3.5 (200 мкс) и защёлкнуть снимок. */
    esp_rom_delay_us(RS485_SNAP_SETTLE_US);
    if (snap_total_us) {
        *snap_total_us = (uint32_t)((uint64_t)esp_timer_get_time() - start_us);
    }
    return ESP_OK;
}

esp_err_t rs485_read_input_regs(uint8_t id, uint16_t first_reg, uint16_t reg_count,
                                uint16_t *out_words, uint32_t timeout_us)
{
    if (id == 0U || reg_count == 0U || reg_count > 64U || !out_words) {
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t req[8];
    uint8_t rx[5U + 2U * 64U];
    rs485_build_fc04(id, first_reg, reg_count, req);

    (void)uart_flush_input(s_uart);
    if (uart_write_bytes(s_uart, req, sizeof req) != (int)sizeof req) {
        return ESP_FAIL;
    }
    esp_err_t err = uart_wait_tx_done(s_uart, pdMS_TO_TICKS(5));
    if (err != ESP_OK) {
        return err;
    }

    size_t got = 0U;
    const size_t expected_len = 5U + (size_t)reg_count * 2U;
    err = rs485_read_exact_or_exc(id, 0x04U, rx, expected_len, &got, timeout_us);
    if (err != ESP_OK) {
        return err;
    }
    return rs485_parse_fc04_reply(rx, got, id, reg_count, out_words)
           ? ESP_OK
           : ESP_ERR_INVALID_CRC;
}

esp_err_t rs485_write_single_reg(uint8_t id, uint16_t reg_addr, uint16_t value,
                                 uint32_t timeout_us)
{
    uint8_t req[8];
    rs485_build_fc06(id, reg_addr, value, req);

    (void)uart_flush_input(s_uart);
    if (uart_write_bytes(s_uart, req, sizeof req) != (int)sizeof req) {
        return ESP_FAIL;
    }
    esp_err_t err = uart_wait_tx_done(s_uart, pdMS_TO_TICKS(5));
    if (err != ESP_OK) {
        return err;
    }
    if (id == 0U) {
        esp_rom_delay_us(RS485_SNAP_SETTLE_US);
        return ESP_OK;
    }

    uint8_t rx[8];
    size_t got = 0U;
    err = rs485_read_exact_or_exc(id, 0x06U, rx, sizeof rx, &got, timeout_us);
    if (err != ESP_OK) {
        return err;
    }
    return rs485_parse_fc06_reply(rx, got, req) ? ESP_OK : ESP_ERR_INVALID_CRC;
}
#endif /* RS485_RTU_HOST_ONLY */
