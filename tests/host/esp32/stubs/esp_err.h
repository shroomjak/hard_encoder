/* Заглушка esp_err.h для хостового теста (ESP-IDF не нужен). */
#ifndef ESP_ERR_STUB_H
#define ESP_ERR_STUB_H
#include <stdlib.h>
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL (-1)
#define ESP_ERR_TIMEOUT 0x107
const char *esp_err_to_name(esp_err_t err);
#define ESP_ERROR_CHECK(x) do { if ((x) != ESP_OK) abort(); } while (0)
#endif
