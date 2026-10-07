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
