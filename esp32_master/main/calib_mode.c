/*
 * Режим калибровки головки на ESP32 master (собирается при ENCODER_MODE_CALIB).
 *
 * Перенос tools/modbus_calib.py: режим run и его помощники (wait_ready,
 * wait EEPROM, dump, show). Значения — те же: порядок команд, скорости и
 * ориентиры по оборотам, таймауты ожидания, интервал опроса, два круга, числа
 * повторов чтения. Карта регистров — зеркало Inc/modbus_calib.h (сверяется
 * тестом tests/calib_map_test.py). Шина та же, что у рабочего режима (bus.c),
 * но в этой сборке рабочего опроса нет: головка получает только команды калибровки.
 *
 * Вывод — printf без префиксов журнала, чтобы выгрузку CSV можно было сохранить
 * из монитора как есть.
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bus.h"
#include "calib_mode.h"

/* ------------------------- коды и карта регистров ------------------------- */
/* Зеркало Inc/modbus_calib.h. Адреса — на проводе, с нуля. */
#define H_CAL_CMD       1   /* FC06: код команды CAL_CMD */
#define H_CAL_PAGE      2   /* FC06: окно выгрузки [15:8] таблица, [7:0] индекс */
#define CAL_FIRST       11  /* FC04: первый регистр статуса */
#define CAL_STATUS_WORDS 20 /* регистров статуса, 11…30 */
#define CAL_WINDOW      31  /* FC04: окно выгрузки таблиц */
#define CAL_WINDOW_FLOATS 16

#define CMD_STOP        0x10
#define CMD_ANGTAB_RIGHT 0x11
#define CMD_ANGTAB_LEFT  0x12
#define CMD_OFFSET      0x13
#define CMD_BUFK        0x14

#define TABLE_ANGTAB    0   /* CAL_PAGE: номер таблицы ang_tab */
#define ANGTAB_LEN      144

#define MB_E_WRITTEN    (1u << 0)  /* запись в EEPROM завершена */

/* Поведение, перенесённое из утилиты */
#define READ_RETRIES    2     /* повторы чтения, как --retries 2 */
#define EEPROM_WAIT_MS  60000 /* утилита ждала без предела; здесь предел для безопасности */

/* Команда, её ожидание и подписи. Скорости, ориентиры и таймауты — из
 * RUN_INFO / DONE утилиты; after_write — STATE сразу после записи. */
typedef struct {
    uint8_t code;
    const char *name;
    const char *desc;
    uint8_t after_write;   /* STATE сразу после записи команды */
    uint8_t done_state;    /* STATE, когда команда отработала */
    const char *done_note;
    const char *speed;     /* обороты, об/мин */
    const char *revs;      /* ориентир по числу оборотов */
    uint32_t timeout_s;    /* верхняя граница ожидания */
} cal_cmd_t;

static const cal_cmd_t CMDS[] = {
    {CMD_ANGTAB_RIGHT, "ANGTAB_RIGHT", "ang_tab, проход 1 — угол должен РАСТИ",
     0x10, 0x20, "проход 1 готов, ждём 0x12", "5…20", "5…10", 300},
    {CMD_ANGTAB_LEFT, "ANGTAB_LEFT", "ang_tab, проход 2 — угол должен УБЫВАТЬ",
     0x30, 0x40, "ang_tab применена и записана", "5…20", "5…10", 300},
    {CMD_OFFSET, "OFFSET", "offset_cal — перебор точки съёма",
     0x50, 0x60, "offset записан", "10…40", "12…68", 600},
    {CMD_BUFK, "BUFK", "angk_cal — выравнивание пикселей",
     0x70, 0x80, "buf_k записана", "10…40", "50…70", 900},
};

/* Порядок запуска — обратный нумерации: ang_tab зависит от offset и выправленных
 * границ (buf_k), а min_max для offset/buf_k считается по ang_tab. */
static const uint8_t RUN_ORDER[] = {CMD_BUFK, CMD_OFFSET, CMD_ANGTAB_RIGHT, CMD_ANGTAB_LEFT};

static const cal_cmd_t *find_cmd(uint8_t code)
{
    for (size_t i = 0; i < sizeof CMDS / sizeof CMDS[0]; ++i) {
        if (CMDS[i].code == code)
            return &CMDS[i];
    }
    return NULL;
}

static const char *state_text(uint16_t code)
{
    switch (code) {
    case 0x00: return "остановлено";
    case 0x10: return "идёт проход 1 ang_tab";
    case 0x20: return "проход 1 готов — нужна команда 0x12";
    case 0x30: return "идёт проход 2 ang_tab";
    case 0x40: return "ang_tab записана";
    case 0x50: return "идёт offset_cal";
    case 0x60: return "offset записан";
    case 0x70: return "идёт angk_cal";
    case 0x80: return "buf_k записана";
    default:   return "неизвестный код";
    }
}

/* Разобранный блок статуса 11…30 (см. tools/modbus_calib.py, REGS/decode). */
typedef struct {
    uint16_t state;
    uint16_t rdy1, rdy2;
    uint16_t offset_phase, offset_cur, offset_found, offset_x256;
    uint16_t angk_phase;
    int16_t  pix_min_max;
    int32_t  minmax_udeg;     /* знаковые микроградусы */
    uint16_t revs, rev_right, rev_left, serr1;
    uint32_t angle_udeg;
    uint32_t frames;
    uint16_t eeprom;
} cal_status_t;

static void decode_status(const uint16_t *w, cal_status_t *s)
{
    s->state = w[0];
    s->rdy1 = w[1];
    s->rdy2 = w[2];
    s->offset_phase = w[3];
    s->offset_cur = w[4];
    s->offset_found = w[5];
    s->offset_x256 = w[6];
    s->angk_phase = w[7];
    s->pix_min_max = (int16_t)w[8];
    s->minmax_udeg = (int32_t)(((uint32_t)w[9] << 16) | w[10]);
    s->revs = w[11];
    s->rev_right = w[12];
    s->rev_left = w[13];
    s->serr1 = w[14];
    s->angle_udeg = ((uint32_t)w[15] << 16) | w[16];
    s->frames = ((uint32_t)w[17] << 16) | w[18];
    s->eeprom = w[19];
}

/* ------------------------------ обмен с шиной ------------------------------ */

static esp_err_t read_regs(uint8_t slave, uint16_t first, uint16_t count, uint16_t *w)
{
    esp_err_t err = ESP_FAIL;
    for (int attempt = 0; attempt < READ_RETRIES; ++attempt) {
        if (attempt) {
            vTaskDelay(pdMS_TO_TICKS(20));  /* пауза перед повтором */
        }
        err = bus_request(slave, 0x04, first, count, w);
        if (err == ESP_OK) {
            return ESP_OK;
        }
    }
    return err;
}

/* FC06 — одна попытка: повтор опасен, если головка команду уже выполнила и
 * эхо потерялось; тогда проход калибровки начался бы заново. */
static esp_err_t write_reg(uint8_t slave, uint16_t reg, uint16_t value)
{
    uint16_t v = value;
    return bus_request(slave, 0x06, reg, 1, &v);
}

static esp_err_t read_status(uint8_t slave, cal_status_t *s)
{
    uint16_t w[CAL_STATUS_WORDS] = {0};
    esp_err_t err = read_regs(slave, CAL_FIRST, CAL_STATUS_WORDS, w);
    if (err == ESP_OK) {
        decode_status(w, s);
    }
    return err;
}

/* ------------------------------- вывод состояния ----------------------------- */

static void show_status(const cal_status_t *s, const char *prefix)
{
    printf("%sSTATE=0x%02X — %s\n", prefix, (unsigned)s->state, state_text(s->state));
    printf("       ang_tab: проход 1 = %u/%d, проход 2 = %u/%d\n",
           s->rdy1, ANGTAB_LEN, s->rdy2, ANGTAB_LEN);
    printf("       offset:  фаза %u, проба %u, итог %u, текущий %.2f пикс.\n",
           s->offset_phase, s->offset_cur, s->offset_found, s->offset_x256 / 256.0);
    printf("       buf_k:   фаза %u, разброс пиков %d (цель <100)\n",
           s->angk_phase, s->pix_min_max);
    /* 1 микроградус = 3.6 угловой миллисекунды (как в утилите: arcsec = udeg * 3.6e-3) */
    printf("       min_max = %.3f″, оборотов в пробе %u, вправо %u, влево %u\n",
           s->minmax_udeg * 3.6e-3, s->revs, s->rev_right, s->rev_left);
    printf("       кадры: %lu, брак %u, угол %.4f°\n",
           (unsigned long)s->frames, s->serr1, s->angle_udeg / 1e6);
    printf("       EEPROM: ");
    static const char *const EEPROM_TEXT[4] = {
        "запись завершена", "ang_tab принята", "buf_k принята", "offset принят"};
    bool any = false;
    for (unsigned bit = 0; bit < 4; ++bit) {
        if (s->eeprom >> bit & 1) {
            printf("%s%s", any ? ", " : "", EEPROM_TEXT[bit]);
            any = true;
        }
    }
    printf("%s\n", any ? "" : "—");
}

/* Булевы опции menuconfig в sdkconfig.h появляются только при значении y,
 * поэтому проверяем их через #if, а не через обычный if. */
static bool dump_enabled(void)
{
#if CONFIG_ENCODER_CALIB_DUMP
    return true;
#else
    return false;
#endif
}

static bool twice_enabled(void)
{
#if CONFIG_ENCODER_CALIB_TWICE
    return true;
#else
    return false;
#endif
}

/* Короткая строка «до/после» по ходу каждой команды. */
static void show_brief(const char *label, const cal_status_t *s)
{
    printf("   %s MINMAX=%.3f″, PIX_MIN_MAX=%d, offset=%.2f пикс.\n", label,
           s->minmax_udeg * 3.6e-3, s->pix_min_max, s->offset_x256 / 256.0);
}

/* ------------------------------- подтверждение ------------------------------ */

/* true — отправить команду, false — оператор выбрал стоп (аналог Ctrl-C в утилите). */
static bool confirm_step(void)
{
#if CONFIG_ENCODER_CALIB_CONFIRM
    char line[32];
    for (;;) {
        printf("   Enter — отправить, q + Enter — стоп: ");
        fflush(stdout);
        if (fgets(line, sizeof line, stdin) == NULL) {
            vTaskDelay(pdMS_TO_TICKS(100));  /* нет ввода — не занимать процессор */
            continue;
        }
        if (line[0] == 'q' || line[0] == 'Q') {
            return false;
        }
        if (line[0] == '\n' || line[0] == '\r') {
            return true;
        }
        printf("   не понято, нажмите Enter или q\n");
    }
#else
    return true;
#endif
}

/* ------------------------------- ожидания ------------------------------------ */

static uint32_t elapsed_ms(TickType_t since)
{
    return (uint32_t)((xTaskGetTickCount() - since) * portTICK_PERIOD_MS);
}

/* Ждать STATE == done_state; при таймауте — сообщение с прогрессом. */
static esp_err_t wait_ready(uint8_t slave, const cal_cmd_t *c, uint32_t interval_ms)
{
    TickType_t start = xTaskGetTickCount();
    int shown = -1;
    for (;;) {
        cal_status_t s;
        esp_err_t err = read_status(slave, &s);
        if (err != ESP_OK) {
            printf("   обмен прерван: %s\n", esp_err_to_name(err));
            return err;
        }
        if ((int)s.state != shown) {
            shown = s.state;
            printf("   STATE=0x%02X — %s\n", s.state, state_text(s.state));
        }
        if (s.state == c->done_state) {
            printf("   %s\n", c->done_note);
            return ESP_OK;
        }
        if (elapsed_ms(start) > c->timeout_s * 1000u) {
            printf("   ожидание STATE=0x%02X превысило %lu с (прогресс: проход 1 = %u, "
                   "проход 2 = %u, оборотов в пробе %u)\n",
                   (unsigned)c->done_state, (unsigned long)c->timeout_s,
                   s.rdy1, s.rdy2, s.revs);
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }
}

/* Ждать бит 0 EEPROM: запись в энергонезависимую память закончена. */
static esp_err_t wait_eeprom(uint8_t slave)
{
    TickType_t start = xTaskGetTickCount();
    for (;;) {
        cal_status_t s;
        esp_err_t err = read_status(slave, &s);
        if (err != ESP_OK) {
            printf("   обмен прерван: %s\n", esp_err_to_name(err));
            return err;
        }
        if (s.eeprom & MB_E_WRITTEN) {
            return ESP_OK;
        }
        if (elapsed_ms(start) > EEPROM_WAIT_MS) {
            printf("   бит записи EEPROM не появился за %d с\n", EEPROM_WAIT_MS / 1000);
            return ESP_ERR_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/* ------------------------------- процедура ----------------------------------- */

typedef enum {
    CAL_OK,
    CAL_QUIT,   /* оператор нажал q */
    CAL_ERROR,  /* обмен или таймаут */
} cal_result_t;

static cal_result_t run_step(uint8_t slave, const cal_cmd_t *c)
{
    uint32_t interval = CONFIG_ENCODER_CALIB_POLL_MS;
    printf("\n0x%02X %s: %s\n", c->code, c->name, c->desc);
    printf("   обороты %s об/мин, ориентир %s оборотов\n", c->speed, c->revs);

    cal_status_t s;
    esp_err_t err = read_status(slave, &s);
    if (err != ESP_OK) {
        printf("   обмен прерван: %s\n", esp_err_to_name(err));
        return CAL_ERROR;
    }
    show_brief("до:    ", &s);
    if (c->code == CMD_ANGTAB_RIGHT || c->code == CMD_ANGTAB_LEFT) {
        printf("   обороты: вправо %u, влево %u — проверьте направление вращения\n",
               s.rev_right, s.rev_left);
    }
    if (!confirm_step()) {
        return CAL_QUIT;
    }

    err = write_reg(slave, H_CAL_CMD, c->code);
    if (err != ESP_OK) {
        printf("   команда 0x%02X не принята: %s\n", c->code, esp_err_to_name(err));
        return CAL_ERROR;
    }
    err = wait_ready(slave, c, interval);
    if (err != ESP_OK) {
        return CAL_ERROR;
    }
    err = wait_eeprom(slave);
    if (err != ESP_OK) {
        return CAL_ERROR;
    }
    err = read_status(slave, &s);
    if (err != ESP_OK) {
        printf("   обмен прерван: %s\n", esp_err_to_name(err));
        return CAL_ERROR;
    }
    show_brief("после: ", &s);
    return CAL_OK;
}

/* Выполнить список команд; каждая — с подтверждением, ожиданием и записью EEPROM. */
static cal_result_t run_steps(uint8_t slave, const uint8_t *codes, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        const cal_cmd_t *c = find_cmd(codes[i]);
        if (c == NULL) {
            return CAL_ERROR;
        }
        cal_result_t r = run_step(slave, c);
        if (r != CAL_OK) {
            return r;
        }
    }
    return CAL_OK;
}

/* Выгрузка ang_tab постранично: CAL_PAGE := номер_таблицы:индекс, FC04 окна.
 * Страница — 16 float (32 регистра), старшее слово первым. Последняя страница
 * обрезается по длине таблицы. Вывод — CSV между строками-метками. */
static esp_err_t dump_angtab(uint8_t slave, const char *label)
{
    static float values[ANGTAB_LEN];
    for (uint16_t idx = 0; idx < ANGTAB_LEN; idx += CAL_WINDOW_FLOATS) {
        esp_err_t err = write_reg(slave, H_CAL_PAGE, (uint16_t)((TABLE_ANGTAB << 8) | idx));
        if (err != ESP_OK) {
            printf("выгрузка ang_tab: CAL_PAGE не принят: %s\n", esp_err_to_name(err));
            return err;
        }
        uint16_t w[2 * CAL_WINDOW_FLOATS] = {0};
        err = read_regs(slave, CAL_WINDOW, 2 * CAL_WINDOW_FLOATS, w);
        if (err != ESP_OK) {
            printf("выгрузка ang_tab: окно не прочитано: %s\n", esp_err_to_name(err));
            return err;
        }
        for (uint16_t k = 0; k < CAL_WINDOW_FLOATS && idx + k < ANGTAB_LEN; ++k) {
            uint32_t bits = ((uint32_t)w[2 * k] << 16) | w[2 * k + 1];
            memcpy(&values[idx + k], &bits, sizeof bits);
        }
    }
    printf("\n# ang_tab (градусы), головка %u, %s\n", slave, label);
    printf("index,value\n");
    for (uint16_t i = 0; i < ANGTAB_LEN; ++i) {
        printf("%u,%.9g\n", i, (double)values[i]);
    }
    printf("# конец ang_tab\n");
    return ESP_OK;
}

void calib_mode_run(void)
{
    const uint8_t slave = CONFIG_ENCODER_CALIB_SLAVE;
    printf("\n=== РЕЖИМ КАЛИБРОВКИ: головка %u, RS-485 115200 8E1 ===\n", slave);

    cal_status_t s;
    if (read_status(slave, &s) != ESP_OK) {
        printf("головка %u не отвечает: проверьте линию A/B, адрес (ENCODER_CALIB_SLAVE), "
               "питание. Калибровка не запущена.\n", slave);
        return;
    }
    show_status(&s, "состояние до запуска: ");

    /* Без копии исходной ang_tab калибровку не начинаем: перезаписанную таблицу
     * вернуть будет нечем (docs/calibration_modbus.md, шаг 2). */
    if (dump_enabled() && dump_angtab(slave, "до калибровки") != ESP_OK) {
        printf("копия ang_tab до калибровки не получена — калибровка не запущена\n");
        return;
    }

    cal_result_t r = run_steps(slave, RUN_ORDER, sizeof RUN_ORDER);
    if (r == CAL_OK) {
        if (twice_enabled()) {
            printf("\nвторой круг: ang_tab уже записана, уточняем buf_k и offset\n");
            static const uint8_t SECOND[] = {CMD_BUFK, CMD_OFFSET};
            r = run_steps(slave, SECOND, sizeof SECOND);
        } else {
            printf("\nповторите 0x14 и 0x13 (ENCODER_CALIB_TWICE), если MINMAX после ang_tab не "
                   "улучшился: docs/calibration_modbus.md, шаг 9\n");
        }
    }

    if (r == CAL_QUIT) {
        printf("\nпрервано — отправляю 0x10\n");
    } else if (r == CAL_ERROR) {
        printf("\nошибка — отправляю 0x10, чтобы головка не осталась в калибровке\n");
    }
    /* 0x10 — стоп: снимает флаги запуска и возвращает штатный режим. */
    esp_err_t stop = write_reg(slave, H_CAL_CMD, CMD_STOP);
    if (stop != ESP_OK) {
        printf("0x10 не принята: %s — проверьте головку вручную\n", esp_err_to_name(stop));
    }
    if (r == CAL_OK && stop == ESP_OK) {
        printf("\n0x10 STOP — калибровка остановлена, режим штатный\n");
        if (read_status(slave, &s) == ESP_OK) {
            show_status(&s, "итог: ");
        }
        if (dump_enabled()) {
            dump_angtab(slave, "после калибровки");
        }
    }
    printf("\nРежим калибровки завершён. Прошивка больше не посылает команд на линию.\n"
           "Для рабочего опроса пересоберите с режимом «Рабочий режим» (menuconfig).\n");
}
