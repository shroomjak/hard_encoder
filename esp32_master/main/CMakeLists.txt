# Как и в примере Espressif, ESP-IDF загружает зависимость esp-modbus по
# idf_component.yml. Добавлять её исходники в проект вручную не требуется.
idf_component_register(SRCS "main.c" INCLUDE_DIRS ".")
