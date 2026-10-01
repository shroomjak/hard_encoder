#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Компиляционная проверка RS-485-части прошивки БЕЗ ARM-тулчейна.

Зачем: весь обмен по шине живёт внутри гигантского Src/main.c, собрать
который на хосте нельзя (IAR + STM32 HAL/LL + CMSIS). При этом именно
эта часть правится чаще всего, и обидно ловить опечатку только на
железе. Скрипт вырезает из main.c ровно новые куски RS-485 (структуры
конвейера, реализацию протокола, обработчик USART1 и публикацию кадра),
подставляет заглушки LL/HAL/CMSIS и компилирует всё это обычным gcc.

Проверяется синтаксис, типы, объявления и предупреждения компилятора -
то есть всё, кроме собственно работы с регистрами.

    python3 tools/lint_slave_rs485.py
"""

import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAIN = os.path.join(ROOT, 'Src', 'main.c')


def read_main():
    with open(MAIN, 'rb') as f:
        return f.read().decode('cp1251')


def cut(text, start_marker, end_marker, include_end=True, what=''):
    i = text.find(start_marker)
    if i < 0:
        sys.exit('не найдено начало блока %s: %r' % (what, start_marker[:60]))
    j = text.find(end_marker, i)
    if j < 0:
        sys.exit('не найден конец блока %s: %r' % (what, end_marker[:60]))
    return text[i:j + (len(end_marker) if include_end else 0)]


STUBS = r'''
/* ---- заглушки окружения STM32 (только для хостовой проверки) ---- */
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "rs485_proto.h"

#define RS485_DEVICE_ID 1U
#define BIT_TAB_SIZE    144

typedef struct { int dummy; } GPIO_TypeDef;
typedef struct { int dummy; } USART_TypeDef;

static GPIO_TypeDef  gpiob_stub;
static USART_TypeDef usart1_stub;

#define RS485_PORT   (&gpiob_stub)
#define RS485_USART  (&usart1_stub)
#define RS485_DE_PIN 0x20U

static void     LL_GPIO_SetOutputPin(GPIO_TypeDef *p, uint32_t m)   { (void)p; (void)m; }
static void     LL_GPIO_ResetOutputPin(GPIO_TypeDef *p, uint32_t m) { (void)p; (void)m; }
static void     LL_USART_ClearFlag_TC(USART_TypeDef *u)             { (void)u; }
static void     LL_USART_ClearFlag_ORE(USART_TypeDef *u)            { (void)u; }
static void     LL_USART_ClearFlag_NE(USART_TypeDef *u)             { (void)u; }
static void     LL_USART_ClearFlag_FE(USART_TypeDef *u)             { (void)u; }
static void     LL_USART_EnableIT_TXE(USART_TypeDef *u)             { (void)u; }
static void     LL_USART_DisableIT_TXE(USART_TypeDef *u)            { (void)u; }
static void     LL_USART_EnableIT_TC(USART_TypeDef *u)              { (void)u; }
static void     LL_USART_DisableIT_TC(USART_TypeDef *u)             { (void)u; }
static void     LL_USART_TransmitData8(USART_TypeDef *u, uint8_t b) { (void)u; (void)b; }
static uint8_t  LL_USART_ReceiveData8(USART_TypeDef *u)             { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_TXE(USART_TypeDef *u)         { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_TC(USART_TypeDef *u)          { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_RXNE(USART_TypeDef *u)        { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_ORE(USART_TypeDef *u)         { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_NE(USART_TypeDef *u)          { (void)u; return 0U; }
static uint32_t LL_USART_IsActiveFlag_FE(USART_TypeDef *u)          { (void)u; return 0U; }
static uint32_t LL_USART_IsEnabledIT_TXE(USART_TypeDef *u)          { (void)u; return 0U; }
static uint32_t LL_USART_IsEnabledIT_TC(USART_TypeDef *u)           { (void)u; return 0U; }
static uint32_t LL_USART_IsEnabledIT_RXNE(USART_TypeDef *u)         { (void)u; return 0U; }

static uint32_t hal_tick_stub = 0U;
static uint32_t HAL_GetTick(void)              { return hal_tick_stub; }
static void     __DMB(void)                    { }
static uint32_t __get_PRIMASK(void)            { return 0U; }
static void     __disable_irq(void)            { }
static void     __set_PRIMASK(uint32_t v)      { (void)v; }

/* глобалы алгоритма энкодера, которые читает публикация кадра */
static int      errorflag = 0;
static int16_t  sector = 0;
static int16_t  rsector = 0;
static float    cur_ang_E = 0.0f;
static unsigned char encoder_state = 0;
'''

TAIL = r'''
/* ---- вызовы, чтобы ни одна функция не осталась "неиспользованной" ---- */
void rs485_lint_entry(void);
void rs485_lint_entry(void)
{
    RS485_PollCommands();
    RS485_SendMeasurement();
    RS485_Test();
    USART1_IRQHandler();
    rs485_publish_probe();
    (void)RS485_AngleToMicro(123.456789f);
    hal_tick_stub++;
}
'''


def main():
    src = read_main()

    meas = cut(src,
               'typedef struct {\n    int32_t  angle_udeg;',
               'static uint32_t measurement_frame_no = 0U;',
               what='структуры конвейера')

    impl = cut(src,
               'typedef struct {\n    uint32_t rx_lines;',
               'static void RS485_PollCommands(void)\n{',
               what='реализация протокола')
    # дотянуть тело RS485_PollCommands() до закрывающей скобки функции
    rest = src[src.index(impl) + len(impl):]
    end = rest.index('\n}\n')
    impl = impl + rest[:end + 3]

    isr = cut(src,
              'void USART1_IRQHandler(void)\n{',
              '\n}\n',
              what='обработчик USART1')

    publish = cut(src,
                  '      measurement_frame_no++;',
                  '/* RS485_SendMeasurement(); */',
                  what='публикация кадра')
    publish = ('static void rs485_publish_probe(void)\n{\n'
               + publish + '\n}\n')

    # прототипы, как в main.c (функции определяются ниже по тексту)
    protos = ('static int  RS485_TxStart(const char *data, size_t len);\n'
              'static void RS485_PollCommands(void);\n'
              'static void RS485_OnStatus(uint8_t id, uint32_t seq);\n'
              'static void RS485_OnRead(uint8_t id, uint32_t seq);\n'
              'static int32_t RS485_AngleToMicro(float deg);\n'
              'static void RS485_Write(const uint8_t *data, size_t len);\n'
              'static void RS485_SendMeasurement(void);\n'
              'static void RS485_Test(void);\n'
              'static void RS485_RxByteFromISR(uint8_t byte);\n'
              'static void rs485_publish_probe(void);\n'
              'void USART1_IRQHandler(void);\n')

    harness = (STUBS + '\n' + meas + '\n\n' + protos + '\n' + impl + '\n'
               + publish + '\n' + isr + '\n' + TAIL)

    tmpdir = tempfile.mkdtemp(prefix='rs485lint')
    path = os.path.join(tmpdir, 'slave_rs485_lint.c')
    with open(path, 'wb') as f:
        f.write(harness.encode('cp1251'))

    cmd = ['gcc', '-std=c99', '-c', '-o', os.path.join(tmpdir, 'out.o'),
           '-Wall', '-Wextra', '-Werror', '-Wshadow', '-Wundef',
           '-finput-charset=CP1251', '-fexec-charset=UTF-8',
           '-I', os.path.join(ROOT, 'Inc'), path]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.stdout.write(res.stdout)
        sys.stderr.write(res.stderr)
        print('\nсобранный для проверки файл: %s' % path)
        sys.exit(1)

    print('    RS-485 часть Src/main.c компилируется без предупреждений '
          '(%d строк кода проверено)' % harness.count('\n'))


if __name__ == '__main__':
    main()
