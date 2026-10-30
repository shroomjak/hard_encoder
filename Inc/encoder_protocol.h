/*
 * Общий контракт команд и синхронизации для STM32 slave и ESP32 master (UTF-8).
 * Это только константы: заголовок не зависит от HAL, FreeRTOS или стека Modbus.
 * Holding-регистры FC06 и input-регистры FC04 — разные адресные пространства.
 * Здесь адреса нулевые, как в запросе на линии; в callback FreeModbus к ним
 * прибавляется 1. Карта снимка объявлена отдельно в snapshot_registers.h.
 */
#ifndef ENCODER_PROTOCOL_H
#define ENCODER_PROTOCOL_H
/* Общие адреса НА ПРОВОДЕ (с нуля). Карта снимка 0..10 сохранена;
 * регистры 9..10 (SNAP_TIME_HI/LO) при публикации через DWT несут знаковый
 * int32 возраст кадра age_us = (t_SNAP_rx - t_exposure_mid) в микросекундах. */
enum {
    ENCODER_MAX_ID = 247,             /* Адреса slave: 1..247; 0 — вещание. */
    ENCODER_SNAP = 0,                 /* FC06: значение — номер снимка seq. */
    ENCODER_CAL_COMMAND = 1,          /* FC06, только адресный запрос */
    ENCODER_CAL_STATE = 0x100,        /* FC04, LIVE, не frozen */
    ENCODER_EEPROM_READY = 0x101,     /* 1 = завершена передача I2C, не проверка CRC EEPROM */
    ENCODER_CAL_VERSION = 0x102,      /* версия командного интерфейса */
    ENCODER_CAL_MAGIC = 0xCA01,       /* Отличает командный интерфейс от старого slave. */
    ENCODER_CAL_REG_COUNT = 3,        /* Читаются подряд: состояние, EEPROM, версия. */
    ENCODER_DWT_TICKS_PER_US = 216    /* Частота DWT CYCCNT на STM32F722 = 216 МГц. */
};
#endif
