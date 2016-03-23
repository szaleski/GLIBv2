/*! \file i2csw.h \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.h'
// Title		: Software I2C interface using port pins
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 5/2/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
///	\ingroup driver_sw
/// \defgroup i2csw Software I2C Serial Interface Function Library (i2csw.c)
/// \code #include "i2csw.h" \endcode
/// \par Overview
///		This library provides a very simple bit-banged I2C serial interface.
/// The library supports MASTER mode send and receive of single or multiple
/// bytes.  Thanks to the standardization of the I2C protocol and register
/// access, the send and receive commands are everything you need to talk to
/// thousands of different I2C devices including: EEPROMS, Flash memory,
/// MP3 players, A/D and D/A converters, electronic potentiometers, etc.
///
/// Although some AVR processors have built-in hardware to help create an I2C
/// interface, this library does not require or use that hardware.
///
/// For general information about I2C, see the i2c library.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef I2CSW_H
#define I2CSW_H

#include "global.h"
#include "project.h"


// Constants
#define READ            0x01  // I2C READ bit

// Macros
#define I2C_SDL_LO      cbi(SDAPORT, SDA)
#define I2C_SDL_HI      sbi(SDAPORT, SDA)
#define I2C_SCL_LO      cbi(SCLPORT, SCL)
#define I2C_SCL_HI      sbi(SCLPORT, SCL)

#define I2C_SCL_TOGGLE  { HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO; }       //MJ: As these macros consist of several sub-commands it is better to put them into "{}"
#define I2C_START       { I2C_SDL_LO; QDEL; I2C_SCL_LO; }             //MJ: Consequently they should be called without a ";" at the end
#define I2C_STOP        { HDEL; I2C_SCL_HI; HDEL; I2C_SDL_HI; HDEL; }
#define QDEL            { asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); }      // i2c quarter-bit delay
#define HDEL            { QDEL; QDEL; }                                                                                                   // i2c half-bit delay


// Functions
void i2cswInit(void);                                                          // initialise I2C interface pins
void i2cswSend(u08 device, u16 subAddr, u08 nsub, u08 length, u08 *data);      // send I2C data to <device> register <subAddr>. <nsub> is the number of sub-address bytes
void i2cswReceive(u08 device, u16 subAddr, u08 nsub, u08 length, u08 *data);   // receive I2C data from <device> register <subAddr>
u08 i2cPutbyte(u08 b);
u08 i2cGetbyte(UINT last);

#endif
