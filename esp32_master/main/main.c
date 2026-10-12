/*
 * ESP32 — ведущий узел (master) многоточечной шины Modbus RTU / RS485.
 * Библиотека: официальный компонент Espressif esp-modbus v2.1.4.
 * Прошивка собирается и загружается ESP-IDF, НЕ IAR: подробная инструкция
 * и схема соединений приведены в docs/modbus_rtu.md и esp32_master/README.md.
 *
 * Два режима, выбираются в menuconfig («Encoder master: режим работы»):
 *   • рабочий  — poll_mode.c: цикл SNAP → STATUS → READ по всем головкам;
 *   • калибровка — calib_mode.c: одна головка, команды 0x14, 0x13, 0x11, 0x12, 0x10.
 * Оба режима используют одну шину (bus.c); в одной прошивке они не сосуществуют.
 */
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bus.h"
#if CONFIG_ENCODER_MODE_CALIB
#include "calib_mode.h"
#else
#include "poll_mode.h"
#endif

/* Точка входа ESP-IDF (аналог main() в обычном C-проекте). */
void app_main(void)
{
    ESP_ERROR_CHECK(bus_init());
#if CONFIG_ENCODER_MODE_CALIB
    calib_mode_run();
    /* После калибровки прошивка ничего не отправляет на линию. */
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
#else
    poll_mode_run();
#endif
}
