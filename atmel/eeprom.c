//***************************************************************************
//
//  File........: eeprom.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: IAR EWAAVR 4.20a
//
//  Description.: AVR Butterfly EEPROM routines
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//
//***************************************************************************

#include <inavr.h>
#include "eeprom.h"

/*****************************************************************************
*
*   Function name : StoreEEPROM
*
*   Returns :       None
*
*   Parameters :    Pointer to string, number of bytes to write, adress in EEPROM
*
*   Purpose :       Write byte(s) to the EEPROM
*
*****************************************************************************/
void StoreEEPROM(char *pBuffer, char num_bytes, unsigned int EEPROM_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++)
        __EEPUT(EEPROM_START_ADR++, pBuffer[i]); // Store parameters
}

/*****************************************************************************
*
*   Function name : ReadEEPROM
*
*   Returns :       None
*
*   Parameters :    Pointer to string, number of bytes to read, adress in EEPROM
*
*   Purpose :       Write byte(s) to the EEPROM
*
*****************************************************************************/
void LoadEEPROM(char *pBuffer, char num_bytes, unsigned int EEPROM_START_ADR)
{
    unsigned char i;
    for (i=0;i<num_bytes;i++)
        __EEGET(pBuffer[i], EEPROM_START_ADR++); // Load parameters
}  
