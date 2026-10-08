/*
 * Прикладные обработчики регистров для FreeModbus slave.
 *
 * Стек из ThirdParty/FreeModbus уже проверил CRC, адрес устройства и формат
 * Modbus RTU. Здесь переводим стандартные функции Modbus в действия датчика:
 * запись FC06 вызывает SNAP или команду самокалибровки, чтение FC04 выдаёт
 * сохранённый снимок кадра и живой статус калибровок.
 *
 * Важная особенность FreeModbus: адрес в callback = адрес на проводе + 1.
 * Например, регистр 0 в запросе FC06 приходит сюда как address=1. Сдвиг
 * выполняется только здесь; в Inc/modbus_calib.h все адреса — на проводе.
 * Карта полных диапазонов: docs/modbus_rtu.md и docs/calibration_modbus.md.
 *
 * Оба callback вызываются из eMBPoll() в главном цикле, поэтому обработчик
 * команды не прерывает обработку кадра АЦП, а живые переменные читаются без
 * гонок: и здесь, и в автоматах один и тот же контекст.
 */
#include "mb.h"
#include "snapshot_registers.h"
#include "modbus_calib.h"

/*
 * Карта отказов команды калибровки в исключения Modbus — это единственная
 * обратная связь, которую видит оператор, работающий руками:
 *   MB_CAL_ERR_REG          -> MB_ENOREG    -> исключение 0x02 (адрес неверный)
 *   BUSY / EEPROM           -> MB_ETIMEDOUT -> исключение 0x06 SLAVE_BUSY
 *   VALUE / LOCKED / PREREQ -> MB_EIO       -> исключение 0x04 (отказ устройства)
 * Точная причина всегда лежит в CAL_R_LAST_RESULT — удобно, если инструмент
 * показывает только номер исключения.
 */
static eMBErrorCode map_cal_result(mb_cal_result_t r)
{
    switch (r) {
    case MB_CAL_OK: return MB_ENOERR;
    case MB_CAL_ERR_REG: return MB_ENOREG;
    case MB_CAL_ERR_BUSY:
    case MB_CAL_ERR_EEPROM: return MB_ETIMEDOUT;
    default: return MB_EIO;
    }
}

/*
 * eMBRegHoldingCB — разрешённые регистры записи (FC06, по одному за запрос):
 *   holding[0] = seq — SNAP, зафиксировать последний готовый кадр;
 *   holding[1…4] — команда, ключ, режим и окно выгрузки (modbus_calib.c).
 * Два байта buf переданы стеком в порядке Modbus: старший первым.
 * Неверные адрес, число регистров и режим чтения дают MB_ENOREG; других
 * holding-регистров нет, так что случайно дёрнуть калибровку нечем.
 */
eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count, eMBRegisterMode mode)
{
    uint16_t value;
    if (count != 1 || mode != MB_REG_WRITE) return MB_ENOREG;
    value = (uint16_t)((uint16_t)buf[0] << 8 | buf[1]);
    if (address == MB_H_REG_SNAP + 1) {
        snapshot_latch(value);
        return MB_ENOERR;
    }
    if (address > MB_H_REG_SNAP + 1 && address <= MB_H_REG_COUNT)
        return map_cal_result(mb_cal_write((uint16_t)(address - 1), value));
    return MB_ENOREG;
}

/*
 * eMBRegInputCB — чтение входных регистров стандартной FC04.
 *   0…SNAP_REG_COUNT-1 — зафиксированный снимок кадра (snapshot_registers.c);
 *   MB_CAL_INPUT_FIRST… — живой статус калибровок и окно выгрузки таблиц
 *                         (modbus_calib.c; он же обновляет базу приращений DIAG).
 * STATUS запрашивает первые 2 регистра (seq, ready), READ — все 11. Полностью
 * блок калибровки читается одним запросом адресов 11…46 — так атомарнее и для
 * мастера, и для человека в терминале; меньшие диапазоны тоже допустимы, но
 * адреса обязаны целиком укладываться в 0…MB_CAL_INPUT_TOTAL-1.
 *
 * Значения снимка берутся из frozen, а не из текущего результата main.c: после
 * SNAP они не меняются при новых кадрах АЦП. Регистры калибровки, наоборот,
 * всегда живые — SNAP на них не влияет.
 */
eMBErrorCode eMBRegInputCB(UCHAR *buf, USHORT address, USHORT count)
{
    if (address < 1 || count == 0 || count > MB_CAL_INPUT_TOTAL ||
        (uint32_t)address + count > MB_CAL_INPUT_TOTAL + 1) return MB_ENOREG;
    for (USHORT i = 0; i < count; ++i) {
        uint16_t reg = (uint16_t)(address - 1 + i);
        uint16_t value = (reg < SNAP_REG_COUNT) ? snapshot_register(reg)
                                                : mb_cal_read(reg);
        *buf++ = (UCHAR)(value >> 8);
        *buf++ = (UCHAR)value;
    }
    return MB_ENOERR;
}
