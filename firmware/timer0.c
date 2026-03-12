#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"

static timer_callback_t CallbackFunc[TIMER0_NUM_CALLBACKS];
static volatile uint8_t CountDownTimers[TIMER0_NUM_COUNTDOWNTIMERS];


void Timer0_Init(void)
{
    uint8_t i;

    for (i = 0; i < TIMER0_NUM_CALLBACKS; i++)
        CallbackFunc[i] = (void *)0;

    for (i = 0; i < TIMER0_NUM_COUNTDOWNTIMERS; i++)
        CountDownTimers[i] = 255;

    TIMSK0 = (1 << OCIE0A);
    OCR0A = 38;
    TCCR0A = (1 << WGM01) | (0 << WGM00) | (4 << CS00);
}


ISR(TIMER0_COMP_vect)
{
    uint8_t i;

    for (i = 0; i < TIMER0_NUM_CALLBACKS; i++)
        if (CallbackFunc[i] != (void *)0)
            CallbackFunc[i]();

    for (i = 0; i < TIMER0_NUM_COUNTDOWNTIMERS; i++)
        if (CountDownTimers[i] != 255 && CountDownTimers[i] != 0)
            CountDownTimers[i]--;
}


bool Timer0_RegisterCallbackFunction(timer_callback_t pFunc)
{
    uint8_t i;

    for (i = 0; i < TIMER0_NUM_CALLBACKS; i++)
        if (CallbackFunc[i] == pFunc)
            return true;

    for (i = 0; i < TIMER0_NUM_CALLBACKS; i++)
    {
        if (CallbackFunc[i] == (void *)0)
        {
            CallbackFunc[i] = pFunc;
            return true;
        }
    }

    return false;
}


bool Timer0_RemoveCallbackFunction(timer_callback_t pFunc)
{
    uint8_t i;

    for (i = 0; i < TIMER0_NUM_CALLBACKS; i++)
    {
        if (CallbackFunc[i] == pFunc)
        {
            CallbackFunc[i] = (void *)0;
            return true;
        }
    }

    return false;
}


uint8_t Timer0_AllocateCountdownTimer(void)
{
    uint8_t i;
    for (i = 0; i < TIMER0_NUM_COUNTDOWNTIMERS; i++)
        if (CountDownTimers[i] == 255)
        {
            CountDownTimers[i] = 0;
            return i + 1;
        }
    return 0;
}


void Timer0_SetCountdownTimer(uint8_t timer, uint8_t value)
{
    cli();
    CountDownTimers[timer - 1] = value;
    sei();
}


uint8_t Timer0_GetCountdownTimer(uint8_t timer)
{
    uint8_t t;
    cli();
    t = CountDownTimers[timer - 1];
    sei();
    return t;
}


void Timer0_ReleaseCountdownTimer(uint8_t timer)
{
    cli();
    CountDownTimers[timer - 1] = 255;
    sei();
}
