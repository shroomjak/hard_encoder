/*
 * Автомат режимов master: поиск slave, рабочий poll и отдельный calib (UTF-8).
 *
 * Только scan → SCAN всех ID 1..247 → IDLE с сохранённым списком.
 * poll/calib используют явные ID или результат завершённого ручного scan.
 * poll → повторение SNAP/STATUS/READ до stop.
 * calib → CONFIRM → next → CALIB → CONFIRM следующего этапа/головки.
 * Шесть этапов: 14 → 13 → 11 → 12 → 14 → 13; затем STOP и проверка 00.
 * Ошибка прерывает весь список с попыткой STOP. Резервного копирования,
 * управления двигателем и восстановления коэффициентов здесь нет.
 *
 * Все аппаратные зависимости вынесены в workflow_io_t: один и тот же код
 * исполняется на ESP32 и в tests/host/workflow_test.c с подменённым транспортом.
 */
#include "workflow.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define CAL_TIMEOUT_MS 600000U /* 10 минут на этап, включая EEPROM */
#define CAL_MAX_FAILURES 5     /* Успешное LIVE-чтение обнуляет счётчик ошибок. */
/* Команда → ожидаемое состояние выполнения → состояние завершения.
 * Повтор buf_k/offset после двух проходов ang_tab учитывает связь коэффициентов.
 * Подсказка описывает ручное вращение: двигателем этот модуль не управляет. */
static const struct {
    uint16_t command, busy, done;
    const char *instruction;
} steps[] = {
    {0x14, 0x70, 0x80, "buf_k: rotate uniformly, either direction, 10..40 rpm"},
    {0x13, 0x50, 0x60, "offset: rotate uniformly, either direction, 10..40 rpm"},
    {0x11, 0x10, 0x20, "ang_tab pass 1: INCREASING angle, 5..20 rpm"},
    {0x12, 0x30, 0x40, "ang_tab pass 2: REVERSE rotation, 5..20 rpm"},
    {0x14, 0x70, 0x80, "buf_k repeat: rotate uniformly, either direction, 10..40 rpm"},
    {0x13, 0x50, 0x60, "offset repeat: rotate uniformly, either direction, 10..40 rpm"}
};

/* Разбираем цифры вручную: запрет знаков/hex и проверка границы после каждой
 * цифры исключают переполнение даже при очень длинном ошибочном числе.
 * seen не позволяет одному ID дважды попасть в последовательность калибровки. */
bool parse_head_ids(const char *text, command_t *command)
{
    command->count = 0;
    if (!text || !*text) return false;
    bool seen[ENCODER_MAX_ID + 1] = {false};
    const char *p = text;
    do {
        unsigned id = 0;
        if (*p < '0' || *p > '9') goto invalid;
        do {
            id = id * 10 + (unsigned)(*p++ - '0');
            if (id > ENCODER_MAX_ID) goto invalid;
        } while (*p >= '0' && *p <= '9');
        if (!id || seen[id]) goto invalid;
        seen[id] = true;
        command->ids[command->count++] = (uint8_t)id;
        if (!*p) return true;
        if (*p++ != ',' || !*p) goto invalid;
    } while (true);
invalid:
    command->count = 0;
    return false;
}

/* Ограниченный локальный буфер для одной строки. Длинные списки адресов
 * печатаются по одному ID, чтобы не потерять хвост при усечении строки. */
static void message(workflow_t *w, const char *fmt, ...)
{
    char text[192];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(text, sizeof text, fmt, ap);
    va_end(ap);
    w->io.log(text);
}

/* Читаем именно LIVE, а не поле 6 frozen-снимка: SNAP здесь не нужен.
 * Сигнатура защищает от запуска калибровки на старой несовместимой прошивке. */
static bool live(workflow_t *w, uint16_t r[ENCODER_CAL_REG_COUNT])
{
    return w->io.request(w->ids[w->head], 4, ENCODER_CAL_STATE,
                         ENCODER_CAL_REG_COUNT, r) && r[2] == ENCODER_CAL_MAGIC;
}

/* Всегда адресная запись выбранной головке; broadcast для калибровки нет.
 * Эхо FC06 подтверждает приём команды, а результат проверяется отдельно. */
static bool write_command(workflow_t *w, uint16_t command)
{
    return w->io.request(w->ids[w->head], 6, ENCODER_CAL_COMMAND, 1, &command);
}

static bool stop_head(workflow_t *w)
{
    /* Проверяем STOP по LIVE, а не только по эху FC06. Без автоматического
     * повтора запуска: потерянный ответ не означает, что команда не исполнена. */
    bool sent = write_command(w, 0x10);
    uint16_t r[ENCODER_CAL_REG_COUNT];
    if (!live(w, r) || r[0] != 0) {
        message(w, "STOP id=%u NOT CONFIRMED: check head locally; do not assume it stopped",
                w->ids[w->head]);
        return false;
    }
    message(w, "STOP id=%u confirmed%s", w->ids[w->head], sent ? "" : " (FC06 reply lost)");
    return true;
}

/* При ошибке прекращаем весь список, а не переходим к следующему ID.
 * STOP — только попытка: при потере связи состояние slave может быть неизвестно.
 * Ни запись EEPROM, ни уже изменённые коэффициенты этим не откатываются. */
static void abort_calibration(workflow_t *w, const char *reason)
{
    message(w, "CALIB aborted: %s. RAM coefficients are NOT rolled back.", reason);
    stop_head(w);
    w->state = WF_IDLE;
}

/* Ожидание оператора не ограничено CAL_TIMEOUT_MS. Каждый этап, включая
 * первую команду следующей головке, требует отдельного подтверждения next. */
static void confirm_step(workflow_t *w)
{
    w->state = WF_CONFIRM;
    message(w, "CALIB head %u/%u id=%u step %u/6: %s. Type next to confirm, stop to cancel.",
            (unsigned)w->head + 1, (unsigned)w->count, w->ids[w->head],
            (unsigned)w->step + 1, steps[w->step].instruction);
}

/* Нулевой state соответствует WF_IDLE. seq сохраняется при смене режимов,
 * но после перезапуска устройства снова начинается с 42 (см. ограничение в docs). */
void workflow_init(workflow_t *w, workflow_io_t io)
{
    memset(w, 0, sizeof *w);
    w->io = io;
    w->seq = 42;
}

/* Обрабатывается задачей шины между шагами автомата. Консоль не вызывает
 * эту функцию параллельно с обменом. Успешная постановка команды в очередь
 * ещё не означает, что она допустима в текущем состоянии автомата. */
void workflow_command(workflow_t *w, const command_t *command)
{
    if (command->kind == CMD_STOP) {
        /* Частичный поиск нельзя использовать как полный список головок. */
        if (w->state == WF_SCAN) w->found_count = 0;
        if (w->state == WF_CALIB || w->state == WF_CONFIRM) {
            /* В WF_CONFIRM при step=0 мы ещё не запускали эту головку.
             * В WF_CALIB даже первый этап уже активен и требует STOP. */
            if (w->state == WF_CALIB || w->step != 0)
                abort_calibration(w, "operator stop");
        }
        w->state = WF_IDLE;
        message(w, "Idle. Commands: scan, poll [id,...], calib [id,...], next, stop");
        return;
    }
    if (command->kind == CMD_NEXT) {
        if (w->state != WF_CONFIRM) {
            message(w, "next: no confirmation pending");
            return;
        }
        uint16_t r[ENCODER_CAL_REG_COUNT];
        if (!live(w, r)) {
            message(w, "CALIB unavailable: id=%u has no LIVE calibration interface or is offline",
                    w->ids[w->head]);
            w->state = WF_IDLE;
            return;
        }
        /* Повторная проверка после ожидания оператора: за это время головка
         * могла перезагрузиться или изменить состояние. Первый этап требует
         * 0x00, остальные — done предыдущего этапа; EEPROM должна быть свободна.
         * Головку с чужой/незавершённой калибровкой не захватываем. */
        if (r[1] != 1 || (w->step == 0 && r[0] != 0) ||
            (w->step != 0 && r[0] != steps[w->step - 1].done)) {
            message(w, "CALIB refused: state=0x%02x eeprom_ready=%u; inspect head", r[0], r[1]);
            w->state = WF_IDLE;
            return;
        }
        w->state = WF_CALIB;
        w->started = w->io.now_ms();
        w->failures = 0;
        w->last_state = 0xffff;
        if (!write_command(w, steps[w->step].command)) {
            abort_calibration(w, "FC06 failed (command may have executed); no launch retry");
            return;
        }
        message(w, "CALIB id=%u sent 0x%02x; wait 0x%02x -> 0x%02x + EEPROM ready",
                w->ids[w->head], steps[w->step].command,
                steps[w->step].busy, steps[w->step].done);
        return;
    }
    if (w->state != WF_IDLE) {
        message(w, "Busy: type stop before starting another mode");
        return;
    }
    if (command->kind == CMD_SCAN) {
        /* Новый ручной поиск заменяет прежний список, stop его сбрасывает. */
        w->found_count = 0;
        w->scan_id = 1;
        w->state = WF_SCAN;
        message(w, "Scanning ALL Modbus IDs 1..247 (about 2 minutes); stop cancels. No writes.");
        return;
    }
    /* Явные ID не требуют scan и не сверяются с потенциально устаревшим
     * списком. Доступность проверят STATUS в poll / LIVE перед командой calib.
     * Без аргумента используем только завершённый ручной поиск. */
    w->count = command->count ? command->count : w->found_count;
    if (!w->count) {
        message(w, "No scanned heads: specify IDs or run scan first; mode NOT started");
        return;
    }
    memcpy(w->ids, command->count ? command->ids : w->found, w->count);
    message(w, "Selected %u head(s), in requested order:", (unsigned)w->count);
    for (size_t i = 0; i < w->count; ++i) message(w, "  id=%u", w->ids[i]);
    if (command->kind == CMD_POLL) {
        w->state = WF_POLL;
    } else {
        message(w, "WARNING: calibration changes RAM and EEPROM. Back up tables first. Motor is MANUAL.");
        w->head = w->step = 0;
        confirm_step(w);
    }
}

/* Поиск только публикует список и возвращается в IDLE; режим не запускает. */
static void finish_scan(workflow_t *w)
{
    message(w, "SCAN complete: %u responding head(s). Available IDs:", (unsigned)w->found_count);
    /* Построчно: список до 247 адресов не усечётся в буфере журнала. */
    for (size_t i = 0; i < w->found_count; ++i) message(w, "  id=%u", w->found[i]);
    w->state = WF_IDLE;
}

/* Продвинуть текущий режим. IDLE и CONFIRM не создают трафика.
 * Вызов не является полностью неблокирующим: request ждёт ответ/тайм-аут,
 * а poll выполняет полный цикл. Очередь stop проверяется между вызовами. */
void workflow_tick(workflow_t *w)
{
    if (w->state == WF_SCAN) {
        /* Для обнаружения достаточно ответа FC04: ready=0 тоже допустим.
         * Только читаем, чтобы поиск не фиксировал снимки и не запускал EEPROM. */
        uint16_t status[2];
        if (w->io.request((uint8_t)w->scan_id, 4, 0, 2, status)) {
            w->found[w->found_count++] = (uint8_t)w->scan_id;
            message(w, "FOUND id=%u (ready=%u)", w->scan_id, status[1]);
        }
        if (w->scan_id % 32 == 0) message(w, "SCAN progress %u/247", w->scan_id);
        if (++w->scan_id > ENCODER_MAX_ID) finish_scan(w);
    } else if (w->state == WF_POLL) {
        w->io.poll(w->ids, w->count, w->seq++);
    } else if (w->state == WF_CALIB) {
        /* Беззнаковая разность корректна и при переходе счётчика мс через 0,
         * если интервал проверки меньше полного периода uint32_t. */
        if ((uint32_t)(w->io.now_ms() - w->started) >= CAL_TIMEOUT_MS) {
            abort_calibration(w, "step timeout (10 minutes)");
            return;
        }
        uint16_t r[ENCODER_CAL_REG_COUNT];
        if (!live(w, r)) {
            if (++w->failures >= CAL_MAX_FAILURES) abort_calibration(w, "5 consecutive LIVE read failures");
            return;
        }
        w->failures = 0;
        if (r[0] != w->last_state) {
            message(w, "CALIB id=%u state=0x%02x eeprom_ready=%u", w->ids[w->head], r[0], r[1]);
            w->last_state = r[0];
        }
        /* Быстрый этап может завершиться до первого чтения busy. Разрешаем
         * сразу done после подтверждённой FC06; старый done запрещён preflight. */
        if (r[0] != steps[w->step].busy && r[0] != steps[w->step].done) {
            abort_calibration(w, "unexpected state / head reset");
            return;
        }
        /* Код done может появиться раньше завершения передачи I2C.
         * Ждём оба условия; EEPROM_READY не заменяет проверку CRC после reboot. */
        if (r[0] != steps[w->step].done || r[1] != 1) return;
        if (++w->step == sizeof steps / sizeof steps[0]) {
            if (!stop_head(w)) { w->state = WF_IDLE; return; }
            message(w, "CALIB id=%u COMPLETE (6 stages, EEPROM transfer complete)", w->ids[w->head]);
            if (++w->head == w->count) {
                message(w, "CALIB list complete. Verify EEPROM CRC after head reboot via SWD.");
                w->state = WF_IDLE;
                return;
            }
            w->step = 0;
        }
        confirm_step(w); /* В том числе обязательное подтверждение новой головки. */
    }
}
