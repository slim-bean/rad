#ifndef TIMER0_H
#define TIMER0_H

#include <stdbool.h>

typedef void (*timer_callback_t)(void);

#define TIMER0_NUM_CALLBACKS        4
#define TIMER0_NUM_COUNTDOWNTIMERS  4

void Timer0_Init(void);
bool Timer0_RegisterCallbackFunction(timer_callback_t pFunc);
bool Timer0_RemoveCallbackFunction(timer_callback_t pFunc);

uint8_t Timer0_AllocateCountdownTimer(void);
uint8_t Timer0_GetCountdownTimer(uint8_t timer);
void    Timer0_SetCountdownTimer(uint8_t timer, uint8_t value);
void    Timer0_ReleaseCountdownTimer(uint8_t timer);

#endif
