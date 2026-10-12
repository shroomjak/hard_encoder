#ifndef CALIB_MODE_H
#define CALIB_MODE_H

/* Режим калибровки одной головки (адрес CONFIG_ENCODER_CALIB_SLAVE).
 * Собирается только при CONFIG_ENCODER_MODE_CALIB. Возвращает управление,
 * когда процедура завершена, прервана оператором или ошибкой обмена. */
void calib_mode_run(void);

#endif /* CALIB_MODE_H */
