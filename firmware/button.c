#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"
#include "button.h"
#include "timer0.h"
#include "geiger.h"

volatile uint8_t gButtonTimeout = 0;

static volatile char KEY = 0;
static volatile char KEY_VALID = 0;


void Button_Init(void)
{
    DDRB &= ~((1 << PB4) | (1 << PB6) | (1 << PB7));
    PORTB |= PINB_MASK;
    DDRE &= ~(PINE_MASK | (1 << PE4) | (1 << PE5));
    PORTE |= PINE_MASK | (1 << PE4) | (1 << PE5);

    PCMSK0 = PINE_MASK | (1 << PINE4) | (1 << PINE5);
    PCMSK1 = PINB_MASK;
    EIFR = (1 << PCIF0) | (1 << PCIF1);
    EIMSK = (1 << PCIE0) | (1 << PCIE1);
}


ISR(PCINT0_vect)
{
    geiger_pin_isr();
    PinChangeInterrupt();
}


ISR(PCINT1_vect)
{
    PinChangeInterrupt();
}


void PinChangeInterrupt(void)
{
    uint8_t buttons;
    char key;

    buttons = (~PINB) & PINB_MASK;
    buttons |= (~PINE) & PINE_MASK;

    if (buttons & (1 << BUTTON_A))
        key = KEY_PLUS;
    else if (buttons & (1 << BUTTON_B))
        key = KEY_MINUS;
    else if (buttons & (1 << BUTTON_C))
        key = KEY_PREV;
    else if (buttons & (1 << BUTTON_D))
        key = KEY_NEXT;
    else if (buttons & (1 << BUTTON_O))
        key = KEY_ENTER;
    else
        key = KEY_NULL;

    if (key != KEY_NULL)
    {
        if (gButtonTimeout)
        {
            if (!KEY_VALID)
            {
                KEY = key;
                KEY_VALID = 1;
            }
            gButtonTimeout = 0;
        }
    }

    EIFR = (1 << PCIF1) | (1 << PCIF0);
}


char getkey(void)
{
    char k;

    cli();

    if (KEY_VALID)
    {
        k = KEY;
        KEY_VALID = 0;
    }
    else
        k = KEY_NULL;

    sei();

    return k;
}
