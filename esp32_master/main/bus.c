/*
 * Общая шина Modbus RTU для ESP32 master: создание стека esp-modbus и UART2.
 * Конфигурация пинов и параметров линии — та же, что была в main.c до разделения
 * режимов. Пример разводки для классической ESP32, сверить со схемой платы.
 */
#include <stdint.h>
#include "esp_modbus_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bus.h"

/* ПРИМЕР разводки для классической ESP32, а не проверенная схема изделия.
 * UART TX -> DI, RX <- RO, RTS -> DE и /RE преобразователя RS485;
 * уровни сигнала RTS и возможность совместного управления DE и /RE нужно
 * сверить с установленным преобразователем. USB платы используется для
 * прошивки/лога, UART2 — исключительно для обмена с головками. */
#define MB_UART UART_NUM_2
#define MB_TX_GPIO 17
#define MB_RX_GPIO 16
#define MB_RTS_GPIO 4
#define MB_BAUD 115200

static void *master;

/* В версии 2.1.4 библиотека требует хотя бы один дескриптор перед start(),
 * даже если все запросы отправляются напрямую через send_request(). Этот
 * элемент позволяет стартовать контроллер; адрес slave в каждом фактическом
 * запросе задаётся отдельно. Полученные слова обрабатываются вручную. */
static const mb_parameter_descriptor_t descriptor[] = {
    {0, "snapshot", "", 1, MB_PARAM_INPUT, 0, REG_COUNT, 0,
     PARAM_TYPE_U16, 2, {{0}}, PAR_PERMS_READ}
};

esp_err_t bus_init(void)
{
    mb_communication_info_t comm = {
        .ser_opts.port = MB_UART,
        .ser_opts.mode = MB_RTU,
        .ser_opts.baudrate = MB_BAUD,
        .ser_opts.parity = UART_PARITY_EVEN,
        .ser_opts.uid = 0,
        .ser_opts.response_tout_ms = CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND,
        .ser_opts.data_bits = UART_DATA_8_BITS,
        .ser_opts.stop_bits = UART_STOP_BITS_1
    };
    ESP_ERROR_CHECK(mbc_master_create_serial(&comm, &master));
    ESP_ERROR_CHECK(uart_set_pin(MB_UART, MB_TX_GPIO, MB_RX_GPIO, MB_RTS_GPIO,
                                 UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(MB_UART, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(mbc_master_set_descriptor(master, descriptor, 1));
    ESP_ERROR_CHECK(mbc_master_start(master));
    return ESP_OK;
}

esp_err_t bus_request(uint8_t id, uint8_t function, uint16_t first,
                      uint16_t count, uint16_t *words)
{
    mb_param_request_t req = {
        .slave_addr = id, .command = function,
        .reg_start = first, .reg_size = count
    };
    esp_err_t err = mbc_master_send_request(master, &req, words);

    /* Запас по времени на шине */
    vTaskDelay(pdMS_TO_TICKS(20));

    return err;
}
