/*
 * Детерминированный буфер снимка угла головки энкодера (lock-free публикация + ISR latch).
 *
 * latest_slots[2] — двойной буфер последнего ПОЛНОСТЬЮ обработанного кадра
 *                   фотолинейки. Обновляется в main() без запрета прерываний:
 *                   сначала заполняется неактивный слот, затем барьер памяти и
 *                   атомарное переключение индекса latest_idx.
 * frozen          — независимая копия активного слота на момент прихода SNAP.
 *                   Фиксируется сразу в прерывании таймера тишины t3.5 (при
 *                   валидном вещательном кадре 00 06 00 00 seq CRC) с точностью
 *                   до единиц микросекунд и защищена от повторной перезаписи
 *                   тем же номером seq при последующем вызове eMBPoll().
 *
 * В регистрах SNAP_TIME_HI / SNAP_TIME_LO передаётся знаковый int32 возраст
 * оптической экспозиции кадра относительно момента приёма последнего байта SNAP
 * на линии RS-485: age_us = round((snap_rx_cyccnt - exp_cyccnt) / 216).
 */
#include "snapshot_registers.h"
#include "encoder_protocol.h"
#include "port.h"
#include <string.h>

#if defined(__ICCARM__) || defined(__ARM_ARCH)
#define SNAPSHOT_MEMORY_BARRIER() __DMB()
#elif defined(__GNUC__) || defined(__clang__)
#define SNAPSHOT_MEMORY_BARRIER() __asm__ volatile("" ::: "memory")
#else
#define SNAPSHOT_MEMORY_BARRIER() ((void)0)
#endif

typedef struct {
    float angle_deg;
    uint32_t sample;
    uint32_t time_raw; /* exp_cyccnt (при has_cyccnt=1) либо tick_ms (при has_cyccnt=0) */
    uint16_t sector;
    uint8_t error;
    uint8_t encoder_state;
    uint8_t valid;
    uint8_t has_cyccnt;
} frame_t;

static frame_t latest_slots[2];
static volatile uint8_t latest_idx;
static uint32_t sample_counter;
static volatile uint32_t last_rx_cyccnt;

static frame_t frozen;
static uint32_t frozen_angle_udeg;
static uint32_t frozen_time_word;
static uint16_t frozen_seq;
static uint8_t frozen_ready;

/*
 * Точное целочисленное масштабирование IEEE-754 float [0, 360) -> мкград [0..359999999]
 * за O(1) без программной эмуляции double на Cortex-M7F (FPv5-SP-D16).
 * Побитово совпадает с (uint32_t)((double)angle_deg * 1000000.0 + 0.5) с защитой
 * от округления 359.9999996f в невалидное значение 360000000.
 */
static uint32_t angle_deg_to_udeg(float angle_deg)
{
    uint32_t bits;
    memcpy(&bits, &angle_deg, sizeof bits);
    if ((bits & 0x7FFFFFFFU) == 0U || (bits >> 31) != 0U)
        return 0U;

    int32_t exp2 = (int32_t)((bits >> 23) & 0xFFU) - 127;
    uint32_t mant = (bits & 0x7FFFFFU) | 0x800000U;
    uint64_t prod = (uint64_t)mant * 1000000ULL;
    int32_t shift = 23 - exp2;

    if (shift <= 0) {
        uint64_t scaled = prod << (-shift);
        return (scaled >= 360000000ULL) ? 0U : (uint32_t)scaled;
    }
    if (shift > 62)
        return 0U;

    uint64_t udeg = (prod + (1ULL << (shift - 1))) >> shift;
    return (udeg >= 360000000ULL) ? 0U : (uint32_t)udeg;
}

static void publish_internal(float angle_deg, uint16_t sector, uint8_t error,
                             uint8_t encoder_state, uint32_t time_raw,
                             uint8_t has_cyccnt)
{
    uint8_t next = (uint8_t)(latest_idx ^ 1U);
    frame_t *dst = &latest_slots[next];

    sample_counter++;
    dst->valid = (uint8_t)(!error && sector < 144U &&
                           (angle_deg >= 0.0f) && (angle_deg < 360.0f));
    dst->angle_deg = angle_deg;
    dst->sector = sector;
    dst->error = error;
    dst->encoder_state = encoder_state;
    dst->time_raw = time_raw;
    dst->sample = sample_counter;
    dst->has_cyccnt = has_cyccnt;

    SNAPSHOT_MEMORY_BARRIER();
    latest_idx = next;
}

void snapshot_publish(float angle_deg, uint16_t sector, uint8_t error,
                      uint8_t encoder_state, uint32_t tick_ms)
{
    publish_internal(angle_deg, sector, error, encoder_state, tick_ms, 0U);
}

void snapshot_publish_timed(float angle_deg, uint16_t sector, uint8_t error,
                            uint8_t encoder_state, uint32_t exp_cyccnt)
{
    publish_internal(angle_deg, sector, error, encoder_state, exp_cyccnt, 1U);
}

void snapshot_set_rx_cyccnt(uint32_t rx_cyccnt)
{
    last_rx_cyccnt = rx_cyccnt;
}

void snapshot_latch_timed(uint16_t seq, uint32_t snap_rx_cyccnt, uint32_t ticks_per_us)
{
    ENTER_CRITICAL_SECTION();

    if (frozen_ready && frozen_seq == seq) {
        EXIT_CRITICAL_SECTION();
        return;
    }

    uint8_t idx = latest_idx;
    SNAPSHOT_MEMORY_BARRIER();
    frozen = latest_slots[idx];

    frozen_seq = seq;
    frozen_ready = (uint8_t)(frozen.valid && frozen.sample != 0U);

    if (frozen_ready) {
        frozen_angle_udeg = angle_deg_to_udeg(frozen.angle_deg);
    }

    if (frozen.has_cyccnt && ticks_per_us > 0U) {
        int32_t delta_ticks = (int32_t)(snap_rx_cyccnt - frozen.time_raw);
        int32_t half = (int32_t)(ticks_per_us >> 1);
        int32_t age_us = (delta_ticks >= 0)
            ? ((delta_ticks + half) / (int32_t)ticks_per_us)
            : ((delta_ticks - half) / (int32_t)ticks_per_us);
        frozen_time_word = (uint32_t)age_us;
    } else {
        frozen_time_word = frozen.time_raw;
    }

    EXIT_CRITICAL_SECTION();
}

void snapshot_latch(uint16_t seq)
{
    snapshot_latch_timed(seq, last_rx_cyccnt, ENCODER_DWT_TICKS_PER_US);
}

uint16_t snapshot_register(uint16_t address)
{
    switch (address) {
    case SNAP_SEQ: return frozen_seq;
    case SNAP_READY: return frozen_ready;
    case SNAP_ANGLE_HI: return (uint16_t)(frozen_angle_udeg >> 16);
    case SNAP_ANGLE_LO: return (uint16_t)frozen_angle_udeg;
    case SNAP_SECTOR: return frozen.sector;
    case SNAP_ERROR: return frozen.error;
    case SNAP_ENCODER_STATE: return frozen.encoder_state;
    case SNAP_SAMPLE_HI: return (uint16_t)(frozen.sample >> 16);
    case SNAP_SAMPLE_LO: return (uint16_t)frozen.sample;
    case SNAP_TIME_HI: return (uint16_t)(frozen_time_word >> 16);
    case SNAP_TIME_LO: return (uint16_t)frozen_time_word;
    default: return 0;
    }
}
