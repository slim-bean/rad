#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define PINB_MASK ((1 << PINB4) | (1 << PINB6) | (1 << PINB7))
#define PINE_MASK ((1 << PINE2) | (1 << PINE3))

#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN
#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT
#define BUTTON_O    4   // PUSH

void Button_Init(void);
void PinChangeInterrupt(void);
char getkey(void);

#endif
