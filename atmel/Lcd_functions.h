//*****************************************************************************
//
//  File........: LCD_functions.h
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Description.: Functions for LCD_functions.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20021015 - 1.0  - File created                                  - LHM
//
//*****************************************************************************




//Functions
void LCD_puts_f(char __flash *pFlashStr, char scrollmode);
void LCD_puts(char *pStr, char scrollmode);
void LCD_UpdateRequired(char update, char scrollmode);
void LCD_putc(char digit, char character);
void LCD_Clear(void);
void LCD_Colon(char show);
void LCD_FlashReset(void);
char SetContrast(char input);
