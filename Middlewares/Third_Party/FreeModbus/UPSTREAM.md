# Imported FreeModbus subset

* Upstream: <https://github.com/cwalter-at/freemodbus>
* Imported revision: `a9ec789a23ff774287e711aeb18492860c8770cf`
* License: BSD-3-Clause, reproduced in [`LICENSE.txt`](LICENSE.txt).

Only the RTU slave core and the FC04/FC06 dependencies required by this
project are included. `modbus/include/mbconfig.h` is intentionally configured
for RTU only, input-register reads and single holding-register writes. The
board-specific port is not upstream code; it is implemented in
`Src/modbus_port.c` and `Inc/port.h`.
