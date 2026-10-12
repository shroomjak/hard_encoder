#ifndef POLL_MODE_H
#define POLL_MODE_H
#include <stdint.h>

/* Рабочий режим: бесконечный цикл SNAP → STATUS → READ по всем головкам.
 * Собирается только при CONFIG_ENCODER_MODE_POLL. */
void poll_mode_run(void);

/* Один полный цикл опроса для номера кадра seq (для тестов на хосте). */
void poll_cycle(uint16_t seq);

#endif /* POLL_MODE_H */
