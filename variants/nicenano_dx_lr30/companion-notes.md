# Companion BLE — nRF52840 + DX-LR30

Environment: `nicenano_dx_lr30_companion_radio_ble`.
Uses the same board, radio wiring, external LDO enable and Wire initialization
as the repeater. No display or additional sensors are required.

Initial name: `Luso DX-LR30 Companion`; BLE prefix: `MeshCore-`.
Initial pairing PIN: `123456`. Previously saved preferences may override these.
Radio defaults: 869.618 MHz, BW 62.5 kHz, SF7, CR 4/6, TX 14 dBm.

In PlatformIO Project Tasks select this environment, then General > Build.
Alternatively:

```powershell
pio run -e nicenano_dx_lr30_companion_radio_ble
```

UF2 is generated automatically at:
`.pio/build/nicenano_dx_lr30_companion_radio_ble/firmware.uf2`.
The repeater remains the project's default build environment.

After flashing, connect from the MeshCore application using Bluetooth.
USB remains available for debug at 115200 baud; it is not the companion
transport in this build. The repeater's text command console (including
`powerregs`) is not provided by companion firmware.

The monitor port is not fixed to COM29 because a second board may receive a
different COM number. Select the detected port when opening the monitor.
Installing this UF2 replaces the running application on the destination board;
it does not make one board run repeater and companion simultaneously.
BLE pairing and radio operation require hardware validation.
