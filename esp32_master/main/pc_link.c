/*
 * Реализация канала ESP32 -> ПК. См. pc_link.h.
 *
 * Почему так:
 *  - печать 1 строки на 4 головки (~120 символов) при 115200 бод занимает
 *    ~10 мс; раньше эти 10 мс выполнялись внутри poll_heads() и сдвигали
 *    следующий SNAP. Теперь Core0 только копирует ~5 байт на головку.
 *  - Кольцевой буфер ESP-IDF (no-split) безопасен между ядрами, хранит
 *    записи переменной длины и выдаёт их строго в порядке поступления.
 *  - Форматирование (snprintf, деление на 1e6) тоже перенесено на Core1.
 */
#include "pc_link.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "../../Inc/encoder_protocol.h"

#define PC_LINK_CORE        1
#define PC_LINK_PRIO        4
#define PC_LINK_STACK       4096
/* 32 КБ: при 4 головках запись ~28 байт -> ~1000 циклов запаса,
 * при 247 головках ~1.2 КБ -> ~25 циклов. При нехватке увеличить. */
#define PC_LINK_RB_BYTES    (32 * 1024)

typedef enum { REC_DATA = 1, REC_STATS = 2 } rec_kind_t;

/* Заголовок записи; за REC_DATA следуют count пар {id, angle_udeg}. */
typedef struct __attribute__((packed)) {
    uint8_t  kind;
    uint8_t  reserved;
    uint16_t seq;
    uint16_t count;
    uint32_t log_ms;         /* esp_log_timestamp() на момент готовности */
} rec_hdr_t;

typedef struct __attribute__((packed)) {
    uint8_t  id;
    uint32_t angle_udeg;
} rec_item_t;

typedef struct __attribute__((packed)) {
    rec_hdr_t hdr;
    uint32_t cycles, complete, transport_err, invalid, dropped;
    int64_t  last_cycle_us;
} rec_stats_t;

static const char *TAG = "encoder";
static RingbufHandle_t rb;
static volatile uint32_t dropped;

uint32_t pc_link_dropped(void) { return dropped; }

bool pc_link_put_data(uint16_t seq, const uint8_t *ids,
                      const uint32_t *angle_udeg, size_t count)
{
    if (!rb || count == 0 || count > ENCODER_MAX_ID) return false;

    const size_t size = sizeof(rec_hdr_t) + count * sizeof(rec_item_t);
    void *p = NULL;
    /* Резервируем место прямо в буфере: без промежуточной копии на стеке. */
    if (xRingbufferSendAcquire(rb, &p, size, 0) != pdTRUE) {
        ++dropped;
        return false;
    }
    rec_hdr_t *h = p;
    h->kind = REC_DATA;
    h->reserved = 0;
    h->seq = seq;
    h->count = (uint16_t)count;
    h->log_ms = esp_log_timestamp();
    rec_item_t *it = (rec_item_t *)(h + 1);
    for (size_t i = 0; i < count; ++i) {
        it[i].id = ids[i];
        it[i].angle_udeg = angle_udeg[i];
    }
    xRingbufferSendComplete(rb, p);
    return true;
}

bool pc_link_put_stats(uint32_t cycles, uint32_t complete,
                       uint32_t transport_err, uint32_t invalid,
                       int64_t last_cycle_us)
{
    if (!rb) return false;
    rec_stats_t s = {
        .hdr = { .kind = REC_STATS, .log_ms = esp_log_timestamp() },
        .cycles = cycles, .complete = complete,
        .transport_err = transport_err, .invalid = invalid,
        .dropped = dropped, .last_cycle_us = last_cycle_us
    };
    if (xRingbufferSend(rb, &s, sizeof s, 0) != pdTRUE) {
        ++dropped;
        return false;
    }
    return true;
}

/* Только Core1. Формат совпадает с прежним ESP_LOGI(TAG, "%s", line),
 * поэтому diagnostics/serial_head_logger.py менять не нужно. */
static void print_data(const rec_hdr_t *h)
{
    static char line[48U + 24U * ENCODER_MAX_ID];
    const rec_item_t *it = (const rec_item_t *)(h + 1);

    int n = snprintf(line, sizeof line, "I (%lu) %s: DATA seq=%u",
                     (unsigned long)h->log_ms, TAG, (unsigned)h->seq);
    if (n < 0 || (size_t)n >= sizeof line) return;
    size_t used = (size_t)n;

    for (uint16_t i = 0; i < h->count; ++i) {
        const uint32_t a = it[i].angle_udeg;
        n = snprintf(line + used, sizeof line - used, " id=%u ang=%u.%06u",
                     (unsigned)it[i].id, (unsigned)(a / 1000000U),
                     (unsigned)(a % 1000000U));
        if (n < 0 || (size_t)n >= sizeof line - used) {
            /* Обрезанную строку как полный набор не выводим. */
            printf("E (%lu) %s: DATA line overflow: heads=%u\n",
                   (unsigned long)h->log_ms, TAG, (unsigned)h->count);
            return;
        }
        used += (size_t)n;
    }
    line[used++] = '\n';
    fwrite(line, 1, used, stdout);   /* одна запись -> строки не перемешиваются */
}

static void print_stats(const rec_stats_t *s)
{
    printf("I (%lu) %s: POLL cycles=%lu complete=%lu transport_err=%lu "
           "invalid=%lu dropped=%lu last_cycle_us=%lld\n",
           (unsigned long)s->hdr.log_ms, TAG,
           (unsigned long)s->cycles, (unsigned long)s->complete,
           (unsigned long)s->transport_err, (unsigned long)s->invalid,
           (unsigned long)s->dropped, (long long)s->last_cycle_us);
}

static void pc_link_task(void *arg)
{
    (void)arg;
    for (;;) {
        size_t size = 0;
        rec_hdr_t *h = xRingbufferReceive(rb, &size, portMAX_DELAY);
        if (!h) continue;
        if (h->kind == REC_DATA &&
            size == sizeof *h + h->count * sizeof(rec_item_t))
            print_data(h);
        else if (h->kind == REC_STATS && size == sizeof(rec_stats_t))
            print_stats((const rec_stats_t *)h);
        vRingbufferReturnItem(rb, h);
        /* Если в буфере ещё есть записи — сразу берём следующую;
         * fflush только когда очередь опустела, чтобы не дробить вывод. */
        UBaseType_t waiting = 0;
        vRingbufferGetInfo(rb, NULL, NULL, NULL, NULL, &waiting);
        if (waiting == 0) fflush(stdout);
    }
}

void pc_link_start(void)
{
    rb = xRingbufferCreate(PC_LINK_RB_BYTES, RINGBUF_TYPE_NOSPLIT);
    ESP_ERROR_CHECK(rb ? ESP_OK : ESP_ERR_NO_MEM);
    if (xTaskCreatePinnedToCore(pc_link_task, "pc_link", PC_LINK_STACK, NULL,
                                PC_LINK_PRIO, NULL, PC_LINK_CORE) != pdPASS)
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
}
