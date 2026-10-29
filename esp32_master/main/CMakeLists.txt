# Прямой детерминированный драйвер RS-485 RTU + конечный автомат workflow + канал ПК.
idf_component_register(SRCS "main.c" "workflow.c" "pc_link.c" "rs485_rtu.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES console esp_timer driver esp_ringbuf esp_rom)
