/* Заглушка FreeRTOS для хостового теста: время — миллисекунды, тик = 1 мс. */
#ifndef FREERTOS_STUB_H
#define FREERTOS_STUB_H
#include <stdint.h>
typedef uint32_t TickType_t;
#define portTICK_PERIOD_MS 1
#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))
#endif
