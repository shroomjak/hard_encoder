/*
 * ESP32 — ведущий узел (master) многоточечной шины Modbus RTU / RS485.
 * Библиотека: официальный компонент Espressif esp-modbus v2.1.4.
 * Прошивка собирается и загружается ESP-IDF, НЕ IAR: подробная инструкция
 * и схема соединений приведены в docs/modbus_rtu.md.
 *
 * Режимы poll / calib выбираются через USB-консоль ESP-IDF.
 * Поиск ID 1..247 выполняется только по отдельной команде scan.
 * poll: broadcast SNAP -> STATUS всех выбранных -> READ только после всех ACK.
 * calib: адресные FC06 + LIVE FC04, подтверждения оператора между этапами.
 *
 * UART2 обслуживает RS485; диагностические ESP_LOGx() видны на USB-консоли
 * отладочной платы, а не на проводах A/B. ESP-IDF реализует Modbus RTU,
 * CRC, паузы и тайм-ауты; здесь задаётся только прикладной сценарий.
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "esp_console.h"
#include "esp_timer.h"
#include "freertos/queue.h"
#include "workflow.h"
#include "esp_log.h"
#include "esp_modbus_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ПРИМЕР разводки для классической ESP32, а не проверенная схема изделия.
 * UART TX -> DI, RX <- RO, RTS -> DE и /RE преобразователя RS485;
 * уровни сигнала RTS и возможность совместного управления DE и /RE нужно
 * сверить с установленным преобразователем. USB платы используется для
 * прошивки/лога, UART2 — исключительно для обмена с головками. */
#define MB_UART UART_NUM_2
#define MB_TX_GPIO 17
#define MB_RX_GPIO 16
#define MB_RTS_GPIO 4
#define MB_BAUD 115200

/* У каждого slave свой MB_SLAVE_ADDRESS. Список выбирается в консоли. */
#define REG_COUNT 11       /* полная карта входных регистров 0..10 */
#define STATUS_COUNT 2     /* только seq и ready */
static const char *TAG = "encoder";
static void *master;

/* В версии 2.1.4 библиотека требует хотя бы один дескриптор перед start(),
 * даже если все запросы отправляются напрямую через send_request(). Этот
 * элемент позволяет стартовать контроллер; адрес slave в каждом фактическом
 * запросе задаётся отдельно. Полученные слова обрабатываются вручную. */
static const mb_parameter_descriptor_t descriptor[] = {
    {0, "snapshot", "", 1, MB_PARAM_INPUT, 0, REG_COUNT, 0,
     PARAM_TYPE_U16, 2, {{0}}, PAR_PERMS_READ}
};

/* Общая оболочка стандартного запроса Modbus. first — адрес регистра на
 * проводе, отсчитываемый от 0; count — число 16-битных регистров.
 * Для FC06 *words = записываемое значение, для FC04 буфер заполняется
 * библиотекой в порядке uint16_t машины ESP32. Фрейм/CRC здесь не строим. */
static esp_err_t request(uint8_t id, uint8_t function, uint16_t first,
                         uint16_t count, uint16_t *words)
{
    mb_param_request_t req = {
        .slave_addr = id, .command = function,
        .reg_start = first, .reg_size = count
    };
    esp_err_t err = mbc_master_send_request(master, &req, words);

    /* Запас по времени на шине */
    vTaskDelay(pdMS_TO_TICKS(20));

    return err;
}

/* Диагностика головки, которая получила SNAP, но вернула ready=0.
 * Читаем все поля зафиксированного кадра, чтобы увидеть причину:
 *   sample=0          — ещё не обработан ни один кадр АЦП;
 *   error!=0          — декодирование кадра не удалось;
 *   sector>=144       — недопустимый сектор;
 *   иначе             — угол вне [0,360) или NaN.
 * Угол при ready=0 не выводим: при невалидном кадре он не обновляется. */
static void log_not_ready(uint8_t id, uint16_t seq)
{
    uint16_t r[REG_COUNT] = {0};
    esp_err_t err = request(id, 0x04, 0, REG_COUNT, r);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DIAG id=%u seq=%u: read failed: %s",
                 (unsigned)id, (unsigned)seq, esp_err_to_name(err));
        return;
    }
    if (r[0] != seq || r[1] != 0) {
        ESP_LOGW(TAG, "DIAG id=%u: snapshot changed (expected=%u got=%u ready=%u)",
                 (unsigned)id, (unsigned)seq, (unsigned)r[0], (unsigned)r[1]);
        return;
    }
    uint32_t sample = ((uint32_t)r[7] << 16) | r[8];
    uint32_t time_ms = ((uint32_t)r[9] << 16) | r[10];
    const char *reason = sample == 0 ? "no frame processed yet"
                       : r[5] != 0   ? "decode error (errorflag)"
                       : r[4] >= 144 ? "invalid sector"
                                     : "angle outside [0,360) or NaN";
    ESP_LOGW(TAG, "DIAG id=%u seq=%u sector=%u error=%u encoder_state=0x%02x "
             "sample=%lu tick_ms=%lu -> %s",
             (unsigned)id, (unsigned)seq, (unsigned)r[4], (unsigned)r[5],
             (unsigned)r[6], (unsigned long)sample, (unsigned long)time_ms, reason);
}


/* STATUS адресной головки: FC04 читает регистры 0 и 1. ACK означает
 * одновременно совпадение seq и ready=1; один только успешный ответ Modbus
 * НЕ подтверждает, что эта головка сохранила нужный кадр. При тайм-ауте,
 * старом seq или невалидном кадре возвращаем false, пояснение пишем в лог. */
static bool status_ack(uint8_t id, uint16_t seq)
{
    uint16_t status[STATUS_COUNT] = {0};
    esp_err_t err = request(id, 0x04, 0, STATUS_COUNT, status);
    if (err != ESP_OK || status[0] != seq || status[1] != 1) {
        ESP_LOGW(TAG, "STATUS id=%u seq=%u error=%s got=%u ready=%u",
                 (unsigned)id, (unsigned)seq, esp_err_to_name(err),
                 (unsigned)status[0], (unsigned)status[1]);
        if (err == ESP_OK && status[0] == seq && status[1] != 1)
            log_not_ready(id, seq);
        return false;
    }
    ESP_LOGI(TAG, "ACK id=%u seq=%u", (unsigned)id, (unsigned)seq);
    return true;
}

/* Полный цикл опроса одного номера кадра.
 * Адрес 0 Modbus зарезервирован для вещания: никто не отвечает на SNAP.
 * Espressif esp-modbus после него выдерживает заданную в sdkconfig.defaults
 * задержку обработки (150 мс), прежде чем принимать следующий запрос.
 * Даже при ошибке одной головки спрашиваем STATUS оставшихся для диагностики,
 * но НЕ выдаём READ ни одной из них до полного набора подтверждений. */
static void poll_heads(const uint8_t *head_ids, size_t head_count, uint16_t seq)
{
    uint16_t command = seq;
    esp_err_t err = request(0, 0x06, 0, 1, &command);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SNAP seq=%u: broadcast failed: %s", (unsigned)seq, esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "SNAP seq=%u", (unsigned)seq);
    bool all_ack = true;
    for (unsigned i = 0; i < head_count; ++i) {
        /* При сбое STATUS одна повторная попытка; повтор не делает SNAP. */
        if (!status_ack(head_ids[i], seq) && !status_ack(head_ids[i], seq))
            all_ack = false;
    }
    if (!all_ack) {
        ESP_LOGW(TAG, "seq=%u: not all ACK; skip all READ", (unsigned)seq);
        return;
    }

    /* READ обращается только к уже зафиксированному frozen-кадру.
     * Повторно проверяем seq/ready, поскольку между STATUS и READ могли
     * произойти ошибка обмена или изменение состояния у slave. */
    for (unsigned i = 0; i < head_count; ++i) {
        uint16_t r[REG_COUNT] = {0};
        err = request(head_ids[i], 0x04, 0, REG_COUNT, r);
        if (err != ESP_OK || r[0] != seq || r[1] != 1 || r[4] >= 144) {
            ESP_LOGE(TAG, "READ id=%u seq=%u failed: %s (got=%u ready=%u)",
                     (unsigned)head_ids[i], (unsigned)seq, esp_err_to_name(err),
                     (unsigned)r[0], (unsigned)r[1]);
            continue;
        }
        /* Пары 16-битных регистров передаются старшим словом первым.
         * Угол — uint32 микроградусов, sample и time_ms — uint32.
         * tick_ms каждой головки считается от ЕЁ перезагрузки; он не
         * синхронизирован со временем другой головки или ESP32. */
        uint32_t udeg = ((uint32_t)r[2] << 16) | r[3];
        uint32_t sample = ((uint32_t)r[7] << 16) | r[8];
        uint32_t time_ms = ((uint32_t)r[9] << 16) | r[10];
        ESP_LOGI(TAG, "DATA id=%u seq=%u angle=%u.%06u deg sector=%u "
                 "error=%u encoder_state=0x%02x sample=%lu tick_ms=%lu",
                 (unsigned)head_ids[i], (unsigned)seq, (unsigned)(udeg / 1000000U),
                 (unsigned)(udeg % 1000000U), (unsigned)r[4], (unsigned)r[5],
                 (unsigned)r[6], (unsigned long)sample, (unsigned long)time_ms);
    }
}

/* Консоль не обращается к Modbus: команды передаются единственной задаче
 * через очередь. Поэтому stop/next можно вводить во время обмена. */
static QueueHandle_t commands;
static workflow_t workflow;

/* Адаптер ESP-IDF к переносимому автомату: тот получает только успех/ошибку.
 * Детальные esp_err_t остаются в прямом рабочем poll и его диагностике. */
static bool workflow_request(uint8_t id, uint8_t function, uint16_t first,
                             uint16_t count, uint16_t *words)
{
    return request(id, function, first, count, words) == ESP_OK;
}
static void workflow_log(const char *text) { ESP_LOGI(TAG, "%s", text); }
static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }

/* Общий обработчик зарегистрированных команд REPL. Дополнительный аргумент
 * разрешён только у poll/calib: один CSV-список, а не отдельные аргументы ID.
 * В очередь копируется вся command_t, поэтому локальный буфер не нужен после
 * возврата. При переполнении очереди команда не исполняется и не теряется молча. */
static int console_command(int argc, char **argv)
{
    command_t command = {0};
    if (!strcmp(argv[0], "scan")) command.kind = CMD_SCAN;
    else if (!strcmp(argv[0], "poll")) command.kind = CMD_POLL;
    else if (!strcmp(argv[0], "calib")) command.kind = CMD_CALIB;
    else if (!strcmp(argv[0], "next")) command.kind = CMD_NEXT;
    else command.kind = CMD_STOP;
    bool has_ids = command.kind == CMD_POLL || command.kind == CMD_CALIB;
    if (argc > (has_ids ? 2 : 1) ||
        (argc == 2 && !parse_head_ids(argv[1], &command))) {
        printf("Usage: %s%s; decimal IDs 1..247, no duplicates\n",
               argv[0], has_ids ? " [id,id,...]" : "");
        return 1;
    }
    if (xQueueSend(commands, &command, 0) != pdTRUE) {
        printf("Command queue full; try again\n");
        return 1;
    }
    return 0;
}

/* Единственная задача-владелец Modbus и workflow. Ожидание очереди задаёт
 * паузу до 100 мс между шагами; сами запросы могут занимать больше времени.
 * Перед каждым следующим шагом проверяется команда из консоли. */
static void worker(void *arg)
{
    (void)arg;
    command_t command;
    for (;;) {
        /* Очередь проверяется между запросами поиска / этапами калибровки /
         * полными циклами poll. stop не прерывает Modbus-транзакцию. */
        if (xQueueReceive(commands, &command, pdMS_TO_TICKS(100)) == pdTRUE)
            workflow_command(&workflow, &command);
        workflow_tick(&workflow);
    }
}

/* UART2: 115200/8E1, RTS управляет RS485. USB/UART0: отдельная консоль.
 * seq не сбрасывается при смене режимов, но после reboot снова 42 (см. docs). */
void app_main(void)
{
    mb_communication_info_t comm = {
        .ser_opts.port = MB_UART,
        .ser_opts.mode = MB_RTU,
        .ser_opts.baudrate = MB_BAUD,
        .ser_opts.parity = UART_PARITY_EVEN,
        .ser_opts.uid = 0,
        .ser_opts.response_tout_ms = CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND ,
        .ser_opts.data_bits = UART_DATA_8_BITS,
        .ser_opts.stop_bits = UART_STOP_BITS_1
    };
    ESP_ERROR_CHECK(mbc_master_create_serial(&comm, &master));
    ESP_ERROR_CHECK(uart_set_pin(MB_UART, MB_TX_GPIO, MB_RX_GPIO, MB_RTS_GPIO,
                                 UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(MB_UART, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(mbc_master_set_descriptor(master, descriptor, 1));
    ESP_ERROR_CHECK(mbc_master_start(master));

    /* Сначала запускаем транспорт и задачу, затем консоль. Автомат остаётся
     * в IDLE: до команды оператора никакие головки не опрашиваются. */
    commands = xQueueCreate(4, sizeof(command_t));
    ESP_ERROR_CHECK(commands ? ESP_OK : ESP_ERR_NO_MEM);
    workflow_init(&workflow, (workflow_io_t){workflow_request, poll_heads, workflow_log, now_ms});
    if (xTaskCreate(worker, "encoder_bus", 6144, NULL, 5, NULL) != pdPASS)
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);

    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "encoder>";
    repl_config.max_cmdline_length = 1024; /* CSV всех 247 ID */
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    /* REPL уже регистрирует help. Команды и сообщения оставлены ASCII,
     * чтобы их можно было вводить/читать в терминалах с разными кодировками;
     * комментарии исходников при этом хранятся в UTF-8. */
    const char *names[] = {"scan", "poll", "calib", "next", "stop"};
    const char *help[] = {
        "Scan all IDs 1..247 without writes",
        "poll [id,id,...]: SNAP/STATUS/READ without scan; omitted IDs = cached scan",
        "calib [id,id,...]: no scan; omitted IDs = cached scan (EEPROM writes!)",
        "Confirm the displayed calibration stage / next head; set rotation first",
        "Stop current mode; active calibration receives addressed 0x10"
    };
    for (unsigned i = 0; i < sizeof names / sizeof names[0]; ++i) {
        const esp_console_cmd_t cmd = {
            .command = names[i], .help = help[i], .func = console_command
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
    }
    ESP_LOGI(TAG, "Ready: poll 1,2 or calib 1,2 uses explicit IDs; scan manually to discover.");
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
