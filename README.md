# MemoryHub Firmware

Firmware for the board in the FirstPCB repository. This project targets the
STM32G0B1 microcontroller and provides a menu-driven interface for display
animation, sensor monitoring, SD card media, settings storage, and EEPROM
emulation.

## Features

- ST7735 display driver and menu interface
- SD card image animation
- Temperature and humidity monitoring
- Persistent settings stored in emulated EEPROM
- Adjustable display brightness
- Configurable animation and sensor update intervals
- Firmware `.elf`, `.hex`, and `.bin` build outputs

## Firmware configuration

- Target MCU: STM32G0B1
- System clock: 64 MHz
- SPI3: display communication
- SPI2: SD card communication
- I2C2: sensor communication
- TIM2 Channel 2: display PWM backlight

## Building

The project uses CMake, Ninja, and the ARM GNU embedded toolchain.

Configure and build the Debug firmware:

```powershell
cmake --preset Debug
cmake --build --preset Debug
```

Configure and build the Release firmware:

```powershell
cmake --preset Release
cmake --build --preset Release
```

Build outputs are placed in `build/Debug/` or `build/Release/`. The post-build
step generates both `MemoryHub.bin` and `MemoryHub.hex` in addition to the ELF
file.

## Release notes

### Version 1.2.2

#### Added

- PWM control for the display backlight through TIM2 Channel 2.
- Five saved brightness levels: 5%, 10%, 30%, 40%, and maximum.
- Automatic generation of `.bin` and `.hex` firmware files after a successful build.
- HAL timer support for TIM2.

#### Changed

- Increased the system clock frequency from 30 MHz to 64 MHz.
- Updated I2C, SPI, DMA, flash, and peripheral timing for the 64 MHz configuration.
- Reduced SPI2 and SPI3 baud-rate prescalers to match the updated clock.
- Updated display settings handling to apply brightness through PWM.
- Updated brightness settings storage and display names.
- Updated the J-Link GDB server path used by the VS Code debug configuration.
- Removed obsolete direct-ST7735 source files and legacy GPIO backlight switching.

#### Removed

- Removed unnecessary legacy display code left from the original ST7735
  implementation.
