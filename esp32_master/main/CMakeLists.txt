# Общий код шины и один из двух режимов. Файлы режима подключаются только для
# выбранного режима, поэтому в рабочей прошивке нет команд калибровки.
# Режим выбирается в menuconfig: «Encoder master: режим работы».
set(ENCODER_SRCS "main.c" "bus.c")
if(CONFIG_ENCODER_MODE_CALIB)
    list(APPEND ENCODER_SRCS "calib_mode.c")
else()
    list(APPEND ENCODER_SRCS "poll_mode.c")
endif()

idf_component_register(SRCS ${ENCODER_SRCS} INCLUDE_DIRS ".")
