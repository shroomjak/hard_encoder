#ifndef MODBUS_CALIB_H
#define MODBUS_CALIB_H
/*
 * Регистры запуска и контроля самокалибровки (Modbus RTU).
 *
 * Запись кода команды в CAL_CMD взводит те же флаги, которые взводит конечный
 * автомат при запуске из отладчика; исполнение продолжает main loop. Код 0x10
 * снимает все флаги запуска. Проверок «можно ли запускать» здесь нет: прерванную
 * или неудачную калибровку повторяют заново, а незаписанный результат при
 * перезагрузке вернётся из EEPROM.
 *
 * FC06 (запись) — адреса на проводе с нуля, допустима запись по одному регистру.
 * FC03 (чтение holding) и FC16 в сборке выключены, поэтому состояние калибровки
 * читается входными регистрами FC04.
 */
#include <stdint.h>

/* --- запись, FC06 --- */
#define MB_H_SNAP    0 /* 400001: зафиксировать снимок кадра (см. snapshot_registers.h) */
#define MB_H_CAL_CMD 1 /* 400002: код команды калибровки                                */
#define MB_H_CAL_PAGE 2 /* 400003: окно выгрузки таблиц: [15:8] таблица, [7:0] индекс    */
#define MB_H_COUNT   3

/* Коды команд. 0x11…0x14 — запуск, 0x10 — останов. */
#define MB_CAL_STOP           0x10u
#define MB_CAL_ANGTAB_RIGHT   0x11u /* angtab_cal, проход 1: угол должен расти   */
#define MB_CAL_ANGTAB_LEFT    0x12u /* angtab_cal, проход 2: угол должен убывать */
#define MB_CAL_OFFSET         0x13u /* offset_cal                                 */
#define MB_CAL_BUFK           0x14u /* angk_cal                                   */

/* CAL_PAGE: таблицы, доступные для выгрузки (ang_tab и buf_k — то, что
 * калибровка перезаписывает в EEPROM). */
#define MB_CAL_T_ANGTAB 0u /* float ang_tab[144], градусы      */
#define MB_CAL_T_BUFK   1u /* float buf_k[128], безразмерный   */
#define MB_CAL_T_COUNT  2u

/* биты MB_R_EEPROM */
#define MB_E_WRITTEN 0 /* записей в EEPROM не осталось (i2c3_tx_wp == 1)           */
#define MB_E_ANGTAB  1 /* ang_tab принята (сумма углов не больше 360 градусов)     */
#define MB_E_BUFK    2 /* buf_k принята (сумма коэффициентов больше 128)           */
#define MB_E_OFFSET  3 /* offset принят                                             */

/* --- чтение, FC04 --- */
#define MB_CAL_FIRST         11 /* 0…10 занимает снимок кадра                  */
#define MB_CAL_WINDOW        31 /* начало окна выгрузки таблиц                 */
#define MB_CAL_WINDOW_FLOATS 16 /* элементов float в окне                      */
#define MB_CAL_TOTAL         63 /* всего входных регистров                     */

enum {
    MB_R_STATE = MB_CAL_FIRST,
    MB_R_RDY1,        /* прогресс прохода 1 ang_tab, 0…144                    */
    MB_R_RDY2,        /* прогресс прохода 2 ang_tab, 0…144                    */
    MB_R_OFFSET_PHASE,
    MB_R_OFFSET_CUR,
    MB_R_OFFSET_FOUND,
    MB_R_OFFSET_X256, /* offset, умноженный на 256 (пиксели)                  */
    MB_R_ANGK_PHASE,
    MB_R_PIX_MIN_MAX, /* знаковый: цель — опустить ниже 100 отсчётов АЦП      */
    MB_R_MINMAX_HI,   /* min_max, знаковые микроградусы                        */
    MB_R_MINMAX_LO,
    MB_R_REVS,        /* оборотов учтено в текущей пробе                       */
    MB_R_REV_RIGHT,   /* оборотов «вправо»                                      */
    MB_R_REV_LEFT,    /* оборотов «влево»                                       */
    MB_R_SERR1,       /* кадров с неисправимой ошибкой декодирования            */
    MB_R_ANGLE_HI,    /* cur_ang_E, микроградусы                                */
    MB_R_ANGLE_LO,
    MB_R_FRAMES_HI,   /* обработанных кадров АЦП                                */
    MB_R_FRAMES_LO,
    MB_R_EEPROM      /* биты: 0 — запись в EEPROM завершена, 1 — ang_tab
                       * принята из EEPROM, 2 — buf_k, 3 — offset              */
};

/* Записать регистр FC06. Адрес — на проводе. 0 — принято,
 * -1 — нет такого регистра, -2 — недопустимое значение. */
int mb_cal_write(uint16_t address, uint16_t value);

/* Прочитать входной регистр MB_CAL_FIRST…MB_CAL_TOTAL-1. */
uint16_t mb_cal_read(uint16_t address);

#endif /* MODBUS_CALIB_H */
