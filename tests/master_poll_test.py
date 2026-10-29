#!/usr/bin/env python3
"""Регрессионный тест детерминированного опроса ESP32 master, драйвера RS-485 RTU,
компенсации возраста кадра (age_us -> t_snap) и парсера serial_head_logger.py.

Запуск из корня: python3 tests/master_poll_test.py
"""
from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "diagnostics"))
import serial_head_logger  # noqa: E402


def extract_c_block(source: str, anchor: str) -> str:
    pos = 0
    while True:
        start = source.index(anchor, pos)
        brace = source.index("{", start)
        semi = source.find(";", start, brace)
        if semi == -1:
            break
        pos = semi + 1
    depth = 0
    for idx in range(brace, len(source)):
        if source[idx] == "{":
            depth += 1
        elif source[idx] == "}":
            depth -= 1
            if depth == 0:
                return source[start:idx + 1]
    raise RuntimeError(f"Unbalanced braces for {anchor!r}")


MAIN_SRC = (ROOT / "esp32_master/main/main.c").read_text(encoding="utf-8")
CHOOSE_FN = extract_c_block(MAIN_SRC, "static uint32_t choose_poll_period_us(")
POLL_FN = extract_c_block(MAIN_SRC, "static void poll_heads(")

HARNESS_C = f"""
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RS485_RTU_HOST_ONLY 1
#define PC_LINK_HOST_MATH_ONLY 1
#include "rs485_rtu.h"
#include "pc_link.h"

#define DEFAULT_POLL_PERIOD_US 10000U

static pc_head_sync_t sync_state[ENCODER_MAX_ID + 1U];
static uint32_t attempted, dropped;

/* Mock clock & RS-485 transport */
static int64_t mock_now_us;
static esp_err_t mock_snap_err;
static esp_err_t mock_read_err[256];
static uint16_t mock_regs[256][PC_REG_COUNT];
static unsigned snap_calls, read_calls[256], gap_calls;
static pc_poll_t last_poll;
static pc_head_t last_heads[ENCODER_MAX_ID];
static bool mock_rb_full;

static int64_t esp_timer_get_time(void) {{ return mock_now_us; }}

static esp_err_t rs485_send_snap(uint16_t seq, uint64_t *wire_edge_us, uint32_t *snap_total_us)
{{
    (void)seq;
    snap_calls++;
    const uint64_t start = (uint64_t)mock_now_us;
    mock_now_us += 382; /* 8 bytes @ 230400 8E1 */
    if (wire_edge_us) *wire_edge_us = (uint64_t)mock_now_us;
    if (mock_snap_err != ESP_OK) {{
        if (snap_total_us) *snap_total_us = (uint32_t)((uint64_t)mock_now_us - start);
        return mock_snap_err;
    }}
    mock_now_us += RS485_SNAP_SETTLE_US;
    if (snap_total_us) *snap_total_us = (uint32_t)((uint64_t)mock_now_us - start);
    return ESP_OK;
}}

static esp_err_t rs485_read_input_regs(uint8_t id, uint16_t first_reg, uint16_t reg_count,
                                       uint16_t *out_words, uint32_t timeout_us)
{{
    (void)timeout_us;
    assert(first_reg == 0U && reg_count == PC_REG_COUNT);
    read_calls[id]++;
    mock_now_us += 1920;
    if (mock_read_err[id] != ESP_OK) return mock_read_err[id];
    memcpy(out_words, mock_regs[id], reg_count * sizeof(uint16_t));
    return ESP_OK;
}}

static void rs485_interframe_gap(uint32_t gap_us)
{{
    gap_calls++;
    mock_now_us += (int64_t)gap_us;
}}

static bool pc_link_put(const pc_poll_t *poll, const pc_head_t *heads)
{{
    if (mock_rb_full) return false;
    last_poll = *poll;
    memcpy(last_heads, heads, poll->count * sizeof(pc_head_t));
    return true;
}}

{CHOOSE_FN}

{POLL_FN}

static void set_head_frame(uint8_t id, uint16_t seq, uint16_t ready,
                           uint32_t udeg, uint16_t sector, uint16_t errflag,
                           int32_t age_us)
{{
    mock_regs[id][0] = seq;
    mock_regs[id][1] = ready;
    mock_regs[id][2] = (uint16_t)(udeg >> 16);
    mock_regs[id][3] = (uint16_t)(udeg & 0xFFFFU);
    mock_regs[id][4] = sector;
    mock_regs[id][5] = errflag;
    mock_regs[id][6] = 0U;
    mock_regs[id][7] = 0U;
    mock_regs[id][8] = 0U;
    mock_regs[id][9] = (uint16_t)((uint32_t)age_us >> 16);
    mock_regs[id][10] = (uint16_t)((uint32_t)age_us & 0xFFFFU);
}}

static void test_rtu_framing_and_crc(void)
{{
    uint8_t req[8];
    assert(rs485_build_fc06(0U, 0U, 0x002AU, req) == 8U);
    assert(rs485_crc16(req, 8U) == 0U);
    assert(rs485_parse_fc06_reply(req, 8U, req));

    uint8_t reply[5U + 2U * PC_REG_COUNT];
    reply[0] = 7U;
    reply[1] = 0x04U;
    reply[2] = (uint8_t)(2U * PC_REG_COUNT);
    for (unsigned i = 0U; i < PC_REG_COUNT; ++i) {{
        uint16_t v = (uint16_t)(0x1200U + i);
        reply[3U + 2U * i] = (uint8_t)(v >> 8);
        reply[4U + 2U * i] = (uint8_t)(v & 0xFFU);
    }}
    uint16_t crc = rs485_crc16(reply, 3U + 2U * PC_REG_COUNT);
    reply[3U + 2U * PC_REG_COUNT] = (uint8_t)(crc & 0xFFU);
    reply[4U + 2U * PC_REG_COUNT] = (uint8_t)(crc >> 8);

    uint16_t out[PC_REG_COUNT] = {{0}};
    assert(rs485_parse_fc04_reply(reply, sizeof reply, 7U, PC_REG_COUNT, out));
    for (unsigned i = 0U; i < PC_REG_COUNT; ++i) {{
        assert(out[i] == (uint16_t)(0x1200U + i));
    }}
    /* Wrong slave ID or corrupted CRC must be rejected */
    assert(!rs485_parse_fc04_reply(reply, sizeof reply, 2U, PC_REG_COUNT, out));
    reply[5] ^= 0x01U;
    assert(!rs485_parse_fc04_reply(reply, sizeof reply, 7U, PC_REG_COUNT, out));
}}

static void test_period_selection(void)
{{
    assert(choose_poll_period_us(2U, 0U) == 10000U);
    assert(choose_poll_period_us(4U, 0U) == 10100U || choose_poll_period_us(4U, 0U) == 11000U);
    assert(choose_poll_period_us(3U, 0U) == 10000U);
    /* Custom 5 ms (200 Hz) for 2 heads fits wire budget (700 + 4700 = 5400 -> clamped to 5400; 6000 -> 6000) */
    assert(choose_poll_period_us(2U, 6000U) == 6000U);
    assert(choose_poll_period_us(2U, 2000U) == 5400U);
}}

static void test_poll_errors_and_wire_timestamp(void)
{{
    const uint8_t ids[2] = {{2, 7}};
    pc_sync_reset_all(sync_state);
    memset(mock_read_err, 0, sizeof mock_read_err);
    mock_snap_err = ESP_OK;
    mock_rb_full = false;
    mock_now_us = 1000000;
    snap_calls = gap_calls = 0;

    set_head_frame(2, 50U, 1U, 90500000U, 36U, 0U, 84);
    set_head_frame(7, 50U, 1U, 270500000U, 108U, 0U, 112);
    poll_heads(ids, 2U, 50U);

    /* Wire-edge timestamp = cycle_start (1000000) + 382 us SNAP TX, BEFORE 300 us settle! */
    assert(last_poll.t_us == 1000382ULL);
    assert(last_poll.snap_us == 382U + RS485_SNAP_SETTLE_US);
    assert(last_poll.error == 0U);
    assert(gap_calls == 1U); /* Gap ONLY between head 2 and head 7, none trailing! */
    assert(last_heads[0].gap_us == RS485_INTERFRAME_GAP_US);
    assert(last_heads[1].gap_us == 0U);
    assert(last_heads[0].age_us == 84 && last_heads[1].age_us == 112);

    /* Stale seq / invalid sector / read failure flags */
    set_head_frame(2, 50U, 1U, 90500000U, 36U, 0U, 80); /* stale seq != 51 */
    set_head_frame(7, 51U, 1U, 270500000U, 144U, 0U, 80); /* bad sector 144 */
    poll_heads(ids, 2U, 51U);
    assert(last_poll.error == POLL_ERROR_FRAME);
    assert(last_heads[0].angle_udeg == PC_ANGLE_INVALID);
    assert(last_heads[1].angle_udeg == PC_ANGLE_INVALID);

    mock_read_err[2] = ESP_ERR_TIMEOUT;
    set_head_frame(7, 52U, 1U, 271000000U, 108U, 0U, 90);
    poll_heads(ids, 2U, 52U);
    assert(last_poll.error == POLL_ERROR_READ);
    assert(last_heads[0].angle_udeg == PC_ANGLE_INVALID);
    assert(last_heads[1].angle_udeg != PC_ANGLE_INVALID);

    mock_snap_err = ESP_FAIL;
    unsigned r2_before = read_calls[2];
    poll_heads(ids, 2U, 53U);
    assert(last_poll.error == POLL_ERROR_SNAP);
    assert(read_calls[2] == r2_before); /* No unicast reads when SNAP fails */
}}

static void test_multihead_async_exposure_compensation(void)
{{
    /* Simulate constant 20 rpm = 120 deg/s = 120 udeg/us CW AND CCW across 0/360 wrap!
     * Head 2 and Head 7 have asynchronous TSL1401 clocks (sawtooth ages 45..135 us). */
    const uint8_t ids[2] = {{2, 7}};
    mock_snap_err = ESP_OK;
    memset(mock_read_err, 0, sizeof mock_read_err);

    for (int dir = -1; dir <= 1; dir += 2) {{
        pc_sync_reset_all(sync_state);
        int64_t grid_us = 5000000LL;
        /* Start right near 359.0 deg for +dir and 1.0 deg for -dir to cross 0/360 wrap! */
        int64_t base_udeg = (dir > 0) ? 358500000LL : 1500000LL;
        const int64_t omega_udeg_per_us = (int64_t)dir * 120LL;

        for (unsigned k = 0U; k < 40U; ++k) {{
            mock_now_us = grid_us + (int64_t)k * 10000LL;
            const int64_t t_snap_wire = mock_now_us + 382LL;
            const int32_t age2 = 45 + (int32_t)((k * 19U) % 91U);  /* 45..135 us */
            const int32_t age7 = 135 - (int32_t)((k * 29U) % 91U); /* 45..135 us */

            int64_t true_at_snap_2 = (base_udeg + omega_udeg_per_us * (t_snap_wire - grid_us)) % 360000000LL;
            if (true_at_snap_2 < 0) true_at_snap_2 += 360000000LL;
            int64_t true_at_snap_7 = (true_at_snap_2 + 180000000LL) % 360000000LL;

            int64_t raw2 = (true_at_snap_2 - omega_udeg_per_us * age2) % 360000000LL;
            if (raw2 < 0) raw2 += 360000000LL;
            int64_t raw7 = (true_at_snap_7 - omega_udeg_per_us * age7) % 360000000LL;
            if (raw7 < 0) raw7 += 360000000LL;

            set_head_frame(2, (uint16_t)(100U + k), 1U, (uint32_t)raw2, 10U, 0U, age2);
            set_head_frame(7, (uint16_t)(100U + k), 1U, (uint32_t)raw7, 82U, 0U, age7);
            poll_heads(ids, 2U, (uint16_t)(100U + k));

            assert(last_poll.error == 0U);
            if (k >= 1U) {{
                int64_t err2 = (int64_t)last_heads[0].angle_udeg - true_at_snap_2;
                if (err2 > 180000000LL) err2 -= 360000000LL;
                if (err2 < -180000000LL) err2 += 360000000LL;
                int64_t err7 = (int64_t)last_heads[1].angle_udeg - true_at_snap_7;
                if (err7 > 180000000LL) err7 -= 360000000LL;
                if (err7 < -180000000LL) err7 += 360000000LL;
                assert(llabs(err2) <= 1LL);
                assert(llabs(err7) <= 1LL);
            }}
        }}
    }}
}}

int main(void)
{{
    test_rtu_framing_and_crc();
    test_period_selection();
    test_poll_errors_and_wire_timestamp();
    test_multihead_async_exposure_compensation();
    return 0;
}}
"""


def test_logger_compatibility() -> None:
    line_new = (
        "DATA seq=105 t_us=5050382 snap_us=682 poll_us=4772 error=0 "
        "id=2 ang=0.145840 raw_ang=0.135760 age_us=84 read_us=1920 gap_us=250 "
        "id=7 ang=180.145840 raw_ang=180.132400 age_us=112 read_us=1920 gap_us=0"
    )
    row = serial_head_logger.parse_data(line_new, (2, 7))
    assert row == [
        105, 5050382, 682, 4772, 0,
        "0.145840", "0.135760", 84, 1920, 250,
        "180.145840", "180.132400", 112, 1920, 0,
    ], row

    # Legacy DATA line without raw_ang / age_us remains 100% supported
    line_legacy = (
        "I (1234) encoder: DATA seq=12 t_us=100000 snap_us=700 poll_us=4800 error=2 "
        "id=2 ang=nan read_us=3500 gap_us=250 "
        "id=7 ang=45.500000 read_us=1900 gap_us=0"
    )
    row_legacy = serial_head_logger.parse_data(line_legacy, (2, 7))
    assert row_legacy == [
        12, 100000, 700, 4800, 2,
        "nan", "nan", 0, 3500, 250,
        "45.500000", "45.500000", 0, 1900, 0,
    ], row_legacy


if __name__ == "__main__":
    with tempfile.TemporaryDirectory(prefix="encoder_poll_") as tmp:
        src = Path(tmp) / "poll_test.c"
        bin_path = Path(tmp) / "poll_test"
        src.write_text(HARNESS_C, encoding="utf-8")
        subprocess.run(
            [
                "cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                "-DRS485_RTU_HOST_ONLY=1", "-DPC_LINK_HOST_MATH_ONLY=1",
                f"-I{ROOT / 'esp32_master/main'}",
                str(src),
                str(ROOT / "esp32_master/main/rs485_rtu.c"),
                str(ROOT / "esp32_master/main/pc_link.c"),
                "-o", str(bin_path),
            ],
            check=True,
        )
        subprocess.run([str(bin_path)], check=True)
    test_logger_compatibility()
    print("ESP32 poll: direct RTU framing/CRC, wire-edge SNAP, age_us sync (<=1 udeg across 0/360), logger compat OK")
