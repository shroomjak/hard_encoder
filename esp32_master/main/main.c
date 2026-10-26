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
#define MB_BAUD 230400U

/* У каждого slave свой MB_SLAVE_ADDRESS. Список выбирается в консоли. */
#define REG_COUNT 9       /* полная карта входных регистров 0..8 */
static const char *TAG = "encoder";
static void *master;


#define MB_INTERFRAME_GAP_US 1800U

static esp_timer_handle_t bus_gap_timer;
static TaskHandle_t bus_gap_waiter;

static void bus_gap_expired(void *arg)
{
    (void)arg;
    xTaskNotifyGive(bus_gap_waiter);
}

static void bus_gap_wait(void)
{
    bus_gap_waiter = xTaskGetCurrentTaskHandle();

    ESP_ERROR_CHECK(
        esp_timer_start_once(bus_gap_timer, MB_INTERFRAME_GAP_US)
    );

    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

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
        .slave_addr = id,
        .command = function,
        .reg_start = first,
        .reg_size = count
    };

    esp_err_t err = mbc_master_send_request(master, &req, words);

    /*
     * Для broadcast задержку обработки выдерживает сам ESP-Modbus.
     * Для успешного адресного запроса сохраняем межкадровый интервал.
     * Обработку тайм-аутов и восстановления транспорта оставляем стеку.
     */
    if (id != 0)
        bus_gap_wait();

    return err;
}

/* Полный цикл опроса одного номера кадра.
 * Адрес 0 Modbus зарезервирован для вещания: никто не отвечает на SNAP.
 * Espressif esp-modbus после него выдерживает заданную в sdkconfig.defaults
 * задержку обработки (150 мс), прежде чем принимать следующий запрос.
 * Даже при ошибке одной головки спрашиваем STATUS оставшихся для диагностики,
 * но НЕ выдаём READ ни одной из них до полного набора подтверждений. */
static void poll_heads(const uint8_t *head_ids,
                       size_t head_count, uint16_t seq)
{
    /*
     * Рабочий буфер принадлежит только worker.
     * Потребителю передаём данные лишь после проверки всей группы.
     */
    static uint16_t frame[ENCODER_MAX_ID][REG_COUNT];

    static uint32_t cycles;
    static uint32_t complete_cycles;
    static uint32_t transport_errors;
    static uint32_t invalid_frames;
    static int64_t last_report_us;

    const int64_t started_us = esp_timer_get_time();
    bool complete = true;
    uint16_t command = seq;

    if (head_count == 0 || head_count > ENCODER_MAX_ID)
        return;

    ++cycles;

    esp_err_t err = request(0, 0x06, 0, 1, &command);

    if (err != ESP_OK) {
        ++transport_errors;
        complete = false;
    } else {
        /*
         * Дополнительной прикладной задержки здесь нет:
         * conversion delay после SNAP уже выдерживает ESP-Modbus.
         */
        for (size_t i = 0; i < head_count; ++i) {
            uint16_t *r = frame[i];

            memset(r, 0, REG_COUNT * sizeof r[0]);

            err = request(head_ids[i], 0x04, 0, REG_COUNT, r);

            if (err != ESP_OK) {
                ++transport_errors;
                complete = false;
                continue;
            }

            const uint32_t angle_udeg =
                ((uint32_t)r[2] << 16) | r[3];

            if (r[0] != seq ||
                r[1] != 1 ||
                r[4] >= 144 ||
                r[5] != 0 ||
                angle_udeg >= 360000000U) {
                ++invalid_frames;
                complete = false;
            }
        }
    }

    if (complete) {
        ++complete_cycles;

        /*
        * Заголовок + до ENCODER_MAX_ID элементов.
        * На один элемент с запасом выделено 32 байта:
        * " id=247 ang=359.999999"
        *
        * Буфер статический: не занимает стек worker.
        * Используется только задачей worker.
        */
        static char line[32U + 32U * ENCODER_MAX_ID];

        int written = snprintf(line, sizeof line,
                            "DATA seq=%u", (unsigned)seq);

        if (written < 0 || (size_t)written >= sizeof line) {
            ESP_LOGE(TAG, "DATA header formatting failed");
            return;
        }

        size_t used = (size_t)written;

        for (size_t i = 0; i < head_count; ++i) {
            const uint16_t *r = frame[i];

            const uint32_t angle_udeg =
                ((uint32_t)r[2] << 16) | r[3];

            const size_t available = sizeof line - used;

            written = snprintf(
                line + used,
                available,
                " id=%u ang=%u.%06u",
                (unsigned)head_ids[i],
                (unsigned)(angle_udeg / 1000000U),
                (unsigned)(angle_udeg % 1000000U)
            );

            if (written < 0 || (size_t)written >= available) {
                /*
                * Не выводим обрезанную строку как полноценный набор.
                */
                ESP_LOGE(TAG, "DATA line overflow: heads=%u",
                        (unsigned)head_count);
                return;
            }

            used += (size_t)written;
        }

        ESP_LOGI(TAG, "%s", line);
    }

    /*
     * Не печатаем каждую транзакцию.
     * Это диагностический вывод примерно раз в секунду.
     */
    const int64_t finished_us = esp_timer_get_time();

    if (finished_us - last_report_us >= 1000000) {
        last_report_us = finished_us;

        ESP_LOGI(TAG,
                "POLL cycles=%lu complete=%lu transport_err=%lu "
                "invalid=%lu last_cycle_us=%lld",
                (unsigned long)cycles,
                (unsigned long)complete_cycles,
                (unsigned long)transport_errors,
                (unsigned long)invalid_frames,
                (long long)(finished_us - started_us));
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
        TickType_t wait_ticks =
            workflow.state == WF_POLL ? 0 : pdMS_TO_TICKS(100);

        if (xQueueReceive(commands, &command, wait_ticks) == pdTRUE)
             workflow_command(&workflow, &command);

        workflow_tick(&workflow);
    }
}

/* UART2: 115200/8E1, RTS управляет RS485. USB/UART0: отдельная консоль.
 * seq не сбрасывается при смене режимов, но после reboot снова 42 (см. docs). */
void app_main(void)
{
    ESP_LOGI(TAG,
         "MB config: baud=%d response=%d ms convert=%d ms "
         "cooldown=%d ms tick=%d Hz",
         MB_BAUD,
         CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND,
         CONFIG_FMB_MASTER_DELAY_MS_CONVERT,
         CONFIG_FMB_MASTER_TIMEOUT_COOLDOWN_MS,
         CONFIG_FREERTOS_HZ);

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

    const esp_timer_create_args_t gap_timer_config = {
        .callback = bus_gap_expired,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "encoder_gap"
    };

    ESP_ERROR_CHECK(
        esp_timer_create(&gap_timer_config, &bus_gap_timer)
    );

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
        "poll [id,id,...]: SNAP/READ without scan; omitted IDs = cached scan",
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
