/* Заглушка esp_log.h: форматы проверяются компилятором, вывода нет. */
#ifndef ESP_LOG_STUB_H
#define ESP_LOG_STUB_H
#include <stdio.h>
#define ESP_LOGE(tag, fmt, ...) do { if (0) { (void)(tag); printf(fmt, ##__VA_ARGS__); } } while (0)
#define ESP_LOGW(tag, fmt, ...) do { if (0) { (void)(tag); printf(fmt, ##__VA_ARGS__); } } while (0)
#define ESP_LOGI(tag, fmt, ...) do { if (0) { (void)(tag); printf(fmt, ##__VA_ARGS__); } } while (0)
#endif
