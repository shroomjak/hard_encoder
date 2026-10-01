/*
 * Хостовые тесты кодека RS-485 (Inc/rs485_proto.h).
 *
 * Собираются обычным gcc/g++ на ПК, железо не нужно:
 *     tools/run_tests.sh
 *
 * Зачем: кодек один и тот же на STM32 и на ESP32, поэтому любая ошибка в
 * формате или в разборе ломает сразу обе стороны и ловится только на
 * стенде. Здесь она ловится за секунду.
 */

#include <stdio.h>
#include <string.h>

#include "rs485_proto.h"

static int g_failed = 0;
static int g_checks = 0;

#define CHECK(cond)                                                        \
    do {                                                                   \
        g_checks++;                                                        \
        if (!(cond)) {                                                     \
            g_failed++;                                                    \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);         \
        }                                                                  \
    } while (0)

#define CHECK_STR(got, want)                                               \
    do {                                                                   \
        g_checks++;                                                        \
        if (strcmp((got), (want)) != 0) {                                  \
            g_failed++;                                                    \
            printf("FAIL %s:%d: got \"%s\", want \"%s\"\n",                \
                   __FILE__, __LINE__, (got), (want));                     \
        }                                                                  \
    } while (0)

/* Табличная CRC8 ровно как в прошивке: crc = crc_table[crc ^ byte].
 * Таблица генерируется из того же полинома; совпадение байт в байт с
 * crc_table[] из Src/main.c дополнительно проверяется в run_tests.sh. */
static unsigned char table[256];

static void build_table(void)
{
    int i, b;
    for (i = 0; i < 256; i++) {
        unsigned c = (unsigned)i;
        for (b = 0; b < 8; b++) {
            c = (c & 1U) ? ((c >> 1) ^ 0x8CU) : (c >> 1);
        }
        table[i] = (unsigned char)c;
    }
}

static unsigned char crc8_table_driven(const char *s, size_t len)
{
    unsigned char crc = 0;
    size_t i;
    for (i = 0; i < len; i++) {
        crc = table[crc ^ (unsigned char)s[i]];
    }
    return crc;
}

/* Вспомогалка: убрать терминатор, чтобы отдать строку в strip_crc так же,
 * как это делает приёмник (он уже снял '\r' и '\n'). */
static void chomp(char *s)
{
    size_t n = strlen(s);
    while ((n > 0U) && ((s[n - 1] == '\n') || (s[n - 1] == '\r'))) {
        s[--n] = '\0';
    }
}

static void test_crc(void)
{
    size_t i;
    static const char *samples[] = {
        "", "S,1", "T,1,7", "R,255,4294967295",
        "ACK,2,123456,frame=99", "D,1,7,359999999,143,1,80,4242",
        "123456789"
    };

    /* эталонное контрольное значение CRC-8/MAXIM */
    CHECK(rs485_crc8("123456789", 9) == 0xA1);

    /* побитовая реализация в заголовке == табличная реализация прошивки */
    for (i = 0; i < sizeof(samples) / sizeof(samples[0]); i++) {
        size_t len = strlen(samples[i]);
        CHECK(rs485_crc8(samples[i], len) == crc8_table_driven(samples[i], len));
    }
}

static void test_build_exact(void)
{
    char buf[RS485_LINE_MAX];

    CHECK(rs485_build_snap(buf, sizeof(buf), 7U) == strlen(buf));
    CHECK_STR(buf, "S,7*2E\n");

    CHECK(rs485_build_status_req(buf, sizeof(buf), 1U, 7U) > 0U);
    CHECK_STR(buf, "T,1,7*2C\n");

    CHECK(rs485_build_read_req(buf, sizeof(buf), 1U, 7U) > 0U);
    CHECK_STR(buf, "R,1,7*B0\n");

    CHECK(rs485_build_ack(buf, sizeof(buf), 1U, 7U, 1234U) > 0U);
    CHECK_STR(buf, "ACK,1,7,frame=1234*0A\r\n");
}

static void test_roundtrip_commands(void)
{
    char        buf[RS485_LINE_MAX];
    rs485_cmd_t cmd;

    CHECK(rs485_build_snap(buf, sizeof(buf), 4294967295U) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_cmd(buf, &cmd) == 1);
    CHECK(cmd.kind == RS485_CMD_SNAP);
    CHECK(cmd.seq == 4294967295U);
    CHECK(cmd.id == 0U);

    CHECK(rs485_build_status_req(buf, sizeof(buf), 255U, 0U) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_cmd(buf, &cmd) == 1);
    CHECK(cmd.kind == RS485_CMD_STATUS);
    CHECK(cmd.id == 255U);
    CHECK(cmd.seq == 0U);

    CHECK(rs485_build_read_req(buf, sizeof(buf), 2U, 99U) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_cmd(buf, &cmd) == 1);
    CHECK(cmd.kind == RS485_CMD_READ);
    CHECK(cmd.id == 2U);
    CHECK(cmd.seq == 99U);
}

static void test_roundtrip_responses(void)
{
    char         buf[RS485_LINE_MAX];
    char         reason[RS485_REASON_MAX];
    uint8_t      id    = 0U;
    uint32_t     seq   = 0U;
    uint32_t     frame = 0U;
    rs485_data_t d;
    rs485_data_t got;

    CHECK(rs485_build_ack(buf, sizeof(buf), 3U, 12345U, 777U) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_ack(buf, &id, &seq, &frame) == 1);
    CHECK((id == 3U) && (seq == 12345U) && (frame == 777U));

    CHECK(rs485_build_nak(buf, sizeof(buf), 3U, 12345U, RS485_NAK_STALE) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_nak(buf, &id, &seq, reason, sizeof(reason)) == 1);
    CHECK((id == 3U) && (seq == 12345U));
    CHECK_STR(reason, "STALE");
    CHECK(rs485_parse_ack(buf, &id, &seq, &frame) == 0);   /* NAK != ACK */

    d.id         = 2U;
    d.seq        = 65535U;
    d.angle_udeg = 359999999L;
    d.sector     = 143;
    d.valid      = 1U;
    d.state      = 0x80U;
    d.frame_no   = 4242U;
    CHECK(rs485_build_data(buf, sizeof(buf), &d) > 0U);
    chomp(buf);
    CHECK_STR(buf, "D,2,65535,359999999,143,1,80,4242*CC");
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_data(buf, &got) == 1);
    CHECK(got.id == d.id);
    CHECK(got.seq == d.seq);
    CHECK(got.angle_udeg == d.angle_udeg);
    CHECK(got.sector == d.sector);
    CHECK(got.valid == d.valid);
    CHECK(got.state == d.state);
    CHECK(got.frame_no == d.frame_no);

    /* "угол неизвестен" допустим только вместе с valid=0 */
    d.angle_udeg = (int32_t)RS485_ANGLE_UNKNOWN;
    d.sector     = (int16_t)RS485_SECTOR_UNKNOWN;
    d.valid      = 0U;
    CHECK(rs485_build_data(buf, sizeof(buf), &d) > 0U);
    chomp(buf);
    CHECK(rs485_strip_crc(buf) == 1);
    CHECK(rs485_parse_data(buf, &got) == 1);
    CHECK(got.angle_udeg == (int32_t)RS485_ANGLE_UNKNOWN);
    CHECK(got.sector == (int16_t)RS485_SECTOR_UNKNOWN);
    CHECK(got.valid == 0U);
}

static void test_crc_rejects_corruption(void)
{
    char        buf[RS485_LINE_MAX];
    char        copy[RS485_LINE_MAX];
    rs485_cmd_t  cmd;
    rs485_data_t d;
    size_t       i;
    size_t       n;
    int          accepted_corrupt = 0;

    d.id         = 1U;
    d.seq        = 7U;
    d.angle_udeg = 123456789L;
    d.sector     = 42;
    d.valid      = 1U;
    d.state      = 0x80U;
    d.frame_no   = 10U;
    CHECK(rs485_build_data(buf, sizeof(buf), &d) > 0U);
    chomp(buf);
    n = strlen(buf);

    /* каждая одиночная подмена символа должна быть отвергнута CRC или парсером */
    for (i = 0; i < n; i++) {
        char saved;
        strcpy(copy, buf);
        saved = copy[i];
        copy[i] = (saved == '9') ? '8' : '9';
        if (rs485_strip_crc(copy)) {
            rs485_data_t tmp;
            if (rs485_parse_data(copy, &tmp)) {
                accepted_corrupt++;
                printf("    corrupted line accepted: %s\n", copy);
            }
        }
    }
    CHECK(accepted_corrupt == 0);

    /* строка без CRC не принимается вовсе */
    strcpy(copy, "S,7");
    CHECK(rs485_strip_crc(copy) == 0);

    /* CRC есть, но неверная */
    strcpy(copy, "S,7*FF");
    CHECK(rs485_strip_crc(copy) == 0);

    /* CRC не hex */
    strcpy(copy, "S,7*ZZ");
    CHECK(rs485_strip_crc(copy) == 0);

    /* правильная CRC, но команда битая по структуре */
    strcpy(copy, "S,7*2E");
    CHECK(rs485_strip_crc(copy) == 1);
    CHECK(rs485_parse_cmd(copy, &cmd) == 1);
}

static void test_parser_strictness(void)
{
    rs485_cmd_t  cmd;
    rs485_data_t d;
    char         s[RS485_LINE_MAX];

    CHECK(rs485_parse_cmd("S,", &cmd) == 0);            /* нет числа        */
    CHECK(rs485_parse_cmd("S,7x", &cmd) == 0);          /* мусорный хвост   */
    CHECK(rs485_parse_cmd("S,-1", &cmd) == 0);          /* seq беззнаковый  */
    CHECK(rs485_parse_cmd("S,4294967296", &cmd) == 0);  /* переполнение     */
    CHECK(rs485_parse_cmd("S,4294967295", &cmd) == 1);  /* граница ок       */
    CHECK(rs485_parse_cmd("T,256,1", &cmd) == 0);       /* id > 255         */
    CHECK(rs485_parse_cmd("T,1", &cmd) == 0);           /* не хватает поля  */
    CHECK(rs485_parse_cmd("T,1,2,3", &cmd) == 0);       /* лишнее поле      */
    CHECK(rs485_parse_cmd("X,1,2", &cmd) == 0);         /* чужая команда    */
    CHECK(rs485_parse_cmd("", &cmd) == 0);

    /* DATA: граничные значения угла */
    strcpy(s, "D,1,7,360000000,42,1,80,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* 360.000000 вне диапазона */
    strcpy(s, "D,1,7,359999999,42,1,80,10");
    CHECK(rs485_parse_data(s, &d) == 1);
    strcpy(s, "D,1,7,-2,42,0,80,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* -2 не бывает     */
    strcpy(s, "D,1,7,-1,-1,1,80,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* valid=1 без угла */
    strcpy(s, "D,1,7,-1,-1,0,80,10");
    CHECK(rs485_parse_data(s, &d) == 1);
    strcpy(s, "D,1,7,100,42,2,80,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* valid > 1        */
    strcpy(s, "D,1,7,100,42,1,800,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* state не 2 hex   */
    strcpy(s, "D,1,7,100,42,1,8,10");
    CHECK(rs485_parse_data(s, &d) == 0);                /* state не 2 hex   */
}

static void test_writer_capacity(void)
{
    char         small[8];
    char         exact[RS485_LINE_MAX];
    rs485_data_t worst;

    /* самый длинный технически возможный ответ DATA */
    worst.id         = 255U;
    worst.seq        = 4294967295U;
    worst.angle_udeg = 359999999L;
    worst.sector     = 32767;
    worst.valid      = 1U;
    worst.state      = 0xFFU;
    worst.frame_no   = 4294967295U;

    /* заведомо не влезает - билдер обязан вернуть 0, а не испортить память */
    CHECK(rs485_build_ack(small, sizeof(small), 255U, 4294967295U, 4294967295U) == 0U);
    CHECK(rs485_build_data(small, sizeof(small), &worst) == 0U);

    /* самый длинный реальный ответ обязан влезать в RS485_LINE_MAX */
    CHECK(rs485_build_data(exact, sizeof(exact), &worst) > 0U);
    printf("    longest DATA (%u bytes): %s", (unsigned)strlen(exact), exact);
}

int main(void)
{
    build_table();

    test_crc();
    test_build_exact();
    test_roundtrip_commands();
    test_roundtrip_responses();
    test_crc_rejects_corruption();
    test_parser_strictness();
    test_writer_capacity();

    printf("%s: %d checks, %d failures\n",
           (g_failed == 0) ? "OK" : "FAILED", g_checks, g_failed);
    return (g_failed == 0) ? 0 : 1;
}
