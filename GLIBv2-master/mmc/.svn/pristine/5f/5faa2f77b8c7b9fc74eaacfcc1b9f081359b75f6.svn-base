//*****************************************************************************
// Copyright (C): 2010 CPPM ( Centre Pysique des Particules de Marseille ) 
// File Name	: mmc_main.c
// Date			:
// Title		: eeprom_cppm control file
// Revision		: 2.0
// Notes		: 7/04/2010
// Target MCU	: Atmel AVR series
//
// Author       : Bompard frédéric CPPM ( Centre Physique des Particules de Marseille )
// Modified by  : Paschalis Vichoudis, CERN
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : EEPROM management 
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************


#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include"eeprom.h"
#include"ipmi_if.h"
#include"fru.h"


//*************************************************************************************
//**			 Write default value in eeprom for Board information area.			 **
//** 			 Contains information about the board on which the FRU 				 **
//**		       information device is located as shown in table 4.				 **
//**			 Specification Module Management Controler mezzanine board	    	 **
//*************************************************************************************


//******************************/
void Write_FRU_Info_default(void)   //Called from main()
//******************************/
{
    static u16 size_M = 0;   //MJ: why static?
    u16 i, size = 0;
    u08 buf[512], Header[8]; //The Header[8] array contains the data of the mandatory common FUR header as defined in: Platform Management FRU Information, Storage Definition, v1.0

    Header[0] = 0x01;
    Header[1] = 0x00;     // area Internal is empty
    Header[2] = 0x00;     // area Chassis is empty
    Header[3] = 0x08 / 8; // Offset area Board start
    size = 8;
    size_M = size;
    size += Board_information(buf);

    Header[4] = size / 8; // offset area Product info

    CRITICAL_SECTION_START;
    for (i = 0; i < (size - size_M); i++)
        EEPROM_write(i + size_M, buf[i]);

    size_M = size;
    size += Product_information(buf);

    for (i = 0; i < (size - size_M); i++)
        EEPROM_write(i + size_M, buf[i]);

    Header[5] = size / 8; // offset area Multi record

    size_M = size;
    size += Multirecord_area_Module(buf);

    for (i = 0; i < (size - size_M); i++)
        EEPROM_write(i + size_M, buf[i]);

    size_M = size;
    size += Multirecord_area_Point_to_point(buf);

    for (i = 0; i < (size - size_M); i++)
        EEPROM_write(i + size_M, buf[i]);

    size_M = size;
    Header[6] = 0x00; // PAD
    Header[7] = checksum_clc(Header, 7);

    for (i = 0; i < 8; i++)
        EEPROM_write(i, Header[i]);

    CRITICAL_SECTION_END;
}


//*****************************************/
void EEPROM_write(u16 uiAddress, u08 ucData)  //Called from multiple locations
//*****************************************/
{
    while (EECR & (1 << EEWE)) ;   // Wait for completion of previous write

    // Set up address and data registers
    EEARL = uiAddress & 0xFF;
    EEARH = uiAddress >> 8;
    EEDR = ucData;

    EECR |= (1 << EEMWE);          // Write logical one to EEMWE
    EECR |= (1 << EEWE);           // Start eeprom write by setting EEWE
}


//***************************/
u08 EEPROM_read(u16 uiAddress)  //Called from multiple locations
//***************************/
{
    while (EECR & (1 << EEWE)) ;   // Wait for completion of previous write

    // Set up address register
    EEARL = uiAddress & 0xFF;
    EEARH = uiAddress >> 8;

    EECR |= (1 << EERE);           // Start eeprom read by writing EERE
    return EEDR;                   // Return data from data register
}

