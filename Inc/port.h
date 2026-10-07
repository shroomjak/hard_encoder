/*
 * Platform types and critical-section hooks required by FreeModbus.
 *
 * FreeModbus itself is BSD-3-Clause; see
 * Middlewares/Third_Party/FreeModbus/LICENSE.txt.
 */
#ifndef HARD_ENCODER_FREEMODBUS_PORT_H
#define HARD_ENCODER_FREEMODBUS_PORT_H

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f7xx.h"

#ifdef __cplusplus
#define PR_BEGIN_EXTERN_C extern "C" {
#define PR_END_EXTERN_C   }
#else
#define PR_BEGIN_EXTERN_C
#define PR_END_EXTERN_C
#endif

#define INLINE inline

typedef uint8_t  BOOL;
typedef uint8_t  UCHAR;
typedef char     CHAR;
typedef uint16_t USHORT;
typedef int16_t  SHORT;
typedef uint32_t ULONG;
typedef int32_t  LONG;

#ifndef TRUE
#define TRUE  ((BOOL)1U)
#endif
#ifndef FALSE
#define FALSE ((BOOL)0U)
#endif

/* The FreeModbus core only uses these in short non-blocking sections. */
#define ENTER_CRITICAL_SECTION() __disable_irq()
#define EXIT_CRITICAL_SECTION()  __enable_irq()

#endif /* HARD_ENCODER_FREEMODBUS_PORT_H */
