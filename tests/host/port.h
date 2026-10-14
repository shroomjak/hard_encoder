/*
 * Минимальный аппаратно-независимый port.h для хостовых тестов (UTF-8).
 * Это не отдельный тест и не порт для прошивки: предоставляет типы FreeModbus
 * и пустые критические секции для однопоточного тестового процесса.
 * В run_*_test.sh путь -Itests/host идёт раньше -IInc, чтобы не включить
 * настоящий STM32 port.h с зависимостью от аппаратных заголовков.
 */
#ifndef HOST_PORT_H
#define HOST_PORT_H
#include <stdint.h>
#include <assert.h>
#define INLINE inline
#define PR_BEGIN_EXTERN_C extern "C" {
#define PR_END_EXTERN_C }
#define ENTER_CRITICAL_SECTION()
#define EXIT_CRITICAL_SECTION()
typedef uint8_t BOOL, UCHAR;
typedef char CHAR;
typedef uint16_t USHORT;
typedef int16_t SHORT;
typedef uint32_t ULONG;
typedef int32_t LONG;
#define TRUE 1
#define FALSE 0
#endif
