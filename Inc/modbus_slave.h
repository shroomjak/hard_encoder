/* Modbus RTU application interface for the STM32 encoder head. */
#ifndef HARD_ENCODER_MODBUS_SLAVE_H
#define HARD_ENCODER_MODBUS_SLAVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FC04 input-register map, expressed as zero-based addresses on the wire. */
enum
{
    MODBUS_INPUT_STATUS          = 0U,
    MODBUS_INPUT_SEQUENCE        = 1U,
    MODBUS_INPUT_SLAVE_ID        = 2U,
    MODBUS_INPUT_SECTOR          = 3U,
    MODBUS_INPUT_ANGLE_CDEG      = 4U,
    MODBUS_INPUT_STARTPIXEL_1    = 5U,
    MODBUS_INPUT_STARTPIXEL_2    = 6U,
    MODBUS_INPUT_FRAME_HI        = 7U,
    MODBUS_INPUT_FRAME_LO        = 8U,
    MODBUS_INPUT_CODEWORD        = 9U,
    MODBUS_INPUT_TEMPERATURE_CDEG = 10U,
    MODBUS_INPUT_RAW_SAMPLE_COUNT = 11U,
    MODBUS_INPUT_PROTOCOL_VERSION = 12U,
    MODBUS_INPUT_META_COUNT      = 16U,
    MODBUS_INPUT_RAW_START       = MODBUS_INPUT_META_COUNT,
    MODBUS_RAW_SAMPLE_WORDS      = 128U,
    MODBUS_INPUT_REGISTER_COUNT  = MODBUS_INPUT_RAW_START + MODBUS_RAW_SAMPLE_WORDS
};

/* Bits in MODBUS_INPUT_STATUS. */
#define MODBUS_SNAPSHOT_VALID       (1U << 0)
#define MODBUS_SNAPSHOT_SENSOR_ERROR (1U << 1)
#define MODBUS_SNAPSHOT_RAW_PRESENT (1U << 2)
#define MODBUS_SNAPSHOT_LIVE_READY  (1U << 3)

/* FC06 holding-register command map, zero-based addresses on the wire. */
enum
{
    MODBUS_HOLDING_SNAP_SEQUENCE = 0U
};

/*
 * Starts FreeModbus in RTU slave mode. The address and UART/DE pins are in
 * Src/modbus_port.c and intentionally kept in one place for board adaptation.
 * Returns 0 on success; a non-zero value is a FreeModbus initialization error.
 */
uint8_t ModbusSlave_Init(void);

/* Must be called often from the foreground loop; it does not busy-wait. */
void ModbusSlave_Poll(void);

/*
 * Publish one fully calculated sensor frame. A following broadcast SNAP copies
 * this complete record atomically into the externally visible snapshot.
 */
void ModbusSlave_Publish(float angle_deg,
                         int16_t sector,
                         uint16_t codeword,
                         uint16_t startpixel_1,
                         uint16_t startpixel_2,
                         uint8_t sensor_error,
                         float temperature_deg_c,
                         const uint16_t raw_pixels[128]);

#ifdef __cplusplus
}
#endif

#endif /* HARD_ENCODER_MODBUS_SLAVE_H */
