#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "main.h"
#include "lcd_driver.h"
#include "lcd.h"
#include "button.h"
#include "timer0.h"
#include "click.h"
#include "geiger.h"


static const char STR_GEIGER[] PROGMEM = "GEIGER";
static const char STR_NOISE[]  PROGMEM = "NOISE";
static const char STR_RESET[]  PROGMEM = "RESET";

static display_mode_t disp_mode = DISP_CPM;

static void OSCCAL_calibration(void);
static void init_hardware(void);
static void display_update(void);
static void display_cpm(void);
static void display_dose(void);
static void display_mrem(void);
static void display_count(void);


int main(void)
{
    char input;

    init_hardware();
    sei();

    LCD_puts_f(STR_GEIGER, 0);
    _delay_ms(1500);

    for (;;)
    {
        if (geiger_click_pending())
        {
            geiger_click_ack();
            click_start();
            _delay_us(1500);
            click_stop();
        }

        input = getkey();

        if (input == KEY_PLUS || input == KEY_NEXT)
        {
            disp_mode = (disp_mode + 1) % DISP_MODE_COUNT;
        }
        else if (input == KEY_MINUS || input == KEY_PREV)
        {
            if (disp_mode == 0)
                disp_mode = DISP_MODE_COUNT - 1;
            else
                disp_mode--;
        }
        else if (input == KEY_ENTER)
        {
            geiger_reset();
            LCD_puts_f(STR_RESET, 0);
            _delay_ms(800);
        }

        display_update();

        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }
}


static void display_update(void)
{
    if (geiger_is_noisy())
    {
        LCD_puts_f(STR_NOISE, 0);
        return;
    }

    switch (disp_mode)
    {
    case DISP_CPM:
        display_cpm();
        break;
    case DISP_DOSE:
        display_dose();
        break;
    case DISP_MREM:
        display_mrem();
        break;
    case DISP_COUNT:
        display_count();
        break;
    default:
        display_cpm();
        break;
    }
}


static void display_cpm(void)
{
    uint16_t cpm = geiger_get_cpm();

    LCD_Colon(0);

    if (cpm <= 999)
    {
        LCD_putc(0, 'C');
        LCD_putc(1, 'P');
        LCD_putc(2, 'M');
        LCD_put_number(cpm, 3, 3);
    }
    else
    {
        if (cpm > 65000)
            cpm = 65000;
        LCD_putc(0, ' ');
        LCD_put_number(cpm, 1, 5);
    }

    LCD_putc(6, '\0');
    LCD_UpdateRequired(1, 0);
}


/*
 * Dose rate in µSv/h displayed as XX:YY (colon = decimal point).
 * The Butterfly LCD colon sits between digit positions 1 and 2.
 * Format: " X:YYU"  →  X.YY µSv/h  (max 99.99 shown as "99:99")
 */
static void display_dose(void)
{
    uint16_t d = geiger_get_dose_x100();

    if (d > 9999)
        d = 9999;

    uint8_t integer  = d / 100;
    uint8_t fraction = d % 100;

    LCD_Colon(1);

    if (integer >= 10)
    {
        LCD_putc(0, '0' + (integer / 10));
        LCD_putc(1, '0' + (integer % 10));
    }
    else
    {
        LCD_putc(0, ' ');
        LCD_putc(1, '0' + integer);
    }

    LCD_putc(2, '0' + (fraction / 10));
    LCD_putc(3, '0' + (fraction % 10));
    LCD_putc(4, 'U');
    LCD_putc(5, 'V');
    LCD_putc(6, '\0');

    LCD_UpdateRequired(1, 0);
}


/*
 * Dose rate in mrem/h displayed as X:YYYР.
 * 1 µSv = 0.1 mrem, so mrem/h × 1000 == µSv/h × 100 (same value).
 * Format: " X:YYYR"  →  X.YYY mrem/h  (max 9.999)
 */
static void display_mrem(void)
{
    uint16_t m = geiger_get_dose_x100();

    if (m > 9999)
        m = 9999;

    uint8_t integer = m / 1000;
    uint16_t frac   = m % 1000;

    LCD_Colon(1);

    LCD_putc(0, ' ');
    LCD_putc(1, '0' + integer);
    LCD_putc(2, '0' + (frac / 100));
    LCD_putc(3, '0' + ((frac / 10) % 10));
    LCD_putc(4, '0' + (frac % 10));
    LCD_putc(5, 'R');
    LCD_putc(6, '\0');

    LCD_UpdateRequired(1, 0);
}


static void display_count(void)
{
    uint32_t total = geiger_get_total();

    LCD_Colon(0);

    if (total > 99999UL)
        total = 99999UL;

    LCD_putc(0, 'N');
    LCD_put_number((uint16_t)total, 1, 5);
    LCD_putc(6, '\0');

    LCD_UpdateRequired(1, 0);
}


static void OSCCAL_calibration(void)
{
    uint8_t calibrate = 0;
    uint16_t temp;

    CLKPR = (1 << CLKPCE);
    CLKPR = (1 << CLKPS1) | (1 << CLKPS0);

    TIMSK2 = 0;
    ASSR = (1 << AS2);
    OCR2A = 200;
    TIMSK0 = 0;
    TCCR1B = (1 << CS10);
    TCCR2A = (1 << CS20);

    while (ASSR & ((1 << TCN2UB) | (1 << TCR2UB)))
        ;

    _delay_ms(1000);

    while (!calibrate)
    {
        cli();

        TIFR1 = 0xFF;
        TIFR2 = 0xFF;
        TCNT1 = 0;
        TCNT2 = 0;

        while (!(TIFR2 & (1 << OCF2A)))
            ;

        TCCR1B = 0;
        sei();

        if (TIFR1 & (1 << TOV1))
        {
            temp = 0xFFFF;
        }
        else
        {
            temp = TCNT1;
        }

        if (temp > 6250)
            OSCCAL--;
        else if (temp < 6120)
            OSCCAL++;
        else
            calibrate = 1;

        TCCR1B = (1 << CS10);
    }

    TCCR1B = 0;
    TCCR2A = 0;
}


static void init_hardware(void)
{
    OSCCAL_calibration();

    CLKPR = (1 << CLKPCE);
    CLKPR = (1 << CLKPS1) | (1 << CLKPS0);

    ACSR = (1 << ACD);
    DIDR1 = (7 << ADC0D);

    PORTB = (0x0F << PORTB0);
    PORTE = (0x0F << PORTE4);

    click_init();
    Button_Init();
    Timer0_Init();
    LCD_Init();
    geiger_init();
}


void delay_ms(uint16_t ms)
{
    while (ms--)
    {
        uint8_t i;
        for (i = 0; i < 125; i++)
            asm volatile("nop");
    }
}
