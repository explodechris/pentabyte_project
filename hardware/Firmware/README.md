# Firmware — ESP32 Audio Capture & Playback

PlatformIO project for the Pentabyte ESP32 hardware prototype. The firmware handles push-to-talk audio recording via an I2S MEMS microphone, WAV file storage to SD card, and audio playback through an I2S DAC/speaker. Visual feedback is provided via a 128×128 SSD1351 OLED display.

---

## Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- ESP32 DevKit (38-pin) connected via USB
- All hardware components wired per the pin assignments below

---

## Setup & Flash

1. **Open the project** in VS Code with the PlatformIO extension, or use the CLI:

   ```bash
   cd hardware/Firmware
   ```

2. **Install dependencies** (PlatformIO fetches these automatically on first build):

   ```
   adafruit/Adafruit GFX Library @ ^1.11.9
   adafruit/Adafruit SSD1351 library @ ^1.3.2
   adafruit/Adafruit BusIO @ ^1.16.0
   esphome/ESP32-audioI2S @ ^2.0.7
   ```

3. **Build and upload:**

   ```bash
   pio run --target upload
   ```

4. **Monitor serial output** (115200 baud):

   ```bash
   pio device monitor
   ```

---

## SD Card Setup

Before first boot, the SD card must contain a valid FAT32 filesystem. Optionally, place a file named `test.mp3` in the root directory — the firmware will attempt to play it on startup to verify audio output.

The firmware writes/overwrites a file called `echo.wav` in the root directory on each recording.

---

## Pin Assignments

### Display — SSD1351 OLED (VSPI)

| Signal | GPIO |
|--------|------|
| MOSI | 23 (default VSPI) |
| CLK | 18 (default VSPI) |
| CS | 5 |
| DC | 21 |
| RST | 22 |

### SD Card (HSPI — secondary SPI bus)

| Signal | GPIO |
|--------|------|
| SCK | 14 |
| MISO | 19 |
| MOSI | 13 |
| CS | 15 |

> The SD card is assigned to the HSPI bus to avoid sharing the SPI bus with the display.

### I2S Microphone (I2S port 1)

| Signal | GPIO |
|--------|------|
| SCK (BCLK) | 16 |
| WS (LRCK) | 17 |
| SD (data in) | 34 |

### I2S Speaker / DAC (I2S port 0)

| Signal | GPIO |
|--------|------|
| BCLK | 26 |
| LRC | 27 |
| DOUT | 25 |

### Button

| Signal | GPIO | Mode |
|--------|------|------|
| Record | 4 | INPUT_PULLUP (active LOW) |

---

## Firmware Behavior

| State | Display | Action |
|-------|---------|--------|
| Boot | Blue screen | Initializes screen, SD card, speaker |
| Idle | "HOLD BUTTON / TO SPEAK" (white on black) | Waits for button press |
| Recording | "RECORDING" (white on red) | Captures I2S audio, streams to SD |
| Playback | "PLAYBACK" (green screen) | Plays `echo.wav` back through speaker |
| SD failure | Red screen + "SD FAIL!" | Halts — check SD card connection |

### Recording Pipeline

1. Button held (GPIO4 pulled LOW)
2. Opens `/echo.wav` on SD card (overwrites previous recording)
3. Writes 44-byte WAV header (placeholder sizes)
4. Reads 32-bit I2S samples in 256-sample chunks via DMA
5. Downsamples to 16-bit (right-shifts raw 32-bit by 14 bits, clips to int16 range)
6. Writes 16-bit PCM samples to file
7. Button released → finalizes WAV header with correct file sizes → plays back immediately

---

## Known Limitations

- No Wi-Fi or network stack in current firmware — audio is stored locally only, not sent to the AI backend
- Recording length is limited only by SD card space (no max duration enforced)
- `echo.wav` is overwritten on each new recording
- Serial debug output (`printf`) is active — disable for production use
