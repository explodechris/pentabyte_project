# Hardware — ESP32 Audio Prototype

This directory contains all hardware assets for the Pentabyte physical device prototype: PCB design files and ESP32 firmware.

---

## Directory Structure

```
hardware/
├── Firmware/                  # PlatformIO project (ESP32 firmware)
│   ├── main.cpp               # Application source
│   └── platformio.ini         # Build configuration
├── PCB_files/                 # Eagle PCB design files
│   ├── Pentabyte_PCB.brd      # Board layout
│   └── Pentabyte_PCB_schematic.sch  # Schematic
├── Breadboard_prototype - 1.png     # Early breadboard layout
├── Integrated_hardware_1.png        # Assembled prototype photos
├── Integrated_hardware_2.png
└── Integrated_hardware_3.png
```

---

## Hardware Overview

The device is a push-to-talk audio recorder and playback unit. Pressing and holding the button triggers recording via the I2S microphone; releasing saves the file to the SD card and immediately plays it back through the speaker. An OLED display shows the current state.

### Bill of Materials (Core Components)

| Component | Part / Spec | Interface |
|-----------|-------------|-----------|
| Microcontroller | ESP32 (38-pin DevKit) | — |
| Display | SSD1351 128×128 OLED | VSPI (MOSI=23, CLK=18, CS=5, DC=21, RST=22) |
| Microphone | I2S MEMS microphone | I2S1 (SCK=16, WS=17, SD=34) |
| Speaker amp + speaker | I2S DAC module | I2S0 (BCLK=26, LRC=27, DOUT=25) |
| SD card module | SPI | HSPI (SCK=14, MISO=19, MOSI=13, CS=15) |
| Push button | Momentary, normally open | GPIO4 (INPUT_PULLUP) |
| Power | 3.3V / 5V via USB or LiPo | — |

> **Note:** The display uses the primary VSPI bus; the SD card is on the secondary HSPI bus to avoid bus contention.

### Audio Specifications

| Parameter | Value |
|-----------|-------|
| Sample rate | 16 kHz |
| Bit depth | 16-bit (downsampled from 32-bit I2S raw) |
| Channels | Mono |
| Output format | WAV (standard PCM, 44-byte header) |
| Gain | Right-shift by 14 bits from 32-bit I2S raw |

---

## Subfolders

- **[`Firmware/`](Firmware/README.md)** — PlatformIO project. Flash this to the ESP32.
- **[`PCB_files/`](PCB_files/README.md)** — Eagle `.brd` and `.sch` files for the custom PCB.

---

## Current Prototype State

The firmware implements standalone audio capture and playback. Integration with the web coaching backend (sending audio to the Gemini API, receiving AI-generated audio responses) is not yet implemented in the firmware — the web application handles that pipeline independently via the browser.
