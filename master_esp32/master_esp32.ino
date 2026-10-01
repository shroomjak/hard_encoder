// ============================================================================
// ESP32 master для протокола SNAP / STATUS / READ по RS-485, версия 2.
//
// Сценарий на цикл опроса (N датчиков, ID = SENSOR_IDS[]):
//
//   ESP32 -> всем:      S,<seq>*CRC              (SNAP, broadcast, без ответа)
//   ESP32 -> датчик i:  T,<id_i>,<seq>*CRC       (STATUS)
//   датчик i -> ESP32:  ACK,<id_i>,<seq>,frame=<F>*CRC
//                       либо NAK,<id_i>,<seq>,<ПРИЧИНА>*CRC
//                       ... для каждого датчика, по очереди ...
//
//   // Только если ВСЕ датчики подтвердили готовность (ACK) для этого seq:
//   ESP32 -> датчик i:  R,<id_i>,<seq>*CRC       (READ)
//   датчик i -> ESP32:  D,<id_i>,<seq>,angle_udeg,sector,valid,state,frame*CRC
//
// Если хотя бы один датчик не прислал ACK (таймаут или NAK), фаза READ для
// этого seq целиком пропускается: набор показаний за кадр всё равно неполный.
//
// ЧТО ИЗМЕНИЛОСЬ В ВЕРСИИ 2 (см. master_esp32/REPORT_v2.md):
//   * формат строк и их разбор вынесены в общий с прошивкой rs485_proto.h;
//   * каждая строка закрыта CRC-8, битые строки отбрасываются;
//   * угол передаётся в МИКРОградусах (6 знаков после запятой);
//   * DE/RE управляет сама периферия UART (режим RS-485 half duplex),
//     а не digitalWrite() вокруг flush();
//   * NAK от датчика разбирается с причиной и не ждёт полного таймаута;
//   * нумерация seq стартует со случайного значения - датчик не может
//     принять за свежий снимок остаток предыдущей сессии мастера.
// ============================================================================

#include <Arduino.h>
#include <string.h>

#include "driver/uart.h"
#include "rs485_proto.h"

// --- железо -----------------------------------------------------------------
// ВНИМАНИЕ: на модулях ESP32-WROVER (с PSRAM) GPIO16/17 заняты под SPI RAM -
// там UART2 нужно перевесить на другие пины, иначе шина работать не будет.
constexpr int RS_RX = 16;
constexpr int RS_TX = 17;
constexpr int RS_DE = 4;

constexpr uart_port_t RS_UART = UART_NUM_2;   // Serial2

// ВАЖНО: должно совпадать с usart.BaudRate в MX_RS485_USART1_Init() на STM32.
constexpr uint32_t RS_BAUD = 115200;

// Аппаратный полудуплекс RS-485: DE/RE дёргает сам контроллер UART по линии
// RTS, строго по границам кадра. Выключите (0), только если ваша версия
// arduino-esp32/IDF не поддерживает uart_set_mode() - тогда включится
// программный путь с uart_wait_tx_done() (тоже корректный, но медленнее).
#ifndef RS485_HW_DE
#define RS485_HW_DE 1
#endif

// --- тайминги ---------------------------------------------------------------
// Таймаут берётся из общего заголовка: он связан с guard time слейва.
// Слейв перестаёт отвечать раньше, чем мастер перестаёт слушать, поэтому
// запоздавший ответ физически не может наложиться на следующую команду.
constexpr uint32_t RESPONSE_TIMEOUT_MS = RS485_MASTER_TIMEOUT_MS;
static_assert(RS485_RESPONSE_GUARD_MS < RS485_MASTER_TIMEOUT_MS,
              "guard time слейва должен быть меньше таймаута мастера");

// Сколько раз подряд можно переспросить STATUS у одного датчика в рамках
// одного seq (например, если SNAP ещё не был обработан в момент опроса).
constexpr uint8_t  STATUS_RETRY_COUNT = 3;
constexpr uint32_t STATUS_RETRY_DELAY_MS = 3;

// Повтор READ при ПОТЕРЕ ответа (таймаут/мусор), но не при NAK: NAK означает,
// что данных действительно нет, и повторять бессмысленно.
// Зачем вообще повтор: на головке есть окна с глобально запрещёнными
// прерываниями (обработчик DMA2_Stream3 ждёт кадр TSL1401 с __disable_irq()),
// и команда, пришедшая внутрь такого окна, теряется целиком. Повтор дешевле,
// чем потерянный цикл измерений: снимок под этот seq у датчика ещё жив
// (RS485_FROZEN_TTL_MS), поэтому запрос идемпотентен.
constexpr uint8_t  READ_RETRY_COUNT = 2;

// Пауза после широковещательного SNAP: запас на дообработку последнего байта
// всеми STM32 на линии. Сам момент "замри" формируется на головке в
// прерывании USART1, поэтому пауза влияет только на порядок команд.
constexpr uint32_t SNAP_SETTLE_MS = 2;

// Период полного цикла опроса.
constexpr uint32_t CYCLE_PERIOD_MS = 100;

// Печать диагностики раз в N циклов.
constexpr uint32_t STATS_EVERY_CYCLES = 50;

constexpr size_t LINE_CAP = RS485_LINE_MAX;

// Список ID датчиков на шине. Для другого N просто меняйте этот массив -
// весь остальной код написан циклами по нему.
constexpr uint8_t SENSOR_IDS[] = {1, 2};
constexpr size_t  SENSOR_COUNT = sizeof(SENSOR_IDS) / sizeof(SENSOR_IDS[0]);

uint32_t nextSeq = 0;
uint32_t cycleCount = 0;

struct Stats {
    uint32_t cycles;
    uint32_t cyclesComplete;   // циклы, где все датчики дали и ACK, и данные
    uint32_t timeouts;
    uint32_t crcErrors;
    uint32_t naks;
    uint32_t badReplies;
};

Stats stats = {0, 0, 0, 0, 0, 0};

// ---------------------------------------------------------------------------
// Низкоуровневый обмен по RS-485
// ---------------------------------------------------------------------------

void clearRx() {
    while (Serial2.available() > 0) {
        Serial2.read();
    }
}

void sendLine(const char *text) {
    const size_t len = strlen(text);

#if RS485_HW_DE
    // DE поднимает и опускает сама периферия: гонки "снял DE раньше, чем
    // ушёл последний бит" не существует в принципе.
    Serial2.write(reinterpret_cast<const uint8_t *>(text), len);
#else
    digitalWrite(RS_DE, HIGH);
    delayMicroseconds(10);                 // запас на включение драйвера
    Serial2.write(reinterpret_cast<const uint8_t *>(text), len);
    // uart_wait_tx_done() ждёт опустошения И сдвигового регистра, в отличие
    // от Serial2.flush(), на котором легко обрезать последний символ.
    uart_wait_tx_done(RS_UART, pdMS_TO_TICKS(50));
    digitalWrite(RS_DE, LOW);
#endif
}

// Считывает одну строку до '\n' ('\r' пропускается).
// false - таймаут или строка длиннее буфера.
bool readLine(char *dst, size_t cap, uint32_t timeoutMs) {
    size_t len = 0;
    const uint32_t start = millis();

    while (static_cast<uint32_t>(millis() - start) < timeoutMs) {
        while (Serial2.available() > 0) {
            const int ch = Serial2.read();
            if (ch < 0) {
                break;
            }
            if (ch == '\r') {
                continue;
            }
            if (ch == '\n') {
                dst[len] = '\0';
                return len != 0;
            }
            if (len + 1 >= cap) {
                return false;
            }
            dst[len++] = static_cast<char>(ch);
        }
        delay(1);
    }
    return false;
}

// Читает строки до истечения общего таймаута и возвращает первую, прошедшую
// проверку CRC (уже без "*XX"). Строки с битой CRC просто считаются и
// отбрасываются: на шине это помеха, а не ответ.
bool readChecked(char *dst, size_t cap, uint32_t timeoutMs) {
    const uint32_t start = millis();

    for (;;) {
        const uint32_t elapsed = static_cast<uint32_t>(millis() - start);
        if (elapsed >= timeoutMs) {
            return false;
        }
        if (!readLine(dst, cap, timeoutMs - elapsed)) {
            return false;
        }
        if (rs485_strip_crc(dst)) {
            return true;
        }
        stats.crcErrors++;
        Serial.printf("  CRC mismatch: %s\n", dst);
    }
}

// ---------------------------------------------------------------------------
// SNAP,seq - широковещательная команда "зафиксировать кадр seq".
// Ответа не предполагает: RS-485 полудуплексный, N одновременных ответов
// превратились бы в кашу на линии.
// ---------------------------------------------------------------------------
void sendSnap(uint32_t seq) {
    char command[LINE_CAP];

    if (rs485_build_snap(command, sizeof(command), seq) == 0) {
        return;
    }
    clearRx();
    sendLine(command);
    delay(SNAP_SETTLE_MS);
}

// ---------------------------------------------------------------------------
// STATUS,id,seq - адресный опрос готовности.
// ---------------------------------------------------------------------------
bool requestStatus(uint8_t id, uint32_t seq, uint32_t &outFrame) {
    char command[LINE_CAP];

    if (rs485_build_status_req(command, sizeof(command), id, seq) == 0) {
        return false;
    }

    for (uint8_t attempt = 0; attempt < STATUS_RETRY_COUNT; attempt++) {
        clearRx();
        sendLine(command);

        char reply[LINE_CAP];
        if (readChecked(reply, sizeof(reply), RESPONSE_TIMEOUT_MS)) {
            uint8_t  respId = 0;
            uint32_t respSeq = 0;
            uint32_t frame = 0;

            if (rs485_parse_ack(reply, &respId, &respSeq, &frame) &&
                (respId == id) && (respSeq == seq)) {
                outFrame = frame;
                return true;
            }

            char reason[RS485_REASON_MAX];
            if (rs485_parse_nak(reply, &respId, &respSeq, reason, sizeof(reason)) &&
                (respId == id) && (respSeq == seq)) {
                // Датчик явно сказал, почему снимка нет. Это быстрее таймаута:
                // даём ему немного времени и переспрашиваем.
                stats.naks++;
                Serial.printf("  ID%u NAK(%s) seq=%lu\n", id, reason,
                              static_cast<unsigned long>(seq));
            } else {
                stats.badReplies++;
                Serial.printf("  ID%u unexpected reply: %s\n", id, reply);
            }
        } else {
            stats.timeouts++;
        }

        if (attempt + 1 < STATUS_RETRY_COUNT) {
            delay(STATUS_RETRY_DELAY_MS);
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// READ,id,seq - адресный запрос зафиксированного кадра.
// ---------------------------------------------------------------------------
bool requestReading(uint8_t id, uint32_t seq, rs485_data_t &out) {
    char command[LINE_CAP];

    if (rs485_build_read_req(command, sizeof(command), id, seq) == 0) {
        return false;
    }

    for (uint8_t attempt = 0; attempt < READ_RETRY_COUNT; attempt++) {
        clearRx();             // только перед запросом, не после его отправки
        sendLine(command);

        char reply[LINE_CAP];
        if (!readChecked(reply, sizeof(reply), RESPONSE_TIMEOUT_MS)) {
            stats.timeouts++;
            Serial.printf("  ID%u seq=%lu: timeout (READ, попытка %u)\n", id,
                          static_cast<unsigned long>(seq),
                          static_cast<unsigned>(attempt + 1));
            continue;
        }

        if (rs485_parse_data(reply, &out) && (out.id == id) && (out.seq == seq)) {
            return true;
        }

        // Явный отказ вместо данных: повторять бессмысленно, выходим сразу
        // и не ждём таймаут (п.9 - раньше датчик на это просто молчал).
        uint8_t  nakId = 0;
        uint32_t nakSeq = 0;
        char     reason[RS485_REASON_MAX];
        if (rs485_parse_nak(reply, &nakId, &nakSeq, reason, sizeof(reason)) &&
            (nakId == id) && (nakSeq == seq)) {
            stats.naks++;
            Serial.printf("  ID%u seq=%lu: NAK(%s) на READ\n", id,
                          static_cast<unsigned long>(seq), reason);
            return false;
        }

        stats.badReplies++;
        Serial.printf("  ID%u seq=%lu: unexpected reply: %s\n", id,
                      static_cast<unsigned long>(seq), reply);
    }

    return false;
}

// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);

    Serial2.begin(RS_BAUD, SERIAL_8N1, RS_RX, RS_TX);

#if RS485_HW_DE
    // RTS = DE: периферия сама поднимает линию перед кадром и снимает после
    // ухода последнего бита.
    uart_set_pin(RS_UART, RS_TX, RS_RX, RS_DE, UART_PIN_NO_CHANGE);
    uart_set_mode(RS_UART, UART_MODE_RS485_HALF_DUPLEX);
#else
    digitalWrite(RS_DE, LOW);
    pinMode(RS_DE, OUTPUT);
#endif

    // Старт нумерации со случайного значения: если мастер перезагрузится,
    // датчик не сможет перепутать новый seq с остатком прошлой сессии.
    nextSeq = esp_random() & 0x7FFFFFFFU;

    Serial.println();
    Serial.printf("RS-485 SNAP/STATUS/READ master v%u, %lu бод, DE=%s\n",
                  RS485_PROTO_VERSION, static_cast<unsigned long>(RS_BAUD),
                  RS485_HW_DE ? "аппаратный (RTS)" : "программный");
    Serial.printf("датчиков: %u, стартовый seq=%lu, таймаут ответа %lu мс\n",
                  static_cast<unsigned>(SENSOR_COUNT),
                  static_cast<unsigned long>(nextSeq),
                  static_cast<unsigned long>(RESPONSE_TIMEOUT_MS));
}

void loop() {
    const uint32_t cycleStart = millis();
    const uint32_t seq = ++nextSeq;

    stats.cycles++;
    cycleCount++;

    // --- Фаза 1: SNAP всем -----------------------------------------------
    sendSnap(seq);

    // --- Фаза 2: STATUS каждому датчику по очереди ------------------------
    bool     ackOk[SENSOR_COUNT];
    uint32_t ackFrame[SENSOR_COUNT];
    bool     allAcked = true;

    for (size_t i = 0; i < SENSOR_COUNT; i++) {
        ackOk[i] = requestStatus(SENSOR_IDS[i], seq, ackFrame[i]);
        if (!ackOk[i]) {
            allAcked = false;
        }
    }

    Serial.printf("seq=%lu:", static_cast<unsigned long>(seq));
    for (size_t i = 0; i < SENSOR_COUNT; i++) {
        if (ackOk[i]) {
            Serial.printf(" ID%u:ACK(frame=%lu)", SENSOR_IDS[i],
                          static_cast<unsigned long>(ackFrame[i]));
        } else {
            Serial.printf(" ID%u:NO-ACK", SENSOR_IDS[i]);
        }
    }

    // --- Фаза 3: READ - только если ВСЕ датчики подтвердили готовность ----
    if (allAcked) {
        rs485_data_t readings[SENSOR_COUNT];
        bool         readOk[SENSOR_COUNT];
        bool         allRead = true;

        for (size_t i = 0; i < SENSOR_COUNT; i++) {
            readOk[i] = requestReading(SENSOR_IDS[i], seq, readings[i]);
            if (!readOk[i]) {
                allRead = false;
            }
        }

        Serial.print(" |");
        for (size_t i = 0; i < SENSOR_COUNT; i++) {
            if (!readOk[i]) {
                Serial.printf(" ID%u READ-ERROR", SENSOR_IDS[i]);
                continue;
            }
            // Угол печатаем с шестью знаками после запятой: на линии он
            // приходит целым числом микроградусов.
            Serial.printf(" ID%u A=%.6f S=%d V=%u ST=%02X F=%lu",
                          readings[i].id,
                          static_cast<double>(readings[i].angle_udeg) /
                              static_cast<double>(RS485_ANGLE_SCALE),
                          readings[i].sector, readings[i].valid,
                          readings[i].state,
                          static_cast<unsigned long>(readings[i].frame_no));
        }
        Serial.println();

        if (allRead) {
            stats.cyclesComplete++;
        }
    } else {
        Serial.println(" | READ пропущен: не все датчики подтвердили seq");
    }

    if ((cycleCount % STATS_EVERY_CYCLES) == 0) {
        Serial.printf("[стат] циклов %lu, полных %lu, таймаутов %lu, "
                      "CRC-ошибок %lu, NAK %lu, мусорных ответов %lu\n",
                      static_cast<unsigned long>(stats.cycles),
                      static_cast<unsigned long>(stats.cyclesComplete),
                      static_cast<unsigned long>(stats.timeouts),
                      static_cast<unsigned long>(stats.crcErrors),
                      static_cast<unsigned long>(stats.naks),
                      static_cast<unsigned long>(stats.badReplies));
    }

    const uint32_t elapsed = static_cast<uint32_t>(millis() - cycleStart);
    if (elapsed < CYCLE_PERIOD_MS) {
        delay(CYCLE_PERIOD_MS - elapsed);
    }
}
