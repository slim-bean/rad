#include <avr/io.h>
#include <avr/interrupt.h>
#include "geiger.h"

static volatile uint8_t  bin_counts[GEIGER_WINDOW_SECONDS];
static volatile uint8_t  current_bin;
static volatile uint8_t  current_bin_count;
static volatile uint32_t total_count;
static volatile uint8_t  noise_flag;
static volatile uint8_t  click_flag;
static volatile uint8_t  last_sig_state;
static volatile uint8_t  seconds_elapsed;


void geiger_init(void)
{
    uint8_t i;
    for (i = 0; i < GEIGER_WINDOW_SECONDS; i++)
        bin_counts[i] = 0;

    current_bin = 0;
    current_bin_count = 0;
    total_count = 0;
    noise_flag = 0;
    click_flag = 0;
    seconds_elapsed = 0;

    last_sig_state = PINE & (1 << GEIGER_SIG_PIN);

    /* Timer2: async from 32.768 kHz crystal, prescaler 128 → 1 Hz overflow */
    TIMSK2 = 0;
    ASSR = (1 << AS2);
    TCNT2 = 0;
    TCCR2A = (1 << CS22) | (1 << CS20);
    while (ASSR & ((1 << TCN2UB) | (1 << TCR2UB)))
        ;
    TIFR2 = (1 << TOV2);
    TIMSK2 = (1 << TOIE2);
}


/*
 * Called from PCINT0 ISR (PORTE pin-change).
 * Detects falling edge on SIG (PE4) and checks NS (PE5).
 */
void geiger_pin_isr(void)
{
    uint8_t pins = PINE;
    uint8_t sig_now = pins & (1 << GEIGER_SIG_PIN);

    noise_flag = !(pins & (1 << GEIGER_NS_PIN));

    if (!sig_now && last_sig_state)
    {
        if (!noise_flag)
        {
            if (current_bin_count < 255)
                current_bin_count++;
            total_count++;
            click_flag = 1;
        }
    }

    last_sig_state = sig_now;
}


/*
 * Called once per second from Timer2 overflow ISR.
 * Advances the sliding window.
 */
void geiger_second_tick(void)
{
    current_bin = (current_bin + 1) % GEIGER_WINDOW_SECONDS;
    bin_counts[current_bin] = current_bin_count;
    current_bin_count = 0;

    if (seconds_elapsed < GEIGER_WINDOW_SECONDS)
        seconds_elapsed++;
}


ISR(TIMER2_OVF_vect)
{
    geiger_second_tick();
}


uint16_t geiger_get_cpm(void)
{
    uint16_t sum = 0;
    uint8_t i;

    cli();
    uint8_t elapsed = seconds_elapsed;
    for (i = 0; i < GEIGER_WINDOW_SECONDS; i++)
        sum += bin_counts[i];
    sum += current_bin_count;
    sei();

    if (elapsed == 0)
        return 0;

    if (elapsed < GEIGER_WINDOW_SECONDS)
        return (uint16_t)(((uint32_t)sum * 60UL) / elapsed);

    return sum;
}


uint16_t geiger_get_dose_x100(void)
{
    uint16_t cpm = geiger_get_cpm();
    return (uint16_t)(((uint32_t)cpm * 100UL) / GEIGER_CPM_PER_USV);
}


uint32_t geiger_get_total(void)
{
    uint32_t t;
    cli();
    t = total_count;
    sei();
    return t;
}


bool geiger_is_noisy(void)
{
    return noise_flag != 0;
}


bool geiger_click_pending(void)
{
    return click_flag != 0;
}


void geiger_click_ack(void)
{
    click_flag = 0;
}


void geiger_reset(void)
{
    uint8_t i;
    cli();
    for (i = 0; i < GEIGER_WINDOW_SECONDS; i++)
        bin_counts[i] = 0;
    current_bin = 0;
    current_bin_count = 0;
    total_count = 0;
    seconds_elapsed = 0;
    sei();
}
