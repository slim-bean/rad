#ifndef LCD_H
#define LCD_H

#include <avr/pgmspace.h>

void LCD_puts_f(const char *pFlashStr, char scrollmode);
void LCD_puts(const char *pStr, char scrollmode);
void LCD_putc(char digit, char character);
void LCD_Clear(void);
void LCD_Colon(char show);
void LCD_UpdateRequired(char update, char scrollmode);
void LCD_FlashReset(void);

void LCD_put_number(uint16_t val, uint8_t start_digit, uint8_t num_digits);

#endif
