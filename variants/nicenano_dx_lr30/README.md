# LusoFW — TENSTAR ProMicro nRF52840 + DX-LR30 repeater / BLE companion

Guia em português para repetidor e companion BLE:
[bootloader e ligações do rádio](../../docs/guia_nicenano_dx_lr30.md).

Hardware: red TENSTAR ROBOT ProMicro nRF52840, S140 6.1.1.
Tested bootloader sequence: nice!nano 0.6.0 → nice!nano 0.9.2 →
ProMicro 0.9.2-OTAFIX2.2-BP1.3. Uses the existing ProMicro board definition with a dedicated
Arduino variant and the S140 v6 extra-filesystem linker script.

| DX-LR30 signal | Nordic GPIO | Board marking | Arduino index |
| --- | --- | --- | --- |
| SCK | P1.11 | 111 | 12 |
| MOSI | P1.15 | 115 | 14 |
| MISO | P0.02 | 002 | 15 |
| NSS | P1.13 | 113 | 13 |
| BUSY | P0.29 | 029 | 16 |
| DIO1 | P0.20 | 020 | 3 |
| RESET | P0.22 | 022 | 4 |
| TXEN | P0.24 | 024 | 5 |
| RXEN | P0.17 | 017 | 2 |
| GND | GND | GND | - |

Radio RESET connects to P0.22, not the board RST pad.
TCXO control is explicitly disabled (0 V); DIO2 RF switching is disabled.
RadioLib controls RXEN/TXEN using its standard active-high switch sequence.
Repeater forwarding has been confirmed on hardware. Supply margin under
transmission load has not been measured across all operating conditions.
The firmware enables the external ME6217 3.3 V regulator via P0.13
(Arduino index 21), then waits 10 ms before radio initialization.
Verify 3.3 V at the board VCC pad before connecting module power.

No display, GPS, external RTC or environmental sensors are enabled.
Wire is initialized on P1.00 (SDA) and P0.11 (SCL) before the shared
sensor manager scans I2C, including when no sensor drivers are enabled.
Battery reporting measures internal VDDH/5 (eight 12-bit samples, 1.2 V
reference), for a battery connected to B+/B- with USB disconnected.
USB power or non-VDDH mode returns 0 (unavailable), not USB voltage as battery.
Repeater CLI command `battery` reports millivolts or an explicit unavailable
message. Standard status/telemetry uses the same reading. With USB connected
use the CLI to check the unavailable message; validate the actual battery
reading remotely with USB disconnected against a multimeter at B+/B-.
Hardware accuracy and the supply path still require this bench validation.
The internal RC low-frequency clock follows the existing ProMicro variant.
Initial SX1262 power is 14 dBm; this is not a calibrated DX-LR30 module output
power. Radio defaults: 869.618 MHz, BW 62.5 kHz, SF7, coding rate 4/6.
The module manual warns about frequency drift below 125 kHz bandwidth;
62.5 kHz is selected to match the requested network settings.
The initial admin password is `password`; configure it before deployment.

## PlatformIO

Local serial diagnostic: send `powerregs` followed by CR at 115200.
COM29 is the current environment default; select the actual port assigned
to your board (COM31 was also observed during testing).
It reads REGOUT0, DCDCEN0, DCDCEN and MAINREGSTATUS and prints hexadecimal
values plus decoded voltage setting, DC/DC enable bits and VDD/VDDH mode.
This does not measure actual voltage or change power configuration.

Open the project root containing platformio.ini in VS Code.
platformio.local.ini selects nicenano_dx_lr30_repeater as the default.
In PlatformIO > Project Tasks > nicenano_dx_lr30_repeater > General,
Build compiles the firmware and automatically generates firmware.uf2.
The environment enables this with `custom_auto_uf2 = yes`.
The explicit UF2 command also remains available in the PlatformIO terminal:

```powershell
pio run -e nicenano_dx_lr30_repeater -t create_uf2
```

Output: .pio/build/nicenano_dx_lr30_repeater/firmware.uf2.
After hardware validation, enter the UF2 bootloader and copy the UF2 to
its USB drive. Compilation and UF2 generation are validated; operation on
hardware must be checked again after each firmware change.
