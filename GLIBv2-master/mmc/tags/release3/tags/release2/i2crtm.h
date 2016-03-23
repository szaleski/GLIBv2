/*! \file i2rtm.h \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.h'
// Title		: Software I2C interface to RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
//*****************************************************************************


#ifndef I2RTM_H
#define I2RTM_H

#include "global.h"
#include "project.h"


// Constants
#define READ		0x01	// I2C READ bit

// i2c quarter-bit delay
#define QDEL    asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
// i2c half-bit delay
#define HDEL    asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");

#define I2C_SDL_LO      cbi( SDARTMPORT, SDARTMDATA)               //MJ: there is a macro with the same name in i2csw.h
#define I2C_SDL_HI      sbi( SDARTMPORT, SDARTMDATA)               //MJ: there is a macro with the same name in i2csw.h

#define I2C_SCL_LO      cbi( SCLRTMPORT, SCLRTMPIN);               //MJ: there is a macro with the same name in i2csw.h
#define I2C_SCL_HI      sbi( SCLRTMPORT, SCLRTMPIN);               //MJ: there is a macro with the same name in i2csw.h

#define I2C_SCL_TOGGLE  HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO;        //MJ: there is a macro with the same name in i2csw.h
#define I2C_START       I2C_SDL_LO; QDEL; I2C_SCL_LO;              //MJ: there is a macro with the same name in i2csw.h
#define I2C_STOP        HDEL; I2C_SCL_HI; QDEL; I2C_SDL_HI; HDEL;  //MJ: there is a macro with the same name in i2csw.h


// Function prototypes
u08 i2crtmPutbyte(u08 b);
u08 i2crtmGetbyte(u08 last);
void i2crtmInit(void);                                              // Initialise I2C interface pins
void i2crtmSend(u08 device, u16 sub, u08 length, u08 *data);     // send I2C data to <device> register <sub>
void i2crtmReceive(u08 device, u16 sub, u08 length, u08 *data);  // receive I2C data from <device> register <sub>

#endif
