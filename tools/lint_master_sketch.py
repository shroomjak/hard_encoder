#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Компиляционная проверка скетча ESP32 БЕЗ Arduino-тулчейна.

Скетч собирается обычным g++ поверх заглушек Arduino API и драйвера UART
из ESP-IDF. Проверяется синтаксис, типы, форматные строки printf и
статические assert'ы - то есть всё, кроме обращения к реальной периферии.

    python3 tools/lint_master_sketch.py
"""

import os
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SKETCH = os.path.join(ROOT, 'master_esp32', 'master_esp32.ino')

ARDUINO_H = r'''
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define HIGH 1
#define LOW  0
#define OUTPUT 1
#define SERIAL_8N1 0x800001c

typedef uint8_t byte;

static inline uint32_t millis(void) { return 0U; }
static inline void delay(uint32_t ms) { (void)ms; }
static inline void delayMicroseconds(uint32_t us) { (void)us; }
static inline void pinMode(int pin, int mode) { (void)pin; (void)mode; }
static inline void digitalWrite(int pin, int level) { (void)pin; (void)level; }
static inline uint32_t esp_random(void) { return 0U; }

class SerialStub {
public:
    void begin(unsigned long baud) { (void)baud; }
    void begin(unsigned long baud, uint32_t cfg, int rx, int tx) {
        (void)baud; (void)cfg; (void)rx; (void)tx;
    }
    int  available() { return 0; }
    int  read() { return -1; }
    size_t write(const uint8_t *buf, size_t len) { (void)buf; return len; }
    void flush(bool txOnly = false) { (void)txOnly; }
    void print(const char *s) { (void)s; }
    void println() { }
    void println(const char *s) { (void)s; }
    __attribute__((format(printf, 2, 3)))
    int printf(const char *fmt, ...) { (void)fmt; return 0; }
};

extern SerialStub Serial;
extern SerialStub Serial2;
'''

UART_H = r'''
#pragma once
#include <stdint.h>

typedef int uart_port_t;
#define UART_NUM_2 2
#define UART_PIN_NO_CHANGE (-1)

typedef enum {
    UART_MODE_UART = 0,
    UART_MODE_RS485_HALF_DUPLEX = 1
} uart_mode_t;

typedef int esp_err_t;
typedef uint32_t TickType_t;

#define pdMS_TO_TICKS(ms) ((TickType_t)(ms))

static inline esp_err_t uart_set_pin(uart_port_t p, int tx, int rx, int rts, int cts) {
    (void)p; (void)tx; (void)rx; (void)rts; (void)cts; return 0;
}
static inline esp_err_t uart_set_mode(uart_port_t p, uart_mode_t m) {
    (void)p; (void)m; return 0;
}
static inline esp_err_t uart_wait_tx_done(uart_port_t p, TickType_t t) {
    (void)p; (void)t; return 0;
}
'''

MAIN_STUB = r'''
SerialStub Serial;
SerialStub Serial2;

int main(void)
{
    setup();
    loop();
    return 0;
}
'''


def build(tmpdir, hw_de):
    inc = os.path.join(tmpdir, 'stub')
    os.makedirs(os.path.join(inc, 'driver'), exist_ok=True)
    with open(os.path.join(inc, 'Arduino.h'), 'w') as f:
        f.write(ARDUINO_H)
    with open(os.path.join(inc, 'driver', 'uart.h'), 'w') as f:
        f.write(UART_H)

    cpp = os.path.join(tmpdir, 'sketch_%d.cpp' % hw_de)
    with open(SKETCH, encoding='utf-8') as f:
        body = f.read()
    with open(cpp, 'w', encoding='utf-8') as f:
        f.write('#include "Arduino.h"\n')
        f.write('void setup();\nvoid loop();\n')
        f.write(body)
        f.write(MAIN_STUB)

    cmd = ['g++', '-std=c++11', '-c', '-o', os.path.join(tmpdir, 'sketch.o'),
           '-Wall', '-Wextra', '-Werror', '-Wshadow',
           '-DRS485_HW_DE=%d' % hw_de,
           '-I', inc,
           '-I', os.path.join(ROOT, 'master_esp32'),
           cpp]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.stdout.write(res.stdout)
        sys.stderr.write(res.stderr)
        print('\nсобранный для проверки файл: %s' % cpp)
        sys.exit(1)


def main():
    tmpdir = tempfile.mkdtemp(prefix='rs485sketch')
    # обе ветки управления DE должны компилироваться
    build(tmpdir, 1)
    build(tmpdir, 0)
    print('    master_esp32.ino компилируется без предупреждений '
          '(RS485_HW_DE=1 и RS485_HW_DE=0)')


if __name__ == '__main__':
    main()
