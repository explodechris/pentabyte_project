# Pentabyte — Voice AI Coach

A voice-first AI coaching application for career and academic goal clarification. The project consists of two components: a **web application** (software) and an **ESP32-based hardware prototype** (physical device).

---

## ESP32 Hardware Integration

The repository includes firmware at [pentabyte electronics/Pentabyte/src/main.cpp](pentabyte%20electronics/Pentabyte/src/main.cpp) that connects your physical device to the web backend over Wi-Fi.

### Supported hardware path

- Press and hold button (GPIO4) to record from I2S microphone
- Release button to send WAV audio to backend (`/api/esp32/turn`)
- Backend runs STT + coaching + TTS
- ESP32 receives MP3 response and plays it via I2S speaker

### Pin mapping used in firmware

- OLED SSD1351 (VSPI): MOSI=23, CLK=18, CS=5, DC=21, RST=22
- I2S microphone: SCK=16, WS=17, SD=34
- I2S speaker DAC: BCLK=26, LRC=27, DOUT=25
- SD card (HSPI): SCK=14, MISO=19, MOSI=13, CS=15
- Push button: GPIO4 (`INPUT_PULLUP`, active low)

### Configure firmware

Edit these constants in [pentabyte electronics/Pentabyte/src/main.cpp](pentabyte%20electronics/Pentabyte/src/main.cpp):

- `WIFI_SSID`
- `WIFI_PASS`
- `SERVER_URL` (your computer's LAN IP + Next.js port, for example `http://192.168.1.42:3000`)

Important: ESP32 and your computer must be on the same Wi-Fi network.

### Backend requirements

- `GEMINI_API_KEY` is required
- `ELEVENLABS_API_KEY` is required for hardware audio playback (the ESP32 endpoint returns MP3)
- Start server with `npm run dev`

### Flash firmware (PlatformIO)

From [pentabyte electronics/Pentabyte](pentabyte%20electronics/Pentabyte):

```bash
pio run -t upload
pio device monitor -b 115200
```

If `pio` is missing, install PlatformIO Core first:

```bash
python3 -m pip install -U platformio
```

### Quick test checklist

1. Start Next.js server and confirm it is reachable from another device on LAN.
2. Power ESP32 and confirm serial log shows Wi-Fi connected and session started.
3. Hold button while speaking, then release.
4. Confirm serial log shows audio upload and MP3 bytes received.
5. Confirm response plays from speaker and OLED state transitions (`REC` -> `THINKING...` -> `SPEAKING...`).

## Project Structure

```
pentabyte_project/
├── website/          # Next.js web application (AI coaching backend + frontend)
└── hardware/         # ESP32 hardware prototype (audio capture + playback device)
```

---

## Components

### Website — AI Coaching Web App

A browser-based, voice-driven coaching tool. The user selects personality traits that shape the coaching style, then holds a push-to-talk button to speak. Audio is transcribed by the Gemini API, processed by an LLM coaching engine, and the AI response is synthesized back to speech via ElevenLabs.

**Stack:** Next.js 14, TypeScript, Tailwind CSS, Gemini API (STT + LLM), ElevenLabs (TTS), Vercel

→ See [`website/README.md`](website/README.md) for setup and deployment instructions.

---

### Hardware — ESP32 Audio Prototype

A standalone embedded prototype built around an ESP32 microcontroller. The device captures audio via an I2S MEMS microphone (push-to-talk, hold button), encodes and stores it as a WAV file on an SD card, then plays it back through an I2S DAC and speaker. A 128×128 OLED display provides visual state feedback (idle, recording, playback).

This prototype was developed in parallel with the web application as a proof-of-concept for a self-contained physical coaching device, prior to hardware–software integration.

**Hardware:** ESP32, SSD1351 OLED display, I2S MEMS microphone, I2S DAC/speaker, SD card
**Firmware:** PlatformIO / Arduino framework (C++)
**PCB:** Custom board designed in Eagle (KiCad-compatible export available)

→ See [`hardware/README.md`](hardware/README.md) for build and instructions.

---

## Quick Start

**Web app:**
```bash
cd website
npm install
# add .env.local (see website/README.md)
npm run dev
```

**Hardware firmware:**
```bash
# Open hardware/Firmware/ in PlatformIO
# Connect ESP32 via USB
pio run --target upload
```

---

## Status

| Component | Status |
|-----------|--------|
| Web app | Functional — deployable to Vercel |
| Hardware firmware | Prototype — standalone audio record/playback working |
| Hardware–software integration | Not yet implemented |

---

## Team

[Sinem Unlu](https://github.com/sinem-unlu-sf)\
[Cristian Rosca](https://github.com/explodechris)\
Enkh-Oyun Tsend-Ayush\
Rida Habib\
Muhammad Habib
