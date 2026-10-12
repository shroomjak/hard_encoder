/* Заглушка task.h: vTaskDelay двигает фиктивные часы теста. */
#ifndef FREERTOS_TASK_STUB_H
#define FREERTOS_TASK_STUB_H
#include "freertos/FreeRTOS.h"
void vTaskDelay(TickType_t ticks);
TickType_t xTaskGetTickCount(void);
#endif
