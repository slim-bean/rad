#include <avr/io.h>
#include "click.h"

/*
 * Geiger click via Timer1 PWM on OC1A (PB5 → piezo buzzer).
 *
 * Timer1 in phase/frequency-correct PWM mode, TOP = ICR1.
 * At F_CPU = 1 MHz, ICR1 = 125 gives ~4 kHz square wave.
 * OCR1A controls duty cycle (lower = louder).
 *
 * Call click_start() to begin the tone, click_stop() to silence.
 * The main loop handles the ~1.5 ms duration between calls.
 */

#define CLICK_FREQ_TOP  125
#define CLICK_VOLUME    60


void click_init(void)
{
    DDRB |= (1 << PB5);
    PORTB |= (1 << PB5);
    TCCR1A = 0;
    TCCR1B = 0;
}


void click_start(void)
{
    TCNT1 = 0;
    ICR1 = CLICK_FREQ_TOP;
    OCR1A = CLICK_VOLUME;
    TCCR1A = (1 << COM1A1);
    TCCR1B = (1 << WGM13) | (1 << CS10);
}


void click_stop(void)
{
    TCCR1A = 0;
    TCCR1B = 0;
    PORTB |= (1 << PB5);
}
