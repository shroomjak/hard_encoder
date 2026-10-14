/*
 * Тест автомата режимов ESP32 без ESP-IDF и оборудования (UTF-8).
 * Запуск из корня: sh tests/host/run_workflow_test.sh
 * Требуются sh и cc с поддержкой C99. Собирается реальный workflow.c.
 *
 * Подменяем транспорт, poll, часы и журнал через workflow_io_t. Ответы
 * головок и время задаёт тест: десятиминутный тайм-аут проверяется мгновенно.
 * Проверяем строгий CSV ID, полный поиск 1..247, принятие ready=0, порядок
 * выбранного списка, отсутствие головок, взаимоисключение режимов и stop.
 * Для двух головок проверяем 14→13→11→12→14→13, next на каждом переходе,
 * ожидание EEPROM, финальный STOP, старую прошивку, неожиданное состояние,
 * потерю квитанции FC06 без повторного запуска, ошибки LIVE и тайм-аут,
 * в том числе при переполнении счётчика мс.
 * Зачем: не запустить калибровку на неверном ID/без подтверждения и не считать
 * завершённым этап с потерянным ответом или незаконченной записью EEPROM.
 * Здесь НЕ проверяются сам poll (см. master_poll_test.py), FreeRTOS-очередь,
 * UART-консоль, стек esp-modbus и реальные алгоритмы STM32.
 * Успех — код 0 и строка ESP32 workflow: ... OK; ошибка — assert.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "workflow.h"

static workflow_t w;
static bool present[248], all_missing, legacy, fail_write, fail_read, fail_stop;
static uint16_t states[248], ready[248];
static uint32_t clock_ms;
static unsigned scanned, writes, polls;
static uint8_t write_ids[64];
static uint16_t written[64];
static char last_message[192];

static uint16_t busy(uint16_t command)
{
    switch (command) {
    case 0x11: return 0x10;
    case 0x12: return 0x30;
    case 0x13: return 0x50;
    case 0x14: return 0x70;
    default: assert(command == 0x10); return 0;
    }
}
static bool request(uint8_t id, uint8_t fn, uint16_t first, uint16_t count, uint16_t *r)
{
    assert(id >= 1 && id <= 247); /* Never broadcast calibration or discovery. */
    if (fn == 4 && first == 0) {
        assert(count == 2 && id == ++scanned);
        r[0] = 0; r[1] = 0; /* Discovery must accept not-ready heads. */
        return !all_missing && present[id];
    }
    if (fn == 6) {
        assert(first == ENCODER_CAL_COMMAND && count == 1 && writes < 64);
        written[writes] = *r; write_ids[writes++] = id;
        if (fail_stop && *r == 0x10) return false;
        states[id] = busy(*r);
        /* Model lost write ACK, despite successfully executed command. */
        return !fail_write;
    }
    assert(fn == 4 && first == ENCODER_CAL_STATE && count == ENCODER_CAL_REG_COUNT);
    if (fail_read) return false;
    r[0] = states[id]; r[1] = ready[id];
    r[2] = legacy ? 0 : ENCODER_CAL_MAGIC;
    return true;
}
static void poll(const uint8_t *ids, size_t count, uint16_t seq)
{
    assert(count == 2 && ids[0] == 247 && ids[1] == 1);
    assert(seq == 42 + polls++);
}
static void log_message(const char *msg) { snprintf(last_message, sizeof last_message, "%s", msg); }
static uint32_t now(void) { return clock_ms; }
static void reset(void)
{
    all_missing = legacy = fail_write = fail_read = fail_stop = false;
    scanned = writes = polls = 0; clock_ms = 0;
    memset(present, 0, sizeof present);
    memset(states, 0, sizeof states);
    for (unsigned i = 1; i <= 247; ++i) ready[i] = 1;
    present[1] = present[247] = true;
    workflow_init(&w, (workflow_io_t){request, poll, log_message, now});
}
static void send(command_kind_t kind, const char *ids)
{
    command_t cmd = {.kind = kind};
    if (ids) assert(parse_head_ids(ids, &cmd));
    workflow_command(&w, &cmd);
}
static void scan(void)
{
    for (unsigned i = 0; i < 247; ++i) workflow_tick(&w);
    assert(scanned == 247);
}
static void start_calib(void)
{
    reset(); send(CMD_CALIB, "247,1"); scan();
    assert(w.state == WF_CONFIRM && writes == 0);
}
static void parser_tests(void)
{
    command_t cmd;
    const char *bad[] = {"", "0", "248", "1,1", "1,", ",1", "1,,2", "1 2", "-1", "+1",
                         "0x10", "1,x", "999999999999999999", "1, 2", "1\n"};
    for (unsigned i = 0; i < sizeof bad / sizeof bad[0]; ++i) {
        assert(!parse_head_ids(bad[i], &cmd)); assert(!cmd.count);
    }
    assert(parse_head_ids("247,1,2", &cmd));
    assert(cmd.count == 3 && cmd.ids[0] == 247 && cmd.ids[1] == 1);
    char all[1024]; size_t n = 0;
    for (unsigned i = 1; i <= 247; ++i)
        n += (size_t)snprintf(all + n, sizeof all - n, "%s%u", i == 1 ? "" : ",", i);
    assert(parse_head_ids(all, &cmd) && cmd.count == 247);
}
int main(void)
{
    parser_tests();
    reset(); send(CMD_POLL, "247,1"); scan();
    assert(w.state == WF_POLL && writes == 0);
    workflow_tick(&w); workflow_tick(&w); assert(polls == 2);
    send(CMD_CALIB, NULL); assert(w.state == WF_POLL); /* Reject overlapping modes. */
    send(CMD_STOP, NULL); assert(w.state == WF_IDLE);
    reset(); send(CMD_POLL, "2"); scan(); assert(w.state == WF_IDLE);
    reset(); all_missing = true; send(CMD_CALIB, NULL); scan(); assert(w.state == WF_IDLE);
    reset(); send(CMD_SCAN, NULL); scan(); assert(w.state == WF_IDLE && writes == 0);
    reset(); send(CMD_CALIB, NULL); scan(); assert(w.ids[0] == 1 && w.ids[1] == 247);
    reset(); send(CMD_SCAN, NULL); workflow_tick(&w); send(CMD_STOP, NULL);
    workflow_tick(&w); assert(scanned == 1 && w.state == WF_IDLE);

    /* Two heads, exact six-step sequence, confirmation at EVERY boundary. */
    static const uint16_t sequence[] = {0x14,0x13,0x11,0x12,0x14,0x13};
    start_calib();
    for (unsigned head = 0; head < 2; ++head) {
        unsigned id = head == 0 ? 247 : 1;
        for (unsigned step = 0; step < 6; ++step) {
            unsigned before = writes;
            workflow_tick(&w); assert(writes == before && w.state == WF_CONFIRM);
            send(CMD_NEXT, NULL);
            assert(w.state == WF_CALIB && written[before] == sequence[step] && write_ids[before] == id);
            workflow_tick(&w); assert(w.state == WF_CALIB); /* busy */
            states[id] += 0x10; ready[id] = 0;
            workflow_tick(&w); assert(w.state == WF_CALIB); /* done, EEPROM still busy */
            ready[id] = 1;
            workflow_tick(&w);
            assert(w.state == ((head == 1 && step == 5) ? WF_IDLE : WF_CONFIRM));
        }
    }
    assert(writes == 14 && written[6] == 0x10 && written[13] == 0x10 && polls == 0);

    start_calib(); legacy = true; send(CMD_NEXT, NULL);
    assert(w.state == WF_IDLE && writes == 0); /* Old firmware must never receive CAL. */
    start_calib(); states[247] = 0x70; send(CMD_NEXT, NULL);
    assert(w.state == WF_IDLE && writes == 0); /* Do not hijack existing calibration. */
    start_calib(); ready[247] = 0; send(CMD_NEXT, NULL);
    assert(w.state == WF_IDLE && writes == 0);
    start_calib(); send(CMD_NEXT, NULL); send(CMD_STOP, NULL);
    assert(w.state == WF_IDLE && written[1] == 0x10 && !states[247]);
    start_calib(); send(CMD_STOP, NULL); assert(writes == 0);
    start_calib(); send(CMD_NEXT, NULL); states[247] = 0x80; workflow_tick(&w);
    assert(w.state == WF_CONFIRM); send(CMD_STOP, NULL); assert(writes == 2);
    start_calib(); fail_write = true; send(CMD_NEXT, NULL);
    assert(w.state == WF_IDLE && writes == 2 && written[0] == 0x14 && written[1] == 0x10);
    start_calib(); send(CMD_NEXT, NULL); states[247] = 0; workflow_tick(&w);
    assert(w.state == WF_IDLE && writes == 2); /* Head reset / unexpected state. */
    start_calib(); send(CMD_NEXT, NULL); clock_ms = 600000; workflow_tick(&w);
    assert(w.state == WF_IDLE && writes == 2);
    start_calib(); clock_ms = UINT32_MAX - 100; send(CMD_NEXT, NULL);
    clock_ms += 600000; workflow_tick(&w); assert(w.state == WF_IDLE); /* Wrap-safe clock. */
    start_calib(); send(CMD_NEXT, NULL); fail_read = true;
    for (unsigned i = 0; i < 4; ++i) { workflow_tick(&w); assert(w.state == WF_CALIB); }
    workflow_tick(&w); assert(w.state == WF_IDLE && writes == 2);
    assert(strstr(last_message, "NOT CONFIRMED"));
    start_calib(); send(CMD_NEXT, NULL); fail_read = true; workflow_tick(&w);
    fail_read = false; workflow_tick(&w); assert(w.failures == 0);
    fail_stop = true; send(CMD_STOP, NULL); assert(w.state == WF_IDLE && states[247] == 0x70);
    puts("ESP32 workflow: parser, discovery, selection, calibration, confirmation, failure paths OK");
    return 0;
}
