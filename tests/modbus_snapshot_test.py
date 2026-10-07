#!/usr/bin/env python3
"""Host test for the STM32 application's SNAP / STATUS register contract.

The protocol core is third-party FreeModbus and target-specific USART code cannot
run on the host.  This test deliberately stubs those two boundaries, compiles
Src/modbus_slave.c with gcc and validates the register callbacks used by FC06
and FC04.
"""
from __future__ import annotations

import shutil
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

PORT_H = r"""
#ifndef PORT_H
#define PORT_H
#include <stdint.h>
typedef uint8_t BOOL; typedef uint8_t UCHAR; typedef char CHAR;
typedef uint16_t USHORT; typedef int16_t SHORT;
typedef uint32_t ULONG; typedef int32_t LONG;
#define TRUE 1
#define FALSE 0
#define PR_BEGIN_EXTERN_C
#define PR_END_EXTERN_C
#define ENTER_CRITICAL_SECTION()
#define EXIT_CRITICAL_SECTION()
#endif
"""

MODBUS_PORT_H = r"""
#ifndef MODBUS_PORT_H
#define MODBUS_PORT_H
#define MODBUS_SLAVE_ADDRESS 2U
#define MODBUS_BAUDRATE 115200UL
#endif
"""

HARNESS = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "modbus_slave.h"
#include "mb.h"

eMBErrorCode eMBInit(eMBMode mode, UCHAR address, UCHAR port, ULONG baud,
                      eMBParity parity, UCHAR stop_bits)
{
    (void)mode; (void)address; (void)port; (void)baud; (void)parity; (void)stop_bits;
    return MB_ENOERR;
}
eMBErrorCode eMBEnable(void) { return MB_ENOERR; }
eMBErrorCode eMBPoll(void) { return MB_ENOERR; }

static uint16_t be16(const UCHAR *bytes)
{
    return ((uint16_t)bytes[0] << 8) | bytes[1];
}

int main(void)
{
    uint16_t raw[128];
    uint16_t i;
    UCHAR snap[] = {0, 42};
    UCHAR registers[32] = {0};

    for (i = 0; i < 128; ++i) raw[i] = (uint16_t)(100 + i);
    ModbusSlave_Publish(12.345f, -2, 0x1ab, 50, 51, 1, -1.234f, raw);

    /* FC06 unit 0/normal unit reaches FreeModbus callback as address 1. */
    assert(eMBRegHoldingCB(snap, 1, 1, MB_REG_WRITE) == MB_ENOERR);
    assert(eMBRegInputCB(registers, 1, 16) == MB_ENOERR);
    assert((be16(&registers[0]) & MODBUS_SNAPSHOT_VALID) != 0);
    assert((be16(&registers[0]) & MODBUS_SNAPSHOT_SENSOR_ERROR) != 0);
    assert(be16(&registers[2]) == 42);       /* sequence */
    assert(be16(&registers[4]) == 2);        /* slave ID */
    assert((int16_t)be16(&registers[6]) == -2);
    assert(be16(&registers[8]) == 1235);     /* rounded centidegrees */
    assert((int16_t)be16(&registers[20]) == -123);
    assert(be16(&registers[22]) == 128);

    /* FC04 address 16 on the wire arrives as callback address 17. */
    assert(eMBRegInputCB(registers, 17, 2) == MB_ENOERR);
    assert(be16(&registers[0]) == 100 && be16(&registers[2]) == 101);
    puts("Modbus SNAP/STATUS/DATA register contract: OK");
    return 0;
}
"""


def main() -> None:
    gcc = shutil.which("gcc")
    if not gcc:
        raise SystemExit("gcc is required for this host test")

    with tempfile.TemporaryDirectory(prefix="hard_encoder_modbus_") as tmp:
        temp = Path(tmp)
        (temp / "port.h").write_text(PORT_H)
        (temp / "modbus_port.h").write_text(MODBUS_PORT_H)
        harness = temp / "snapshot_harness.c"
        binary = temp / "snapshot_harness"
        harness.write_text(HARNESS)
        command = [
            gcc, "-std=c99", "-Wall", "-Wextra", "-Werror",
            "-I", str(temp),
            "-I", str(ROOT / "Inc"),
            "-I", str(ROOT / "Middlewares/Third_Party/FreeModbus/modbus/include"),
            str(ROOT / "Src/modbus_slave.c"), str(harness), "-o", str(binary),
        ]
        subprocess.run(command, check=True)
        subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    main()
