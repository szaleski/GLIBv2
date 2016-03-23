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
// Author       : Bompard Frédéric, CPPM
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description  : EEPROM Management
//					
//
//*****************************************************************************

#ifndef EEPROM_CPPM_H
#define EEPROM_CPPM_H

#include "user_code_select.h"
#include"avrlibdefs.h"
#include"avrlibtypes.h"

u08 EEPROM_read(u16 uiAddress);
void Write_FRU_Info_default(void);
void EEPROM_write(u16 uiAddress, u08 ucData);

#endif
