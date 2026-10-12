/*
 * Прикладные обработчики регистров для FreeModbus slave.
 *
 * Стек из ThirdParty/FreeModbus уже проверил CRC, адрес устройства и формат
 * Modbus RTU. Здесь стандартные функции Modbus переводятся в действия датчика:
 * запись FC06 фиксирует снимок кадра или запускает калибровку, чтение FC04
 * выдаёт сохранённый снимок и живой статус калибровки.
 *
 * FreeModbus передаёт в callback адрес на 1 больше адреса на проводе: запись по
 * адресу 0 приходит сюда как address=1. Сдвиг выполняется только здесь; все
 * константы в заголовках Inc/ — адреса на проводе. Карта: docs/modbus_rtu.md и
 * docs/calibration_modbus.md.
 *
 * Оба обработчика вызываются из eMBPoll() в главном цикле, а не из прерывания,
 * поэтому не мешают обработке кадра АЦП и не конкурируют с конечными автоматами
 * за переменные.
 */
#include "mb.h"
#include "snapshot_registers.h"
#include "modbus_calib.h"

/*
 * FC06: holding[0] = seq — SNAP; holding[1] = код команды калибровки;
 * holding[2] = окно выгрузки таблиц. Других записей нет: ни у holding-регистров
 * калибровки, ни у снимка побочных записей не предусмотрено (FC03 выключен).
 */
eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count, eMBRegisterMode mode)
{
    uint16_t value;

    /* Один регистр и только запись: FC16 и FC03 в сборке выключены,
     * вне MB_H_COUNT записывать нечего. */
    if (mode != MB_REG_WRITE || count != 1 || address < 1 || address > MB_H_COUNT)
        return MB_ENOREG;
    value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);

    if (address == MB_H_SNAP + 1) {
        snapshot_latch(value);
        return MB_ENOERR;
    }
    switch (mb_cal_write((uint16_t)(address - 1), value)) {  /* CAL_CMD, CAL_PAGE */
    case 0: return MB_ENOERR;
    case -1: return MB_ENOREG;
    default: return MB_EIO;   /* нет такого кода команды или таблицы */
    }
}

/*
 * FC04: 0…10 — зафиксированный снимок кадра (snapshot_registers.c),
 * MB_CAL_FIRST…MB_CAL_TOTAL-1 — живой статус калибровки и окно выгрузки таблиц
 * (modbus_calib.c). Снимок обновляется только командой SNAP, блок калибровки —
 * всегда актуален на момент запроса.
 *
 * Допустимы любые диапазоны внутри 0…MB_CAL_TOTAL-1; всё остальное —
 * MB_ENOREG (исключение 0x02). 32-битные значения кладутся двумя регистрами:
 * старшее слово первым.
 */
eMBErrorCode eMBRegInputCB(UCHAR *buf, USHORT address, USHORT count)
{
    if (address < 1 || count == 0 || count > MB_CAL_TOTAL ||
        (uint32_t)address + count > MB_CAL_TOTAL + 1) return MB_ENOREG;
    for (USHORT i = 0; i < count; ++i) {
        uint16_t reg = (uint16_t)(address - 1 + i);
        uint16_t value = (reg < SNAP_REG_COUNT) ? snapshot_register(reg)
                                                : mb_cal_read(reg);
        *buf++ = (UCHAR)(value >> 8);
        *buf++ = (UCHAR)value;
    }
    return MB_ENOERR;
}
