# RAD — AVR Butterfly Geiger Counter

Firmware for an AVR Butterfly (ATmega169) connected to a
[Radiation Watch Pocket Geiger Type 5](https://www.radiation-watch.org/)
sensor. Displays radiation measurements on the onboard LCD and produces
a classic Geiger counter "click" through the piezo buzzer.

## Hardware

| Component | Description |
|-----------|-------------|
| **MCU board** | Atmel AVR Butterfly (ATmega169, 1 MHz) |
| **Sensor** | Radiation Watch Pocket Geiger Type 5 |
| **Programmer** | Atmel AVRISP mkII (ISP) |

## Wiring

Connect the Pocket Geiger Type 5 to the AVR Butterfly J1 header:

```
Pocket Geiger          AVR Butterfly (J1 header)
─────────────          ─────────────────────────
VCC  ───────────────── VCC  (3.3 V)
GND  ───────────────── GND
SIG  ───────────────── PE4  (pin-change interrupt, radiation pulse)
NS   ───────────────── PE5  (noise/vibration indicator)
```

The Type 5 runs at 3 V, matching the Butterfly's supply voltage — no
level shifting needed.

### ISP Connection

Connect the AVRISP mkII to the Butterfly's 6-pin ISP header (J403).
The AVRISP mkII's 6-pin cable plugs straight in.

## Building

Prerequisites: `avr-gcc`, `avr-libc`, `avrdude`.

```bash
# Debian / Ubuntu
sudo apt install gcc-avr avr-libc avrdude

# Build
cd firmware
make

# Flash via AVRISP mkII
make flash
```

The build targets ATmega169 at 1 MHz (internal 8 MHz RC / 8 prescaler,
calibrated against the 32.768 kHz watch crystal on the Butterfly).

## Usage

Once flashed, the display cycles through three screens using the
joystick:

| Button | Action |
|--------|--------|
| **UP / RIGHT** | Next display mode |
| **DOWN / LEFT** | Previous display mode |
| **PUSH (center)** | Reset counters |

### Display Modes

1. **CPM** — Counts per minute (e.g., `CPM 42`). Uses a 60-second
   sliding window that scales up during the first minute.
2. **µSv/h** — Dose rate (e.g., ` 0:79UV` for 0.79 µSv/h). The LCD
   colon acts as a decimal point. Conversion uses the Type 5 Cs-137
   sensitivity factor (53 CPM per µSv/h).
3. **mrem/h** — Dose rate in millirems (e.g., ` 0:079R` for 0.079
   mrem/h). Same measurement, traditional US unit (1 µSv = 0.1 mrem).
4. **Count** — Total accumulated detections (e.g., `N  123`).

If the sensor detects vibration / noise, the display shows `NOISE`
until the disturbance passes. Counts during noise are discarded.

Every detected radiation event produces a short click on the piezo
buzzer, just like a real Geiger counter.

## Project Layout

```
atmel/          Original Atmel AVR Butterfly sample application (IAR)
firmware/       Geiger counter firmware (avr-gcc)
  main.c        Entry point, display state machine
  lcd_driver.*  Low-level LCD segment driver (ported from Atmel sample)
  lcd.*         High-level LCD string/number helpers
  button.*      Joystick input via pin-change interrupts
  timer0.*      Timer0 callback infrastructure
  click.*       Piezo click sound via Timer1 PWM
  geiger.*      Pocket Geiger Type 5 driver + CPM sliding window
  Makefile      Build & flash targets
```

## References

- [AVR Butterfly Hardware User Guide](https://ww1.microchip.com/downloads/en/DeviceDoc/doc4271.pdf)
- [ATmega169 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8018-8-bit-AVR-Microcontroller-ATmega169P_datasheet.pdf)
- [Pocket Geiger Type 5 Specifications](https://www.radiation-watch.org/p/pocketgeiger-type5.html)
