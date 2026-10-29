# esp-modbus загружается Component Manager из idf_component.yml.
idf_component_register(SRCS "main.c" "workflow.c" "pc_link.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES console esp_timer driver esp_ringbuf)
