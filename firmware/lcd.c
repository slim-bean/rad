#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd_driver.h"
#include "lcd.h"
#include "main.h"

extern char gLCD_Start_Scroll_Timer;


void LCD_puts_f(const char *pFlashStr, char scrollmode)
{
    uint8_t i;

    while (gLCD_Update_Required)
        ;

    for (i = 0; pgm_read_byte(&pFlashStr[i]) && i < TEXTBUFFER_SIZE; i++)
        gTextBuffer[i] = pgm_read_byte(&pFlashStr[i]);

    gTextBuffer[i] = '\0';

    if (i > 6)
    {
        gScrollMode = 1;
        gScroll = 0;
        gLCD_Start_Scroll_Timer = 3;
    }
    else
    {
        gScrollMode = 0;
        gScroll = 0;
    }

    gLCD_Update_Required = 1;
}


void LCD_puts(const char *pStr, char scrollmode)
{
    uint8_t i;

    while (gLCD_Update_Required)
        ;

    for (i = 0; pStr[i] && i < TEXTBUFFER_SIZE; i++)
        gTextBuffer[i] = pStr[i];

    gTextBuffer[i] = '\0';

    if (i > 6)
    {
        gScrollMode = 1;
        gScroll = 0;
        gLCD_Start_Scroll_Timer = 3;
    }
    else
    {
        gScrollMode = 0;
        gScroll = 0;
    }

    gLCD_Update_Required = 1;
}


void LCD_putc(char digit, char character)
{
    if ((uint8_t)digit < TEXTBUFFER_SIZE)
        gTextBuffer[(uint8_t)digit] = character;
}


void LCD_Clear(void)
{
    uint8_t i;
    for (i = 0; i < TEXTBUFFER_SIZE; i++)
        gTextBuffer[i] = ' ';
}


void LCD_Colon(char show)
{
    gColon = show;
}


void LCD_UpdateRequired(char update, char scrollmode)
{
    while (gLCD_Update_Required)
        ;

    gScrollMode = scrollmode;
    gScroll = 0;
    gLCD_Update_Required = update;
}


void LCD_FlashReset(void)
{
    gFlashTimer = 0;
}


void LCD_put_number(uint16_t val, uint8_t start_digit, uint8_t num_digits)
{
    uint8_t i;
    uint16_t divisor = 1;
    uint8_t started = 0;

    for (i = 1; i < num_digits; i++)
        divisor *= 10;

    for (i = 0; i < num_digits; i++)
    {
        uint8_t d = val / divisor;
        val %= divisor;
        divisor /= 10;

        if (d > 0 || started || i == num_digits - 1)
        {
            LCD_putc(start_digit + i, '0' + d);
            started = 1;
        }
        else
        {
            LCD_putc(start_digit + i, ' ');
        }
    }
}
