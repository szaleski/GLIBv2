/*! \file i2rtm.h \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.h'
// Title		: Software I2C interface to RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
//*****************************************************************************



#ifndef I2RTM_H
#define I2RTM_H

#include "global.h"

// include project-dependent settings
#include "project.h"


// defines and constants
#define READ		0x01	// I2C READ bit

// functions

// initialize I2C interface pins
void i2crtmInit(void);

// send I2C data to <device> register <sub>
void i2crtmSend(BYTE device, u16 sub, BYTE length, BYTE *data);

// receive I2C data from <device> register <sub>
void i2crtmReceive(BYTE device, u16 sub, BYTE length, BYTE *data);

#endif
