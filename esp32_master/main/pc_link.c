/*
 * Реализация канала ESP32 -> ПК и целочисленной синхронизации экспозиций головок.
 */
#include "pc_link.h"
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define FULL_TURN_UDEG  360000000LL
#define HALF_TURN_UDEG  180000000LL

void pc_sync_reset_all(pc_head_sync_t state[ENCODER_MAX_ID + 1])
{
    if (state) {
        memset(state, 0, (ENCODER_MAX_ID + 1U) * sizeof(state[0]));
    }
}

uint32_t pc_sync_compensate_udeg(pc_head_sync_t *st, uint32_t raw_udeg,
                                 uint64_t snap_wire_us, int32_t age_us)
{
    if (!st || raw_udeg >= (uint32_t)FULL_TURN_UDEG) {
        return PC_ANGLE_INVALID;
    }

    const bool age_valid = (age_us >= PC_SYNC_MIN_AGE_US && age_us <= PC_SYNC_MAX_AGE_US);
    const int64_t exp_us = age_valid
        ? ((int64_t)snap_wire_us - (int64_t)age_us)
        : (int64_t)snap_wire_us;

    uint32_t out_udeg = raw_udeg;
    if (st->valid && age_valid) {
        const int64_t dt_us = exp_us - st->prev_exp_us;
        if (dt_us >= PC_SYNC_MIN_DT_US && dt_us <= PC_SYNC_MAX_DT_US) {
            int64_t d_udeg = (int64_t)raw_udeg - (int64_t)st->prev_raw_udeg;
            if (d_udeg > HALF_TURN_UDEG) {
                d_udeg -= FULL_TURN_UDEG;
            } else if (d_udeg < -HALF_TURN_UDEG) {
                d_udeg += FULL_TURN_UDEG;
            }
            const int64_t num = d_udeg * (int64_t)age_us;
            const int64_t corr = (num >= 0)
                ? ((num + dt_us / 2) / dt_us)
                : ((num - dt_us / 2) / dt_us);
            int64_t sync = ((int64_t)raw_udeg + corr) % FULL_TURN_UDEG;
            if (sync < 0) {
                sync += FULL_TURN_UDEG;
            }
            out_udeg = (uint32_t)sync;
        }
    }

    st->valid = true;
    st->prev_raw_udeg = raw_udeg;
    st->prev_exp_us = exp_us;
    return out_udeg;
}

#ifndef PC_LINK_HOST_MATH_ONLY
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"

#define PC_LINK_CORE        1
#define PC_LINK_PRIO        4
#define PC_LINK_STACK       6144
#define PC_LINK_RB_BYTES    (64 * 1024)

typedef enum {
    REC_BEGIN = 1,
    REC_POLL  = 2,
    REC_END   = 3,
} rec_kind_t;

typedef struct {
    uint8_t  kind;
    uint8_t  reserved;
    uint16_t seq;
} rec_hdr_t;

typedef struct {
    rec_hdr_t hdr;
    uint32_t  period_us;
} rec_begin_t;

typedef struct {
    rec_hdr_t hdr;
    pc_poll_t poll;
    /* Далее в буфере без разрыва лежат poll.count структур pc_head_t. */
} rec_poll_t;

typedef struct {
    rec_hdr_t hdr;
    uint32_t  attempted;
    uint32_t  dropped;
    uint32_t  overruns;
} rec_end_t;

typedef struct {
    uint32_t count;
    int64_t  min_v;
    int64_t  max_v;
    int64_t  sum_v;
} stat_acc_t;

typedef struct {
    bool       seen;
    stat_acc_t read_us;
    stat_acc_t age_us;
} head_stat_t;

static RingbufHandle_t s_rb;

static struct {
    uint32_t    period_us;
    uint32_t    delivered;
    uint32_t    complete;
    uint32_t    snap_err;
    uint32_t    read_err;
    uint32_t    frame_err;
    bool        have_prev_t;
    uint64_t    prev_t_us;
    stat_acc_t  dt_us;
    stat_acc_t  snap_us;
    stat_acc_t  poll_us;
    head_stat_t heads[ENCODER_MAX_ID + 1U];
} s_stats;

static void stat_reset(stat_acc_t *a)
{
    a->count = 0U;
    a->min_v = INT64_MAX;
    a->max_v = INT64_MIN;
    a->sum_v = 0;
}

static void stat_add(stat_acc_t *a, int64_t v)
{
    if (v < a->min_v) a->min_v = v;
    if (v > a->max_v) a->max_v = v;
    a->sum_v += v;
    a->count++;
}

static void stats_reset_all(uint32_t period_us)
{
    memset(&s_stats, 0, sizeof s_stats);
    s_stats.period_us = period_us;
    stat_reset(&s_stats.dt_us);
    stat_reset(&s_stats.snap_us);
    stat_reset(&s_stats.poll_us);
    for (size_t i = 0; i <= ENCODER_MAX_ID; ++i) {
        stat_reset(&s_stats.heads[i].read_us);
        stat_reset(&s_stats.heads[i].age_us);
    }
}

static int fmt_angle(char *dst, size_t cap, uint32_t udeg)
{
    if (udeg == PC_ANGLE_INVALID || udeg >= (uint32_t)FULL_TURN_UDEG) {
        return snprintf(dst, cap, "nan");
    }
    return snprintf(dst, cap, "%" PRIu32 ".%06" PRIu32,
                    udeg / 1000000U, udeg % 1000000U);
}

static void handle_poll_record(const pc_poll_t *p, const uint8_t *heads_bytes)
{
    static char line[96U + 96U * ENCODER_MAX_ID];

    s_stats.delivered++;
    if (p->error == 0U) s_stats.complete++;
    if (p->error & POLL_ERROR_SNAP)  s_stats.snap_err++;
    if (p->error & POLL_ERROR_READ)  s_stats.read_err++;
    if (p->error & POLL_ERROR_FRAME) s_stats.frame_err++;

    if (s_stats.have_prev_t && p->t_us >= s_stats.prev_t_us) {
        stat_add(&s_stats.dt_us, (int64_t)(p->t_us - s_stats.prev_t_us));
    }
    s_stats.prev_t_us = p->t_us;
    s_stats.have_prev_t = true;
    stat_add(&s_stats.snap_us, (int64_t)p->snap_us);
    stat_add(&s_stats.poll_us, (int64_t)p->poll_us);

    int n = snprintf(line, sizeof line,
                     "DATA seq=%" PRIu16 " t_us=%" PRIu64 " snap_us=%" PRIu32
                     " poll_us=%" PRIu32 " error=%" PRIu16,
                     p->seq, p->t_us, p->snap_us, p->poll_us, p->error);
    if (n < 0 || (size_t)n >= sizeof line) return;
    size_t used = (size_t)n;

    for (uint32_t i = 0; i < p->count; ++i) {
        pc_head_t h;
        memcpy(&h, heads_bytes + (size_t)i * sizeof(pc_head_t), sizeof h);

        if (h.id >= 1U && h.id <= ENCODER_MAX_ID) {
            head_stat_t *hs = &s_stats.heads[h.id];
            hs->seen = true;
            stat_add(&hs->read_us, (int64_t)h.read_us);
            if (h.age_us != PC_AGE_INVALID) {
                stat_add(&hs->age_us, (int64_t)h.age_us);
            }
        }
        char ang_buf[20];
        char raw_buf[20];
        (void)fmt_angle(ang_buf, sizeof ang_buf, h.angle_udeg);
        (void)fmt_angle(raw_buf, sizeof raw_buf, h.raw_angle_udeg);
        const int32_t age_out = (h.age_us == PC_AGE_INVALID) ? 0 : h.age_us;

        n = snprintf(line + used, sizeof line - used,
                     " id=%u ang=%s raw_ang=%s age_us=%" PRId32
                     " read_us=%" PRIu32 " gap_us=%" PRIu32,
                     (unsigned)h.id, ang_buf, raw_buf, age_out,
                     h.read_us, h.gap_us);
        if (n < 0 || (size_t)n >= sizeof line - used) {
            return;
        }
        used += (size_t)n;
    }

    line[used++] = '\n';
    (void)fwrite(line, 1U, used, stdout);
}

static void print_stat_line(const char *name, const stat_acc_t *a)
{
    if (a->count == 0U) {
        printf("TIMING %s count=0 min=0 avg=0 max=0 p2p=0\n", name);
        return;
    }
    const int64_t avg = a->sum_v / (int64_t)a->count;
    const int64_t p2p = a->max_v - a->min_v;
    printf("TIMING %s count=%" PRIu32 " min=%" PRId64 " avg=%" PRId64
           " max=%" PRId64 " p2p=%" PRId64 "\n",
           name, a->count, a->min_v, avg, a->max_v, p2p);
}

static void handle_end_record(const rec_end_t *e)
{
    printf("POLL_END attempted=%" PRIu32 " delivered=%" PRIu32
           " complete=%" PRIu32 " dropped=%" PRIu32 " overruns=%" PRIu32
           " snap_err=%" PRIu32 " read_err=%" PRIu32 " frame_err=%" PRIu32
           " period_us=%" PRIu32 "\n",
           e->attempted, s_stats.delivered, s_stats.complete,
           e->dropped, e->overruns, s_stats.snap_err,
           s_stats.read_err, s_stats.frame_err, s_stats.period_us);

    print_stat_line("dt_us", &s_stats.dt_us);
    print_stat_line("snap_us", &s_stats.snap_us);
    print_stat_line("poll_us", &s_stats.poll_us);

    for (unsigned id = 1U; id <= ENCODER_MAX_ID; ++id) {
        const head_stat_t *hs = &s_stats.heads[id];
        if (!hs->seen) continue;
        const int64_t r_min = hs->read_us.count ? hs->read_us.min_v : 0;
        const int64_t r_avg = hs->read_us.count ? (hs->read_us.sum_v / (int64_t)hs->read_us.count) : 0;
        const int64_t r_max = hs->read_us.count ? hs->read_us.max_v : 0;
        const int64_t a_min = hs->age_us.count ? hs->age_us.min_v : 0;
        const int64_t a_avg = hs->age_us.count ? (hs->age_us.sum_v / (int64_t)hs->age_us.count) : 0;
        const int64_t a_max = hs->age_us.count ? hs->age_us.max_v : 0;
        printf("TIMING head id=%u count=%" PRIu32
               " read_min_us=%" PRId64 " read_avg_us=%" PRId64 " read_max_us=%" PRId64
               " age_min_us=%" PRId64 " age_avg_us=%" PRId64 " age_max_us=%" PRId64 "\n",
               id, hs->read_us.count, r_min, r_avg, r_max, a_min, a_avg, a_max);
    }

    printf("POLL_REPORT_DONE\n");
    fflush(stdout);
}

static void pc_link_task(void *arg)
{
    (void)arg;
    stats_reset_all(0U);
    for (;;) {
        size_t size = 0U;
        uint8_t *raw = (uint8_t *)xRingbufferReceive(s_rb, &size, portMAX_DELAY);
        if (!raw) continue;

        if (size >= sizeof(rec_hdr_t)) {
            rec_hdr_t hdr;
            memcpy(&hdr, raw, sizeof hdr);

            if (hdr.kind == REC_BEGIN && size == sizeof(rec_begin_t)) {
                rec_begin_t b;
                memcpy(&b, raw, sizeof b);
                stats_reset_all(b.period_us);
            } else if (hdr.kind == REC_POLL && size >= sizeof(rec_poll_t)) {
                rec_poll_t rp;
                memcpy(&rp, raw, sizeof rp);
                const size_t expected = sizeof(rec_poll_t) +
                                        (size_t)rp.poll.count * sizeof(pc_head_t);
                if (size == expected && rp.poll.count <= ENCODER_MAX_ID) {
                    handle_poll_record(&rp.poll, raw + sizeof(rec_poll_t));
                }
            } else if (hdr.kind == REC_END && size == sizeof(rec_end_t)) {
                rec_end_t e;
                memcpy(&e, raw, sizeof e);
                handle_end_record(&e);
            }
        }

        vRingbufferReturnItem(s_rb, (void *)raw);
        UBaseType_t waiting = 0;
        vRingbufferGetInfo(s_rb, NULL, NULL, NULL, NULL, &waiting);
        if (waiting == 0) {
            fflush(stdout);
        }
    }
}

void pc_link_start(void)
{
    s_rb = xRingbufferCreate(PC_LINK_RB_BYTES, RINGBUF_TYPE_NOSPLIT);
    ESP_ERROR_CHECK(s_rb ? ESP_OK : ESP_ERR_NO_MEM);
    if (xTaskCreatePinnedToCore(pc_link_task, "pc_link", PC_LINK_STACK, NULL,
                                PC_LINK_PRIO, NULL, PC_LINK_CORE) != pdPASS) {
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
}

void pc_link_begin(uint32_t period_us)
{
    if (!s_rb) return;
    rec_begin_t b = {
        .hdr = { .kind = REC_BEGIN },
        .period_us = period_us,
    };
    (void)xRingbufferSend(s_rb, &b, sizeof b, pdMS_TO_TICKS(20));
}

bool pc_link_put(const pc_poll_t *poll, const pc_head_t *heads)
{
    if (!s_rb || !poll || !heads || poll->count == 0U || poll->count > ENCODER_MAX_ID) {
        return false;
    }
    const size_t bytes = sizeof(rec_poll_t) + (size_t)poll->count * sizeof(pc_head_t);
    void *mem = NULL;
    if (xRingbufferSendAcquire(s_rb, &mem, bytes, 0) != pdTRUE) {
        return false;
    }
    rec_poll_t rec = {
        .hdr = { .kind = REC_POLL, .reserved = 0U, .seq = poll->seq },
        .poll = *poll,
    };
    uint8_t *dst = (uint8_t *)mem;
    memcpy(dst, &rec, sizeof rec);
    memcpy(dst + sizeof rec, heads, (size_t)poll->count * sizeof(pc_head_t));
    (void)xRingbufferSendComplete(s_rb, mem);
    return true;
}

void pc_link_end(uint32_t attempted, uint32_t dropped, uint32_t overruns)
{
    if (!s_rb) return;
    rec_end_t e = {
        .hdr = { .kind = REC_END },
        .attempted = attempted,
        .dropped = dropped,
        .overruns = overruns,
    };
    (void)xRingbufferSend(s_rb, &e, sizeof e, pdMS_TO_TICKS(100));
}
#endif /* PC_LINK_HOST_MATH_ONLY */
