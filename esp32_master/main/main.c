#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_console.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "pc_link.h"
#include "rs485_rtu.h"
#include "workflow.h"

#define MB_UART                 2   /* UART_NUM_2 */
#define MB_TX_GPIO              17
#define MB_RX_GPIO              16
#define MB_RTS_GPIO             4
#define MB_BAUD                 RS485_BAUD_RATE

#ifdef CONFIG_ESP_CONSOLE_UART_BAUDRATE
#define PC_UART_BAUD            ((uint32_t)CONFIG_ESP_CONSOLE_UART_BAUDRATE)
#else
#define PC_UART_BAUD            921600U
#endif

#ifdef CONFIG_ESP_CONSOLE_UART_NUM
#define PC_UART_NUM             CONFIG_ESP_CONSOLE_UART_NUM
#else
#define PC_UART_NUM             0
#endif

#ifdef CONFIG_ESP_CONSOLE_UART_TX_GPIO
#define PC_UART_TX_GPIO         CONFIG_ESP_CONSOLE_UART_TX_GPIO
#else
#define PC_UART_TX_GPIO         1
#endif

/* Период аппаратной сетки SNAP по умолчанию: 10 000 мкс (строго 100.000 Гц) для 1..4 головок. */
#define DEFAULT_POLL_PERIOD_US  10000U
#define MIN_POLL_PERIOD_US      2000U
#define MAX_POLL_PERIOD_US      1000000U

#define BUS_TASK_CORE           0
#define BUS_TASK_PRIO           (configMAX_PRIORITIES - 2)
#define BUS_TASK_STACK          6144

#define CONSOLE_INIT_CORE       1
#define CONSOLE_INIT_PRIO       5
#define CONSOLE_INIT_STACK      4096

typedef struct {
    command_t cmd;
    uint32_t  period_us; /* 0 = автоматический выбор (10 мс для <=4 головок) */
} bus_msg_t;

static const char *TAG = "encoder";
static QueueHandle_t commands;
static TaskHandle_t bus_task_handle;
static esp_timer_handle_t poll_timer;
static workflow_t workflow;
static pc_head_sync_t sync_state[ENCODER_MAX_ID + 1U];
static uint32_t attempted, dropped, overruns;
static uint32_t requested_period_us, active_period_us;

static uint32_t choose_poll_period_us(size_t head_count, uint32_t user_period_us)
{
    /* Минимальный физический бюджет шины RS-485 на 230400 8E1:
     * ~685 мкс (SNAP + 300 мкс settle) + ~2300 мкс на каждую головку. */
    const uint32_t min_wire_us = 700U + (uint32_t)head_count * 2350U;
    if (user_period_us != 0U) {
        return (user_period_us < min_wire_us) ? min_wire_us : user_period_us;
    }
    if (min_wire_us <= DEFAULT_POLL_PERIOD_US) {
        return DEFAULT_POLL_PERIOD_US;
    }
    return ((min_wire_us + 999U) / 1000U) * 1000U;
}

static bool workflow_request(uint8_t id, uint8_t function, uint16_t first,
                             uint16_t count, uint16_t *words)
{
    esp_err_t err = ESP_FAIL;
    if (function == 4U) {
        err = rs485_read_input_regs(id, first, count, words, RS485_CMD_TIMEOUT_US);
    } else if (function == 6U && count == 1U && words != NULL) {
        err = rs485_write_single_reg(id, first, words[0], RS485_CMD_TIMEOUT_US);
    }
    if (id != 0U) {
        rs485_interframe_gap(RS485_INTERFRAME_GAP_US);
    }
    return err == ESP_OK;
}

/* Core 0: детерминированный цикл SNAP -> чтение всех головок -> приведение к t_snap. */
static void poll_heads(const uint8_t *ids, size_t count, uint16_t seq)
{
    static pc_head_t heads[ENCODER_MAX_ID];
    uint16_t r[PC_REG_COUNT];
    pc_poll_t p = {
        .seq = seq,
        .error = 0U,
        .count = (uint32_t)count,
    };

    attempted++;
    memset(heads, 0, count * sizeof(heads[0]));
    for (size_t i = 0; i < count; ++i) {
        heads[i].id = ids[i];
        heads[i].angle_udeg = PC_ANGLE_INVALID;
        heads[i].raw_angle_udeg = PC_ANGLE_INVALID;
        heads[i].age_us = PC_AGE_INVALID;
    }

    const uint64_t cycle_start_us = (uint64_t)esp_timer_get_time();
    uint64_t snap_wire_us = cycle_start_us;
    uint32_t snap_total_us = 0U;
    esp_err_t err = rs485_send_snap(seq, &snap_wire_us, &snap_total_us);
    p.t_us = snap_wire_us;
    p.snap_us = snap_total_us;

    if (err != ESP_OK) {
        p.error |= POLL_ERROR_SNAP;
    } else {
        for (size_t i = 0; i < count; ++i) {
            memset(r, 0, sizeof r);
            const uint64_t before_us = (uint64_t)esp_timer_get_time();
            err = rs485_read_input_regs(ids[i], 0U, PC_REG_COUNT, r, RS485_POLL_TIMEOUT_US);
            const uint64_t after_us = (uint64_t)esp_timer_get_time();
            heads[i].read_us = (uint32_t)(after_us - before_us);

            /* Пауза t3.5 нужна между соседними головками; после последней головки
             * роль паузы до следующего SNAP играет остаток периода таймера 10 мс. */
            if (i + 1U < count) {
                rs485_interframe_gap(RS485_INTERFRAME_GAP_US);
                heads[i].gap_us = (uint32_t)((uint64_t)esp_timer_get_time() - after_us);
            } else {
                heads[i].gap_us = 0U;
            }

            if (err != ESP_OK) {
                p.error |= POLL_ERROR_READ;
                continue;
            }

            const uint32_t raw_angle = ((uint32_t)r[2] << 16) | (uint32_t)r[3];
            if (r[0] != seq || r[1] != 1U || r[4] >= 144U || r[5] != 0U ||
                raw_angle >= 360000000U) {
                p.error |= POLL_ERROR_FRAME;
                continue;
            }

            const int32_t age_us = (int32_t)(((uint32_t)r[9] << 16) | (uint32_t)r[10]);
            heads[i].raw_angle_udeg = raw_angle;
            heads[i].age_us = age_us;
            heads[i].angle_udeg = pc_sync_compensate_udeg(
                &sync_state[ids[i]], raw_angle, p.t_us, age_us);
        }
    }

    const uint64_t end_us = (uint64_t)esp_timer_get_time();
    p.poll_us = (uint32_t)(end_us - cycle_start_us);
    if (!pc_link_put(&p, heads)) {
        dropped++;
    }
}

static void workflow_log(const char *text) { ESP_LOGI(TAG, "%s", text); }
static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }

#if defined(CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD) && CONFIG_ESP_TIMER_SUPPORTS_ISR_DISPATCH_METHOD
static void IRAM_ATTR poll_timer_cb(void *arg)
{
    (void)arg;
    BaseType_t hpw = pdFALSE;
    if (bus_task_handle) {
        vTaskNotifyGiveFromISR(bus_task_handle, &hpw);
        if (hpw == pdTRUE) {
            portYIELD_FROM_ISR();
        }
    }
}
#define POLL_TIMER_DISPATCH ESP_TIMER_ISR
#else
static void poll_timer_cb(void *arg)
{
    (void)arg;
    if (bus_task_handle) {
        xTaskNotifyGive(bus_task_handle);
    }
}
#define POLL_TIMER_DISPATCH ESP_TIMER_TASK
#endif

static bool parse_period_arg(const char *s, uint32_t *out_us)
{
    if (!s || !*s) return false;
    char *end = NULL;
    unsigned long v = strtoul(s, &end, 10);
    if (*end != '\0' || v < MIN_POLL_PERIOD_US || v > MAX_POLL_PERIOD_US) {
        return false;
    }
    *out_us = (uint32_t)v;
    return true;
}

static int console_command(int argc, char **argv)
{
    bus_msg_t msg = {0};
    if (!strcmp(argv[0], "scan")) msg.cmd.kind = CMD_SCAN;
    else if (!strcmp(argv[0], "poll")) msg.cmd.kind = CMD_POLL;
    else if (!strcmp(argv[0], "calib")) msg.cmd.kind = CMD_CALIB;
    else if (!strcmp(argv[0], "next")) msg.cmd.kind = CMD_NEXT;
    else msg.cmd.kind = CMD_STOP;

    if (msg.cmd.kind == CMD_POLL) {
        if (argc >= 2 && !parse_head_ids(argv[1], &msg.cmd)) {
            printf("Usage: poll [id,id,...] [period_us %u..%u]\n",
                   MIN_POLL_PERIOD_US, MAX_POLL_PERIOD_US);
            return 1;
        }
        if (argc == 3 && !parse_period_arg(argv[2], &msg.period_us)) {
            printf("Invalid period_us: expected integer %u..%u\n",
                   MIN_POLL_PERIOD_US, MAX_POLL_PERIOD_US);
            return 1;
        }
        if (argc > 3) {
            printf("Usage: poll [id,id,...] [period_us]\n");
            return 1;
        }
    } else if (msg.cmd.kind == CMD_CALIB) {
        if (argc > 2 || (argc == 2 && !parse_head_ids(argv[1], &msg.cmd))) {
            printf("Usage: calib [id,id,...]; decimal IDs 1..247, no duplicates\n");
            return 1;
        }
    } else if (argc > 1) {
        printf("Usage: %s\n", argv[0]);
        return 1;
    }

    if (xQueueSend(commands, &msg, 0) != pdTRUE) {
        puts("Command queue full; try again");
        return 1;
    }
    return 0;
}

static void stop_poll_grid(void)
{
    if (poll_timer) {
        (void)esp_timer_stop(poll_timer);
    }
    (void)ulTaskNotifyTake(pdTRUE, 0);
}

static void start_poll_grid(size_t head_count, uint32_t user_period_us)
{
    active_period_us = choose_poll_period_us(head_count, user_period_us);
    attempted = dropped = overruns = 0U;
    pc_sync_reset_all(sync_state);
    pc_link_begin(active_period_us);
    (void)ulTaskNotifyTake(pdTRUE, 0);
    ESP_LOGI(TAG, "POLL grid started: heads=%u period_us=%" PRIu32 " (%.2f Hz)",
             (unsigned)head_count, active_period_us, 1000000.0 / (double)active_period_us);
    ESP_ERROR_CHECK(esp_timer_start_periodic(poll_timer, active_period_us));
}

static void handle_bus_msg(const bus_msg_t *msg)
{
    const workflow_state_t old = workflow.state;
    if (old == WF_POLL && msg->cmd.kind == CMD_STOP) {
        stop_poll_grid();
        pc_link_end(attempted, dropped, overruns);
    }
    requested_period_us = msg->period_us;
    workflow_command(&workflow, &msg->cmd);
    if (old != WF_POLL && workflow.state == WF_POLL) {
        start_poll_grid(workflow.count, requested_period_us);
    }
}

static void worker(void *arg)
{
    SemaphoreHandle_t boot_done = (SemaphoreHandle_t)arg;
    bus_task_handle = xTaskGetCurrentTaskHandle();

    /* Инициализируем UART2 (RS-485) на Core 0 -> прерывания RS-485 живут строго на Core 0. */
    ESP_ERROR_CHECK(rs485_rtu_init(MB_UART, MB_TX_GPIO, MB_RX_GPIO, MB_RTS_GPIO, MB_BAUD));

    const esp_timer_create_args_t tcfg = {
        .callback = poll_timer_cb,
        .dispatch_method = POLL_TIMER_DISPATCH,
        .name = "snap_grid",
        .skip_unhandled_events = true,
    };
    ESP_ERROR_CHECK(esp_timer_create(&tcfg, &poll_timer));
    xSemaphoreGive(boot_done);

    bus_msg_t msg;
    for (;;) {
        if (workflow.state == WF_POLL) {
            while (xQueueReceive(commands, &msg, 0) == pdTRUE) {
                handle_bus_msg(&msg);
                if (workflow.state != WF_POLL) break;
            }
            if (workflow.state != WF_POLL) {
                continue;
            }
            /* Ждём строго тактированный импульс аппаратного таймера (ESP_TIMER_ISR). */
            const uint32_t ticks = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(50));
            if (ticks == 0U) {
                continue;
            }
            if (ticks > 1U) {
                overruns += (ticks - 1U);
            }
            workflow_tick(&workflow);
        } else if (workflow.state == WF_IDLE || workflow.state == WF_CONFIRM) {
            if (xQueueReceive(commands, &msg, pdMS_TO_TICKS(100)) == pdTRUE) {
                handle_bus_msg(&msg);
            }
        } else {
            /* WF_SCAN или WF_CALIB */
            if (xQueueReceive(commands, &msg, 0) == pdTRUE) {
                handle_bus_msg(&msg);
            }
            if (workflow.state == WF_SCAN || workflow.state == WF_CALIB) {
                workflow_tick(&workflow);
            }
        }
    }
}

static void core1_io_init_task(void *arg)
{
    SemaphoreHandle_t done = (SemaphoreHandle_t)arg;

    /* 1. Дожидаемся полного опустошения сдвигового регистра ROM UART0 (не вызывая
     *    uart_wait_tx_done до установки драйвера!) и подавляем INFO-сообщение
     *    драйвера UART ("ESP_INTR_FLAG_IRAM flag not set..."), которое иначе
     *    печатается ВНУТРИ uart_driver_install на сброшенной скорости 115200 бод
     *    прямо перед вызовом uart_param_config(921600) и сбивает конечный автомат TX. */
    fflush(stdout);
    esp_rom_output_tx_wait_idle((uint8_t)PC_UART_NUM);
    esp_rom_delay_us(2000);
    esp_log_level_set("uart", ESP_LOG_WARN);

    /* 2. Инициализация драйвера UART0 и консоли на Core 1 (изоляция от Core 0). */
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "encoder>";
    repl_config.max_cmdline_length = 1024;
    repl_config.task_core_id = CONSOLE_INIT_CORE;

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    uart_config.channel = PC_UART_NUM;
    uart_config.baud_rate = (int)PC_UART_BAUD;
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    /* 3. Обход бага ESP-IDF v5.x в esp_console_new_repl_uart() (esp_console_repl_chip.c):
     *    для всех скоростей <= 1 000 000 бод функция жёстко выбирает источник тактирования
     *    UART_SCLK_REF_TICK (1 МГц)! На 921600 бод делитель от 1 МГц вырождается в
     *    div_int=1, div_frag=1 (фактическая частота ~1 Мбод, ошибка +8.5% -> сдвиг битов),
     *    а вызов uart_set_baudrate() читает обратно тот же REF_TICK и не меняет источник.
     *    Явный вызов uart_param_config() с source_clk = UART_SCLK_DEFAULT (APB = 80 МГц)
     *    переключает UART0 обратно на шину APB (делитель 86.75 -> 922190 бод, погрешность
     *    всего +0.064%, один в один с чистым выводом загрузчика). */
    esp_rom_output_tx_wait_idle((uint8_t)PC_UART_NUM);
    const uart_config_t apb_uart_cfg = {
        .baud_rate = (int)PC_UART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config((uart_port_t)PC_UART_NUM, &apb_uart_cfg));
    ESP_ERROR_CHECK(gpio_set_drive_capability((gpio_num_t)PC_UART_TX_GPIO, GPIO_DRIVE_CAP_3));
    esp_rom_delay_us(1000);

    pc_link_start();

    const char *names[] = {"scan", "poll", "calib", "next", "stop"};
    const char *help[] = {
        "Scan Modbus IDs 1..247",
        "poll [id,id,...] [period_us]: hardware-timed SNAP/READ (default 10000 us = 100 Hz)",
        "calib [id,id,...]: 4-stage calibration (writes EEPROM)",
        "Confirm calibration step",
        "Stop active mode and print timing report",
    };
    for (unsigned i = 0U; i < sizeof names / sizeof names[0]; ++i) {
        const esp_console_cmd_t cmd = {
            .command = names[i],
            .help = help[i],
            .func = console_command,
        };
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
    }

    uint32_t actual_baud = 0U;
    (void)uart_get_baudrate((uart_port_t)PC_UART_NUM, &actual_baud);
    ESP_LOGI(TAG, "Ready: RS485=%u (Core0 direct RTU), PC_UART=%" PRIu32 " 8N1 APB (Core1), "
             "grid=%u us, tick=%d Hz, timer=%s",
             MB_BAUD, actual_baud, DEFAULT_POLL_PERIOD_US, CONFIG_FREERTOS_HZ,
             (POLL_TIMER_DISPATCH == ESP_TIMER_ISR) ? "ISR" : "TASK");
    fflush(stdout);
    (void)uart_wait_tx_done((uart_port_t)PC_UART_NUM, pdMS_TO_TICKS(50));

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
    xSemaphoreGive(done);
    vTaskDelete(NULL);
}

void app_main(void)
{
    commands = xQueueCreate(8, sizeof(bus_msg_t));
    ESP_ERROR_CHECK(commands ? ESP_OK : ESP_ERR_NO_MEM);
    workflow_init(&workflow, (workflow_io_t){
        .request = workflow_request,
        .poll = poll_heads,
        .log = workflow_log,
        .now_ms = now_ms,
    });

    SemaphoreHandle_t init_sem = xSemaphoreCreateBinary();
    ESP_ERROR_CHECK(init_sem ? ESP_OK : ESP_ERR_NO_MEM);

    /* Сначала поднимаем шину RS-485 и таймер SNAP на Core 0 (пока UART0 ещё работает
     * через чистый ROM-вывод без гонки двух ядер), затем переключаем консоль на Core 1. */
    if (xTaskCreatePinnedToCore(worker, "encoder_bus", BUS_TASK_STACK,
                                init_sem, BUS_TASK_PRIO, NULL, BUS_TASK_CORE) != pdPASS) {
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
    (void)xSemaphoreTake(init_sem, portMAX_DELAY);

    if (xTaskCreatePinnedToCore(core1_io_init_task, "core1_io_init", CONSOLE_INIT_STACK,
                                init_sem, CONSOLE_INIT_PRIO, NULL, CONSOLE_INIT_CORE) != pdPASS) {
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }
    (void)xSemaphoreTake(init_sem, portMAX_DELAY);
    vSemaphoreDelete(init_sem);
}
