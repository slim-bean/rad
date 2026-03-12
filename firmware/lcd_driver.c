#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "lcd_driver.h"
#include "main.h"

extern volatile uint8_t gButtonTimeout;

volatile char gLCD_Update_Required = 0;
char LCD_Data[LCD_REGISTER_COUNT];
char gTextBuffer[TEXTBUFFER_SIZE];
volatile int8_t gScroll;
char gScrollMode;
char gLCD_Start_Scroll_Timer = 0;
char gFlashTimer = 0;
char gColon = 0;

static const uint16_t LCD_character_table[] PROGMEM = {
    0x0A51,     // '*'
    0x2A80,     // '+'
    0x0000,     // ','
    0x0A00,     // '-'
    0x0A51,     // '.' (degree sign)
    0x0000,     // '/'
    0x5559,     // '0'
    0x0118,     // '1'
    0x1e11,     // '2'
    0x1b11,     // '3'
    0x0b50,     // '4'
    0x1b41,     // '5'
    0x1f41,     // '6'
    0x0111,     // '7'
    0x1f51,     // '8'
    0x1b51,     // '9'
    0x0000,     // ':'
    0x0000,     // ';'
    0x0000,     // '<'
    0x0000,     // '='
    0x0000,     // '>'
    0x0000,     // '?'
    0x0000,     // '@'
    0x0f51,     // 'A'
    0x3991,     // 'B'
    0x1441,     // 'C'
    0x3191,     // 'D'
    0x1e41,     // 'E'
    0x0e41,     // 'F'
    0x1d41,     // 'G'
    0x0f50,     // 'H'
    0x2080,     // 'I'
    0x1510,     // 'J'
    0x8648,     // 'K'
    0x1440,     // 'L'
    0x0578,     // 'M'
    0x8570,     // 'N'
    0x1551,     // 'O'
    0x0e51,     // 'P'
    0x9551,     // 'Q'
    0x8e51,     // 'R'
    0x9021,     // 'S'
    0x2081,     // 'T'
    0x1550,     // 'U'
    0x4448,     // 'V'
    0xc550,     // 'W'
    0xc028,     // 'X'
    0x2028,     // 'Y'
    0x5009,     // 'Z'
    0x0000,     // '['
    0x0000,     // '\'
    0x0000,     // ']'
    0x0000,     // '^'
    0x0000      // '_'
};


void LCD_Init(void)
{
    LCD_AllSegments(0);

    LCD_CONTRAST_LEVEL(LCD_INITIAL_CONTRAST);

    LCDCRB = (1 << LCDCS) | (3 << LCDMUX0) | (7 << LCDPM0);
    LCDFRR = (0 << LCDPS0) | (7 << LCDCD0);
    LCDCRA = (1 << LCDEN) | (1 << LCDAB);
    LCDCRA |= (1 << LCDIE);
    LCDCCR |= (1 << LCDDC2) | (1 << LCDDC1) | (1 << LCDDC0);

    gLCD_Update_Required = 0;
}


void LCD_WriteDigit(char c, char digit)
{
    uint16_t seg = 0x0000;
    char mask, nibble;
    char *ptr;
    char i;

    if (digit > 5)
        return;

    if ((c >= '*') && (c <= 'z'))
    {
        if (c >= 'a')
            c &= ~0x20;
        c -= '*';
        seg = pgm_read_word(&LCD_character_table[(uint8_t)c]);
    }

    if (digit & 0x01)
        mask = 0x0F;
    else
        mask = 0xF0;

    ptr = LCD_Data + (digit >> 1);

    for (i = 0; i < 4; i++)
    {
        nibble = seg & 0x000F;
        seg >>= 4;
        if (digit & 0x01)
            nibble <<= 4;
        *ptr = (*ptr & mask) | nibble;
        ptr += 5;
    }
}


void LCD_AllSegments(char show)
{
    uint8_t i;

    if (show)
        show = 0xFF;

    for (i = 0; i < LCD_REGISTER_COUNT; i++)
        *(LCD_Data + i) = show;
}


ISR(LCD_vect)
{
    static char LCD_timer = LCD_TIMER_SEED;
    char c;
    char c_flash;
    char flash;
    char EOL;
    uint8_t i;
    static uint8_t timeout_count;

    if (!gButtonTimeout)
    {
        timeout_count++;
        if (timeout_count > 3)
        {
            gButtonTimeout = 1;
            timeout_count = 0;
        }
    }

    LCD_timer--;

    if (gScrollMode)
    {
        if (LCD_timer == 0)
        {
            if (gLCD_Start_Scroll_Timer == 0)
                gLCD_Update_Required = 1;
            else
                gLCD_Start_Scroll_Timer--;
        }
    }
    else
    {
        gScroll = 0;
    }

    EOL = 0;
    if (gLCD_Update_Required)
    {
        if (gFlashTimer < (LCD_FLASH_SEED >> 1))
            flash = 0;
        else
            flash = 1;

        for (i = 0; i < 6; i++)
        {
            c_flash = 0;
            if ((gScroll + (int8_t)i) >= 0 && (!EOL))
            {
                c = gTextBuffer[i + gScroll];
                c_flash = c & 0x80 ? 1 : 0;
                c = c & 0x7F;
                if (c == '\0')
                    EOL = i + 1;
            }
            else
                c = ' ';

            if (c_flash && flash)
                LCD_WriteDigit(' ', i);
            else
                LCD_WriteDigit(c, i);
        }

        volatile uint8_t *reg = &LCDDR0;
        for (i = 0; i < LCD_REGISTER_COUNT; i++)
            reg[i] = (uint8_t)LCD_Data[i];

        if (gColon)
            *(&LCDDR0 + 8) = 0x01;
        else
            *(&LCDDR0 + 8) = 0x00;

        if (EOL == 1)
            gScroll = -6;
        else
            gScroll++;

        gLCD_Update_Required = 0;
    }

    if (LCD_timer == 0)
        LCD_timer = LCD_TIMER_SEED;

    if (gFlashTimer == LCD_FLASH_SEED)
        gFlashTimer = 0;
    else
        gFlashTimer++;
}
