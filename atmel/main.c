//***************************************************************************
//
//  File........: main.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 4.20a
//
//  Description.: AVR Butterfly main module
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - KS
//
//***************************************************************************

#include <inavr.h>

#include "iom169.h"
#include "main.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"
#include "RTC.h"
#include "timer0.h"
#include "BCD.h"
#include "usart.h"
#include "sound.h"
#include "ADC.h"
#include "dataflash.h"
//#include "eeprom.h"
#include "test.h"
#include "vcard.h"
#include "menu.h"

#define pLCDREG_test (*(char *)(0xEC))

extern __flash unsigned int LCD_character_table[];

extern char gPowerSaveTimer;    // external Counter from "RTC.c"
char PowerSaveTimeout = 30;     // Initial value, enable power save mode after 30 min
BOOL AutoPowerSave = TRUE;      // Variable to enable/disable the Auto Power Save func

char gAutoPressJoystick = FALSE;    // global variable used in "LCD_driver.c"

char PowerSave = FALSE;         // 

char gPlaying = FALSE;           // global variable from "sound.c". To prevent  
                                // entering power save, when playing.

char gUART = FALSE;      // global variable from "vcard.c". To prevent 
                                // entering power save, when using the UART

unsigned char state;            // helds the current state, according to 
                                // "menu.h"



/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/
__C_task void main(void)
{    
//    unsigned char state, nextstate;
    unsigned char nextstate;
    static char __flash *statetext;
    char (*pStateFunc)(char);
    char input;
    char i;
    char buttons;
    char last_buttons;

    // Initial state variables
    state = nextstate = ST_AVRBF;
    statetext = MT_AVRBF;
    pStateFunc = NULL;


    // Program initalization
    Initialization();
    __enable_interrupt();


    for (;;)            // Main loop
    {
        if(!PowerSave)          // Do not enter main loop in power-save
        {
            // Plain menu text
            if (statetext)
            {
                LCD_puts_f(statetext, 1);
                LCD_Colon(0);
                statetext = NULL;
            }
    
    
            input = getkey();           // Read buttons
    
    
            if (pStateFunc)
            {
                // When in this state, we must call the state function
                nextstate = pStateFunc(input);
            }
            else if (input != KEY_NULL)
            {
                // Plain menu, clock the state machine
                nextstate = StateMachine(state, input);
            }
    
            if (nextstate != state)
            {
                state = nextstate;
                for (i=0; menu_state[i].state; i++)
                {
                    if (menu_state[i].state == state)
                    {
                        statetext =  menu_state[i].pText;
                        pStateFunc = menu_state[i].pFunc;
                        break;
                    }
                }
            }
        }
        
        
        //Put the ATmega169 enable power save modus if autopowersave
        if(AutoPowerSave)
        {
            if(gPowerSaveTimer >= PowerSaveTimeout)
            {
                state = ST_AVRBF;
                gPowerSaveTimer = 0;
                PowerSave = TRUE;
            }
        }
        
        
        // If the joystick is held in the UP and DOWN position at the same time,
        // activate test-mode
        if( !(PINB & (1<<PORTB7)) && !(PINB & (1<<PORTB6)) )    
            Test();    
        
        
        // Check if the joystick has been in the same position for some time, 
        // then activate auto press of the joystick
        buttons = (~PINB) & PINB_MASK;
        buttons |= (~PINE) & PINE_MASK;
        
        if( buttons != last_buttons ) 
        {
            last_buttons = buttons;
            gAutoPressJoystick = FALSE;
        }
        else if( buttons )
        {
            if( gAutoPressJoystick == TRUE)
            {
                PinChangeInterrupt();
                gAutoPressJoystick = AUTO;
            }
            else    
                gAutoPressJoystick = AUTO;
        }

        
        
        // go to SLEEP
        if(!gPlaying && !gUART)              // Do not enter Power save if using UART or playing tunes
        {
            if(PowerSave)
                cbi(LCDCRA, 7);             // disable LCD
        
            SMCR = (3<<SM0) | (1<<SE);      // Enable Power-save mode
            __sleep();                      // Go to sleep
            
            if(PowerSave)
            {
                if(!(PINB & 0x40))              // press UP to wake from SLEEP
                {
                    PowerSave = FALSE;
                    
                    for(i = 0; i < 20; i++) // set all LCD segment register to the variable ucSegments
                    {
                        *(&pLCDREG_test + i) = 0x00;
                    }
                    
                    sbi(LCDCRA, 7);             // enable LCD
                    input = getkey();           // Read buttons
                }
            }
        }
        else
        {
                SMCR = (1<<SE);                 // Enable idle mode
                __sleep();                      // Go to sleep        
        }   

        SMCR = 0;                       // Just woke, disable sleep

    } //End Main loop
}




/*****************************************************************************
*
*   Function name : StateMachine
*
*   Returns :       nextstate
*
*   Parameters :    state, stimuli
*
*   Purpose :       Shifts between the different states
*
*****************************************************************************/
unsigned char StateMachine(char state, unsigned char stimuli)
{
    unsigned char nextstate = state;    // Default stay in same state
    unsigned char i;

    for (i=0; menu_nextstate[i].state; i++)
    {
        if (menu_nextstate[i].state == state && menu_nextstate[i].input == stimuli)
        {
            // This is the one!
            nextstate = menu_nextstate[i].nextstate;
            break;
        }
    }

    return nextstate;
}




/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
    char tst;           // dummy

    OSCCAL_calibration();       // calibrate the OSCCAL byte
        
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable

    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

    // Disable Analog Comparator (power save)
    ACSR = (1<<ACD);

    // Diable Digital input on PF0-2 (power save)
    DIDR1 = (7<<ADC0D);

    PORTB = (15<<PORTB0);       // Enable pullup on 
    PORTE = (15<<PORTE4);

    sbi(DDRB, 5);               // set OC1A as output
    sbi(PORTB, 5);              // set OC1A high
            
    Button_Init();              // Initialize pin change interrupt on joystick
    
    RTC_init();                 // Start timer2 asynchronous, used for RTC clock

    Timer0_Init();              // Used when playing music etc.

    USART_Init(12);             // Baud rate = 9600bps
    
    DF_SPI_init();              // init the SPI interface to communicate with the DataFlash
    
    tst = Read_DF_status();

    DF_CS_inactive;             // disable DataFlash
        
    LCD_Init();                 // initialize the LCD
}





/*****************************************************************************
*
*   Function name : BootFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Reset the ATmega169 which will cause it to start up in the 
*                   Bootloader-section. (the BOOTRST-fuse must be programmed)
*
*****************************************************************************/
__flash char TEXT_BOOT[]                     = "Jump to bootloader";

char BootFunc(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(TEXT_BOOT, 1);
    }
    else if(input == KEY_ENTER)
    {
        WDTCR = (1<<WDCE) | (1<<WDE);     //Enable Watchdog Timer to give reset
        while(1);   // wait for watchdog-reset, since the BOOTRST-fuse is 
                    // programmed, the Boot-section will be entered upon reset.
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_BOOT;
    }
    
    return ST_OPTIONS_BOOT_FUNC;
}





/*****************************************************************************
*
*   Function name : PowerSaveFunc
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable power save
*
*****************************************************************************/
__flash char TEXT_POWER[]                     = "Press enter to sleep";

char PowerSaveFunc(char input)
{
    static char enter = 1;    
    
    if(enter)
    {
        enter = 0;
        LCD_puts_f(TEXT_POWER, 1);
    }
    else if(input == KEY_ENTER)
    {
        PowerSave = TRUE;
        enter = 1;
        return ST_AVRBF;
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_OPTIONS_POWER_SAVE;
    }
        
    return ST_OPTIONS_POWER_SAVE_FUNC;

}




/*****************************************************************************
*
*   Function name : AutoPower
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Enable/Disable auto power save
*
*****************************************************************************/
char AutoPower(char input)
{
    static char enter = 1;    
    
    char PH;
    char PL;
    
    if(enter)
    {
        enter = 0;
        
        if(AutoPowerSave)  
        {     
            PH = CHAR2BCD2(PowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
                
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
        
            LCD_UpdateRequired(TRUE, 0);    
        }
        else
            LCD_puts("Off", 1);        
                    
    }
    else if(input == KEY_ENTER)
    {
         enter = 1;

         return ST_OPTIONS_AUTO_POWER_SAVE;
    }
    else if (input == KEY_PLUS)
    {

        PowerSaveTimeout += 5;
         
        if(PowerSaveTimeout > 90)
        {
            PowerSaveTimeout = 90;
        }
        else
        {    
            AutoPowerSave = TRUE;
           
            PH = CHAR2BCD2(PowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
                
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
        
            LCD_UpdateRequired(TRUE, 0);        
        }
    }
    else if (input == KEY_MINUS)
    {
        if(PowerSaveTimeout)
            PowerSaveTimeout -= 5;

        if(PowerSaveTimeout < 5)
        {
            AutoPowerSave = FALSE;
            PowerSaveTimeout = 0;
            LCD_puts("Off", 1);
        }
        else
        {   
            AutoPowerSave = TRUE;
                      
            PH = CHAR2BCD2(PowerSaveTimeout);
            PL = (PH & 0x0F) + '0';
            PH = (PH >> 4) + '0';
            
            LCD_putc(0, 'M');
            LCD_putc(1, 'I');
            LCD_putc(2, 'N');
            LCD_putc(3, ' ');
            LCD_putc(4, PH);
            LCD_putc(5, PL);
            LCD_putc(6, '\0');
        
            LCD_UpdateRequired(TRUE, 0);                     
        }
    }
        
    return ST_OPTIONS_AUTO_POWER_SAVE_FUNC;    

}




/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
    int i;
    
    while (millisec--)
        for (i=0; i<125; i++);
}



/*****************************************************************************
*
*   Function name : Revision
*
*   Returns :       None
*
*   Parameters :    char input
*
*   Purpose :       Display the software revision
*
*****************************************************************************/
char Revision(char input)
{
    static char enter = 1;
    
    if(enter)
    {
        enter = 0;
        
        LCD_putc(0, 'R');
        LCD_putc(1, 'E');
        LCD_putc(2, 'V');
        LCD_putc(3, ' ');
        LCD_putc(4, (SWHIGH + 0x30));       //SWHIGH/LOW are defined in "main.h"
        LCD_putc(5, (SWLOW + 0x30));
        LCD_putc(6, '\0');
        
        LCD_UpdateRequired(TRUE, 0);          
    }
    else if (input == KEY_PREV)
    {
        enter = 1;
        return ST_AVRBF;
    }
    
    return ST_AVRBF_REV;
}




/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag
    
        TCCR1B = 0; // stop timer1

        __enable_interrupt();  // enable global interrupt
    
        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct
    
        TCCR1B = (1<<CS10); // start timer1
    }
}
