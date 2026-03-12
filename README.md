# RAD — ESP32 Geiger Counter

Firmware for a DFRobot FireBeetle 2 ESP32-E (DFR0654) with a 2" IPS TFT
display (DFR0664) and a [Radiation Watch Pocket Geiger Type 5](https://www.radiation-watch.org/)
sensor. Shows a real-time radiation dashboard with CPM, dose rates in both
uSv/h and mrem/h, and produces a classic Geiger counter click through a
piezo buzzer.

## Hardware

| Component | Part | Notes |
|-----------|------|-------|
| MCU board | DFRobot FireBeetle 2 ESP32-E (DFR0654) | LiPo connector, GDI display port |
| Display | DFRobot 2" IPS TFT (DFR0664) | 320x240, ST7789, connects via GDI cable |
| Sensor | Radiation Watch Pocket Geiger Type 5 | 3V, SIG + NS outputs |
| Buzzer | Adafruit piezo #1739 | Driven by LEDC PWM |
| Battery | 3.7V LiPo, 1000-2000mAh | JST PH2.0 connector |

## Wiring

The display connects to the FireBeetle via the GDI FPC ribbon cable
(included with the DFR0664) — no soldering needed for the display.
The LiPo plugs into the FireBeetle's JST connector.

Only the sensor and piezo need wires:

```
Pocket Geiger Type 5       FireBeetle 2 ESP32-E
────────────────────       ────────────────────
VCC  ───────────────────── 3V3
GND  ───────────────────── GND
SIG  ───────────────────── GPIO 4  (D12)
NS   ───────────────────── GPIO 13 (D7)

Adafruit Piezo #1739
────────────────────
+    ───────────────────── GPIO 15 (A4)
-    ───────────────────── GND
```

## Building

Requires [PlatformIO](https://platformio.org/).

```bash
cd esp32
pio run              # build
pio run -t upload    # flash via USB
pio device monitor   # serial log
```

## Display Dashboard

Everything is visible at once on the 320x240 color TFT — no mode cycling
needed:

```
+----------------------------------+
|                                  |
|    0   0   0   4   2             |  <-- Large 7-segment CPM
|                                  |
|  CPM                             |
|                                  |
|  uSv/h            mrem/h        |
|   0.79             0.079         |
|                                  |
|  [########.................] CPM |  <-- Color bar graph
|                                  |
|  N: 1234                         |
+----------------------------------+
```

- **CPM digits** change color: green (normal), yellow (>100), red (>1000)
- **NOISE** banner appears in red when the sensor detects vibration
- Clicking on the piezo tracks each radiation event

## Battery Life

| Battery | Estimated runtime |
|---------|-------------------|
| 1000mAh | ~14 hours |
| 2000mAh | ~27 hours |

Power budget: ~42mA (ESP32) + ~29mA (TFT) + ~2mA (sensor + piezo).

## Project Layout

```
firmware/       AVR Butterfly version (ATmega169, original prototype)
esp32/          ESP32 FireBeetle version (current)
  platformio.ini
  src/
    main.c      Entry point, display update loop, dashboard layout
    display.*   ST7789 init via esp_lcd, 7-segment + bitmap font rendering
    geiger.*    Pocket Geiger Type 5 driver, 60s sliding window CPM
    click.*     Piezo click via LEDC PWM
    pins.h      All GPIO assignments (GDI + sensor + piezo)
atmel/          Original Atmel AVR Butterfly sample application (IAR)
```

## References

- [DFR0654 FireBeetle 2 ESP32-E Wiki](https://wiki.dfrobot.com/FireBeetle_Board_ESP32_E_SKU_DFR0654)
- [DFR0664 2" IPS TFT Wiki](https://wiki.dfrobot.com/2.0_Inches_320_240_IPS_TFT_LCD_Display_with_MicroSD_Card_Breakout_SKU_DFR0664)
- [Pocket Geiger Type 5](https://www.radiation-watch.org/p/pocketgeiger-type5.html)
- [ESP-IDF esp_lcd API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd/)
