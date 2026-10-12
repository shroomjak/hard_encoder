/*
 * Хостовой тест режимов ESP32 master без ESP-IDF.
 *
 * bus_request() подменён: здесь живёт модель головки (регистры статуса,
 * переходы STATE после CAL_CMD, бит EEPROM, окно выгрузки ang_tab). Тест
 * записывает все запросы и проверяет:
 *   • TEST_CALIB: калибровка шлёт команды в заданном порядке, на заданный адрес,
 *     выгружает ang_tab нужное число раз и не шлёт SNAP;
 *   • TEST_POLL (по умолчанию): рабочий цикл не пишет CAL_CMD и CAL_PAGE.
 * Ожидаемые значения приходят из -D (см. run_esp32_modes_test.sh).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "bus.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#ifdef TEST_CALIB
#include "calib_mode.h"
#else
#include "poll_mode.h"
#endif

/* ----------------------------- заглушки -------------------------------- */

static TickType_t fake_now;
void vTaskDelay(TickType_t ticks) { fake_now += ticks; }
TickType_t xTaskGetTickCount(void) { return fake_now; }
const char *esp_err_to_name(esp_err_t err) { return err == ESP_OK ? "OK" : "ERR"; }

/* ----------------------------- журнал запросов ------------------------- */

typedef struct {
    uint8_t id, fn;
    uint16_t reg, count, value;
} call_t;

#define MAX_CALLS 8192
static call_t calls[MAX_CALLS];
static size_t ncalls;
static int failures;

#define CHECK(cond, ...) do { if (!(cond)) { failures++; \
    fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); } } while (0)

/* ----------------------------- модель головки -------------------------- */

#define ANGTAB_N 144
static float angtab[ANGTAB_N];
static uint16_t snap_seq;
static uint16_t head_state;      /* encoder_state */
static uint16_t eeprom = 0x0F;   /* бит 0 — запись завершена, 1…3 — таблицы приняты */
static uint16_t page_word;       /* последний CAL_PAGE */
static int busy_reads;           /* сколько чтений STATE до конца команды */
static uint16_t pending_done;    /* STATE, который ставится по окончании */

static uint16_t after_write_state(uint16_t cmd)
{
    switch (cmd) {
    case 0x11: return 0x10;
    case 0x12: return 0x30;
    case 0x13: return 0x50;
    case 0x14: return 0x70;
    default:   return 0x00;
    }
}

static uint16_t done_state(uint16_t cmd)
{
    switch (cmd) {
    case 0x11: return 0x20;
    case 0x12: return 0x40;
    case 0x13: return 0x60;
    case 0x14: return 0x80;
    default:   return 0x00;
    }
}

static void head_write_cmd(uint16_t cmd)
{
    if (cmd == 0x10) {               /* стоп: сразу штатный режим */
        head_state = 0;
        eeprom |= 1;
        busy_reads = 0;
        return;
    }
    head_state = after_write_state(cmd);
    pending_done = done_state(cmd);
    eeprom &= (uint16_t)~1u;         /* запись EEPROM начинается */
    busy_reads = 3;                  /* три опроса STATE до готовности */
}

static void put_u32(uint16_t *w, uint32_t v)
{
    w[0] = (uint16_t)(v >> 16);
    w[1] = (uint16_t)(v & 0xFFFFu);
}

esp_err_t bus_request(uint8_t id, uint8_t function, uint16_t first,
                      uint16_t count, uint16_t *words)
{
    if (ncalls < MAX_CALLS) {
        call_t c = {id, function, first, count, 0};
        if (function == 0x06) {
            c.value = words[0];
        }
        calls[ncalls++] = c;
    }

    if (function == 0x06) {
        if (first == 0) {
            snap_seq = words[0];             /* SNAP: broadcast, ответа нет */
        } else if (first == 1) {
            head_write_cmd(words[0]);
        } else if (first == 2) {
            page_word = words[0];
        }
        return ESP_OK;
    }

    /* FC04 */
    if (first == 0) {                        /* снимок: seq, ready, … */
        memset(words, 0, count * sizeof *words);
        words[0] = snap_seq;
        if (count > 1) words[1] = 1;
        if (count > 4) words[4] = 4;         /* сектор */
        if (count > 6) words[6] = 0x10;      /* encoder_state */
        return ESP_OK;
    }
    if (first == 11) {                       /* живой блок статуса, 20 слов */
        if (busy_reads > 0 && --busy_reads == 0) {
            head_state = pending_done;
            eeprom |= 1;
        }
        memset(words, 0, count * sizeof *words);
        words[0] = head_state;
        words[1] = 144;                      /* RDY1 */
        words[2] = 144;                      /* RDY2 */
        words[19] = eeprom;
        put_u32(&words[9], 0);               /* min_max */
        return ESP_OK;
    }
    if (first == 31) {                       /* окно: 16 float за страницу */
        unsigned base = page_word & 0xFFu;
        for (unsigned k = 0; k < count / 2; ++k) {
            unsigned idx = base + k;
            float v = idx < ANGTAB_N ? angtab[idx] : 0.0f;
            uint32_t bits;
            memcpy(&bits, &v, sizeof bits);
            words[2 * k] = (uint16_t)(bits >> 16);
            words[2 * k + 1] = (uint16_t)(bits & 0xFFFFu);
        }
        return ESP_OK;
    }
    memset(words, 0, count * sizeof *words);
    return ESP_OK;
}

/* ----------------------------- проверки -------------------------------- */

#ifdef TEST_CALIB

/* Калибровка говорит только с одной головкой и не шлёт вещания (адрес 0). */
static void check_only_slave_id(uint8_t id)
{
    for (size_t i = 0; i < ncalls; ++i) {
        CHECK(calls[i].id == id,
              "запрос #%zu адресован %u, ожидалась головка %u", i, calls[i].id, id);
    }
}

static const uint8_t expect_cmds[] = {EXPECT_CMDS};
#define EXPECT_CMD_N (sizeof expect_cmds / sizeof expect_cmds[0])

static void test_calibration(void)
{
    /* 1. Адрес головки — тот же, что задан в menuconfig; вещания нет. */
    ncalls = 0;
    calib_mode_run();
    check_only_slave_id(CONFIG_ENCODER_CALIB_SLAVE);

    /* 2. Команды CAL_CMD (FC06, регистр 1) — в ожидаемом порядке. */
    size_t got = 0;
    int shape_ok = 1;
    for (size_t i = 0; i < ncalls; ++i) {
        if (calls[i].fn == 0x06 && calls[i].reg == 1) {
            if (got < EXPECT_CMD_N && calls[i].value != expect_cmds[got]) {
                shape_ok = 0;
            }
            got++;
        }
    }
    CHECK(got == EXPECT_CMD_N, "CAL_CMD записан %zu раз, ожидалось %zu", got, EXPECT_CMD_N);
    CHECK(shape_ok, "порядок команд CAL_CMD не совпал с ожидаемым");

    /* 3. Выгрузка ang_tab: страницы 0,16,…,128 — EXPECT_PAGES записей. */
    size_t pages = 0;
    for (size_t i = 0; i < ncalls; ++i) {
        if (calls[i].fn == 0x06 && calls[i].reg == 2) {
            unsigned idx = calls[i].value & 0xFFu;
            CHECK((calls[i].value >> 8) == 0, "CAL_PAGE: таблица %u вместо ang_tab", calls[i].value >> 8);
            CHECK(idx % 16 == 0 && idx < ANGTAB_N, "CAL_PAGE: индекс %u не кратен странице", idx);
            pages++;
        }
    }
    CHECK(pages == EXPECT_PAGES, "записей CAL_PAGE %zu, ожидалось %d", pages, EXPECT_PAGES);

    /* 4. Последняя запись — стоп 0x10. */
    /* Последняя запись CAL_CMD — стоп (после неё идут только чтения и дамп «после»). */
    int last_cmd = -1;
    for (size_t i = 0; i < ncalls; ++i) {
        if (calls[i].fn == 0x06 && calls[i].reg == 1) {
            last_cmd = calls[i].value;
        }
    }
    CHECK(last_cmd == 0x10, "калибровка должна завершаться командой 0x10, а не 0x%02X", last_cmd);
    CHECK(head_state == 0x00, "после стопа STATE должен быть 0x00, а не 0x%02X", head_state);
}

#else /* TEST_POLL */

static void test_working_cycle(void)
{
    ncalls = 0;
    poll_cycle(42);
    poll_cycle(43);

    int snaps = 0, cal_writes = 0, reads = 0;
    for (size_t i = 0; i < ncalls; ++i) {
        if (calls[i].fn == 0x06) {
            CHECK(calls[i].id == 0 && calls[i].reg == 0,
                  "рабочий режим пишет FC06 не в SNAP: id=%u reg=%u", calls[i].id, calls[i].reg);
            if (calls[i].reg == 0) snaps++;
            if (calls[i].reg == 1 || calls[i].reg == 2) cal_writes++;
        } else {
            reads++;
        }
    }
    CHECK(cal_writes == 0, "рабочий режим отправил команды калибровки: %d", cal_writes);
    CHECK(snaps == 2, "SNAP ожидался дважды, получено %d", snaps);
    CHECK(reads == 2 * (2 + 2), "чтений STATUS/READ за два цикла %d, ожидалось 8", reads);
}

#endif

int main(void)
{
    for (int i = 0; i < ANGTAB_N; ++i) {
        angtab[i] = (float)i + 0.5f;
    }
#ifdef TEST_CALIB
    test_calibration();
#else
    test_working_cycle();
#endif
    if (failures) {
        fprintf(stderr, "провалено проверок: %d\n", failures);
        return 1;
    }
    printf("ok: %zu запросов на шине\n", ncalls);
    return 0;
}
