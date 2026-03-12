//***************************************************************************
//
//  File........: vcard.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 4.20a
//
//  Description.: AVR Butterfly Name-tag
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//
//***************************************************************************

#include <inavr.h>
#include "iom169.h"
#include "main.h"
#include "button.h"
#include "LCD_functions.h"
#include "usart.h"
#include "eeprom.h"
#include "vcard.h"

char index = 0;         //variable to keep the lenght of the present string

extern char gUART;      //global variable from "main.c"

char Name[STRLENGHT];

__flash char TEXT_WAIT[]                     = "waiting for input on RS232";


/*****************************************************************************
*
*   Function name : vCard
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Puts the name in EEPROM on the LCD
*
*****************************************************************************/
char vCard(char input)
{
    static char enter = 1;

    if (enter)
    {
        enter = 0;

        __EEGET(index, EEPROM_START);           // Load the length if the name
        
        if((index < 1) | (index > STRLENGHT))   // if illegal length     
        {
            index = 0;
            Name[0] = 'A';
            
            enter = 1;
            return ST_VCARD_ENTER_NAME;               //enter new name
        }
        else
        {
            LoadEEPROM(Name, index, EEPROM_START + 1);  // Load name 
            LCD_puts(Name, 1);
        }
    }

    else if (input == KEY_NEXT)
    {
        enter = 1;
        return ST_VCARD_ENTER_NAME;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_VCARD;
    }
    
    return ST_VCARD_FUNC;
}


/*****************************************************************************
*
*   Function name : EnterName
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Lets the user enter a name using the joystick. Pressing the
*                   joystick UP/DOWN will browse the alphabet and NEXT/PREV 
*                   will shift between the characters in the name.
*
*****************************************************************************/
char EnterName(char input)
{
    static char enter = 1;

    static char temp_index = 0;
    static char temp_name[6];
    
    char i;

    if (enter)
    {
        LoadEEPROM(Name, index, EEPROM_START + 1);  // Load name from EEPROM
                
        if(index)
            index -= 1;         //make the last character in name blink

        enter = 0;        
    }
    else
    {
        temp_index = index;
                
        for(i = 5; (i != 255); i--, temp_index--)
        {
            if ((Name[temp_index] >= ' ') && (Name[temp_index] <= 'z') && (temp_index != 255)) //check if it's legal character
                temp_name[i] = Name[temp_index];
            else
                temp_name[i] = ' '; // if not, put in a space
        }       
        
        LCD_putc(0, temp_name[0]);
        LCD_putc(1, temp_name[1]);
        LCD_putc(2, temp_name[2]);
        LCD_putc(3, temp_name[3]);
        LCD_putc(4, temp_name[4]);
        LCD_putc(5, temp_name[5] | 0x80);   //Make this digit blink
        LCD_putc(6, '\0');

        if (input != KEY_NULL)
            LCD_FlashReset();
     
        LCD_UpdateRequired(TRUE, 0);
    }
    
    if (input != KEY_NULL)
        LCD_FlashReset();

    if (input == KEY_PLUS)
    {
       
        Name[index]--;

        if( (('!' <= Name[index]) && (Name[index] <= '/')) && (Name[index] != ' '))
            Name[index] = ' ';
        else if( (':' <= Name[index]) && (Name[index] <= '@'))
            Name[index] = '9';
        else if(Name[index] >= '[')
            Name[index] = 'Z';
        else if(Name[index] < ' ')
            Name[index] = 'Z';

    }
    else if (input == KEY_MINUS)
    {
        Name[index]++;

        if( (('!' <= Name[index]) && (Name[index] <= '/')) && (Name[index] != ' '))
            Name[index] = '0';
        else if( (':' <= Name[index]) && (Name[index] <= '@'))
            Name[index] = 'A';
        else if(Name[index] >= '[')
            Name[index] = ' ';
        else if(Name[index] < ' ')
            Name[index] = ' ';
    }
    else if (input == KEY_PREV)
    {
        if(index)
        {
            index--;
        }
    }
    else if (input == KEY_NEXT)
    {
        if(index < STRLENGHT)
        {
            index++;
            Name[index] = 'A';            
        }
    }
    else if (input == KEY_ENTER)
    {
        index++;
        
        Name[index] = '\0';
        
        __EEPUT(EEPROM_START, index);   //store the length of name in EEPROM
    
        StoreEEPROM(Name, index, EEPROM_START + 1);  //store the Name in EEPROM
        
        enter = 1;
        return ST_VCARD_FUNC;
    }

    return ST_VCARD_ENTER_NAME_FUNC;
}



/*****************************************************************************
*
*   Function name : RS232
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Store data from the UART to EEPROM
*
*****************************************************************************/
char RS232(char input)
{
    static char enter = 1;
    char c;
    static char buffer[STRLENGHT];
    static char temp_index;
    
    if (enter)
    {
        __disable_interrupt();
        
        // boost IntRC to 2Mhz to achieve 19200 baudrate
        CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
        // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
        CLKPR = (1<<CLKPS1);
        
        __enable_interrupt();
        
        LCD_puts_f(TEXT_WAIT, 0);
        enter = 0;
        temp_index = 0;
        c = UDR0;                       // Dummy read to clear receive buffer
        gUART = TRUE;
    }

    if (UCSR0A & (1<<RXC0))
    {
        c = UDR0;
        if (c != '\r')
        {
            if (temp_index < STRLENGHT)
                buffer[temp_index++] = c;
        }
        else    // UART transmission completed
        {
            __disable_interrupt();
                
            CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
            // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
            CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
                
            __enable_interrupt();
                
            if(temp_index)   
            {
                buffer[temp_index] = '\0';
                for (temp_index = 0; buffer[temp_index]; temp_index++)
                    Name[temp_index] = buffer[temp_index];
                Name[temp_index] = '\0';
                
                enter = 1;
                        
                __EEPUT(EEPROM_START, temp_index);   //store the length of name in EEPROM
                
                StoreEEPROM(Name, temp_index, EEPROM_START + 1);  //store the Name in EEPROM
                
                index = temp_index;
                
                gUART = FALSE;               
                return ST_VCARD_FUNC;
            }
            else    // if no characters received 
            {
                enter = 1;            
                return ST_VCARD_DOWNLOAD_NAME;
            }
        }
    }


    if (input != KEY_NULL)
    {
        enter = 1;
        
        __disable_interrupt();
        
        CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
        // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
        CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
        
        __enable_interrupt();
        
        gUART = FALSE;
        return ST_VCARD_DOWNLOAD_NAME;
    }


    return ST_VCARD_DOWNLOAD_NAME_FUNC;
}
