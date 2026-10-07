# ESP32 Modbus RTU master

This is an ESP-IDF project for the RS-485 master. It uses the official
[`espressif/esp-modbus`](https://github.com/espressif/esp-modbus) component,
not a locally implemented Modbus stack.

## Build

```bash
cd esp32_master
idf.py set-target esp32
idf.py build flash monitor
```

`main/idf_component.yml` downloads `espressif/esp-modbus` version `^2.1.4`
through the ESP-IDF Component Manager. Therefore there is no need to copy the
whole ESP-Modbus repository into this project.

## Wiring to the ESP32 RS-485 converter

Defaults in `main/encoder_master.c` are for a usual ESP32 DevKit:

| ESP32 signal | GPIO | RS-485 converter pin |
|---|---:|---|
| UART2 TX | 17 | DI / D |
| UART2 RX | 16 | RO / R |
| UART2 RTS | 4 | joined DE + `/RE` direction input |
| GND | — | signal reference GND |

`UART_MODE_RS485_HALF_DUPLEX` makes the ESP32 UART drive RTS only while it
transmits. Change the three GPIO macros at the start of `encoder_master.c` to
match the actual master PCB. The electrical direction input must be such that
**RTS high selects transmit**; add/invert external logic if the selected
transceiver wiring has the opposite polarity.

The code targets 115200, 8N1. The baud rate, parity and stop bits must match
all STM32 heads.
