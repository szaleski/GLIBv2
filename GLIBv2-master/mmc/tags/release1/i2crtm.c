/*! \file i2rtm.c \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2rtm.c'
// Title		: Software I2C interface to RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>

#include "i2crtm.h"
#include "project.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

//#define QDEL	delay(5)		// i2c quarter-bit delay
//#define HDEL	delay(10)		// i2c half-bit delay

// i2c quarter-bit delay
#define QDEL	asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");
// i2c half-bit delay
#define HDEL	asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop"); asm volatile("nop");

#define I2C_SDL_LO      cbi( SDARTMPORT, SDARTMDATA)
#define I2C_SDL_HI      sbi( SDARTMPORT, SDARTMDATA)

#define I2C_SCL_LO      cbi( SCLRTMPORT, SCLRTMPIN); 
#define I2C_SCL_HI      sbi( SCLRTMPORT, SCLRTMPIN); 

#define I2C_SCL_TOGGLE  HDEL; I2C_SCL_HI; HDEL; I2C_SCL_LO;
#define I2C_START       I2C_SDL_LO; QDEL; I2C_SCL_LO; 
#define I2C_STOP        HDEL; I2C_SCL_HI; QDEL; I2C_SDL_HI; HDEL;



UINT i2crtmPutbyte(u08 b)
{
	int i;
	
	for (i=7;i>=0;i--)
	{
		if ( b & (1<<i) )
			I2C_SDL_HI;
		else
			I2C_SDL_LO;			// address bit
			I2C_SCL_TOGGLE;		// clock HI, delay, then LO
	}

	I2C_SDL_HI;					// leave SDL HI
	// added    
	cbi(SDARTMDDR, SDARTMDATA);			// change direction to input on SDA line (may not be needed)
	HDEL;
	I2C_SCL_HI;					// clock back up
  	b = inb(SDARTMPIN) & (1<<SDARTMDATA);	// get the ACK bit

	HDEL;
	I2C_SCL_LO;					// not really ??
	sbi(SDARTMDDR, SDARTMDATA);			// change direction back to output
	HDEL;
	return (b == 0);			// return ACK value
}


u08 i2crtmGetbyte(UINT last)
{
	int i;
	u08 c,b = 0;
		
	I2C_SDL_HI;					// make sure pullups are ativated
	cbi(SDARTMDDR, SDARTMDATA);			// change direction to input on SDA line (may not be needed)

	for(i=7;i>=0;i--)
	{
		HDEL;
		I2C_SCL_HI;				// clock HI
	  	c = inb(SDARTMPIN) & (1<<SDARTMDATA);  
		b <<= 1;
		if(c) b |= 1;
		HDEL;
    	I2C_SCL_LO;				// clock LO
	}

	sbi(SDARTMDDR, SDARTMDATA);			// change direction to output on SDA line
  
	if (last)
		I2C_SDL_HI;				// set NAK
	else
		I2C_SDL_LO;				// set ACK

	I2C_SCL_TOGGLE;				// clock pulse
	I2C_SDL_HI;					// leave with SDL HI
	return b;					// return received byte
}


//************************
//* I2C public functions *
//************************

//! Initialize I2C communication
void i2crtmInit(void)
{
	sbi( SDARTMDDR, SDARTMDATA);			// set SDA as output
	sbi( SCLRTMDDR, SCLRTMPIN);			// set SCL as output
	I2C_SDL_HI;					// set I/O state and pull-ups
	I2C_SCL_HI;					// set I/O state and pull-ups
}

//! Send a byte sequence on the I2C bus
void i2crtmSend(u08 device, u16 subAddr, u08 length, u08 *data)
{
	I2C_START;      			// do start transition
	i2crtmPutbyte(device); 		// send DEVICE address
 	if( (device >> 1 ) == RTM_EEPROM_ADDR ){
    	i2crtmPutbyte(subAddr >> 8);   		// and the subaddress
        i2crtmPutbyte(subAddr & 0xff);		// and the subaddress
		}
	while (length--)
		i2crtmPutbyte(*data++);

	I2C_SDL_LO;					// clear data line and
	I2C_STOP;					// send STOP transition
}

//! Retrieve a byte sequence on the I2C bus
void i2crtmReceive(u08 device, u16 subAddr, u08 length, u08 *data)
{
	int j = length;
	u08 *p = data;


	I2C_START;					// do start transition
	
	if( (device >> 1 ) == RTM_EEPROM_ADDR ){
	    i2crtmPutbyte(device);	             // send DEVICE address
    	i2crtmPutbyte(subAddr >> 8);   		// and the subaddress
        i2crtmPutbyte(subAddr & 0xff);		// and the subaddress
		}
    else
	    i2crtmPutbyte(device | READ);			// send DEVICE address
	HDEL;
    if( (device >> 1 ) == RTM_EEPROM_ADDR ){
	    I2C_SCL_HI;      			// do a repeated START
    	I2C_START;					// transition

	    i2crtmPutbyte(device | READ);	// resend DEVICE, with READ bit set
     } 
	// receive data bytes

	while (j--)
		*p++ = i2crtmGetbyte(j == 0);


	I2C_SDL_LO;					// clear data line and
	I2C_STOP;					// send STOP transition
}

