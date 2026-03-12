#pragma once

#include <stdint.h>
#include <stdbool.h>

/* RGB565 color helpers */
#define RGB565(r, g, b) \
    ((uint16_t)(((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))

#define COL_BLACK   RGB565(0,   0,   0)
#define COL_WHITE   RGB565(255, 255, 255)
#define COL_GREEN   RGB565(0,   255, 0)
#define COL_DGREEN  RGB565(0,   180, 0)
#define COL_YELLOW  RGB565(255, 255, 0)
#define COL_RED     RGB565(255, 40,  40)
#define COL_GREY    RGB565(60,  60,  60)
#define COL_DGREY   RGB565(30,  30,  30)

void display_init(void);

void display_fill_rect(int x, int y, int w, int h, uint16_t color);
void display_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg, int scale);
void display_draw_string(int x, int y, const char *s, uint16_t fg, uint16_t bg, int scale);

/* Large 7-segment style digit (w=28, h=48 at scale=1) */
void display_draw_7seg(int x, int y, int digit, uint16_t fg, uint16_t bg, int scale);

void display_backlight(bool on);
