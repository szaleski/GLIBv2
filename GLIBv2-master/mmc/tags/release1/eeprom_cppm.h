//*****************************************************************************
// Copyright (C) 2010 CPPM(Centre Pysique des Particules de Marseille) 
//
//File Name	: eeprom_cppm.h
// 
// Title		: eeprom_cppm header file
// Revision		: 2.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Bompard Frédéric 
//				: CPPM
//
// Description  : EEPROM Management
//					
//
//*****************************************************************************

#ifndef EEPROM_CPPM_H
#define EEPROM_CPPM_H


#include"avrlibdefs.h"
#include"avrlibtypes.h"

u08 Board_information(u08 buf[]);
u08 Product_information(u08 buf[]);
u08 Multirecord_area_Module(u08 buf[]);
u08 Multirecord_area_Point_to_point(u08 buf[]);

void Write_FRU_Info_default(void);

void EEPROM_write(unsigned int uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned int uiAddress);




#endif
