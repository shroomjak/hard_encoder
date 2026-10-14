#ifndef ENCODER_PROTOCOL_H
#define ENCODER_PROTOCOL_H
/* Общие адреса НА ПРОВОДЕ (с нуля). Карта снимка 0..10 не меняется. */
enum {
    ENCODER_MAX_ID = 247,
    ENCODER_SNAP = 0,
    ENCODER_CAL_COMMAND = 1,        /* FC06, только адресный запрос */
    ENCODER_CAL_STATE = 0x100,      /* FC04, LIVE, не frozen */
    ENCODER_EEPROM_READY = 0x101,   /* 1 = запись завершена */
    ENCODER_CAL_VERSION = 0x102,    /* версия командного интерфейса */
    ENCODER_CAL_MAGIC = 0xCA01,
    ENCODER_CAL_REG_COUNT = 3
};
#endif
