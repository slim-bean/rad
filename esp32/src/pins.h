#pragma once

/*
 * DFR0654 FireBeetle 2 ESP32-E  +  DFR0664 IPS TFT via GDI cable
 *
 * GDI FPC carries SPI + control + power; no soldering for the display.
 * DFRobot "D" labels to ESP32 GPIO mapping taken from the DFR0654 wiki.
 */

/* ---- TFT (ST7789, 320x240, SPI via GDI) ---- */
#define PIN_TFT_SCLK   18          /* GDI → SCK  */
#define PIN_TFT_MOSI   23          /* GDI → MOSI */
#define PIN_TFT_DC      25         /* GDI → D2   */
#define PIN_TFT_CS      14         /* GDI → D6   */
#define PIN_TFT_RST     26         /* GDI → D3   */
#define PIN_TFT_BL      12         /* GDI → D13  */

#define TFT_WIDTH       320
#define TFT_HEIGHT      240
#define TFT_SPI_HOST    SPI3_HOST
#define TFT_SPI_FREQ_HZ (40 * 1000 * 1000)

/* ---- Pocket Geiger Type 5 ---- */
#define PIN_GEIGER_SIG  4           /* D12 — radiation pulse (active LOW) */
#define PIN_GEIGER_NS   13          /* D7  — noise indicator (active HIGH) */

/* ---- Piezo buzzer (Adafruit #1739) ---- */
#define PIN_PIEZO       15          /* A4  — LEDC PWM output */

/* ---- User acknowledge button ---- */
#define PIN_ACK_BUTTON  27          /* NO switch to GND via 220 ohm, active LOW */

/* ---- Battery voltage ---- */
#define PIN_VBAT        34          /* A2  — input-only, 1M/1M divider to VBAT */
