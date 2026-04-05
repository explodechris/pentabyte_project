# Pentabyte — Voice AI Coach

A voice-first AI coaching application for career and academic goal clarification. The project consists of two components: a **web application** (software) and an **ESP32-based hardware prototype** (physical device).

---

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

This repository is a fork. The web application was developed by the original author. The `hardware/` directory (PCB design, firmware) was added separately as the embedded hardware prototype.
