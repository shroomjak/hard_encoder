/*
 * Канал ESP32 (Core 0 -> Core 1 -> ПК, UART0 921600 бод) и математика
 * компенсации возраста экспозиции головок к единому моменту SNAP (UTF-8).
 *
 * Архитектура:
 *  - Core 0 (задача реального времени encoder_bus) выполняет только шинный обмен
 *    RS-485, пересчитывает углы к фронту SNAP целочисленной 64-битной арифметикой
 *    в микроградусах и за < 1 мкс кладёт компактную двоичную запись в 64 КБ
 *    неблокирующий кольцевой буфер RINGBUF_TYPE_NOSPLIT.
 *  - Core 1 (pc_link_task + UART0 921600 бод) форматирует строки DATA и итоговый
 *    отчёт POLL_END / TIMING / POLL_REPORT_DONE, полностью изолируя Core 0 от
 *    прерываний UART0, блокировок VFS и вызовов snprintf/fwrite.
 */
#ifndef PC_LINK_H
#define PC_LINK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../../Inc/encoder_protocol.h"

/* Читаем полный блок снимка (11 регистров), включая SNAP_TIME_HI/LO (age_us по DWT). */
#define PC_REG_COUNT           11U
#define PC_ANGLE_INVALID       0xFFFFFFFFU
#define PC_AGE_INVALID         INT32_MIN

/* Битовая маска ошибок цикла опроса (совместима с diagnostics/serial_head_logger.py). */
#define POLL_ERROR_SNAP        1U
#define POLL_ERROR_READ        2U
#define POLL_ERROR_FRAME       4U

/* Допустимое окно компенсации возраста экспозиции:
 *  - возраст кадра TSL1401CL при периоде ~80 мкс и 2-тактном конвейере составляет ~40..250 мкс;
 *    ограничиваем [-500..2500] мкс для защиты от устаревших/невалидных меток;
 *  - интервал между соседними валидными экспозициями головки [500..100000] мкс (0.5..100 мс). */
#define PC_SYNC_MIN_AGE_US     (-500)
#define PC_SYNC_MAX_AGE_US     2500
#define PC_SYNC_MIN_DT_US      500LL
#define PC_SYNC_MAX_DT_US      100000LL

typedef struct {
    bool     valid;
    uint32_t prev_raw_udeg;
    int64_t  prev_exp_us;
} pc_head_sync_t;

typedef struct {
    uint8_t  id;
    uint32_t angle_udeg;       /* Угол, приведённый к общему моменту SNAP (или PC_ANGLE_INVALID) */
    uint32_t raw_angle_udeg;   /* Сырой угол замороженного кадра головки (или PC_ANGLE_INVALID) */
    int32_t  age_us;           /* Возраст кадра (t_snap_rx - t_exp_mid) в мкс по DWT 216 МГц */
    uint32_t read_us;          /* Длительность адресного чтения FC04 в мкс */
    uint32_t gap_us;           /* Межкадровая пауза после чтения в мкс */
} pc_head_t;

typedef struct {
    uint16_t seq;
    uint16_t error;
    uint32_t count;
    uint64_t t_us;             /* Аппаратная метка стоп-бита SNAP на линии RS-485 (мкс) */
    uint32_t snap_us;          /* Полное время транзакции SNAP + settle (мкс) */
    uint32_t poll_us;          /* Полное время цикла SNAP + чтение всех головок (мкс) */
} pc_poll_t;

/* Чистая целочисленная функция приведения угла головки к единому моменту SNAP:
 * theta_sync = (theta_raw + omega_hat * age_us) mod 360 deg.
 * Доступна напрямую в host unit-тестах без ESP-IDF. */
void pc_sync_reset_all(pc_head_sync_t state[ENCODER_MAX_ID + 1]);
uint32_t pc_sync_compensate_udeg(pc_head_sync_t *st, uint32_t raw_udeg,
                                 uint64_t snap_wire_us, int32_t age_us);

#ifndef PC_LINK_HOST_MATH_ONLY
/* Запустить буфер (64 КБ) и задачу вывода на Core 1. */
void pc_link_start(void);

/* Сбросить статистику перед стартом нового сеанса poll. */
void pc_link_begin(uint32_t period_us);

/* Неблокирующая отправка результата цикла с Core 0 в очередь Core 1. */
bool pc_link_put(const pc_poll_t *poll, const pc_head_t *heads);

/* Завершить сеанс poll и вывести отчёт POLL_END / TIMING / POLL_REPORT_DONE через Core 1. */
void pc_link_end(uint32_t attempted, uint32_t dropped, uint32_t overruns);
#endif

#endif /* PC_LINK_H */
