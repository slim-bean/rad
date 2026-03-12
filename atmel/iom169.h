/****************************************************************************
 **             - iom169.h -
 **
 **     This file declares the internal register addresses for ATmega169.
 **
 **     Used with iccAVR and aAVR.
 **
 **     Copyright IAR Systems 2001. All rights reserved.
 **
 **     $Name:  $
 **
 ************************************************************************** */

#include "iomacro.h"

#if TID_GUARD(3)
#error This file should only be compiled with iccavr or aavr whith processor option -v3
#endif /* TID_GUARD(3) */

/* Include the SFR part if this file has not been included before,
 * OR this file is included by the assembler (SFRs must be defined in
 * each assembler module). */
#if !defined(__IOM169_H) || defined(__IAR_SYSTEMS_ASM__)

#pragma language=extended

/*==========================*/
/* Predefined SFR Addresses */
/*==========================*/

SFR_B(PINA,   0x00)     	/* Input Pins, Port A */
SFR_B(DDRA,   0x01)     	/* Data Direction Register, Port A */
SFR_B(PORTA,  0x02)     	/* Data Register, Port A */
SFR_B(PINB,   0x03)     	/* Input Pins, Port B */
SFR_B(DDRB,   0x04)     	/* Data Direction Register, Port B */
SFR_B(PORTB,  0x05)     	/* Data Register, Port B */
SFR_B(PINC,   0x06)     	/* Input Pins, Port C */
SFR_B(DDRC,   0x07)     	/* Data Direction Register, Port C */
SFR_B(PORTC,  0x08)     	/* Data Register, Port C */
SFR_B(PIND,   0x09)     	/* Input Pins, Port D */
SFR_B(DDRD,   0x0A)     	/* Data Direction Register, Port D */
SFR_B(PORTD,  0x0B)     	/* Data Register, Port D */
SFR_B(PINE,   0x0C)     	/* Input Pins, Port E */
SFR_B(DDRE,   0x0D)     	/* Data Direction Register, Port E */
SFR_B(PORTE,  0x0E)     	/* Data Register, Port E */
SFR_B(PINF,   0x0F)     	/* Input Pins, Port F */
SFR_B(DDRF,   0x10)     	/* Data Direction Register, Port F */
SFR_B(PORTF,  0x11)   	  /* Data Register, Port F */
SFR_B(PING,   0x12)  	  	/* Input Pins, Port G */
SFR_B(DDRG,   0x13)     	/* Data Direction Register, Port G */
SFR_B(PORTG,  0x14)   	  /* Data Register, Port G */
SFR_B(TIFR0,  0x15)				/* Timer Interrupt Flag Register, Timer 0 */
SFR_B(TIFR1,  0x16)				/* Timer Interrupt Flag Register, Timer 1 */
SFR_B(TIFR2,  0x17)				/* Timer Interrupt Flag Register, Timer 2 */
SFR_B(EIFR,   0x1C)     	/* External Interrupt Flag Register */
SFR_B(EIMSK,  0x1D)     	/* External Interrupt Mask Register */
SFR_B(GPIOR0, 0x1E)				/* General Perpose I/O Register 0 */
SFR_B(EECR,   0x1F)     	/* EEPROM Control Register */
SFR_B(EEDR,   0x20)     	/* EEPROM Data Register */
SFR_W(EEAR ,  0x21)    		/* EEPROM Address Register */
//SFR_B(EEARL,  0x21)    		/* EEPROM Address Register Low byte*/
//SFR_B(EEARH,  0x22)     	/* EEPROM Address Register High byte */
SFR_B(GTCCR,  0x23)    		/* General Timer/Counter Control Register */
SFR_B(TCCR0A, 0x24)    		/* Timer/Counter 0 Control Register A */
SFR_B(TCNT0,  0x26)     	/* Timer/Counter 0 */
SFR_B(OCR0A,  0x27)     	/* Timer/Counter 0 Output Compare Register */	
SFR_B(GPIOR1, 0x2A)				/* General Perpose I/O Register 1 */
SFR_B(GPIOR2, 0x2B)				/* General Perpose I/O Register 2 */
SFR_B(SPCR,   0x2C)     	/* SPI Control Register */
SFR_B(SPSR,   0x2D)     	/* SPI Status Register */
SFR_B(SPDR,   0x2E)     	/* SPI I/O Data Register */
SFR_B(ACSR,   0x30)     	/* Analog Comparator Control and Status Register */
SFR_B(OCDR,   0x31)     	/* On-Chip Debug Register */
SFR_B(SMCR,   0x33)     	/* Sleep Mode Control Register */
SFR_B(MCUSR,  0x34)     	/* MCU general Status Register */
SFR_B(MCUCR,  0x35)     	/* MCU general Control Register */
SFR_B(SPMCSR, 0x37)     	/* Store Program Memory Control and Status Register */
SFR_W(SP,     0x3D)     	/* Stack Pointer*/
//SFR_B(SPL,    0x3D)     	/* Stack Pointer, Low Byte */
//SFR_B(SPH,    0x3E)     	/* Stack Pointer, High Byte */
SFR_B(SREG,   0x3F)     	/* Status Register */

/* Extended I/O space */
SFR_B_EXT(WDTCR,  0x60)   /* Watchdog Timer Control Register */
SFR_B_EXT(CLKPR,  0x61)	  /* System Clock Prescaler Register */
SFR_B_EXT(OSCCAL, 0x66)   /* Oscillator Calibration Register */
SFR_B_EXT(EICRA,  0x69)   /* External Interrupt Control Register A */
SFR_B_EXT(PCMSK0, 0x6B)   /* Pin Change Mask Register 0 */
SFR_B_EXT(PCMSK1, 0x6C)   /* Pin Change Mask Register 1 */
SFR_B_EXT(TIMSK0, 0x6E)	  /* Timer Interrupt Control Register, Timer 0 */
SFR_B_EXT(TIMSK1, 0x6F)	  /* Timer Interrupt Control Register, Timer 1 */
SFR_B_EXT(TIMSK2, 0x70)	  /* Timer Interrupt Control Register, Timer 2 */
//SFR_W_EXT(ADC,    0x78)   /* ADC Data register */
SFR_B_EXT(ADCL,   0x78)   /* ADC Data register, Low Byte */
SFR_B_EXT(ADCH,   0x79)   /* ADC Data register, High Byte */
SFR_B_EXT(ADCSRA, 0x7A)   /* ADC Control and Status Register A */
SFR_B_EXT(ADCSRB, 0x7B)   /* ADC Control and Status Register B */
SFR_B_EXT(ADMUX,  0x7C)   /* ADC Multiplexer Selection Register */
SFR_B_EXT(DIDR0,  0x7E)   /* Digital Input Disable Register 0 */
SFR_B_EXT(DIDR1,  0x7F)   /* Digital Input Disable Register 1 */
SFR_B_EXT(TCCR1A, 0x80)   /* Timer/Counter 1 Control Register A */
SFR_B_EXT(TCCR1B, 0x81)   /* Timer/Counter 1 Control Register B */
SFR_B_EXT(TCCR1C, 0x82)   /* Timer/Counter 1 Control Register B */
//SFR_W_EXT(TCNT1, 0x84)    /* Timer/Counter 1 Register */	
SFR_B_EXT(TCNT1L, 0x84)   /* Timer/Counter 1 Register, Low Byte */
SFR_B_EXT(TCNT1H, 0x85)   /* Timer/Counter 1 Register, High Byte */
//SFR_W_EXT(ICR1,  0x86)    /* Timer/Counter 1 Input Capture Register */
SFR_B_EXT(ICR1L,  0x86)   /* Timer/Counter 1 Input Capture Register, Low Byte */
SFR_B_EXT(ICR1H,  0x87)   /* Timer/Counter 1 Input Capture Register, High Byte */
//SFR_W_EXT(OCR1A, 0x88)    /* Timer/Counter 1 Output Compare Register A */					
SFR_B_EXT(OCR1AL, 0x88)   /* Timer/Counter 1 Output Compare Register A, Low Byte */
SFR_B_EXT(OCR1AH, 0x89)   /* Timer/Counter 1 Output Compare Register A, High Byte */
//SFR_W_EXT(OCR1B, 0x8A)    /* Timer/Counter 1 Output Compare Register B */					
SFR_B_EXT(OCR1BL, 0x8A)   /* Timer/Counter 1 Output Compare Register B, Low Byte */
SFR_B_EXT(OCR1BH, 0x8B)   /* Timer/Counter 1 Output Compare Register B, High Byte */
SFR_B_EXT(TCCR2A, 0xB0)	  /* Timer/Counter 2 Control Register A */
SFR_B_EXT(TCNT2,  0xB2)   /* Timer/Counter 2 */
SFR_B_EXT(OCR2A,  0xB3)   /* Timer/Counter 2 Output Compare Register */
SFR_B_EXT(ASSR,   0xB6)   /* Asynchronous mode Status Register */
SFR_B_EXT(USICR,  0xB8)   /* USI Control Register */
SFR_B_EXT(USISR,  0xB9)   /* USI Status Register */
SFR_B_EXT(USIDR,  0xBA)   /* USI Data Register */
SFR_B_EXT(UCSR0A, 0xC0)   /* USART0 Control and Status Register A */
SFR_B_EXT(UCSR0B, 0xC1)   /* USART0 Control and Status Register B */
SFR_B_EXT(UCSR0C, 0xC2)   /* USART0 Control and Status Register C */
//SFR_W_EXT(UBRR0, 0xC4)    /* USART0 Baud Rate Register */												
SFR_B_EXT(UBRR0L, 0xC4)   /* USART0 Baud Rate Register, Low Byte */
SFR_B_EXT(UBRR0H, 0xC5)   /* USART0 Baud Rate Register, High Byte */
SFR_B_EXT(UDR0,   0xC6)   /* USART0 I/O Data Register */
SFR_B_EXT(LCDCRA, 0xE4)   /* LCD Control and Status Register A */
SFR_B_EXT(LCDCRB, 0xE5)   /* LCD Control and Status Register B */
SFR_B_EXT(LCDFRR, 0xE6)   /* LCD Frame Rate Register */
SFR_B_EXT(LCDCCR, 0xE7)   /* LCD Contrast Control Register */
SFR_B_EXT(LCDDR0, 0xEC)	  /* LCD Data Register 0 */
SFR_B_EXT(LCDDR1, 0xED)	  /* LCD Data Register 1 */
SFR_B_EXT(LCDDR2, 0xEE)	  /* LCD Data Register 2 */
SFR_B_EXT(LCDDR3, 0xEF)	  /* LCD Data Register 3 */
SFR_B_EXT(LCDDR5, 0xF1)	  /* LCD Data Register 5 */
SFR_B_EXT(LCDDR6, 0xF2)	  /* LCD Data Register 6 */
SFR_B_EXT(LCDDR7, 0xF3)	  /* LCD Data Register 7 */
SFR_B_EXT(LCDDR8, 0xF4)	  /* LCD Data Register 8 */
SFR_B_EXT(LCDDR10,0xF6)	  /* LCD Data Register 10 */
SFR_B_EXT(LCDDR11,0xF7)	  /* LCD Data Register 11 */
SFR_B_EXT(LCDDR12,0xF8)	  /* LCD Data Register 12 */
SFR_B_EXT(LCDDR13,0xF9)	  /* LCD Data Register 13 */
SFR_B_EXT(LCDDR15,0xFB)	  /* LCD Data Register 15 */
SFR_B_EXT(LCDDR16,0xFC)	  /* LCD Data Register 16 */
SFR_B_EXT(LCDDR17,0xFD)	  /* LCD Data Register 17 */
SFR_B_EXT(LCDDR18,0xFE)	  /* LCD Data Register 18 */

#ifndef __IOM169_H
#define __IOM169_H


/* SFRs are local in assembler modules (so this file may need to be */
/* included in more than one module in the same source file), */
/* but #defines must only be made once per source file. */

/*==============================*/
/* Interrupt Vector Definitions */
/*==============================*/

/* NB! vectors are specified as byte addresses */

#define    RESET_vect           (0x00)
#define    INT0_vect            (0x04)
#define    PCINT0_vect          (0x08)
#define    PCINT1_vect          (0x0C)
#define    TIMER2_COMP_vect     (0x10)
#define    TIMER2_OVF_vect      (0x14)
#define    TIMER1_CAPT_vect     (0x18)
#define    TIMER1_COMPA_vect    (0x1C)
#define    TIMER1_COMPB_vect    (0x20)
#define    TIMER1_OVF_vect      (0x24)
#define    TIMER0_COMP_vect     (0x28)
#define    TIMER0_OVF_vect      (0x2C)
#define    SPI_STC_vect         (0x30)
#define    USART0_RXC_vect      (0x34)
#define    USART0_UDRE_vect     (0x38)
#define    USART0_TXC_vect      (0x3C)
#define    USI_START_vect	(0x40)
#define    USI_OVF_vect		(0x44)
#define    ANA_COMP_vect        (0x48)
#define    ADC_vect             (0x4C)
#define    EE_RDY_vect          (0x50)
#define    SPM_RDY_vect         (0x54)
#define    LCD_SOF_vect         (0x58)


#ifdef __IAR_SYSTEMS_ASM__   
#ifndef ENABLE_BIT_DEFINITIONS
#define  ENABLE_BIT_DEFINITIONS
#endif /* ENABLE_BIT_DEFINITIONS */
#endif /* __IAR_SYSTEMS_ASM__ */

#ifdef ENABLE_BIT_DEFINITIONS


/* Bit definitions for use with the IAR Assembler   
   The Register Bit names are represented by their bit number (0-7).
*/


/* LCDDR18, LCDDR13, LCDDR8 and LCDDR3 bit organization */
#define SEG24       0

/* LCDDR17, LCDDR12, LCDDR7 and LCDDR2 bit organization */
#define SEG23       7
#define SEG22       6
#define SEG21       5
#define SEG20       4
#define SEG19       3
#define SEG18       2
#define SEG17       1
#define SEG16       0

/* LCDDR16, LCDDR11, LCDDR6, LCDDR1 bit organization */ 
#define SEG15       7
#define SEG14       6
#define SEG13       5
#define SEG12       4
#define SEG11       3
#define SEG10       2
#define SEG9        1
#define SEG8        0

/* LCDDR15, LCDDR10, LCDDR5, LCDDR0 bit organization */
#define SEG7        7
#define SEG6        6
#define SEG5        5
#define SEG4        4
#define SEG3        3
#define SEG2        2
#define SEG1        1
#define SEG0        0

/* LCDCCR bit organization */
#define LCDDC2      7
#define LCDDC1      6
#define LCDDC0      5
#define LCDCC3      3
#define LCDCC2      2
#define LCDCC1      1
#define LCDCC0      0

/* LCDFRR bit organization */
#define LCDPS2      6
#define LCDPS1      5
#define LCDPS0      4
#define LCDCD2      2
#define LCDCD1      1
#define LCDCD0      0

/* LCDCRB bit organization */ 
#define LCDCS       7
#define LCD2B       6
#define LCDMUX1     5
#define LCDMUX0     4
#define LCDPM2      2
#define LCDPM1      1
#define LCDPM0      0

/* LCDCRA bit organization */
#define LCDEN       7
#define LCDAB       6
#define LCDIF       4
#define LCDIE       3
#define LCDBL       0

/* UCSR0C bit organization */
#define UMSEL0      6
#define UPM01       5
#define UPM00       4
#define USBS0       3
#define UCSZ01      2
#define UCSZ00      1
#define UCPOL0      0

/* UCSR0B bit organization */
#define RXCIE0      7
#define TXCIE0      6
#define UDRIE0      5
#define RXEN0       4
#define TXEN0       3
#define UCSZ02      2
#define RXB80       1
#define TXB80       0

/* UCSR0A bit organization */
#define RXC0        7
#define TXC0        6
#define UDRE0       5
#define FE0         4
#define DOR0        3
#define PE0         2
#define U2X0        1
#define MPCM0       0

/* USISR bit organization */
#define USISIF      7
#define USIOIF      6
#define USIPF       5
#define USIDC       4
#define USICNT3     3
#define USICNT2     2
#define USICNT1     1
#define USICNT0     0

/* USICR bit organization */
#define USISIE      7
#define USIOIE      6
#define USIWM1      5
#define USIWM0      4
#define USICS1      3
#define USICS0      2
#define USICLK      1
#define USITC       0

/* ASSR bit organization */
#define EXCLK       4
#define AS2         3
#define TCN2UB      2
#define OCR2UB      1
#define TCR2UB      0

/* TCCR2A bit organization */
#define FOC2        7
#define WGM20       6
#define COM2A1      5
#define COM2A0      4
#define WGM21       3
#define CS22        2
#define CS21        1
#define CS20        0

/* TCCR1C bit organization */
#define FOC1A       7
#define FOC1B       6

/* TCCR1B bit organization */
#define ICNC1       7
#define ICES1       6
#define WGM13       4
#define WGM12       3
#define CS12        2
#define CS11        1
#define CS10        0

/* TCCR1A bit organization */
#define COM1A1      7
#define COM1A0      6
#define COM1B1      5
#define COM1B0      4
#define WGM11       1
#define WGM10       0

/* DIDR1 bit organization */
#define ADC7D       7
#define ADC6D       6
#define ADC5D       5
#define ADC4D       4
#define ADC3D       3
#define ADC2D       2
#define ADC1D       1
#define ADC0D       0

/* DIDR0 bit organization */
#define AIN1D       1
#define AIN0D       0

/* ADMUX bit organization */
#define REFS1       7
#define REFS0       6
#define ADLAR       5
#define MUX4        4
#define MUX3        3
#define MUX2        2
#define MUX1        1
#define MUX0        0

/* ADCSRB bit organization */
#define ADHSM       7
#define ACME        6
#define ADTS2       2
#define ADTS1       1
#define ADTS0       0

/* ADCSRA bit organization */
#define ADEN        7
#define ADSC        6
#define ADATE       5
#define ADIF        4
#define ADIE        3
#define ADPS2       2
#define ADPS1       1
#define ADPS0       0

/* TIMSK2 bit organization */
#define OCIE2A      1
#define TOIE2       0

/* TIMSK1 bit organization */
#define ICIE1       5
#define OCIE1B      2
#define OCIE1A      1
#define TOIE1       0

/* TIMSK0 bit organization */
#define OCIE0A      1				
#define TOIE0       0

/* PCMSK1 bit organization */
#define PCINT15     7
#define PCINT14     6
#define PCINT13     5
#define PCINT12     4
#define PCINT11     3
#define PCINT10     2
#define PCINT9      1
#define PCINT8      0

/* PCMSK0 bit organization */
#define PCINT7      7
#define PCINT6      6
#define PCINT5      5
#define PCINT4      4
#define PCINT3      3
#define PCINT2      2
#define PCINT1      1
#define PCINT0      0

/* EICRA bit organization */
#define ISC01       1
#define ISC00       0

/* CLKPR bit organization */
#define CLKPCE      7
#define CLKPS3      3
#define CLKPS2      2
#define CLKPS1      1
#define CLKPS0      0

/* WDTCR bit organization */
#define WDCE        4
#define WDE         3
#define WDP2        2
#define WDP1        1
#define WDP0        0

/* SREG  bit organization */
#define I           7
#define T           6
#define H           5
#define S           4
#define V           3
#define N           2
#define Z           1
#define C           0

/* SPH bit organization */
#define SP15        7
#define SP14        6
#define SP13        5
#define SP12        4
#define SP11        3
#define SP10        2
#define SP9         1
#define SP8         0

/* SPL bit organization */
#define SP7         7
#define SP6         6
#define SP5         5
#define SP4         4
#define SP3         3
#define SP2         2
#define SP1         1
#define SP0         0

/* SPMCSR bit organization */
#define SPMIE       7
#define RWWSB       6
#define RWWSRE      4
#define BLBSET      3
#define PGWRT       2
#define PGERS       1
#define SPMEN       0

/* MCUCR bit organization */
#define JTD         7
#define PUD         4
#define IVSEL       1
#define IVCE        0

/* MCUSR bit organization */
#define JTRF        4
#define WDRF        3
#define BORF        2
#define EXTRF       1
#define PORF        0

/* SMCR bit organization */
#define SM2         3
#define SM1         2
#define SM0         1
#define SE          0

/* OCDR bit organization */
#define IDRD        7			
#define OCDR7       7			
#define OCDR6       6
#define OCDR5       5
#define OCDR4       4
#define OCDR3       3
#define OCDR2       2
#define OCDR1       1
#define OCDR0       0

/* ACSR bit organization */
#define ACD         7
#define ACBG        6
#define ACO         5
#define ACI         4
#define ACIE        3
#define ACIC        2
#define ACIS1       1
#define ACIS0       0

/* SPSR bit organization */
#define SPIF        7
#define WCOL        6
#define SPI2X       0

/* SPCR bit organization */
#define SPIE        7
#define SPE         6
#define DORD        5
#define MSTR        4
#define CPOL        3
#define CPHA        2
#define SPR1        1
#define SPR0        0

/* TCCR0A bit organization */
#define FOC0A       7
#define WGM00       6
#define COM0A1      5
#define COM0A0      4
#define WGM01       3
#define CS02        2
#define CS01        1
#define CS00        0

/* GTCCR bit organization */
#define TSM         7
#define PSR2        1
#define PSR10       0

/* EECR bit organization */
#define EERIE       3
#define EEMWE       2
#define EEWE        1
#define EERE        0

/* EIMSK bit organization */
#define PCIE1       7
#define PCIE0       6
#define INT0        0

/* EIFR bit organization */
#define PCIF1       7
#define PCIF0       6
#define INTF0       0

/* TIFR2 bit organization */
#define OCF2A       1
#define TOV2        0

/* TIFR1 bit organization */
#define ICF1        5
#define OCF1B       2
#define OCF1A       1
#define TOV1        0

/* TIFR0 bit organization */
#define OCF0A       1
#define TOV0        0

/* PORTG bit organization */
#define PORTG5      5
#define PORTG4      4
#define PORTG3      3
#define PORTG2      2
#define PORTG1      1
#define PORTG0      0

/* DDRG bit organization */
#define DDG4        4
#define DDG3        3
#define DDG2        2
#define DDG1        1
#define DDG0        0

/* PING bit organization */
#define PING5       5
#define PING4       4
#define PING3       3
#define PING2       2
#define PING1       1
#define PING0       0

/* PORTF bit organization */
#define PORTF7      7
#define PORTF6      6
#define PORTF5      5
#define PORTF4      4
#define PORTF3      3
#define PORTF2      2
#define PORTF1      1
#define PORTF0      0

/* DDRF bit organization */  
#define DDF7        7
#define DDF6        6
#define DDF5        5
#define DDF4        4
#define DDF3        3
#define DDF2        2
#define DDF1        1
#define DDF0        0

/* PINF bit organization */
#define PINF7       7
#define PINF6       6
#define PINF5       5
#define PINF4       4
#define PINF3       3
#define PINF2       2
#define PINF1       1
#define PINF0       0

/* PORTE bit organization */
#define PORTE7      7
#define PORTE6      6
#define PORTE5      5
#define PORTE4      4
#define PORTE3      3
#define PORTE2      2
#define PORTE1      1
#define PORTE0      0

/* DDRE bit organization */
#define DDE7        7
#define DDE6        6
#define DDE5        5
#define DDE4        4
#define DDE3        3
#define DDE2        2
#define DDE1        1
#define DDE0        0

/* PINE bit organization */
#define PINE7       7
#define PINE6       6
#define PINE5       5
#define PINE4       4
#define PINE3       3
#define PINE2       2
#define PINE1       1
#define PINE0       0

/* PORTD bit organization */
#define PORTD7      7
#define PORTD6      6
#define PORTD5      5
#define PORTD4      4
#define PORTD3      3
#define PORTD2      2
#define PORTD1      1
#define PORTD0      0

/* DDRD bit organization */
#define DDD7        7
#define DDD6        6
#define DDD5        5
#define DDD4        4
#define DDD3        3
#define DDD2        2
#define DDD1        1
#define DDD0        0

/* PIND bit organization */
#define PIND7       7
#define PIND6       6
#define PIND5       5
#define PIND4       4
#define PIND3       3
#define PIND2       2
#define PIND1       1
#define PIND0       0

/* PORTC bit organization */
#define PORTC7      7
#define PORTC6      6
#define PORTC5      5
#define PORTC4      4
#define PORTC3      3
#define PORTC2      2
#define PORTC1      1
#define PORTC0      0

/* DDRC bit organization */  
#define DDC7        7
#define DDC6        6
#define DDC5        5
#define DDC4        4
#define DDC3        3
#define DDC2        2
#define DDC1        1
#define DDC0        0

/* PINC bit organization */
#define PINC7       7
#define PINC6       6
#define PINC5       5
#define PINC4       4
#define PINC3       3
#define PINC2       2
#define PINC1       1
#define PINC0       0

/* PORTB bit organization */
#define PORTB7      7
#define PORTB6      6
#define PORTB5      5
#define PORTB4      4
#define PORTB3      3
#define PORTB2      2
#define PORTB1      1
#define PORTB0      0

/* DDRB bit organization */
#define DDB7        7
#define DDB6        6
#define DDB5        5
#define DDB4        4
#define DDB3        3
#define DDB2        2
#define DDB1        1
#define DDB0        0

/* PINB bit organization */
#define PINB7       7
#define PINB6       6
#define PINB5       5
#define PINB4       4
#define PINB3       3
#define PINB2       2
#define PINB1       1
#define PINB0       0

/* PORTA bit organization */
#define PORTA7      7
#define PORTA6      6
#define PORTA5      5
#define PORTA4      4
#define PORTA3      3
#define PORTA2      2
#define PORTA1      1
#define PORTA0      0

/* DDRA bit organization */
#define DDA7        7
#define DDA6        6
#define DDA5        5
#define DDA4        4
#define DDA3        3
#define DDA2        2
#define DDA1        1
#define DDA0        0

/* PINA bit organization */
#define PINA7       7
#define PINA6       6
#define PINA5       5
#define PINA4       4
#define PINA3       3
#define PINA2       2
#define PINA1       1
#define PINA0       0


/* Pointer definition */
#define    XL     r26
#define    XH     r27
#define    YL     r28
#define    YH     r29
#define    ZL     r30
#define    ZH     r31

/* Contants */
#define    RAMEND   0x04FF   /*Last On-Chip SRAM location*/
#define    E2END    0x01FF
#define    FLASHEND 0x1FFF

#endif /* ENABLE_BIT_DEFINITIONS */ 
#endif /* __IOM169_H (define part) */
#endif /* __IOM169_H (SFR part) */
