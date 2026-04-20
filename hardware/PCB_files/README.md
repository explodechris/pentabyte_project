# PCB Design Files

Custom PCB for the Pentabyte ESP32 hardware prototype, designed in Autodesk Eagle.

---

## Files

| File | Description |
|------|-------------|
| `Pentabyte_PCB.brd` | Eagle board layout — copper layers, component placement, routing |
| `Pentabyte_PCB_schematic.sch` | Eagle schematic — full circuit diagram |

---

## Opening the Files

**Eagle (Autodesk):**
- Open `.brd` in Eagle for the board layout
- Open `.sch` in Eagle for the schematic
- Eagle 7.x or later recommended

**KiCad (alternative):**
- Use the [Eagle-to-KiCad importer](https://docs.kicad.org/7.0/en/pcbnew/pcbnew.html#importing_eagle_files) built into KiCad 5.1+
- File → Import → Eagle Project

---

## Design Notes

The PCB integrates the core components of the prototype onto a single board:

- **ESP32** (main microcontroller)
- **SSD1351 OLED display** connector — VSPI bus (MOSI, CLK, CS, DC, RST)
- **SD card slot** — HSPI bus (separate from display SPI to avoid bus contention)
- **I2S MEMS microphone** footprint — I2S port 1
- **I2S DAC / speaker amplifier** connector — I2S port 0
- **Push button** — GPIO4 with pull-up

Refer to [`../Firmware/README.md`](../Firmware/README.md) for the full pin assignment table.

---

## Fabrication

Standard 2-layer PCB. To order:

1. Generate Gerber files from the `.brd` file (Eagle: File → CAM Processor → Gerber)
2. Upload Gerbers to your preferred PCB manufacturer (JLCPCB, PCBWay, OSH Park, etc.)
3. Standard 1.6mm FR4, 1oz copper, HASL or ENIG finish

---

## Prototype Photos

See the images in [`../`](../) for the breadboard prototype and assembled hardware.
