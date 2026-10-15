/*
 * Прикладные обработчики регистров для FreeModbus slave.
 *
 * Стек из ThirdParty/FreeModbus уже проверил CRC, адрес устройства и
 * формат Modbus RTU. Здесь переводим стандартные функции Modbus в
 * действия датчика: FC06 holding[0] вызывает SNAP, holding[1] управляет
 * калибровкой; FC04 выдаёт frozen-данные либо отдельный LIVE-диапазон.
 * Передача сырых строк или собственный протокол поверх UART здесь не реализуются.
 *
 * Важная особенность FreeModbus: адрес в callback = адрес на проводе + 1.
 * Например, регистр 0 в запросе FC06 приходит сюда как address=1.
 * Таблица всех допустимых адресов описана в docs/modbus_rtu.md.
 */
#include "mb.h"
#include "snapshot_registers.h"
#include "encoder_protocol.h"
#include "encoder_calibration.h"

/* LIVE читает состояние тех же автоматов, которые получают команды ниже. */
extern unsigned char encoder_state;
extern uint8_t i2c3_tx_wp;

/* Callback address = адрес на линии + 1. Broadcast CAL запрещён в mb.c
 * до вызова callback; broadcast SNAP остаётся разрешённым. */
eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count, eMBRegisterMode mode)
{
    uint16_t value;
    if (count != 1 || mode != MB_REG_WRITE) return MB_ENOREG;
    value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);
    if (address == ENCODER_SNAP + 1) {
        snapshot_latch(value);
        return MB_ENOERR;
    }
    if (address == ENCODER_CAL_COMMAND + 1) {
        /* Транспорт только переводит результат прикладной команды в Modbus.
         * Все переключения флагов/фаз собраны в encoder_calibration.c. */
        switch (encoder_calibration_command(value)) {
        case ENCODER_CAL_OK: return MB_ENOERR;
        case ENCODER_CAL_BUSY: return MB_ETIMEDOUT; /* exception 06 */
        case ENCODER_CAL_INVALID: return MB_EINVAL; /* exception 03 */
        }
        return MB_EINVAL;
    }
    return MB_ENOREG;
}

/*
 * eMBRegInputCB — чтение стандартной FC04: два независимых диапазона.
 * LIVE 0x100..0x102 читает текущее состояние автоматов без SNAP.
 * Ниже описан диапазон снимка 0..10:
 * STATUS запрашивает первые 2 регистра (seq, ready), READ — все 11.
 * Допустимы и меньшие диапазоны, но их адреса обязаны полностью
 * укладываться в 0..SNAP_REG_COUNT-1. Сначала проверяем диапазон с
 * расширенной арифметикой uint32_t, затем складываем каждый uint16_t
 * в ответ в сетевом порядке байтов: старший, младший.
 *
 * Значения снимка берутся из frozen в snapshot_registers.c, а не из текущего
 * результата main.c: после SNAP они не меняются при новых кадрах АЦП.
 */
eMBErrorCode eMBRegInputCB(UCHAR *buf, USHORT address, USHORT count)
{
    if (address >= ENCODER_CAL_STATE + 1 && count > 0 &&
        (uint32_t)address + count <= ENCODER_CAL_STATE + 1 + ENCODER_CAL_REG_COUNT) {
        /* Текущее состояние, доступно даже без ADC/валидного SNAP. */
        const uint16_t live[] = {encoder_state, i2c3_tx_wp ? 1 : 0, ENCODER_CAL_MAGIC};
        for (USHORT i = 0; i < count; ++i) {
            uint16_t value = live[address - ENCODER_CAL_STATE - 1 + i];
            *buf++ = (UCHAR)(value >> 8);
            *buf++ = (UCHAR)value;
        }
        return MB_ENOERR;
    }
    if (address < 1 || count == 0 || count > SNAP_REG_COUNT ||
        (uint32_t)address + count > SNAP_REG_COUNT + 1) return MB_ENOREG;
    for (USHORT i = 0; i < count; ++i) {
        uint16_t value = snapshot_register(address - 1 + i);
        *buf++ = (UCHAR)(value >> 8);
        *buf++ = (UCHAR)value;
    }
    return MB_ENOERR;
}
