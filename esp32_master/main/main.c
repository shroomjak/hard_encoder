/*
 * ESP32 — ведущий узел (master) многоточечной шины Modbus RTU / RS485.
 * Библиотека: официальный компонент Espressif esp-modbus v2.1.4.
 * Прошивка собирается и загружается ESP-IDF, НЕ IAR: подробная инструкция
 * и схема соединений приведены в docs/modbus_rtu.md.
 *
 * Цикл для двух головок:
 *   1. SNAP(seq)    — однократная FC06 на адрес 0: все slave фиксируют
 *                     последний готовый результат; ответа НЕ будет.
 *   2. STATUS(1/2) — адресные FC04 (seq, ready); проверяем ACK обеих.
 *   3. READ(1/2)   — адресные FC04 для всех полей, только если ВСЕ ACK.
 *
 * UART2 обслуживает RS485; диагностические ESP_LOGx() видны на USB-консоли
 * отладочной платы, а не на проводах A/B. ESP-IDF реализует Modbus RTU,
 * CRC, паузы и тайм-ауты; здесь задаётся только прикладной сценарий.
 */
#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_modbus_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

/* У каждого slave в прошивке STM32 свой адрес MB_SLAVE_ADDRESS.
 * Добавляя головки, изменить и массив, и HEAD_COUNT. */
#define REG_COUNT 11       /* полная карта входных регистров 0..10 */
#define STATUS_COUNT 2     /* только seq и ready */
#define HEAD_COUNT 2
static const uint8_t head_ids[HEAD_COUNT] = {1, 2};
static const char *TAG = "encoder";
static void *master;

/* В версии 2.1.4 библиотека требует хотя бы один дескриптор перед start(),
 * даже если все запросы отправляются напрямую через send_request(). Этот
 * элемент позволяет стартовать контроллер; адрес slave в каждом фактическом
 * запросе задаётся отдельно. Полученные слова обрабатываются вручную. */
static const mb_parameter_descriptor_t descriptor[] = {
    {0, "snapshot", "", 1, MB_PARAM_INPUT, 0, REG_COUNT, 0,
     PARAM_TYPE_U16, 2, {0}, PAR_PERMS_READ}
};

/* Общая оболочка стандартного запроса Modbus. first — адрес регистра на
 * проводе, отсчитываемый от 0; count — число 16-битных регистров.
 * Для FC06 *words = записываемое значение, для FC04 буфер заполняется
 * библиотекой в порядке uint16_t машины ESP32. Фрейм/CRC здесь не строим. */
static esp_err_t request(uint8_t id, uint8_t function, uint16_t first,
                         uint16_t count, uint16_t *words)
{
    mb_param_request_t req = {
        .slave_addr = id, .command = function,
        .reg_start = first, .reg_size = count
    };
    return mbc_master_send_request(master, &req, words);
}

/* STATUS адресной головки: FC04 читает регистры 0 и 1. ACK означает
 * одновременно совпадение seq и ready=1; один только успешный ответ Modbus
 * НЕ подтверждает, что эта головка сохранила нужный кадр. При тайм-ауте,
 * старом seq или невалидном кадре возвращаем false, пояснение пишем в лог. */
static bool status_ack(uint8_t id, uint16_t seq)
{
    uint16_t status[STATUS_COUNT] = {0};
    esp_err_t err = request(id, 0x04, 0, STATUS_COUNT, status);
    if (err != ESP_OK || status[0] != seq || status[1] != 1) {
        ESP_LOGW(TAG, "STATUS id=%u seq=%u error=%s got=%u ready=%u",
                 (unsigned)id, (unsigned)seq, esp_err_to_name(err),
                 (unsigned)status[0], (unsigned)status[1]);
        return false;
    }
    ESP_LOGI(TAG, "ACK id=%u seq=%u", (unsigned)id, (unsigned)seq);
    return true;
}

/* Полный цикл опроса одного номера кадра.
 * Адрес 0 Modbus зарезервирован для вещания: никто не отвечает на SNAP.
 * Espressif esp-modbus после него выдерживает заданную в sdkconfig.defaults
 * задержку обработки (200 мс), прежде чем принимать следующий запрос.
 * Даже при ошибке одной головки спрашиваем STATUS оставшихся для диагностики,
 * но НЕ выдаём READ ни одной из них до полного набора подтверждений. */
static void poll_heads(uint16_t seq)
{
    uint16_t command = seq;
    esp_err_t err = request(0, 0x06, 0, 1, &command);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SNAP seq=%u: broadcast failed: %s", (unsigned)seq, esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "SNAP seq=%u", (unsigned)seq);
    bool all_ack = true;
    for (unsigned i = 0; i < HEAD_COUNT; ++i) {
        /* При сбое STATUS одна повторная попытка; повтор не делает SNAP. */
        if (!status_ack(head_ids[i], seq) && !status_ack(head_ids[i], seq))
            all_ack = false;
    }
    if (!all_ack) {
        ESP_LOGW(TAG, "seq=%u: not all ACK; skip all READ", (unsigned)seq);
        return;
    }

    /* READ обращается только к уже зафиксированному frozen-кадру.
     * Повторно проверяем seq/ready, поскольку между STATUS и READ могли
     * произойти ошибка обмена или изменение состояния у slave. */
    for (unsigned i = 0; i < HEAD_COUNT; ++i) {
        uint16_t r[REG_COUNT] = {0};
        err = request(head_ids[i], 0x04, 0, REG_COUNT, r);
        if (err != ESP_OK || r[0] != seq || r[1] != 1 || r[4] >= 144) {
            ESP_LOGE(TAG, "READ id=%u seq=%u failed: %s (got=%u ready=%u)",
                     (unsigned)head_ids[i], (unsigned)seq, esp_err_to_name(err),
                     (unsigned)r[0], (unsigned)r[1]);
            continue;
        }
        /* Пары 16-битных регистров передаются старшим словом первым.
         * Угол — uint32 миллиградусов, sample и time_ms — uint32.
         * tick_ms каждой головки считается от ЕЁ перезагрузки; он не
         * синхронизирован со временем другой головки или ESP32. */
        uint32_t mdeg = ((uint32_t)r[2] << 16) | r[3];
        uint32_t sample = ((uint32_t)r[7] << 16) | r[8];
        uint32_t time_ms = ((uint32_t)r[9] << 16) | r[10];
        ESP_LOGI(TAG, "DATA id=%u seq=%u angle=%u.%03u deg sector=%u "
                 "error=%u encoder_state=0x%02x sample=%lu tick_ms=%lu",
                 (unsigned)head_ids[i], (unsigned)seq, (unsigned)(mdeg / 1000),
                 (unsigned)(mdeg % 1000), (unsigned)r[4], (unsigned)r[5],
                 (unsigned)r[6], (unsigned long)sample, (unsigned long)time_ms);
    }
}

/* Точка входа ESP-IDF (аналог main() в обычном C-проекте).
 * UART_DATA_8_BITS + UART_PARITY_EVEN + один стоп-бит = 8E1, как на STM32.
 * RTS автоматически переключает направление в режиме RS485_HALF_DUPLEX.
 * Сначала создаём стек и настраиваем UART, затем задаём требуемый
 * дескриптор и запускаем master. Далее только master инициирует обмен;
 * задержка FreeRTOS задаёт интервал между ПОЛНЫМИ циклами опроса.
 * seq демонстрационно начинается с 42; для защиты от старого ACK после
 * перезапуска master использовать сохранённый в NVS неповторяющийся номер. */
void app_main(void)
{
    mb_communication_info_t comm = {
        .ser_opts.port = MB_UART,
        .ser_opts.mode = MB_RTU,
        .ser_opts.baudrate = MB_BAUD,
        .ser_opts.parity = UART_PARITY_EVEN,
        .ser_opts.uid = 0,
        .ser_opts.response_tout_ms = 400,
        .ser_opts.data_bits = UART_DATA_8_BITS,
        .ser_opts.stop_bits = UART_STOP_BITS_1
    };
    ESP_ERROR_CHECK(mbc_master_create_serial(&comm, &master));
    ESP_ERROR_CHECK(uart_set_pin(MB_UART, MB_TX_GPIO, MB_RX_GPIO, MB_RTS_GPIO,
                                 UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(MB_UART, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(mbc_master_set_descriptor(master, descriptor, 1));
    ESP_ERROR_CHECK(mbc_master_start(master));

    uint16_t seq = 42;
    while (true) {
        poll_heads(seq++); /* uint16_t переполняется после 65535 */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
