/*
 * Интерфейс автомата режимов ESP32 (UTF-8).
 * Модуль не привязан к ESP-IDF: обмен, журнал и часы поступают через callbacks.
 * Доступ к одному workflow_t строго последовательный, из задачи-владельца шины.
 * Консоль передаёт команды этой задаче через очередь, а не меняет поля напрямую.
 */
#ifndef ENCODER_WORKFLOW_H
#define ENCODER_WORKFLOW_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* Включаем только общий контракт, НЕ добавляем STM32 Inc в include path
 * ESP-IDF: там лежит собственный port.h, конфликтующий с esp-modbus. */
#include "../../Inc/encoder_protocol.h"

/* Команды оператора. NEXT подтверждает текущую подсказку, а не запускает
 * отдельный режим. STOP отменяет режим; условия отправки 0x10 см. workflow.c. */
typedef enum { CMD_SCAN, CMD_POLL, CMD_CALIB, CMD_NEXT, CMD_STOP } command_kind_t;
typedef struct {
    command_kind_t kind;
    uint8_t ids[ENCODER_MAX_ID];
    size_t count; /* 0 = все из завершённого ручного scan */
} command_t;
/* Строгий CSV десятичных ID, например "7,1,247": без пробелов и повторов.
 * Сохраняет kind и порядок ID. При ошибке возвращает false и обнуляет count;
 * отсутствие аргумента обрабатывает консоль, не этот парсер. */
bool parse_head_ids(const char *text, command_t *command);

/* Зависимости передаёт вызывающая сторона: реальный ESP-IDF или host-тест.
 * request синхронно выполняет FC04/FC06; слова uint16_t в порядке машины.
 * true означает успешный обмен, но не завершённую калибровку.
 * poll выполняет целый SNAP → STATUS → READ, log принимает готовую строку,
 * now_ms возвращает монотонное время в мс с допустимым переполнением uint32_t. */
typedef struct {
    bool (*request)(uint8_t id, uint8_t function, uint16_t first,
                    uint16_t count, uint16_t *words);
    void (*poll)(const uint8_t *ids, size_t count, uint16_t seq);
    void (*log)(const char *message);
    uint32_t (*now_ms)(void);
} workflow_io_t;
/* IDLE — ожидание команды; SCAN — поиск; POLL — рабочий опрос;
 * CONFIRM — ожидание next без тайм-аута; CALIB — ожидание результата этапа. */
typedef enum { WF_IDLE, WF_SCAN, WF_POLL, WF_CONFIRM, WF_CALIB } workflow_state_t;
typedef struct {
    workflow_io_t io;
    workflow_state_t state;
    uint8_t found[ENCODER_MAX_ID], ids[ENCODER_MAX_ID]; /* Кэш scan / выбранные ID. */
    /* Во время SCAN кэш частичный; stop сбрасывает found_count в 0. */
    size_t found_count, count, head, step; /* Длины списков; индексы head/step с нуля. */
    unsigned scan_id, failures; /* Следующий адрес поиска; ошибки LIVE подряд. */
    uint16_t seq, last_state; /* Номер SNAP; последний код для журнала. */
    uint32_t started; /* Время запуска текущего этапа, не ожидания next. */
} workflow_t;
/* Инициализировать один раз при старте: повторный вызов сбросит seq и режим. */
void workflow_init(workflow_t *w, workflow_io_t io);
/* Обработать уже разобранную команду в том же контексте, что workflow_tick.
 * Новый scan/poll/calib принимается только в IDLE; для смены режима нужен stop.
 * poll/calib не запускают scan: явные ID или кэш завершённого ручного поиска. */
void workflow_command(workflow_t *w, const command_t *command);
/* Один шаг за вызов: один адрес поиска / полный цикл poll / проверка calib.
 * При завершении или ошибке calib шаг может дополнительно отправить STOP
 * и прочитать его результат. Только одна задача владеет workflow и Modbus. */
void workflow_tick(workflow_t *w);
#endif
