/**
  ******************************************************************************
  * @file    rs485_proto.h
  * @brief   Кодек протокола SNAP/STATUS/READ поверх RS-485, версия 2.
  *
  * ЭТОТ ФАЙЛ ОБЩИЙ ДЛЯ ОБЕИХ СТОРОН ШИНЫ:
  *   - slave  (STM32F722): Src/main.c
  *   - master (ESP32):     master_esp32/rs485_proto.h  (побайтовая копия,
  *                         потому что Arduino IDE берёт заголовки только из
  *                         каталога скетча; совпадение копий проверяется
  *                         тестом tools/run_tests.sh)
  *
  * Смысл общего кодека: формат строки описан ровно один раз. Раньше мастер
  * и слейв формировали/разбирали строки независимо (snprintf здесь, sscanf
  * там), и любое расхождение вылезало бы только на живом стенде.
  *
  * Свойства, важные для прошивки:
  *   - никакого stdio: ни snprintf, ни sscanf, ни float-форматирования.
  *     Только целочисленная арифметика, поэтому код можно звать в том числе
  *     из обработчика прерывания;
  *   - переполнение буфера невозможно по построению (writer сам следит за
  *     capacity и выставляет флаг ошибки);
  *   - парсер строгий: любой лишний/недостающий символ => отказ.
  *
  * Формат строки:
  *
  *     <payload>*<CRC8 2 hex><терминатор>
  *
  *   CRC8 считается по всем символам payload (всё до '*'), терминатор
  *   "\n" для команд мастера и "\r\n" для ответов слейва (обе стороны
  *   допускают необязательный '\r').
  *
  *   Команды мастера:
  *     S,<seq>            SNAP   - broadcast, ответа нет
  *     T,<id>,<seq>       STATUS - адресный запрос готовности
  *     R,<id>,<seq>       READ   - адресный запрос данных
  *
  *   Ответы слейва:
  *     ACK,<id>,<seq>,frame=<N>
  *     NAK,<id>,<seq>,<REASON>
  *     D,<id>,<seq>,<angle_udeg>,<sector>,<valid>,<state hex2>,<frame>
  *
  *   angle_udeg - угол в МИКРОградусах (6 знаков после запятой в градусах),
  *   диапазон [0 .. 359999999] либо -1 = "угол неизвестен".
  ******************************************************************************
  */

#ifndef RS485_PROTO_H
#define RS485_PROTO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Версия протокола на линии. Меняется при несовместимом изменении формата. */
#define RS485_PROTO_VERSION      2U

/* Разделитель перед контрольной суммой. */
#define RS485_CRC_SEP            '*'

/* Угол передаётся в микроградусах: 1 градус = 1 000 000 единиц. */
#define RS485_ANGLE_SCALE        1000000L
#define RS485_ANGLE_MAX          359999999L   /* 359.999999 градуса */
#define RS485_ANGLE_UNKNOWN      (-1L)        /* достоверного угла ещё не было */

#define RS485_SECTOR_UNKNOWN     (-1)

/*
 * Guard time слейва: если к моменту, когда главный цикл добрался до
 * команды, с момента её приёма прошло больше этого времени, слейв
 * НЕ отвечает вообще. Требование: мастерский RS485_MASTER_TIMEOUT_MS
 * должен быть строго больше, иначе запоздавший ответ ляжет поверх
 * следующей команды мастера (коллизия на полудуплексной шине).
 */
#define RS485_RESPONSE_GUARD_MS  20U
#define RS485_MASTER_TIMEOUT_MS  30U

/* Время жизни снимка (frozen). Снимок старше этого времени считается
 * протухшим: слейв ответит NAK,...,STALE вместо выдачи старых данных. */
#define RS485_FROZEN_TTL_MS      500U

/* Если SNAP не приходил дольше этого времени, следующий SNAP считается
 * началом новой "эпохи" (мастер перезагрузился) и принимается даже с
 * меньшим seq. */
#define RS485_EPOCH_IDLE_MS      500U

/* Причины отказа в NAK. Только латиница в верхнем регистре, без запятых. */
#define RS485_NAK_NOSNAP         "NOSNAP"   /* SNAP ещё не приходил вовсе     */
#define RS485_NAK_SEQ            "SEQ"      /* снимок есть, но под другой seq */
#define RS485_NAK_STALE          "STALE"    /* снимок протух по TTL           */

/* Максимальная длина строки протокола с терминатором и '\0'. */
#define RS485_LINE_MAX           64U
#define RS485_REASON_MAX         12U

#if defined(__GNUC__) || defined(__clang__)
#  define RS485_UNUSED __attribute__((unused))
#else
#  define RS485_UNUSED
#endif

#define RS485_FN static RS485_UNUSED

/* ========================================================================== */
/*  CRC-8/MAXIM (poly 0x31, reflected 0x8C, init 0x00, xorout 0x00).          */
/*  Ровно та же CRC, что считает crc_table[]/crc_8_step() в прошивке, но без  */
/*  таблицы: 256 байт таблицы ради ~30-байтных строк не нужны, а на ESP32     */
/*  такой таблицы нет вовсе. Эквивалентность проверяется тестом.              */
/* ========================================================================== */

RS485_FN uint8_t rs485_crc8_update(uint8_t crc, uint8_t byte)
{
    uint8_t i;
    crc ^= byte;
    for (i = 0U; i < 8U; i++) {
        crc = (uint8_t)((crc & 1U) ? ((crc >> 1) ^ 0x8CU) : (crc >> 1));
    }
    return crc;
}

RS485_FN uint8_t rs485_crc8(const char *data, size_t len)
{
    uint8_t crc = 0U;
    size_t  i;
    for (i = 0U; i < len; i++) {
        crc = rs485_crc8_update(crc, (uint8_t)data[i]);
    }
    return crc;
}

/* ========================================================================== */
/*  Writer: формирование строки без stdio и без шансов на переполнение.       */
/* ========================================================================== */

typedef struct {
    char  *buf;
    size_t cap;   /* размер буфера вместе с местом под '\0' */
    size_t len;
    int    ok;
} rs485_wr_t;

RS485_FN void rs485_wr_init(rs485_wr_t *w, char *buf, size_t cap)
{
    w->buf = buf;
    w->cap = cap;
    w->len = 0U;
    w->ok  = (buf != NULL) && (cap > 1U);
    if (w->ok) {
        buf[0] = '\0';
    }
}

RS485_FN void rs485_wr_char(rs485_wr_t *w, char c)
{
    if (!w->ok) {
        return;
    }
    if ((w->len + 1U) >= w->cap) {   /* +1 - место под завершающий '\0' */
        w->ok = 0;
        return;
    }
    w->buf[w->len++] = c;
    w->buf[w->len]   = '\0';
}

RS485_FN void rs485_wr_str(rs485_wr_t *w, const char *s)
{
    if (s == NULL) {
        w->ok = 0;
        return;
    }
    while (*s != '\0') {
        rs485_wr_char(w, *s++);
    }
}

RS485_FN void rs485_wr_u32(rs485_wr_t *w, uint32_t v)
{
    char     tmp[10];
    uint8_t  n = 0U;

    if (v == 0U) {
        rs485_wr_char(w, '0');
        return;
    }
    while ((v > 0U) && (n < sizeof(tmp))) {
        tmp[n++] = (char)('0' + (char)(v % 10U));
        v /= 10U;
    }
    while (n > 0U) {
        rs485_wr_char(w, tmp[--n]);
    }
}

RS485_FN void rs485_wr_i32(rs485_wr_t *w, int32_t v)
{
    uint32_t mag;
    if (v < 0) {
        rs485_wr_char(w, '-');
        mag = (uint32_t)(-(int64_t)v);
    } else {
        mag = (uint32_t)v;
    }
    rs485_wr_u32(w, mag);
}

RS485_FN void rs485_wr_hex8(rs485_wr_t *w, uint8_t v)
{
    static const char hex[] = "0123456789ABCDEF";
    rs485_wr_char(w, hex[(v >> 4) & 0x0FU]);
    rs485_wr_char(w, hex[v & 0x0FU]);
}

/* Дописывает "*<CRC8>" и терминатор. Возвращает итоговую длину строки
 * (без '\0') либо 0, если что-то не влезло. */
RS485_FN size_t rs485_wr_finish(rs485_wr_t *w, int crlf)
{
    uint8_t crc;

    if (!w->ok || (w->len == 0U)) {
        return 0U;
    }
    crc = rs485_crc8(w->buf, w->len);
    rs485_wr_char(w, RS485_CRC_SEP);
    rs485_wr_hex8(w, crc);
    if (crlf) {
        rs485_wr_char(w, '\r');
    }
    rs485_wr_char(w, '\n');

    return w->ok ? w->len : 0U;
}

/* ========================================================================== */
/*  Reader: строгий разбор без sscanf.                                        */
/* ========================================================================== */

typedef struct {
    const char *p;
    int         ok;
} rs485_rd_t;

RS485_FN void rs485_rd_init(rs485_rd_t *r, const char *s)
{
    r->p  = s;
    r->ok = (s != NULL);
}

RS485_FN void rs485_rd_lit(rs485_rd_t *r, const char *lit)
{
    if (!r->ok) {
        return;
    }
    while (*lit != '\0') {
        if (*r->p != *lit) {
            r->ok = 0;
            return;
        }
        r->p++;
        lit++;
    }
}

RS485_FN uint32_t rs485_rd_u32(rs485_rd_t *r)
{
    uint32_t v     = 0U;
    uint8_t  digits = 0U;

    if (!r->ok) {
        return 0U;
    }
    while ((*r->p >= '0') && (*r->p <= '9')) {
        uint32_t d = (uint32_t)(*r->p - '0');
        if ((v > 429496729U) || ((v == 429496729U) && (d > 5U))) {
            r->ok = 0;                      /* переполнение uint32 */
            return 0U;
        }
        v = (v * 10U) + d;
        r->p++;
        if (digits < 255U) {
            digits++;
        }
    }
    if (digits == 0U) {
        r->ok = 0;
    }
    return v;
}

RS485_FN int32_t rs485_rd_i32(rs485_rd_t *r)
{
    int      neg = 0;
    uint32_t mag;

    if (!r->ok) {
        return 0;
    }
    if (*r->p == '-') {
        neg = 1;
        r->p++;
    }
    mag = rs485_rd_u32(r);
    if (!r->ok) {
        return 0;
    }
    if (neg) {
        if (mag > 2147483648U) {
            r->ok = 0;
            return 0;
        }
        return (int32_t)(-(int64_t)mag);
    }
    if (mag > 2147483647U) {
        r->ok = 0;
        return 0;
    }
    return (int32_t)mag;
}

RS485_FN uint8_t rs485_rd_hex8(rs485_rd_t *r)
{
    uint8_t v = 0U;
    uint8_t i;

    if (!r->ok) {
        return 0U;
    }
    for (i = 0U; i < 2U; i++) {
        char c = *r->p++;
        uint8_t d;
        if ((c >= '0') && (c <= '9')) {
            d = (uint8_t)(c - '0');
        } else if ((c >= 'A') && (c <= 'F')) {
            d = (uint8_t)(10 + (c - 'A'));
        } else if ((c >= 'a') && (c <= 'f')) {
            d = (uint8_t)(10 + (c - 'a'));
        } else {
            r->ok = 0;
            return 0U;
        }
        v = (uint8_t)((v << 4) | d);
    }
    return v;
}

/* Токен из заглавных латинских букв (причина NAK). */
RS485_FN void rs485_rd_token(rs485_rd_t *r, char *dst, size_t cap)
{
    size_t n = 0U;

    if (!r->ok) {
        return;
    }
    while ((*r->p >= 'A') && (*r->p <= 'Z')) {
        if ((n + 1U) >= cap) {
            r->ok = 0;
            return;
        }
        dst[n++] = *r->p++;
    }
    dst[n] = '\0';
    if (n == 0U) {
        r->ok = 0;
    }
}

RS485_FN int rs485_rd_end(rs485_rd_t *r)
{
    if (!r->ok) {
        return 0;
    }
    if (*r->p != '\0') {        /* хвост после последнего поля - строка кривая */
        r->ok = 0;
    }
    return r->ok;
}

/* ========================================================================== */
/*  Контрольная сумма целой строки.                                           */
/* ========================================================================== */

/* Проверяет "<payload>*<CRC8>" и ОБРЕЗАЕТ строку до payload (на месте '*'
 * ставится '\0'). Возвращает 1, если CRC сошлась.
 * Строка уже должна быть без '\r' и '\n'. */
RS485_FN int rs485_strip_crc(char *line)
{
    size_t  len;
    size_t  i;
    size_t  sep  = 0U;
    int     seen = 0;
    uint8_t want;
    rs485_rd_t rd;

    if (line == NULL) {
        return 0;
    }
    for (len = 0U; line[len] != '\0'; len++) {
        if (line[len] == RS485_CRC_SEP) {
            sep  = len;
            seen = 1;
        }
    }
    if (!seen || (sep == 0U) || ((len - sep) != 3U)) {
        return 0;                  /* нет '*' либо не ровно две hex-цифры */
    }

    rs485_rd_init(&rd, &line[sep + 1U]);
    want = rs485_rd_hex8(&rd);
    if (!rs485_rd_end(&rd)) {
        return 0;
    }

    i = sep;
    if (rs485_crc8(line, i) != want) {
        return 0;
    }
    line[sep] = '\0';
    return 1;
}

/* ========================================================================== */
/*  Сообщения                                                                 */
/* ========================================================================== */

typedef enum {
    RS485_CMD_NONE   = 0,
    RS485_CMD_SNAP   = 1,   /* S,<seq>        broadcast */
    RS485_CMD_STATUS = 2,   /* T,<id>,<seq>             */
    RS485_CMD_READ   = 3    /* R,<id>,<seq>             */
} rs485_cmd_kind_t;

typedef struct {
    rs485_cmd_kind_t kind;
    uint8_t          id;    /* 0 для SNAP */
    uint32_t         seq;
} rs485_cmd_t;

typedef struct {
    uint8_t  id;
    uint32_t seq;
    int32_t  angle_udeg;    /* микроградусы, -1 = неизвестно */
    int16_t  sector;        /* -1 = неизвестно */
    uint8_t  valid;
    uint8_t  state;
    uint32_t frame_no;
} rs485_data_t;

/* ---- сборка (мастер) ---------------------------------------------------- */

RS485_FN size_t rs485_build_snap(char *buf, size_t cap, uint32_t seq)
{
    rs485_wr_t w;
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "S,");
    rs485_wr_u32(&w, seq);
    return rs485_wr_finish(&w, 0);
}

RS485_FN size_t rs485_build_status_req(char *buf, size_t cap, uint8_t id, uint32_t seq)
{
    rs485_wr_t w;
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "T,");
    rs485_wr_u32(&w, id);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, seq);
    return rs485_wr_finish(&w, 0);
}

RS485_FN size_t rs485_build_read_req(char *buf, size_t cap, uint8_t id, uint32_t seq)
{
    rs485_wr_t w;
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "R,");
    rs485_wr_u32(&w, id);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, seq);
    return rs485_wr_finish(&w, 0);
}

/* ---- сборка (слейв) ----------------------------------------------------- */

RS485_FN size_t rs485_build_ack(char *buf, size_t cap, uint8_t id,
                                uint32_t seq, uint32_t frame_no)
{
    rs485_wr_t w;
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "ACK,");
    rs485_wr_u32(&w, id);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, seq);
    rs485_wr_str(&w, ",frame=");
    rs485_wr_u32(&w, frame_no);
    return rs485_wr_finish(&w, 1);
}

RS485_FN size_t rs485_build_nak(char *buf, size_t cap, uint8_t id,
                                uint32_t seq, const char *reason)
{
    rs485_wr_t w;
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "NAK,");
    rs485_wr_u32(&w, id);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, seq);
    rs485_wr_char(&w, ',');
    rs485_wr_str(&w, reason);
    return rs485_wr_finish(&w, 1);
}

RS485_FN size_t rs485_build_data(char *buf, size_t cap, const rs485_data_t *d)
{
    rs485_wr_t w;

    if (d == NULL) {
        return 0U;
    }
    rs485_wr_init(&w, buf, cap);
    rs485_wr_str(&w, "D,");
    rs485_wr_u32(&w, d->id);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, d->seq);
    rs485_wr_char(&w, ',');
    rs485_wr_i32(&w, d->angle_udeg);
    rs485_wr_char(&w, ',');
    rs485_wr_i32(&w, d->sector);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, d->valid);
    rs485_wr_char(&w, ',');
    rs485_wr_hex8(&w, d->state);
    rs485_wr_char(&w, ',');
    rs485_wr_u32(&w, d->frame_no);
    return rs485_wr_finish(&w, 1);
}

/* ---- разбор (слейв) ----------------------------------------------------- */

/* payload - строка УЖЕ без CRC и терминатора (после rs485_strip_crc). */
RS485_FN int rs485_parse_cmd(const char *payload, rs485_cmd_t *out)
{
    rs485_rd_t rd;
    uint32_t   id;
    uint32_t   seq;

    if ((payload == NULL) || (out == NULL)) {
        return 0;
    }
    out->kind = RS485_CMD_NONE;
    out->id   = 0U;
    out->seq  = 0U;

    rs485_rd_init(&rd, payload);

    if (payload[0] == 'S') {
        rs485_rd_lit(&rd, "S,");
        seq = rs485_rd_u32(&rd);
        if (!rs485_rd_end(&rd)) {
            return 0;
        }
        out->kind = RS485_CMD_SNAP;
        out->seq  = seq;
        return 1;
    }

    if ((payload[0] == 'T') || (payload[0] == 'R')) {
        rs485_rd_lit(&rd, (payload[0] == 'T') ? "T," : "R,");
        id = rs485_rd_u32(&rd);
        rs485_rd_lit(&rd, ",");
        seq = rs485_rd_u32(&rd);
        if (!rs485_rd_end(&rd) || (id > 255U)) {
            return 0;
        }
        out->kind = (payload[0] == 'T') ? RS485_CMD_STATUS : RS485_CMD_READ;
        out->id   = (uint8_t)id;
        out->seq  = seq;
        return 1;
    }

    return 0;
}

/* ---- разбор (мастер) ---------------------------------------------------- */

RS485_FN int rs485_parse_ack(const char *payload, uint8_t *id,
                             uint32_t *seq, uint32_t *frame_no)
{
    rs485_rd_t rd;
    uint32_t   vid;
    uint32_t   vseq;
    uint32_t   vframe;

    if (payload == NULL) {
        return 0;
    }
    rs485_rd_init(&rd, payload);
    rs485_rd_lit(&rd, "ACK,");
    vid = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    vseq = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",frame=");
    vframe = rs485_rd_u32(&rd);
    if (!rs485_rd_end(&rd) || (vid > 255U)) {
        return 0;
    }
    if (id != NULL) {
        *id = (uint8_t)vid;
    }
    if (seq != NULL) {
        *seq = vseq;
    }
    if (frame_no != NULL) {
        *frame_no = vframe;
    }
    return 1;
}

RS485_FN int rs485_parse_nak(const char *payload, uint8_t *id, uint32_t *seq,
                             char *reason, size_t reason_cap)
{
    rs485_rd_t rd;
    uint32_t   vid;
    uint32_t   vseq;
    char       tmp[RS485_REASON_MAX];

    if (payload == NULL) {
        return 0;
    }
    rs485_rd_init(&rd, payload);
    rs485_rd_lit(&rd, "NAK,");
    vid = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    vseq = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    rs485_rd_token(&rd, tmp, sizeof(tmp));
    if (!rs485_rd_end(&rd) || (vid > 255U)) {
        return 0;
    }
    if (id != NULL) {
        *id = (uint8_t)vid;
    }
    if (seq != NULL) {
        *seq = vseq;
    }
    if ((reason != NULL) && (reason_cap > 0U)) {
        size_t i = 0U;
        while ((tmp[i] != '\0') && ((i + 1U) < reason_cap)) {
            reason[i] = tmp[i];
            i++;
        }
        reason[i] = '\0';
    }
    return 1;
}

RS485_FN int rs485_parse_data(const char *payload, rs485_data_t *out)
{
    rs485_rd_t rd;
    rs485_data_t d;
    uint32_t   vid;
    int32_t    vsector;
    uint32_t   vvalid;

    if ((payload == NULL) || (out == NULL)) {
        return 0;
    }
    rs485_rd_init(&rd, payload);
    rs485_rd_lit(&rd, "D,");
    vid = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    d.seq = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    d.angle_udeg = rs485_rd_i32(&rd);
    rs485_rd_lit(&rd, ",");
    vsector = rs485_rd_i32(&rd);
    rs485_rd_lit(&rd, ",");
    vvalid = rs485_rd_u32(&rd);
    rs485_rd_lit(&rd, ",");
    d.state = rs485_rd_hex8(&rd);
    rs485_rd_lit(&rd, ",");
    d.frame_no = rs485_rd_u32(&rd);

    if (!rs485_rd_end(&rd)) {
        return 0;
    }
    if ((vid > 255U) || (vvalid > 1U)) {
        return 0;
    }
    if ((vsector < RS485_SECTOR_UNKNOWN) || (vsector > 32767)) {
        return 0;
    }
    /* угол: либо "неизвестен", либо строго в допустимом диапазоне */
    if ((d.angle_udeg != (int32_t)RS485_ANGLE_UNKNOWN) &&
        ((d.angle_udeg < 0) || (d.angle_udeg > (int32_t)RS485_ANGLE_MAX))) {
        return 0;
    }
    /* valid=1 обязан приходить с настоящим углом и сектором */
    if ((vvalid == 1U) &&
        ((d.angle_udeg == (int32_t)RS485_ANGLE_UNKNOWN) ||
         (vsector == RS485_SECTOR_UNKNOWN))) {
        return 0;
    }

    d.id     = (uint8_t)vid;
    d.sector = (int16_t)vsector;
    d.valid  = (uint8_t)vvalid;
    *out = d;
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* RS485_PROTO_H */
