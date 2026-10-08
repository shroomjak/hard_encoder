#ifndef MODBUS_CALIB_H
#define MODBUS_CALIB_H
/*
 * Карта командного канала самокалибровки поверх Modbus RTU.
 *
 * Это ЕДИНСТВЕННЫЙ источник правды по адресам: и Src/modbus_calib.c, и
 * tools/modbus_calib.py, и docs/calibration_modbus.md описывают одно и то же,
 * а tests/calib_map_test.py сличает Python-копию с этим заголовком.
 *
 * Зачем так устроено. Коды 0x10…0x14 раньше приходили по SPI (обработчик
 * spi_recv_process() удалён из main.c вместе с каналом). Здесь те же коды
 * принимаются записью в holding-регистр, адресно, для одной головки; живой
 * статус читается блоком входных регистров, который НЕ зависит от SNAP —
 * иначе вести калибровку в реальном времени было бы нельзя.
 *
 * FreeModbus передаёт в callback адрес на 1 больше адреса на проводе.
 * Все константы здесь — АДРЕСА НА ПРОВОДЕ (с нуля); сдвиг делает только
 * Src/modbus_registers.c. FC03 и FC16 в сборке выключены (mbconfig.h), поэтому
 * holding-регистры нельзя прочитать обратно: последние принятая команда и её
 * результат продублированы во входных регистрах.
 */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------- запись, FC06 ----------------------------- */

#define MB_H_REG_SNAP     0 /* = 400001: зафиксировать снимок (см. snapshot_registers.h) */
#define MB_H_REG_CAL_CMD  1 /* = 400002: код команды 0x10…0x14                           */
#define MB_H_REG_CAL_KEY  2 /* = 400003: ключ MB_CAL_KEY_UNLOCK                           */
#define MB_H_REG_CAL_CFG  3 /* = 400004: флаги режима                                     */
#define MB_H_REG_CAL_PAGE 4 /* = 400005: окно выгрузки таблиц                             */
#define MB_H_REG_COUNT    5

/* Одна запись ключа разрешает РОВНО ОДНУ команду (one-shot). Это отключает
 * и случайный повтор кадра на шине, и «залипший» опросник. Любое другое значение
 * запирает запуск. При отказе ключ НЕ сгорает: можно исправить условия и
 * повторить команду, не перезаписывая ключ. */
#define MB_CAL_KEY_UNLOCK 0xCA1Bu

/* Флаги CAL_CFG */
#define MB_CAL_CFG_RELAX 0x0001u /* не проверять условия запуска (только для стенда) */
#define MB_CAL_CFG_KEEP  0x0002u /* при 0x10 не откатывать offset и buf_k             */

/* Байты команды */
#define MB_CAL_CMD_STOP         0x10u /* всё снять, encoder_state = 0x00              */
#define MB_CAL_CMD_ANGTAB_RIGHT 0x11u /* ang_tab, проход 1 (угол растёт)              */
#define MB_CAL_CMD_ANGTAB_LEFT  0x12u /* ang_tab, проход 2 (угол убывает)             */
#define MB_CAL_CMD_OFFSET       0x13u /* offset_cal                                    */
#define MB_CAL_CMD_BUFK         0x14u /* angk_cal                                      */

/* Коды encoder_state, которые имеет смысл ждать (смысл — в docs §8) */
#define MB_CAL_STATE_IDLE        0x00
#define MB_CAL_STATE_ANGTAB_1    0x10
#define MB_CAL_STATE_ANGTAB_1_RDY 0x20 /* ждём команду 0x12                            */
#define MB_CAL_STATE_ANGTAB_2    0x30
#define MB_CAL_STATE_ANGTAB_2_RDY 0x40
#define MB_CAL_STATE_OFFSET      0x50
#define MB_CAL_STATE_OFFSET_RDY  0x60
#define MB_CAL_STATE_BUFK        0x70
#define MB_CAL_STATE_BUFK_RDY    0x80

/* ------------------------- чтение, FC04: входные регистры ------------------ */

/* Регистры 0…10 принадлежат снимку кадра (snapshot_registers.h, SNAP_REG_COUNT). */
#define MB_CAL_INPUT_FIRST      11 /* начало живого блока калибровки                  */
#define MB_CAL_BLOCK_ID_VALUE   0xCA10u /* маркер: прошивка умеет блок калибровок      */

/* Порядок обязан совпадать с таблицей §2.3 docs/calibration_modbus.md и с
 * CAL_REGS в tools/modbus_calib.py; счёт проверяет tests/calib_map_test.py. */
enum {
    CAL_R_BLOCK_ID = MB_CAL_INPUT_FIRST,
    CAL_R_STATE,              /* encoder_state                                      */
    CAL_R_AUTO_CAL,           /* 0 нет, 1 «вправо», 2 «влево»                       */
    CAL_R_RDY1,               /* прогресс прохода 1 ang_tab, 0…144                  */
    CAL_R_RDY2,               /* прогресс прохода 2 ang_tab, 0…144                  */
    CAL_R_OFFSET_PHASE,       /* фаза автомата offset_cal                             */
    CAL_R_OFFSET_CUR,         /* текущая проба offset                                 */
    CAL_R_OFFSET_FOUND,       /* найденный оптимум                                   */
    CAL_R_OFFSET_X256,        /* живой offset, 1/256 пикселя                         */
    CAL_R_ANGLEK_PHASE,       /* фаза автомата angk_cal                               */
    CAL_R_ANGLEK_CUR,         /* номер кандидата усиления buf_k                      */
    CAL_R_PIXMINMAX,          /* pix_min_max, отсчёты АЦП (цель < 100)               */
    CAL_R_MINMAX_HI,          /* min_max, знаковые микроградусы, старшее слово       */
    CAL_R_MINMAX_LO,
    CAL_R_REVS,               /* avg_minmax_num — оборотов учтено в пробе            */
    CAL_R_REV_RIGHT,          /* rev_right_cnt, младшее слово                        */
    CAL_R_REV_LEFT,           /* rev_left_cnt, младшее слово                         */
    CAL_R_SERR1,              /* неисправимые кадры                                  */
    CAL_R_SERR2,              /* сработавшие побитовые коррекции                     */
    CAL_R_SECTOR,             /* текущий номер сектора                               */
    CAL_R_ANGLE_HI,           /* живой cur_ang_E, микроградусы, старшее слово        */
    CAL_R_ANGLE_LO,
    CAL_R_FLAGS,              /* см. CAL_F_*                                         */
    CAL_R_DIAG,               /* см. CAL_D_*; приращения к прошлому опросу           */
    CAL_R_LAST_CMD,           /* код последней команды                               */
    CAL_R_LAST_RESULT,        /* 0 — принята, 1…6 — причина отказа                   */
    CAL_R_CMD_COUNT,          /* принято команд (16 бит, переполняется)              */
    CAL_R_REJ_COUNT,          /* отклонено команд                                    */
    CAL_R_CYCLES_HI,          /* cycles_max, такты                                    */
    CAL_R_CYCLES_LO,
    CAL_R_BACKLIGHT,          /* backlight_width_ticks                               */
    CAL_R_TEMP,               /* температура кристалла, 0,1 °C                        */
    CAL_R_PIX_AVG,            /* pix_dif_num_avg                                      */
    CAL_R_OFFSET_AVG,         /* offset_avg_num                                       */
    CAL_R_SAMPLE_HI,          /* обработано кадров АЦП (живой счётчик), ст. слово    */
    CAL_R_SAMPLE_LO,
    CAL_R_INPUT_END           /* на 1 больше последнего живого регистра               */
};

/* Отсчёт ведётся от CAL_R_BLOCK_ID, чтобы вставка регистра в середину не
 * меняла количество незаметно: тест сверяет и это число, и порядок имён. */
#define MB_CAL_INPUT_COUNT (CAL_R_INPUT_END - CAL_R_BLOCK_ID)

/* Поле CAL_PAGE: [15:8] — номер таблицы, [7:0] — индекс первого элемента. */
#define MB_CAL_PAGE_TABLE(v) ((uint8_t)((v) >> 8))
#define MB_CAL_PAGE_INDEX(v) ((uint16_t)((v) & 0x00FFu))

/* Окно выгрузки таблиц: MB_CAL_WINDOW_FLOATS float = 32 регистра IEEE-754,
 * в каждом числе сначала старшее слово (порядок Modbus). Адрес окна — сразу за
 * живым блоком; выбирается записью в CAL_PAGE: [15:8] = таблица, [7:0] = индекс
 * первого элемента. */
#define MB_CAL_WINDOW_FLOATS 16
#define MB_CAL_WINDOW_FIRST  (CAL_R_INPUT_END)
#define MB_CAL_WINDOW_COUNT  (2 * MB_CAL_WINDOW_FLOATS)
#define MB_CAL_WINDOW_LAST   (MB_CAL_WINDOW_FIRST + MB_CAL_WINDOW_COUNT - 1)

/* Всего входных регистров. FreeModbus для FC04 пропускает count < 0x7D. */
#define MB_CAL_INPUT_TOTAL (MB_CAL_WINDOW_FIRST + MB_CAL_WINDOW_COUNT)

/* Идентификаторы таблиц для CAL_PAGE[15:8] */
#define CAL_T_ANG_TAB       0 /* ang_tab[144],        градусы  */
#define CAL_T_BUF_K         1 /* buf_k[128],          безразм. */
#define CAL_T_PIXDIF1       2 /* pix_dif_tab1[144],   пиксели  */
#define CAL_T_PIXDIF2       3 /* pix_dif_tab2[144],   пиксели  */
#define CAL_T_OFFSET_MINMAX 4 /* offset_minmax[128],  градусы  */
#define CAL_T_COUNT         5

/* Биты CAL_R_FLAGS — «что есть сейчас» */
#define CAL_F_CAL_PENDING  0u  /* start_calibrate ещё не съеден автоматом            */
#define CAL_F_OFFSET_RUN   1u  /* идёт offset_cal                                     */
#define CAL_F_ANGK_RUN     2u  /* идёт angk_cal                                       */
#define CAL_F_REV_EN       3u  /* детектор перехода через 0/360 взведён               */
#define CAL_F_AGC_ON       4u  /* АРУ подсветки разрешена (на время 0x14 её нет)      */
#define CAL_F_EEPROM_IDLE  5u  /* i2c3_tx_wp == 1: запись в EEPROM завершена          */
#define CAL_F_ERROR        6u  /* errorflag: последний кадр бракован                  */
#define CAL_F_ARMED        7u  /* ключ взведён, ждёт ровно одну команду               */
#define CAL_F_RELAX        8u
#define CAL_F_KEEP         9u
#define CAL_F_ANG_TAB_RDY  10u /* ang_tab принята из EEPROM (CRC16 сошлась)           */
#define CAL_F_BUF_K_RDY    11u /* buf_k принята из EEPROM                             */
#define CAL_F_OFFSET_RDY   12u /* offset принят из EEPROM                             */
#define CAL_F_LASDAC_RDY   13u /* lasdac принят из EEPROM                             */
#define CAL_F_BUSY         14u /* идёт калибровка: новые пусковые команды отклоняются */
#define CAL_F_WAIT_PASS2   15u /* состояние 0x20: проход 1 готов, ждём 0x12           */

/* Биты CAL_R_DIAG — предупреждения. Кроме EVERY_OTHER все считаются как
 * приращение относительно ПРЕДЫДУЩЕГО чтения блока, то есть требуют
 * периодического опроса (режим watch делает это сам). */
#define CAL_D_MOVING_RIGHT    0u /* угол растёт  — нужно для 0x11                    */
#define CAL_D_MOVING_LEFT     1u /* угол убывает — нужно для 0x12                    */
#define CAL_D_WRONG_DIRECTION 2u /* направление не соответствует текущему проходу    */
#define CAL_D_ANGTAB_STALLED  3u /* прогресс ang_tab между опросами не двигался      */
#define CAL_D_OFFSET_STALLED  4u /* offset_phase стоит                               */
#define CAL_D_ANGK_STALLED    5u /* anglek_phase стоит                               */
#define CAL_D_MINMAX_UP       6u /* min_max вырос относительно прошлого опроса       */
#define CAL_D_SERR_GROWING    7u /* прибавляются неисправимые кадры                  */
#define CAL_D_NO_REVS         8u /* оборот не детектируется: скорость слишком велика */
#define CAL_D_EVERY_OTHER     9u /* прогресс застрял на 72 — неверное направление     */
#define CAL_D_NO_FRAMES      10u /* между опросами не обработано ни одного кадра АЦП */

/* Результат приёма команды. Дублируется в CAL_R_LAST_RESULT; в шине
 * отображается в исключение Modbus (см. Src/modbus_registers.c). */
typedef enum {
    MB_CAL_OK = 0,
    MB_CAL_ERR_REG = 1,     /* нет такого holding-регистра / адрес вне карты */
    MB_CAL_ERR_VALUE = 2,   /* неизвестный код команды или таблицы            */
    MB_CAL_ERR_LOCKED = 3,  /* ключ CAL_KEY не взведён                        */
    MB_CAL_ERR_BUSY = 4,    /* уже идёт калибровка                            */
    MB_CAL_ERR_EEPROM = 5,  /* предыдущий блок ещё пишется в EEPROM           */
    MB_CAL_ERR_PREREQ = 6   /* нарушен порядок или физическое условие          */
} mb_cal_result_t;

/*
 * Запись holding-регистра. address — адрес НА ПРОВОДЕ (0…MB_H_REG_COUNT-1),
 * значение уже собрано из двух байтов. Побочный эффект только один: взвод
 * флагов запуска; сами автоматы крутит main loop.
 */
mb_cal_result_t mb_cal_write(uint16_t address, uint16_t value);

/*
 * Чтение одного входного регистра блока калибровки (адрес на проводе,
 * MB_CAL_INPUT_FIRST…MB_CAL_INPUT_TOTAL-1). Побочный эффект — обновление
 * базы для приращений DIAG; остальное состояние DIAG не меняет.
 */
uint16_t mb_cal_read(uint16_t address);

/* Сброс внутреннего состояния модуля (хостовые тесты, возврат к исходному виду). */
void mb_cal_reset(void);

#ifdef __cplusplus
}
#endif
#endif /* MODBUS_CALIB_H */
