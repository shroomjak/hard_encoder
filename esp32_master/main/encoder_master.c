/*
 * ESP-IDF / Espressif ESP-Modbus master for the hard_encoder RS-485 bus.
 *
 * Sequence on every cycle:
 *   1) FC06, unit 0, holding[0] = seq       -> broadcast SNAP (no reply)
 *   2) FC04, unit 1 and unit 2, input[0..]  -> STATUS / ACK
 *   3) only after both ACKs, FC04 reads the frozen DATA records.
 */
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbcontroller.h"

#define RS485_UART_PORT          UART_NUM_2
#define RS485_UART_TX_GPIO       GPIO_NUM_17
#define RS485_UART_RX_GPIO       GPIO_NUM_16
#define RS485_UART_RTS_GPIO      GPIO_NUM_4
#define RS485_BAUDRATE           115200

#define MODBUS_BROADCAST_ADDRESS 0U
#define MODBUS_FC_READ_INPUT     0x04U
#define MODBUS_FC_WRITE_SINGLE   0x06U

#define ENCODER_HEAD_1           1U
#define ENCODER_HEAD_2           2U
#define ENCODER_HEAD_COUNT       2U

/* Must match Inc/modbus_slave.h. All addresses are zero-based Modbus PDU
   addresses, not the human-facing 30001/40001 documentation notation. */
#define INPUT_STATUS             0U
#define INPUT_SEQUENCE           1U
#define INPUT_SECTOR             3U
#define INPUT_ANGLE_CDEG         4U
#define INPUT_FRAME_HI           7U
#define INPUT_FRAME_LO           8U
#define INPUT_CODEWORD           9U
#define INPUT_TEMPERATURE_CDEG   10U
#define INPUT_META_REGS          16U
#define INPUT_RAW_START          INPUT_META_REGS
#define INPUT_RAW_WORDS          128U
#define INPUT_RAW_FIRST_CHUNK    125U /* Modbus FC04 maximum is 125 registers */
#define INPUT_RAW_SECOND_CHUNK   (INPUT_RAW_WORDS - INPUT_RAW_FIRST_CHUNK)

#define SNAPSHOT_VALID           (1U << 0)
#define SNAPSHOT_SENSOR_ERROR    (1U << 1)

#define POLL_PERIOD_MS           250U

static const char *TAG = "ENCODER_MB_MASTER";
static void *master_handle;

/* ESP-Modbus requires a descriptor table before mbc_master_start(), even
   though this application deliberately uses the direct request API. */
static const mb_parameter_descriptor_t startup_descriptor[] = {
    {
        0U, "direct_fc04_requests", "-", ENCODER_HEAD_1,
        MB_PARAM_INPUT, INPUT_STATUS, 1U,
        0U, PARAM_TYPE_U16, PARAM_SIZE_U16,
        { .opt1 = 0, .opt2 = 0, .opt3 = 0 }, PAR_PERMS_READ
    }
};

typedef struct
{
    uint8_t slave_address;
    uint16_t meta[INPUT_META_REGS];
    uint16_t raw[INPUT_RAW_WORDS];
} encoder_snapshot_t;

static esp_err_t mb_read_input(uint8_t slave_address, uint16_t start,
                               uint16_t count, uint16_t *result)
{
    mb_param_request_t request = {
        .slave_addr = slave_address,
        .command = MODBUS_FC_READ_INPUT,
        .reg_start = start,
        .reg_size = count
    };

    return mbc_master_send_request(master_handle, &request, result);
}

static esp_err_t mb_broadcast_snap(uint16_t sequence)
{
    mb_param_request_t request = {
        .slave_addr = MODBUS_BROADCAST_ADDRESS,
        .command = MODBUS_FC_WRITE_SINGLE,
        .reg_start = 0U, /* holding register SNAP_SEQUENCE */
        .reg_size = 1U
    };

    /* ESP-Modbus recognizes unit 0 as a broadcast and waits its configured
       conversion delay instead of waiting for a reply.  A reply to broadcast
       would cause a collision and is prohibited by Modbus RTU. */
    return mbc_master_send_request(master_handle, &request, &sequence);
}

static bool snapshot_is_ack(const uint16_t meta[INPUT_META_REGS], uint16_t sequence)
{
    return ((meta[INPUT_STATUS] & SNAPSHOT_VALID) != 0U)
           && (meta[INPUT_SEQUENCE] == sequence);
}

static esp_err_t mb_read_status(uint8_t slave_address, uint16_t sequence,
                                encoder_snapshot_t *snapshot)
{
    esp_err_t err;

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->slave_address = slave_address;
    err = mb_read_input(slave_address, INPUT_STATUS, INPUT_META_REGS, snapshot->meta);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "STATUS from head %u failed: %s", (unsigned)slave_address, esp_err_to_name(err));
        return err;
    }
    if (!snapshot_is_ack(snapshot->meta, sequence)) {
        ESP_LOGW(TAG, "head %u has no ACK for seq=%u (flags=0x%04x, seq=%u)",
                 (unsigned)slave_address, (unsigned)sequence,
                 (unsigned)snapshot->meta[INPUT_STATUS],
                 (unsigned)snapshot->meta[INPUT_SEQUENCE]);
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

static esp_err_t mb_read_snapshot_data(encoder_snapshot_t *snapshot)
{
    esp_err_t err;

    err = mb_read_input(snapshot->slave_address, INPUT_RAW_START,
                        INPUT_RAW_FIRST_CHUNK, &snapshot->raw[0]);
    if (err != ESP_OK) {
        return err;
    }

    return mb_read_input(snapshot->slave_address,
                         INPUT_RAW_START + INPUT_RAW_FIRST_CHUNK,
                         INPUT_RAW_SECOND_CHUNK,
                         &snapshot->raw[INPUT_RAW_FIRST_CHUNK]);
}

static void log_snapshot(const encoder_snapshot_t *snapshot)
{
    const uint16_t *meta = snapshot->meta;
    uint32_t frame_number = ((uint32_t)meta[INPUT_FRAME_HI] << 16)
                            | (uint32_t)meta[INPUT_FRAME_LO];
    int16_t sector = (int16_t)meta[INPUT_SECTOR];
    int16_t temperature_cdeg = (int16_t)meta[INPUT_TEMPERATURE_CDEG];

    ESP_LOGI(TAG,
             "DATA head=%u seq=%u frame=%" PRIu32 " angle=%.2f deg sector=%d "
             "code=0x%03x temp=%.2f C error=%u raw[0]=%u raw[127]=%u",
             (unsigned)snapshot->slave_address, (unsigned)meta[INPUT_SEQUENCE], frame_number,
             (double)meta[INPUT_ANGLE_CDEG] / 100.0, (int)sector,
             (unsigned)meta[INPUT_CODEWORD], (double)temperature_cdeg / 100.0,
             (unsigned)((meta[INPUT_STATUS] & SNAPSHOT_SENSOR_ERROR) != 0U),
             (unsigned)snapshot->raw[0], (unsigned)snapshot->raw[INPUT_RAW_WORDS - 1U]);
}

static esp_err_t master_init(void)
{
    esp_err_t err;
    mb_communication_info_t communication = {
        .ser_opts.port = RS485_UART_PORT,
        .ser_opts.mode = MB_RTU,
        .ser_opts.baudrate = RS485_BAUDRATE,
        .ser_opts.parity = MB_PARITY_NONE,
        .ser_opts.uid = 0U,
        .ser_opts.response_tout_ms = 250U,
        .ser_opts.data_bits = UART_DATA_8_BITS,
        .ser_opts.stop_bits = UART_STOP_BITS_1
    };

    err = mbc_master_create_serial(&communication, &master_handle);
    if ((err != ESP_OK) || (master_handle == NULL)) {
        return (err == ESP_OK) ? ESP_ERR_INVALID_STATE : err;
    }

    err = uart_set_pin(RS485_UART_PORT, RS485_UART_TX_GPIO, RS485_UART_RX_GPIO,
                       RS485_UART_RTS_GPIO, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        return err;
    }

    /* ESP32 UART hardware drives RTS while transmitting. Connect this GPIO to
       the transceiver's combined /RE+DE direction input. */
    err = uart_set_mode(RS485_UART_PORT, UART_MODE_RS485_HALF_DUPLEX);
    if (err != ESP_OK) {
        return err;
    }

    err = mbc_master_set_descriptor(master_handle, startup_descriptor,
                                    sizeof(startup_descriptor) / sizeof(startup_descriptor[0]));
    if (err != ESP_OK) {
        return err;
    }

    err = mbc_master_start(master_handle);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "ESP-Modbus RTU master ready: UART%d, %d 8N1, TX=%d RX=%d RTS(RE/DE)=%d",
                 RS485_UART_PORT, RS485_BAUDRATE, RS485_UART_TX_GPIO,
                 RS485_UART_RX_GPIO, RS485_UART_RTS_GPIO);
    }
    return err;
}

void app_main(void)
{
    uint16_t sequence = 0U;
    encoder_snapshot_t head[ENCODER_HEAD_COUNT];
    esp_err_t err;
    bool all_ack;

    ESP_ERROR_CHECK(master_init());

    for (;;) {
        ++sequence;
        if (sequence == 0U) {
            ++sequence; /* keep 0 available for future diagnostics */
        }

        err = mb_broadcast_snap(sequence);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "SNAP seq=%u failed: %s", (unsigned)sequence, esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
            continue;
        }

        all_ack = true;
        err = mb_read_status(ENCODER_HEAD_1, sequence, &head[0]);
        if (err != ESP_OK) {
            all_ack = false;
        }
        err = mb_read_status(ENCODER_HEAD_2, sequence, &head[1]);
        if (err != ESP_OK) {
            all_ack = false;
        }

        if (all_ack) {
            err = mb_read_snapshot_data(&head[0]);
            if (err == ESP_OK) {
                log_snapshot(&head[0]);
            } else {
                ESP_LOGW(TAG, "READ DATA from head %u failed: %s", (unsigned)ENCODER_HEAD_1,
                         esp_err_to_name(err));
            }

            err = mb_read_snapshot_data(&head[1]);
            if (err == ESP_OK) {
                log_snapshot(&head[1]);
            } else {
                ESP_LOGW(TAG, "READ DATA from head %u failed: %s", (unsigned)ENCODER_HEAD_2,
                         esp_err_to_name(err));
            }
        } else {
            ESP_LOGW(TAG, "seq=%u is incomplete; DATA phase skipped", (unsigned)sequence);
        }

        vTaskDelay(pdMS_TO_TICKS(POLL_PERIOD_MS));
    }
}
