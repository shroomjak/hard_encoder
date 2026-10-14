#ifndef ENCODER_WORKFLOW_H
#define ENCODER_WORKFLOW_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Включаем только общий контракт, НЕ добавляем STM32 Inc в include path
 * ESP-IDF: там лежит собственный port.h, конфликтующий с esp-modbus. */
#include "../../Inc/encoder_protocol.h"

typedef enum { CMD_SCAN, CMD_POLL, CMD_CALIB, CMD_NEXT, CMD_STOP } command_kind_t;
typedef struct {
    command_kind_t kind;
    uint8_t ids[ENCODER_MAX_ID];
    size_t count; /* 0 = все обнаруженные */
} command_t;
/* Строгий CSV десятичных ID; порядок сохраняется, дубликаты запрещены. */
bool parse_head_ids(const char *text, command_t *command);

typedef struct {
    bool (*request)(uint8_t id, uint8_t function, uint16_t first,
                    uint16_t count, uint16_t *words);
    void (*poll)(const uint8_t *ids, size_t count, uint16_t seq);
    void (*log)(const char *message);
    uint32_t (*now_ms)(void);
} workflow_io_t;
typedef enum { WF_IDLE, WF_SCAN, WF_POLL, WF_CONFIRM, WF_CALIB } workflow_state_t;
typedef struct {
    workflow_io_t io;
    workflow_state_t state;
    command_t pending;
    uint8_t found[ENCODER_MAX_ID], ids[ENCODER_MAX_ID];
    size_t found_count, count, head, step;
    unsigned scan_id, failures;
    uint16_t seq, last_state;
    uint32_t started;
} workflow_t;
void workflow_init(workflow_t *w, workflow_io_t io);
void workflow_command(workflow_t *w, const command_t *command);
/* Один шаг за вызов: один адрес поиска / один цикл poll / одно чтение calib.
 * Только одна задача владеет workflow и Modbus — параллельных запросов нет. */
void workflow_tick(workflow_t *w);
#endif
