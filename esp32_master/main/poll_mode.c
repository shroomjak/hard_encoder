/*
 * Рабочий режим ESP32 master: цикл опроса головок.
 *   1. SNAP(seq)    — однократная FC06 на адрес 0: все slave фиксируют
 *                     последний готовый результат; ответа НЕ будет.
 *   2. STATUS(1/2) — адресные FC04 (seq, ready); проверяем ACK обеих.
 *   3. READ(1/2)   — адресные FC04 для всех полей, только если ВСЕ ACK.
 *
 * Команды калибровки (CAL_CMD, CAL_PAGE) этот режим не посылает и в сборку
 * не включается, если выбран режим калибровки.
 */
#include <stdint.h>
#include <stdbool.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bus.h"
#include "poll_mode.h"

/* У каждого slave в прошивке STM32 свой адрес MB_SLAVE_ADDRESS.
 * Добавляя головки, изменить массив и HEAD_COUNT. */
#define STATUS_COUNT 2     /* только seq и ready */
#define HEAD_COUNT 2
static const uint8_t head_ids[HEAD_COUNT] = {1, 2};
static const char *TAG = "encoder";

/* Диагностика головки, которая получила SNAP, но вернула ready=0.
 * Читаем все поля зафиксированного кадра, чтобы увидеть причину:
 *   sample=0          — ещё не обработан ни один кадр АЦП;
 *   error!=0          — декодирование кадра не удалось;
 *   sector>=144       — недопустимый сектор;
 *   иначе             — угол вне [0,360) или NaN.
 * Угол при ready=0 не выводим: при невалидном кадре он не обновляется. */
static void log_not_ready(uint8_t id, uint16_t seq)
{
    uint16_t r[REG_COUNT] = {0};
    esp_err_t err = bus_request(id, 0x04, 0, REG_COUNT, r);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DIAG id=%u seq=%u: read failed: %s",
                 (unsigned)id, (unsigned)seq, esp_err_to_name(err));
        return;
    }
    if (r[0] != seq || r[1] != 0) {
        ESP_LOGW(TAG, "DIAG id=%u: snapshot changed (expected=%u got=%u ready=%u)",
                 (unsigned)id, (unsigned)seq, (unsigned)r[0], (unsigned)r[1]);
        return;
    }
    uint32_t sample = ((uint32_t)r[7] << 16) | r[8];
    uint32_t time_ms = ((uint32_t)r[9] << 16) | r[10];
    const char *reason = sample == 0 ? "no frame processed yet"
                       : r[5] != 0   ? "decode error (errorflag)"
                       : r[4] >= 144 ? "invalid sector"
                                     : "angle outside [0,360) or NaN";
    ESP_LOGW(TAG, "DIAG id=%u seq=%u sector=%u error=%u encoder_state=0x%02x "
             "sample=%lu tick_ms=%lu -> %s",
             (unsigned)id, (unsigned)seq, (unsigned)r[4], (unsigned)r[5],
             (unsigned)r[6], (unsigned long)sample, (unsigned long)time_ms, reason);
}

/* STATUS адресной головки: FC04 читает регистры 0 и 1. ACK означает
 * одновременно совпадение seq и ready=1; один только успешный ответ Modbus
 * НЕ подтверждает, что эта головка сохранила нужный кадр. При тайм-ауте,
 * старом seq или невалидном кадре возвращаем false, пояснение пишем в лог. */
static bool status_ack(uint8_t id, uint16_t seq)
{
    uint16_t status[STATUS_COUNT] = {0};
    esp_err_t err = bus_request(id, 0x04, 0, STATUS_COUNT, status);
    if (err != ESP_OK || status[0] != seq || status[1] != 1) {
        ESP_LOGW(TAG, "STATUS id=%u seq=%u error=%s got=%u ready=%u",
                 (unsigned)id, (unsigned)seq, esp_err_to_name(err),
                 (unsigned)status[0], (unsigned)status[1]);
        if (err == ESP_OK && status[0] == seq && status[1] != 1)
            log_not_ready(id, seq);
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
void poll_cycle(uint16_t seq)
{
    uint16_t command = seq;
    esp_err_t err = bus_request(0, 0x06, 0, 1, &command);
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
        err = bus_request(head_ids[i], 0x04, 0, REG_COUNT, r);
        if (err != ESP_OK || r[0] != seq || r[1] != 1 || r[4] >= 144) {
            ESP_LOGE(TAG, "READ id=%u seq=%u failed: %s (got=%u ready=%u)",
                     (unsigned)head_ids[i], (unsigned)seq, esp_err_to_name(err),
                     (unsigned)r[0], (unsigned)r[1]);
            continue;
        }
        /* Пары 16-битных регистров передаются старшим словом первым.
         * Угол — uint32 микроградусов, sample и time_ms — uint32.
         * tick_ms каждой головки считается от ЕЁ перезагрузки; он не
         * синхронизирован со временем другой головки или ESP32. */
        uint32_t udeg = ((uint32_t)r[2] << 16) | r[3];
        uint32_t sample = ((uint32_t)r[7] << 16) | r[8];
        uint32_t time_ms = ((uint32_t)r[9] << 16) | r[10];
        ESP_LOGI(TAG, "DATA id=%u seq=%u angle=%u.%06u deg sector=%u "
                 "error=%u encoder_state=0x%02x sample=%lu tick_ms=%lu",
                 (unsigned)head_ids[i], (unsigned)seq, (unsigned)(udeg / 1000000U),
                 (unsigned)(udeg % 1000000U), (unsigned)r[4], (unsigned)r[5],
                 (unsigned)r[6], (unsigned long)sample, (unsigned long)time_ms);
    }
}

/* seq демонстрационно начинается с 42; для защиты от старого ACK после
 * перезапуска master использовать сохранённый в NVS неповторяющийся номер.
 * Задержка FreeRTOS задаёт интервал между ПОЛНЫМИ циклами опроса. */
void poll_mode_run(void)
{
    uint16_t seq = 42;
    while (true) {
        poll_cycle(seq++); /* uint16_t переполняется после 65535 */
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
