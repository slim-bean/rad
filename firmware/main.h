#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include <stdbool.h>

#define KEY_NULL    0
#define KEY_ENTER   1
#define KEY_NEXT    2
#define KEY_PREV    3
#define KEY_PLUS    4
#define KEY_MINUS   5

typedef enum {
    DISP_CPM,
    DISP_DOSE,
    DISP_COUNT,
    DISP_MODE_COUNT
} display_mode_t;

void delay_ms(uint16_t ms);

#endif
