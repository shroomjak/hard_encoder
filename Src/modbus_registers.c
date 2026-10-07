/*
 * Прикладные обработчики регистров для FreeModbus slave.
 *
 * Стек из ThirdParty/FreeModbus уже проверил CRC, адрес устройства и
 * формат Modbus RTU. Здесь переводим стандартные функции Modbus в
 * действия датчика: запись FC06 вызывает SNAP, чтение FC04 выдаёт
 * сохранённые данные. Передача сырых строк или собственный протокол
 * поверх UART здесь не реализуются.
 *
 * Важная особенность FreeModbus: адрес в callback = адрес на проводе + 1.
 * Например, регистр 0 в запросе FC06 приходит сюда как address=1.
 * Таблица всех допустимых адресов описана в docs/modbus_rtu.md.
 */
#include "mb.h"
#include "snapshot_registers.h"

/*
 * eMBRegHoldingCB — единственный разрешённый регистр записи:
 *   FC06, holding[0] = seq (16-битный номер снимка).
 * Два байта buf переданы стеком в порядке Modbus: старший первым.
 * Callback вызывается из eMBPoll() в главном цикле, поэтому он не
 * прерывает обработку кадра изображения в main.c. Вещательный адрес 0
 * обрабатывает тот же FreeModbus, но в отличие от адресного запроса
 * на broadcast ответ не передаётся.
 *
 * Неверные адрес, число регистров и режим чтения дают MB_ENOREG;
 * другие holding-регистры нельзя случайно использовать для калибровки.
 */
eMBErrorCode eMBRegHoldingCB(UCHAR *buf, USHORT address, USHORT count, eMBRegisterMode mode)
{
    if (address != 1 || count != 1 || mode != MB_REG_WRITE) return MB_ENOREG;
    snapshot_latch((uint16_t)((uint16_t)buf[0] << 8 | buf[1]));
    return MB_ENOERR;
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
    if (address < 1 || count == 0 || count > SNAP_REG_COUNT ||
        (uint32_t)address + count > SNAP_REG_COUNT + 1) return MB_ENOREG;
    for (USHORT i = 0; i < count; ++i) {
        uint16_t value = snapshot_register(address - 1 + i);
        *buf++ = (UCHAR)(value >> 8);
        *buf++ = (UCHAR)value;
    }
    return MB_ENOERR;
}
