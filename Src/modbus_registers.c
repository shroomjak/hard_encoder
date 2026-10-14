/*
 * Прикладные обработчики регистров для FreeModbus slave.
 *
 * Стек из ThirdParty/FreeModbus уже проверил CRC, адрес устройства и
 * формат Modbus RTU. Здесь переводим стандартные функции Modbus в
 * действия датчика: FC06 holding[0] вызывает SNAP, holding[1] управляет
 * калибровкой; FC04 выдаёт frozen-данные либо отдельный LIVE-диапазон. Передача сырых строк или собственный протокол
 * поверх UART здесь не реализуются.
 *
 * Важная особенность FreeModbus: адрес в callback = адрес на проводе + 1.
 * Например, регистр 0 в запросе FC06 приходит сюда как address=1.
 * Таблица всех допустимых адресов описана в docs/modbus_rtu.md.
 */
#include "mb.h"
#include "snapshot_registers.h"
#include "encoder_protocol.h"
#include <string.h>

/* Мост к существующим автоматам main.c. Все обращения идут из eMBPoll()
 * в том же суперцикле, что и алгоритмы. Базовый main.c не изменяется.
 * Типы должны совпадать с определениями в main.c; host-тест даёт заглушки. */
extern int start_calibrate, auto_cal, start_offset_cal, offset_phase;
extern int start_angk_cal, anglek_phase, offset_cur, offset_start, offset_end;
extern int avg_minmax_num, old_sector;
extern float offset_minmax[128], buf_x3[128];
extern unsigned char encoder_state;
extern uint8_t backlight_width_en, i2c3_tx_wp;

static eMBErrorCode calibration_command(uint16_t command)
{
    if (command == 0x10) {
        /* STOP останавливает ВСЕ автоматы, в том числе buf_k.
         * Не отменяет начатую I2C-запись и не откатывает коэффициенты в RAM. */
        start_calibrate = auto_cal = start_offset_cal = start_angk_cal = 0;
        offset_phase = anglek_phase = 101;
        backlight_width_en = 1;
        encoder_state = 0;
        return MB_ENOERR;
    }
    if (command < 0x11 || command > 0x14) return MB_EINVAL;
    if (start_calibrate || auto_cal || start_offset_cal || start_angk_cal ||
        !i2c3_tx_wp) return MB_ETIMEDOUT; /* exception: slave busy */
    /* Второй проход допустим только после первого, без промежуточного STOP. */
    if (command == 0x12 && encoder_state != 0x20) return MB_EINVAL;
    switch (command) {
    case 0x11:
    case 0x12:
        old_sector = -1;
        start_calibrate = command == 0x11 ? 1 : 2;
        encoder_state = command == 0x11 ? 0x10 : 0x30;
        break;
    case 0x13:
        offset_start = 63;
        offset_end = 65;
        offset_cur = offset_start;
        offset_phase = avg_minmax_num = 0;
        memset(offset_minmax, 0, sizeof offset_minmax);
        encoder_state = 0x50;
        start_offset_cal = 1;
        break;
    case 0x14:
        anglek_phase = avg_minmax_num = 0;
        memset(buf_x3, 0, sizeof buf_x3);
        backlight_width_en = 0;
        encoder_state = 0x70;
        start_angk_cal = 1;
        break;
    }
    return MB_ENOERR;
}

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
    if (address == ENCODER_CAL_COMMAND + 1) return calibration_command(value);
    return MB_ENOREG;
}

/*
 * eMBRegInputCB — чтение зафиксированного снимка стандартной FC04.
 * STATUS запрашивает первые 2 регистра (seq, ready), READ — все 11.
 * Допустимы и меньшие диапазоны, но их адреса обязаны полностью
 * укладываться в 0..SNAP_REG_COUNT-1. Сначала проверяем диапазон с
 * расширенной арифметикой uint32_t, затем складываем каждый uint16_t
 * в ответ в сетевом порядке байтов: старший, младший.
 *
 * Значения берутся из frozen в snapshot_registers.c, а не из текущего
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
