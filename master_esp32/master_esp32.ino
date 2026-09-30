// ============================================================================
// ESP32 master для протокола SNAP / STATUS / READ по RS-485.
//
// Сценарий на цикл опроса (N датчиков, ID = SENSOR_IDS[]):
//
//   ESP32 -> всем:      S,<seq>                (SNAP, широковещательно, без ответа)
//   ESP32 -> датчик i:  T,<id_i>,<seq>         (STATUS)
//   датчик i -> ESP32:  ACK,<id_i>,<seq>,frame=<F>   либо   NAK,<id_i>,<seq>
//                       ... для каждого датчика, по очереди ...
//
//   // Только если ВСЕ датчики подтвердили готовность (ACK) для этого seq:
//   ESP32 -> датчик i:  R,<id_i>,<seq>         (READ)
//   датчик i -> ESP32:  D,<id_i>,<seq>,angle_mdeg,sector,valid,state,frame
//
// Если хотя бы один датчик не прислал ACK (таймаут или NAK), фаза READ для
// этого seq целиком пропускается - таково требование сценария ("Только если
// получены оба корректных ACK"). Следующий цикл начнётся с нового SNAP,seq+1.
//
// Протокол текстовый, ASCII, разделитель ',', конец строки '\n'
// (опциональный '\r' перед ним допускается и игнорируется).
// ============================================================================

#include <Arduino.h>
#include <string.h>

constexpr int RS_RX = 16;
constexpr int RS_TX = 17;
constexpr int RS_DE = 4;

// ВАЖНО: должно совпадать с usart.BaudRate в MX_RS485_USART1_Init() на STM32.
constexpr uint32_t RS_BAUD = 115200;

// Таймаут ожидания ОДНОЙ строки ответа (ACK/NAK/DATA) от адресованного датчика.
constexpr uint32_t RESPONSE_TIMEOUT_MS = 30;

// Сколько раз подряд можно переспросить STATUS у одного датчика в рамках
// одного seq, пока он не пришлёт ACK/NAK (например, если SNAP ещё не был
// обработан датчиком в момент первого опроса). Общий бюджет времени на
// один STATUS = STATUS_RETRY_COUNT * (RESPONSE_TIMEOUT_MS + STATUS_RETRY_DELAY_MS).
constexpr uint8_t  STATUS_RETRY_COUNT = 3;
constexpr uint32_t STATUS_RETRY_DELAY_MS = 3;

// Период полного цикла опроса.
constexpr uint32_t CYCLE_PERIOD_MS = 100;

constexpr size_t LINE_CAP = 96;

// Список ID датчиков на шине. Для другого N просто меняйте этот массив -
// весь остальной код уже общий (см. requestStatus()/requestReading() в
// циклах ниже).
constexpr uint8_t SENSOR_IDS[] = {1, 2};
constexpr size_t  SENSOR_COUNT = sizeof(SENSOR_IDS) / sizeof(SENSOR_IDS[0]);

struct Reading {
    uint8_t  id;
    uint32_t seq;
    int32_t  angle_mdeg;
    int      sector;
    unsigned valid;
    unsigned state;
    uint32_t frame_no;
};

uint32_t nextSeq = 0;

// ---------------------------------------------------------------------------
// Низкоуровневый обмен по RS-485 (общий для SNAP/STATUS/READ)
// ---------------------------------------------------------------------------

void clearRx() {
    while (Serial2.available() > 0) {
        Serial2.read();
    }
}

void sendLine(const char *text) {
    digitalWrite(RS_DE, HIGH);
    delayMicroseconds(10); // запас на включение передатчика модуля RS-485
    Serial2.write(reinterpret_cast<const uint8_t *>(text), strlen(text));
    Serial2.flush(true);   // дождаться завершения TX; RX не очищать
    digitalWrite(RS_DE, LOW);
}

// Считывает одну строку до '\n' (символ '\r' пропускается). Возвращает
// false по таймауту или ошибке переполнения буфера.
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

// ---------------------------------------------------------------------------
// SNAP,seq — широковещательная команда "зафиксировать кадр seq".
// Ответа не предполагает (RS-485 полудуплекс: если бы слейвы отвечали
// одновременно, шина бы просто "смешалась").
// ---------------------------------------------------------------------------
void sendSnap(uint32_t seq) {
    char command[32];
    snprintf(command, sizeof(command), "S,%lu\n", static_cast<unsigned long>(seq));

    clearRx();
    sendLine(command);

    // Небольшая пауза - запас на обработку последнего байта команды всеми
    // STM32 на линии (не точная метка момента фиксации, она формируется на
    // самой головке в RS485_OnSnap()).
    delay(2);
}

// ---------------------------------------------------------------------------
// STATUS,id,seq — адресный опрос готовности. true, если пришёл ACK именно
// с этим id и seq; при этом наружу отдаётся frame_no из ответа.
// ---------------------------------------------------------------------------
bool requestStatus(uint8_t id, uint32_t seq, uint32_t &outFrame) {
    char command[32];
    snprintf(command, sizeof(command), "T,%u,%lu\n", id,
              static_cast<unsigned long>(seq));

    for (uint8_t attempt = 0; attempt < STATUS_RETRY_COUNT; attempt++) {
        clearRx();
        sendLine(command);

        char reply[LINE_CAP];
        if (readLine(reply, sizeof(reply), RESPONSE_TIMEOUT_MS)) {
            unsigned respId = 0;
            unsigned long respSeq = 0;
            unsigned long frame = 0;
            char tail = '\0';

            int fields = sscanf(reply, "ACK,%u,%lu,frame=%lu%c",
                                 &respId, &respSeq, &frame, &tail);
            if (fields == 3 && respId == id && respSeq == seq) {
                outFrame = static_cast<uint32_t>(frame);
                return true;
            }

            // Явный NAK или ответ от другого датчика/на другой seq -
            // переспрашивать смысла нет только если это NAK именно нам;
            // в остальных случаях (эхо/помеха) тоже просто повторяем.
            fields = sscanf(reply, "NAK,%u,%lu%c", &respId, &respSeq, &tail);
            if (fields == 2 && respId == id && respSeq == seq) {
                // Датчик явно сказал "снимок с этим seq ещё не готов".
                // Даём ему ещё немного времени и пробуем снова.
            }
        }

        if (attempt + 1 < STATUS_RETRY_COUNT) {
            delay(STATUS_RETRY_DELAY_MS);
        }
    }

    return false;
}

// ---------------------------------------------------------------------------
// READ,id,seq — адресный запрос данных зафиксированного кадра.
// ---------------------------------------------------------------------------
bool parseData(const char *line, uint8_t expectedId,
               uint32_t expectedSeq, Reading &out) {
    unsigned id = 0;
    unsigned long seq = 0;
    long angle = 0;
    int sector = 0;
    unsigned valid = 0;
    unsigned state = 0;
    unsigned long frame = 0;
    char tail = '\0';

    // Конечный %c отвергает лишние поля; формат state — две hex-цифры.
    const int fields = sscanf(line, "D,%u,%lu,%ld,%d,%u,%2x,%lu%c",
                               &id, &seq, &angle, &sector,
                               &valid, &state, &frame, &tail);
    if (fields != 7 || id != expectedId || seq != expectedSeq ||
        id > 255 || valid > 1 || state > 255 ||
        angle < 0 || angle >= 360000) {
        return false;
    }

    out.id = static_cast<uint8_t>(id);
    out.seq = static_cast<uint32_t>(seq);
    out.angle_mdeg = static_cast<int32_t>(angle);
    out.sector = sector;
    out.valid = valid;
    out.state = state;
    out.frame_no = static_cast<uint32_t>(frame);
    return true;
}

bool requestReading(uint8_t id, uint32_t seq, Reading &out) {
    char command[32];
    snprintf(command, sizeof(command), "R,%u,%lu\n",
              id, static_cast<unsigned long>(seq));

    clearRx(); // только перед новым запросом, не после его отправки
    sendLine(command);

    char reply[LINE_CAP];
    if (!readLine(reply, sizeof(reply), RESPONSE_TIMEOUT_MS)) {
        Serial.printf("ID=%u seq=%lu: timeout/line error (READ)\n",
                      id, static_cast<unsigned long>(seq));
        return false;
    }

    if (!parseData(reply, id, seq, out)) {
        Serial.printf("ID=%u seq=%lu: unexpected reply: %s\n",
                      id, static_cast<unsigned long>(seq), reply);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);

    digitalWrite(RS_DE, LOW);
    pinMode(RS_DE, OUTPUT);
    Serial2.begin(RS_BAUD, SERIAL_8N1, RS_RX, RS_TX);

    Serial.println("RS-485 SNAP/STATUS/READ master started");
}

void loop() {
    const uint32_t cycleStart = millis();
    const uint32_t seq = ++nextSeq;

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

    // --- Фаза 3: READ — только если ВСЕ датчики подтвердили готовность ----
    if (allAcked) {
        Reading readings[SENSOR_COUNT];
        bool    readOk[SENSOR_COUNT];

        for (size_t i = 0; i < SENSOR_COUNT; i++) {
            readOk[i] = requestReading(SENSOR_IDS[i], seq, readings[i]);
        }

        Serial.print(" |");
        for (size_t i = 0; i < SENSOR_COUNT; i++) {
            if (readOk[i]) {
                Serial.printf(" ID%u A=%.3f S=%d V=%u ST=%02X F=%lu",
                              readings[i].id,
                              readings[i].angle_mdeg / 1000.0,
                              readings[i].sector, readings[i].valid,
                              readings[i].state,
                              static_cast<unsigned long>(readings[i].frame_no));
            } else {
                Serial.printf(" ID%u READ-ERROR", SENSOR_IDS[i]);
            }
        }
        Serial.println();
    } else {
        Serial.println(" | READ пропущен: не все датчики подтвердили seq");
    }

    const uint32_t elapsed = static_cast<uint32_t>(millis() - cycleStart);
    if (elapsed < CYCLE_PERIOD_MS) {
        delay(CYCLE_PERIOD_MS - elapsed);
    }
}
