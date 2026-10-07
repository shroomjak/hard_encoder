/*
 * Application register map on top of the BSD-licensed FreeModbus slave.
 *
 * A standard Modbus write (FC06, holding register 0) is the SNAP command.
 * A standard input-register read (FC04) is both STATUS/ACK and DATA.  Thus no
 * vendor function code is needed and address 0 remains a true Modbus broadcast.
 */
#include <string.h>
#include "modbus_slave.h"
#include "modbus_port.h"
#include "mb.h"

#define MODBUS_PROTOCOL_VERSION 0x0100U
#define MODBUS_REGISTER_BASE    1U /* FreeModbus callbacks use one-based addresses. */

typedef struct
{
    uint8_t ready;
    uint8_t sensor_error;
    uint16_t codeword;
    int16_t sector;
    uint16_t angle_cdeg;
    uint16_t startpixel_1;
    uint16_t startpixel_2;
    int16_t temperature_cdeg;
    uint32_t frame_number;
    uint16_t raw_pixels[MODBUS_RAW_SAMPLE_WORDS];
} modbus_live_frame_t;

static modbus_live_frame_t modbus_live_frame;
static uint16_t modbus_snapshot[MODBUS_INPUT_REGISTER_COUNT];

static uint16_t ModbusSlave_AngleToCentiDegrees(float angle_deg)
{
    float scaled = angle_deg * 100.0f;

    /* cur_ang_E normally lies in [0, 360).  The clamps keep the wire format
       well-defined during startup or if the upstream algorithm is unstable. */
    if (scaled <= 0.0f) {
        return 0U;
    }
    if (scaled >= 35999.0f) {
        return 35999U;
    }
    return (uint16_t)(scaled + 0.5f);
}

static int16_t ModbusSlave_TemperatureToCentiDegrees(float temperature_deg_c)
{
    float scaled = temperature_deg_c * 100.0f;

    if (scaled > 32767.0f) {
        return 32767;
    }
    if (scaled < -32768.0f) {
        return -32768;
    }
    return (int16_t)((scaled >= 0.0f) ? (scaled + 0.5f) : (scaled - 0.5f));
}

static void ModbusSlave_TakeSnapshot(uint16_t sequence)
{
    uint16_t i;
    uint16_t flags = 0U;

    /* Invalid is observable until the complete copy below is finished. */
    modbus_snapshot[MODBUS_INPUT_STATUS] = 0U;

    if (modbus_live_frame.ready == 0U) {
        /* Do not ACK a SNAP for which there has not yet been a calculated
           encoder frame. The ESP32 will retry STATUS after its timeout. */
        modbus_snapshot[MODBUS_INPUT_SEQUENCE] = sequence;
        return;
    }

    flags = MODBUS_SNAPSHOT_VALID | MODBUS_SNAPSHOT_RAW_PRESENT | MODBUS_SNAPSHOT_LIVE_READY;
    if (modbus_live_frame.sensor_error != 0U) {
        flags |= MODBUS_SNAPSHOT_SENSOR_ERROR;
    }

    modbus_snapshot[MODBUS_INPUT_SEQUENCE] = sequence;
    modbus_snapshot[MODBUS_INPUT_SLAVE_ID] = (uint16_t)MODBUS_SLAVE_ADDRESS;
    modbus_snapshot[MODBUS_INPUT_SECTOR] = (uint16_t)modbus_live_frame.sector;
    modbus_snapshot[MODBUS_INPUT_ANGLE_CDEG] = modbus_live_frame.angle_cdeg;
    modbus_snapshot[MODBUS_INPUT_STARTPIXEL_1] = modbus_live_frame.startpixel_1;
    modbus_snapshot[MODBUS_INPUT_STARTPIXEL_2] = modbus_live_frame.startpixel_2;
    modbus_snapshot[MODBUS_INPUT_FRAME_HI] = (uint16_t)(modbus_live_frame.frame_number >> 16);
    modbus_snapshot[MODBUS_INPUT_FRAME_LO] = (uint16_t)modbus_live_frame.frame_number;
    modbus_snapshot[MODBUS_INPUT_CODEWORD] = modbus_live_frame.codeword;
    modbus_snapshot[MODBUS_INPUT_TEMPERATURE_CDEG] = (uint16_t)modbus_live_frame.temperature_cdeg;
    modbus_snapshot[MODBUS_INPUT_RAW_SAMPLE_COUNT] = MODBUS_RAW_SAMPLE_WORDS;
    modbus_snapshot[MODBUS_INPUT_PROTOCOL_VERSION] = MODBUS_PROTOCOL_VERSION;

    for (i = 0U; i < MODBUS_RAW_SAMPLE_WORDS; ++i) {
        modbus_snapshot[MODBUS_INPUT_RAW_START + i] = modbus_live_frame.raw_pixels[i];
    }

    /* Store VALID last: FC04 sees either the old complete frame or a valid new
       frame, never an ACK for a partially copied record. */
    modbus_snapshot[MODBUS_INPUT_STATUS] = flags;
}

uint8_t ModbusSlave_Init(void)
{
    eMBErrorCode status;

    memset(&modbus_live_frame, 0, sizeof(modbus_live_frame));
    memset(modbus_snapshot, 0, sizeof(modbus_snapshot));

    status = eMBInit(MB_RTU, (UCHAR)MODBUS_SLAVE_ADDRESS, 0U,
                     (ULONG)MODBUS_BAUDRATE, MB_PAR_NONE, 1U);
    if (status != MB_ENOERR) {
        return (uint8_t)status;
    }

    status = eMBEnable();
    return (uint8_t)status;
}

void ModbusSlave_Poll(void)
{
    (void)eMBPoll();
}

void ModbusSlave_Publish(float angle_deg,
                         int16_t sector,
                         uint16_t codeword,
                         uint16_t startpixel_1,
                         uint16_t startpixel_2,
                         uint8_t sensor_error,
                         float temperature_deg_c,
                         const uint16_t raw_pixels[128])
{
    uint16_t i;

    modbus_live_frame.angle_cdeg = ModbusSlave_AngleToCentiDegrees(angle_deg);
    modbus_live_frame.sector = sector;
    modbus_live_frame.codeword = codeword;
    modbus_live_frame.startpixel_1 = startpixel_1;
    modbus_live_frame.startpixel_2 = startpixel_2;
    modbus_live_frame.sensor_error = sensor_error;
    modbus_live_frame.temperature_cdeg = ModbusSlave_TemperatureToCentiDegrees(temperature_deg_c);

    if (raw_pixels != 0) {
        for (i = 0U; i < MODBUS_RAW_SAMPLE_WORDS; ++i) {
            modbus_live_frame.raw_pixels[i] = raw_pixels[i];
        }
    }

    ++modbus_live_frame.frame_number;
    modbus_live_frame.ready = 1U;
}

/* FC04: STATUS and DATA are views of the same frozen record. */
eMBErrorCode eMBRegInputCB(UCHAR *register_buffer, USHORT address, USHORT register_count)
{
    uint16_t index;

    if ((register_buffer == 0)
        || (address < MODBUS_REGISTER_BASE)
        || (((uint32_t)address + (uint32_t)register_count)
            > (MODBUS_REGISTER_BASE + MODBUS_INPUT_REGISTER_COUNT))) {
        return MB_ENOREG;
    }

    index = (uint16_t)(address - MODBUS_REGISTER_BASE);
    while (register_count > 0U) {
        *register_buffer++ = (UCHAR)(modbus_snapshot[index] >> 8);
        *register_buffer++ = (UCHAR)(modbus_snapshot[index] & 0xFFU);
        ++index;
        --register_count;
    }
    return MB_ENOERR;
}

/* FC06 holding register 0: SNAP(seq).  This is deliberately accepted for
   both a broadcast (address 0) and a unicast service/debug request. */
eMBErrorCode eMBRegHoldingCB(UCHAR *register_buffer, USHORT address,
                              USHORT register_count, eMBRegisterMode mode)
{
    uint16_t sequence;

    if ((register_buffer == 0) || (mode != MB_REG_WRITE)
        || (address != (MODBUS_HOLDING_SNAP_SEQUENCE + MODBUS_REGISTER_BASE))
        || (register_count != 1U)) {
        return MB_ENOREG;
    }

    sequence = (uint16_t)((uint16_t)register_buffer[0] << 8);
    sequence |= register_buffer[1];
    ModbusSlave_TakeSnapshot(sequence);
    return MB_ENOERR;
}
