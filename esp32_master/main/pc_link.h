/*
 * Канал ESP32 -> ПК (UTF-8).
 * Core0 (encoder_bus) кладёт в кольцевой буфер компактную двоичную запись
 * готового цикла и сразу продолжает опрос шины. Core1 (pc_link) форматирует
 * запись в прежнюю строку "DATA seq=N id=.. ang=.." и печатает её.
 * Порядок строк = порядок seq (FIFO), записи не склеиваются и не теряются,
 * пока буфер не переполнен; переполнение считается и выводится в POLL.
 */
#ifndef PC_LINK_H
#define PC_LINK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Создать буфер и задачу печати на Core1. Вызывать один раз из app_main. */
void pc_link_start(void);

/* Core0: поставить полный набор углов. Не блокируется.
 * false = буфер полон, запись отброшена (учтена в счётчике dropped). */
bool pc_link_put_data(uint16_t seq, const uint8_t *ids,
                      const uint32_t *angle_udeg, size_t count);

/* Core0: поставить строку статистики POLL (тоже через Core1). */
bool pc_link_put_stats(uint32_t cycles, uint32_t complete,
                       uint32_t transport_err, uint32_t invalid,
                       int64_t last_cycle_us);

/* Сколько записей отброшено из-за переполнения (для диагностики). */
uint32_t pc_link_dropped(void);
#endif
