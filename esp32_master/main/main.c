#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "esp_console.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_modbus_master.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "workflow.h"
#include "pc_link.h"

#define MB_UART UART_NUM_2
#define MB_TX_GPIO 17
#define MB_RX_GPIO 16
#define MB_RTS_GPIO 4
#define MB_BAUD 230400U
#define PC_UART_BAUD 115200U
#define MB_INTERFRAME_GAP_US 1800U

static const char *TAG = "encoder";
static void *master;
static esp_timer_handle_t gap_timer;
static TaskHandle_t gap_waiter;
static QueueHandle_t commands;
static workflow_t workflow;
static uint32_t attempted, dropped;

static const mb_parameter_descriptor_t descriptor[] = {
    {0, "snapshot", "", 1, MB_PARAM_INPUT, 0, PC_REG_COUNT, 0,
     PARAM_TYPE_U16, 2, {{0}}, PAR_PERMS_READ}
};

static void gap_expired(void *arg) { (void)arg; xTaskNotifyGive(gap_waiter); }
static void bus_gap_wait(void)
{
    gap_waiter = xTaskGetCurrentTaskHandle();
    ESP_ERROR_CHECK(esp_timer_start_once(gap_timer, MB_INTERFRAME_GAP_US));
    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
}

static esp_err_t request_raw(uint8_t id, uint8_t function, uint16_t first,
                             uint16_t count, uint16_t *words)
{
    mb_param_request_t req = {
        .slave_addr = id, .command = function, .reg_start = first, .reg_size = count
    };
    return mbc_master_send_request(master, &req, words);
}

static bool workflow_request(uint8_t id, uint8_t function, uint16_t first,
                              uint16_t count, uint16_t *words)
{
    esp_err_t err = request_raw(id, function, first, count, words);
    if (id != 0) bus_gap_wait();
    return err == ESP_OK;
}

/* Core0 records durations; all statistical arithmetic runs on Core1. */
static void poll_heads(const uint8_t *ids, size_t count, uint16_t seq)
{
    static pc_head_t heads[ENCODER_MAX_ID];
    uint16_t r[PC_REG_COUNT];
    pc_poll_t p = {.seq = seq, .count = (uint32_t)count};
    attempted++;
    memset(heads, 0, count * sizeof heads[0]);
    for (size_t i = 0; i < count; ++i) {
        heads[i].id = ids[i];
        heads[i].angle_udeg = PC_ANGLE_INVALID;
    }
    uint16_t command = seq;
    p.t_us = (uint64_t)esp_timer_get_time();
    esp_err_t err = request_raw(0, 6, 0, 1, &command);
    p.snap_us = (uint64_t)esp_timer_get_time() - p.t_us;
    if (err != ESP_OK) p.error |= POLL_ERROR_SNAP;
    else {
        for (size_t i = 0; i < count; ++i) {
            memset(r, 0, sizeof r);
            uint64_t before = (uint64_t)esp_timer_get_time();
            err = request_raw(ids[i], 4, 0, PC_REG_COUNT, r);
            uint64_t after = (uint64_t)esp_timer_get_time();
            heads[i].read_us = after - before;
            bus_gap_wait();
            heads[i].gap_us = (uint64_t)esp_timer_get_time() - after;
            if (err != ESP_OK) { p.error |= POLL_ERROR_READ; continue; }
            uint32_t angle = ((uint32_t)r[2] << 16) | r[3];
            if (r[0] != seq || r[1] != 1U || r[4] >= 144U || r[5] != 0U ||
                angle >= 360000000U) {
                p.error |= POLL_ERROR_FRAME;
                continue;
            }
            heads[i].angle_udeg = angle;
        }
    }
    p.poll_us = (uint64_t)esp_timer_get_time() - p.t_us;
    if (!pc_link_put(&p, heads)) dropped++;
}

static void workflow_log(const char *text) { ESP_LOGI(TAG, "%s", text); }
static uint32_t now_ms(void) { return (uint32_t)(esp_timer_get_time() / 1000); }

static int console_command(int argc, char **argv)
{
    command_t c = {0};
    if (!strcmp(argv[0], "scan")) c.kind = CMD_SCAN;
    else if (!strcmp(argv[0], "poll")) c.kind = CMD_POLL;
    else if (!strcmp(argv[0], "calib")) c.kind = CMD_CALIB;
    else if (!strcmp(argv[0], "next")) c.kind = CMD_NEXT;
    else c.kind = CMD_STOP;
    bool has_ids = c.kind == CMD_POLL || c.kind == CMD_CALIB;
    if (argc > (has_ids ? 2 : 1) ||
        (argc == 2 && !parse_head_ids(argv[1], &c))) {
        printf("Usage: %s%s; decimal IDs 1..247, no duplicates\n", argv[0], has_ids ? " [id,id,...]" : "");
        return 1;
    }
    if (xQueueSend(commands, &c, 0) != pdTRUE) {
        puts("Command queue full; try again");
        return 1;
    }
    return 0;
}

static void worker(void *arg)
{
    (void)arg;
    command_t c;
    for (;;) {
        TickType_t wait = workflow.state == WF_POLL ? 0 : pdMS_TO_TICKS(100);
        if (xQueueReceive(commands, &c, wait) == pdTRUE) {
            workflow_state_t old = workflow.state;
            if (old == WF_POLL && c.kind == CMD_STOP) pc_link_end(attempted, dropped);
            workflow_command(&workflow, &c);
            if (old != WF_POLL && workflow.state == WF_POLL) {
                attempted = dropped = 0;
                pc_link_begin();
            }
        }
        workflow_tick(&workflow);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "MB baud=%u response=%d convert=%d cooldown=%d tick=%d",
             MB_BAUD, CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND,
             CONFIG_FMB_MASTER_DELAY_MS_CONVERT,
             CONFIG_FMB_MASTER_TIMEOUT_COOLDOWN_MS, CONFIG_FREERTOS_HZ);
    mb_communication_info_t comm = {
        .ser_opts.port = MB_UART, .ser_opts.mode = MB_RTU,
        .ser_opts.baudrate = MB_BAUD, .ser_opts.parity = UART_PARITY_EVEN,
        .ser_opts.uid = 0, .ser_opts.response_tout_ms = CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND,
        .ser_opts.data_bits = UART_DATA_8_BITS, .ser_opts.stop_bits = UART_STOP_BITS_1
    };
    ESP_ERROR_CHECK(mbc_master_create_serial(&comm, &master));
    ESP_ERROR_CHECK(uart_set_pin(MB_UART, MB_TX_GPIO, MB_RX_GPIO, MB_RTS_GPIO, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_mode(MB_UART, UART_MODE_RS485_HALF_DUPLEX));
    ESP_ERROR_CHECK(mbc_master_set_descriptor(master, descriptor, 1));
    ESP_ERROR_CHECK(mbc_master_start(master));
    commands = xQueueCreate(4, sizeof(command_t));
    ESP_ERROR_CHECK(commands ? ESP_OK : ESP_ERR_NO_MEM);
    const esp_timer_create_args_t gap = {
        .callback = gap_expired, .dispatch_method = ESP_TIMER_TASK, .name = "encoder_gap"
    };
    ESP_ERROR_CHECK(esp_timer_create(&gap, &gap_timer));
    workflow_init(&workflow, (workflow_io_t){workflow_request, poll_heads, workflow_log, now_ms});
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "encoder>";
    repl_config.max_cmdline_length = 1024;
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    uart_config.baud_rate = PC_UART_BAUD;
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    pc_link_start();
    if (xTaskCreatePinnedToCore(worker, "encoder_bus", 6144, NULL, 5, NULL, 0) != pdPASS)
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    const char *names[] = {"scan", "poll", "calib", "next", "stop"};
    const char *help[] = {
        "Scan Modbus IDs 1..247", "poll [id,id,...]: SNAP/READ",
        "calib [id,id,...]: calibration writes EEPROM", "Confirm calibration step",
        "Stop polling and print timing statistics"
    };
    for (unsigned i = 0; i < sizeof names / sizeof names[0]; ++i) {
        const esp_console_cmd_t cmd = {.command = names[i], .help = help[i], .func = console_command};
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
    }
    ESP_LOGI(TAG, "PC UART=%u; poll 2,7; stop", PC_UART_BAUD);
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
