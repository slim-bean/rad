#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>

#define LCD_INITIAL_CONTRAST    0x0F
#define LCD_TIMER_SEED          3
#define LCD_FLASH_SEED          10
#define LCD_REGISTER_COUNT      20
#define TEXTBUFFER_SIZE         25

#define LCD_CONTRAST_LEVEL(level) \
    (LCDCCR = ((LCDCCR & 0xF0) | (0x0F & (level))))

extern volatile char gLCD_Update_Required;
extern char LCD_Data[LCD_REGISTER_COUNT];
extern char gTextBuffer[TEXTBUFFER_SIZE];
extern char gScrollMode;
extern char gFlashTimer;
extern char gColon;
extern volatile int8_t gScroll;

void LCD_Init(void);
void LCD_WriteDigit(char c, char digit);
void LCD_AllSegments(char show);

#endif
