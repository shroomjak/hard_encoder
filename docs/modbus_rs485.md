# RS-485 / Modbus RTU: STM32 heads and ESP32 master

## Chosen existing implementations

The application code in this repository does **not** parse RTU frames or
calculate CRC itself. It is an integration layer around two existing stacks:

| Side | Stack | Version/source | What to put in the project |
|---|---|---|---|
| STM32F722 slave | [FreeModbus](https://github.com/cwalter-at/freemodbus) | upstream commit `a9ec789a23ff774287e711aeb18492860c8770cf`, BSD-3-Clause | The required RTU slave subset is already vendored in `Middlewares/Third_Party/FreeModbus/`; its license is `LICENSE.txt`. |
| ESP32 master | [ESP-Modbus](https://github.com/espressif/esp-modbus) | official Espressif component, `^2.1.4`, Apache-2.0 | Do **not** copy a hand-maintained source tree. `esp32_master/main/idf_component.yml` declares the component; ESP-IDF Component Manager fetches it at build time. |

The STM32 subset comprises FreeModbus core, RTU transport, CRC and the input/
holding-register functions only. It is deliberately configured without ASCII,
TCP, coils, FC03 or arbitrary multiple-register writes. This reduces ROM and
prevents accidental writable commands on the encoder bus. The source code has
not been copied from a GPL demo; the imported FreeModbus core is under the
BSD license stated in its source headers and `LICENSE.txt`.

To repeat the ESP32 dependency installation manually, from `esp32_master`:

```bash
idf.py add-dependency "espressif/esp-modbus^2.1.4"
idf.py build
```

## Topology and electrical assumptions

* ESP32 is the **only** master. Each STM32 head is a slave with a unique unit
  address in `1..247`; `0` is reserved for broadcast.
* RS-485 is a daisy chain, not a star. Fit 120 ohm termination only at the two
  physical cable ends and provide biasing at one point only. Run a signal/GND
  reference with the differential pair when the installation needs it.
* The JTAG/SWD connector (TCK, TMS, SWO/TDO, NRST) is unrelated to RS-485 and
  must remain reserved for programming/debugging.
* The default STM32 proposal is `USART3`: `PC10 TX -> DI/D`, `PC11 RX <- RO/R`,
  `PB14 -> joined DE + /RE`. It is centralized in `Inc/modbus_port.h` because
  the current repository does not include the RS-485 PCB schematic. **Verify
  the actual routed pins and polarity before programming.** The defaults avoid
  pins already used by ADC, EEPROM, TSL sensor and debug connector.
* The STM32 port drives DE only around a response and waits for USART `TC`
  (last stop bit), not merely `TXE`, before returning to receive mode. On the
  ESP32, hardware RTS and `UART_MODE_RS485_HALF_DUPLEX` perform that role.

All examples use **115200 bit/s, 8N1**. The STM32 t3.5 delimiter is generated
by TIM7 at 50-us resolution; USART and TIM7 IRQ priorities (2 and 3) remain
below the existing sensor DMA priority 0.

## STM32 integration

`Src/main.c` now does three things:

1. After the existing encoder/EEPROM initialization it calls `ModbusSlave_Init()`.
2. Every pass through the foreground loop calls `ModbusSlave_Poll()`. UART RX,
   TX and t3.5 interrupts only enqueue FreeModbus events, so no Modbus parsing
   is done in the ADC DMA interrupt.
3. After a complete calculated sensor frame it calls `ModbusSlave_Publish()`
   with `cur_ang_E`, sector, code word, start pixels, error flag, temperature
   and all 128 `buf_x0` ADC words.

`Src/modbus_slave.c` owns a live record and a frozen record. A `SNAP` first
copies the fully published live record to the frozen input-register map. Thus
an angle cannot be from one frame while raw pixels are from a later frame.

### Assigning different slave IDs

`MODBUS_SLAVE_ADDRESS` defaults to `1` in `Inc/modbus_port.h`. It must differ
for every physical head. Build/program the heads separately, for example by
adding one IAR C/C++ compiler preprocessor define for each programming build:

```text
MODBUS_SLAVE_ADDRESS=1
MODBUS_SLAVE_ADDRESS=2
```

Do not put two heads with the same address on a live bus: both will answer a
unicast STATUS/READ request and corrupt the response. A future service command
can move the address into EEPROM, but it must only be enabled on an isolated
single-head service connection; it is intentionally not exposed on the shared
production bus in this first implementation.

### IAR source list

The following sources must be compiled, and the listed include directory must
be added. `EWARM/taos.ewp` is updated accordingly.

```text
Src/modbus_port.c
Src/modbus_slave.c
Inc/port.h
Middlewares/Third_Party/FreeModbus/modbus/mb.c
Middlewares/Third_Party/FreeModbus/modbus/rtu/mbcrc.c
Middlewares/Third_Party/FreeModbus/modbus/rtu/mbrtu.c
Middlewares/Third_Party/FreeModbus/modbus/functions/mbfuncholding.c
Middlewares/Third_Party/FreeModbus/modbus/functions/mbfuncinput.c
Middlewares/Third_Party/FreeModbus/modbus/functions/mbutils.c

Middlewares/Third_Party/FreeModbus/modbus/include
```

## Protocol contract: mapping the requested SNAP / STATUS / READ scenario

The requested words are mapped to standard Modbus functions. This matters
because a Modbus RTU broadcast has no response by specification; asking every
slave to send `ACK` to broadcast would create the collision the bus is intended
to avoid.

| Pseudocode action | RTU transaction | Response |
|---|---|---|
| `ESP32 all: SNAP,seq=42` | unit `0`, **FC06** Write Single Holding Register, address `0`, value `42` | None (broadcast) |
| `ESP32 -> head 1: STATUS,1,42` | unit `1`, **FC04** Read Input Registers, start `0`, count `16` | FC04 data. This is the `ACK` when `VALID=1` and `sequence=42`. |
| `ESP32 -> head 2: STATUS,2,42` | unit `2`, FC04, start `0`, count `16` | Same, independently. |
| `ESP32 -> head N: READ,N,42` | unit `N`, FC04 raw-data chunks `start=16,count=125` then `start=141,count=3` | Frozen `buf_x0[0..127]`; metadata is the STATUS response cached for the same sequence. |

The ESP32 project executes exactly this state machine for heads 1 and 2. It
performs the DATA reads only if both unicast STATUS responses acknowledge the
same sequence. A broadcast FC06 is a write and is processed by every STM32;
FreeModbus suppresses replies for unit 0 automatically.

### Input-register map (FC04)

Addresses below are zero-based PDU addresses, as passed to ESP-Modbus. Tools
that display `3xxxx` references may show a one-based or 30001-based rendering;
do not add that rendering offset to code.

| Address | Name | Encoding |
|---:|---|---|
| 0 | `status` | bit 0 `VALID`; bit 1 sensor algorithm reported `errorflag`; bit 2 raw ADC record included; bit 3 live record was ready |
| 1 | `sequence` | last accepted `SNAP` sequence, `uint16` |
| 2 | `slave_id` | compiled slave ID |
| 3 | `sector` | signed `int16` |
| 4 | `angle_cdeg` | unsigned angle in 0.01 degree, `0..35999` |
| 5 | `startpixel_1` | `uint16` |
| 6 | `startpixel_2` | `uint16` |
| 7–8 | `frame_number` | 32-bit counter, high word then low word |
| 9 | `codeword` | decoded optical code word (`data_byte`) |
| 10 | `temperature_cdeg` | signed temperature in 0.01 C |
| 11 | `raw_sample_count` | always 128 in protocol v1 |
| 12 | `protocol_version` | `0x0100` |
| 13–15 | reserved | read as zero; preserve for compatible future metadata |
| 16–143 | `raw_pixels[0..127]` | frozen 12-bit ADC values from `buf_x0`, one value per register |

Holding register address `0` is write-only **`SNAP_SEQUENCE`** (FC06). There
are no other writable production registers in this version.

A head only sets `VALID` after at least one complete encoder frame has been
published. Therefore the ESP32 treats missing `VALID`, a wrong sequence, a
Modbus timeout, or an exception as *no ACK*. A sensor error flag still means
that the record was frozen consistently; policy may either log it or reject
that measurement at the master.

## ESP32 master setup

`esp32_master/` is a directly buildable ESP-IDF project:

```bash
cd esp32_master
idf.py set-target esp32
idf.py build flash monitor
```

The initial pin proposal in `esp32_master/main/encoder_master.c` is UART2
`TX=GPIO17`, `RX=GPIO16`, `RTS=GPIO4`, 115200 8N1. Change those three macros
for the actual ESP32 board. RTS must connect to the converter direction input;
never leave DE permanently asserted.

The first raw block is 125 registers and the second is 3 because the standard
Modbus read-register maximum is 125. If raw ADC payload is not needed in a
particular experiment, STATUS alone supplies angle/sector/frame metadata and
the two DATA requests can be skipped to reduce bus time.

## Bring-up checklist

1. With only one STM32 connected, verify the USART pins/DE polarity on a logic
   analyzer. At idle DE is receive mode; no STM32 drives A/B.
2. Give that STM32 address 1 and run ESP32 master. Confirm broadcast SNAP has
   no slave response, then FC04 unit 1 returns `VALID` and the same sequence.
3. Add the second STM32 programmed with address 2. Confirm STATUS 1 and 2 are
   two separate response frames and only one driver is active per response.
4. Confirm a raw read yields exactly 128 values and `frame_number` remains the
   one recorded in STATUS. Rotate the shaft and confirm `angle_cdeg` and sector
   change only on the next broadcast SNAP.
5. Test disconnect/noise cases: the ESP32 must report a failed STATUS and skip
   DATA, then recover at the next sequence without resetting the bus.
