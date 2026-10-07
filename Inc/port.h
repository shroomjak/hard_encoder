/*
 * Типы аппаратной адаптации FreeModbus (реализация в Src/modbus_port.c).
 * Библиотека подключает этот заголовок как port.h; имена типов и макросов
 * заданы её API, поэтому менять их на русские нельзя.
 */
#ifndef ENCODER_MB_PORT_H
#define ENCODER_MB_PORT_H
#include <stdint.h>
#include <assert.h>
#include "stm32f7xx.h"
#define INLINE inline
#define PR_BEGIN_EXTERN_C extern "C" {
#define PR_END_EXTERN_C }
typedef uint8_t BOOL;
typedef uint8_t UCHAR;
typedef char CHAR;
typedef uint16_t USHORT;
typedef int16_t SHORT;
typedef uint32_t ULONG;
typedef int32_t LONG;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
/* Сохраняем PRIMASK до запрета IRQ и восстанавливаем исходное значение;
 * это важно и для вызовов из обычного кода, и для вызовов из обработчика. */
#define ENTER_CRITICAL_SECTION() uint32_t mb_saved_primask = __get_PRIMASK(); __disable_irq()
#define EXIT_CRITICAL_SECTION() __set_PRIMASK(mb_saved_primask)
#endif
