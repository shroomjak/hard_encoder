#!/usr/bin/env python3
"""Регрессионный тест рабочего poll ESP32 на компьютере (UTF-8).

Запуск из корня: python3 tests/master_poll_test.py
Требуются Python 3 и компилятор cc (C99) в PATH; ESP-IDF и платы не нужны.

Из main/main.c извлекаются реальные log_not_ready(), status_ack(), poll_heads().
Подменяются только интерфейс request и журнал ESP_LOG; копии алгоритма нет.
Проверяются произвольные ID 247/3, один SNAP на цикл, STATUS всех головок перед
рабочими READ, повтор STATUS без нового SNAP, полный отказ от рабочих READ
при неполном ACK, устаревший seq, невалидный сектор и ошибка broadcast.
При ready=0 диагностическое чтение разрешено, но DATA публиковаться не должно.

Зачем: сохранить согласованность измерений нескольких головок при изменении
списка ID или логики опроса. Это не тест поиска/calib (см. workflow_test.c),
не сборка ESP-IDF и не проверка настоящего драйвера UART/esp-modbus.
Временные исходник и исполняемый файл автоматически удаляются. Успех — код 0
и строка ESP32 poll: ... OK; ошибка компиляции/assert даёт ненулевой код.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = (ROOT / "esp32_master/main/main.c").read_text(encoding="utf-8")
POLL = SOURCE[SOURCE.index("static void log_not_ready("):SOURCE.index("static QueueHandle_t commands;")]
PREAMBLE = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#define REG_COUNT 11
#define STATUS_COUNT 2
#define ESP_OK 0
#define ESP_FAIL -1
typedef int esp_err_t;
static const char *TAG = "test";
static unsigned snap_count, statuses[248], reads[248], data_count;
static unsigned mode;
static uint16_t expected_seq;
static const char *esp_err_to_name(int error) { return error ? "fail" : "ok"; }
static void log_stub(const char *tag, const char *format, ...)
{
    (void)tag;
    if (!strncmp(format, "DATA ", 5)) ++data_count;
}
#define ESP_LOGI log_stub
#define ESP_LOGW log_stub
#define ESP_LOGE log_stub
static esp_err_t request(uint8_t id, uint8_t fn, uint16_t first, uint16_t count, uint16_t *r)
{
    assert(first == 0);
    if (!id) {
        assert(fn == 6 && count == 1 && *r == expected_seq);
        ++snap_count;
        return mode == 5 ? ESP_FAIL : ESP_OK;
    }
    assert(fn == 4 && (id == 247 || id == 3));
    memset(r, 0, count * sizeof *r);
    r[0] = expected_seq; r[1] = 1;
    if (count == 2) {
        ++statuses[id];
        if (id == 247 && (mode == 1 || (mode == 2 && statuses[id] == 1))) return ESP_FAIL;
        if (id == 247 && mode == 3) r[0]--; /* Stale ACK. */
        if (id == 247 && mode == 6) r[1] = 0; /* Invalid snapshot (diagnostic allowed). */
    } else {
        assert(count == 11);
        ++reads[id];
        if (mode != 6) assert(statuses[247] && statuses[3]); /* All statuses precede READ. */
        r[4] = 10;
        if (id == 247 && mode == 4) r[0]--; /* Snapshot changed between ACK and READ. */
        if (id == 247 && mode == 7) r[4] = 144;
    }
    return ESP_OK;
}
'''
HARNESS = r'''
int main(void)
{
    const uint8_t ids[] = {247, 3};
    for (mode = 0; mode < 8; ++mode) {
        memset(statuses, 0, sizeof statuses);
        memset(reads, 0, sizeof reads);
        snap_count = data_count = 0; expected_seq = 50 + mode;
        poll_heads(ids, 2, expected_seq);
        assert(snap_count == 1);
        if (mode == 5) { assert(!statuses[247] && !statuses[3]); continue; }
        assert(statuses[3] == 1);
        if (mode == 1 || mode == 3) {
            assert(statuses[247] == 2 && !reads[247] && !reads[3] && !data_count);
        } else if (mode == 6) {
            assert(statuses[247] == 2 && reads[247] == 2 && !reads[3] && !data_count);
        } else {
            assert(reads[247] == 1 && reads[3] == 1);
            assert(data_count == ((mode == 4 || mode == 7) ? 1 : 2));
            assert(statuses[247] == (mode == 2 ? 2 : 1));
        }
    }
    return 0;
}
'''
if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="encoder_poll_") as tmp:
        source = Path(tmp) / "poll_test.c"
        binary = Path(tmp) / "poll_test"
        source.write_text(PREAMBLE + POLL + HARNESS, encoding="utf-8")
        subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                        str(source), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
    print("ESP32 poll: dynamic IDs, all-ACK barrier, retries, stale snapshots OK")
