/*! \file i2csw.c \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2csw.c'
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
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include "i2csw.h"
#include "project.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed


//*******************/
UINT i2cPutbyte(u08 b)            //Called from i2csw.c
//*******************/
{
    short i;                      //MJ: Do not use a char because it does not seem to have a sign bit and i=-1 terminates the loop

    for (i = 7; i >= 0; i--)
    {
        if (b & (1 << i))
            I2C_SDL_HI;
        else
            I2C_SDL_LO;           // address bit

        I2C_SCL_TOGGLE            // clock HI, delay, then LO
    }

    I2C_SDL_HI;                   // leave SDL HI
    cbi(SDADDR, SDA);             // change direction to input on SDA line (may not be needed)
    HDEL
    I2C_SCL_HI;                   // clock back up
    b = inb(SDAPIN) & (1 << SDA); // get the ACK bit   //MJ: are we not interested in the value of "b"? There is not code to evaluate it.
    HDEL
    I2C_SCL_LO;                   // not really ??
    sbi(SDADDR, SDA);             // change direction back to output
    HDEL
    return (b == 0);              // return ACK value  //MJ: why do we assign "0" to "b"?
}


//**********************/
u08 i2cGetbyte(UINT last)         //Called from i2csw.c
//**********************/
{
    short i;
    u08 c, b = 0;

    I2C_SDL_HI;                   // make sure pull-ups are activated
    cbi(SDADDR, SDA);             // change direction to input on SDA line (may not be needed)

    for (i = 7; i >= 0; i--)
    {
        HDEL
        I2C_SCL_HI;               // clock HI
        c = inb(SDAPIN) & (1 << SDA);
        b <<= 1;
        if (c)
            b |= 1;
        HDEL
        I2C_SCL_LO;               // clock LO
    }

    sbi(SDADDR, SDA);             // change direction to output on SDA line

    if (last)
        I2C_SDL_HI;               // set NAK
    else
        I2C_SDL_LO;               // set ACK

    I2C_SCL_TOGGLE                // clock pulse
    I2C_SDL_HI;                   // leave with SDL HI
    return(b);                    // return received byte
}


//************************
//* I2C public functions *
//************************

//*****************/
void i2cswInit(void)
//*****************/
{
    sbi(SDADDR, SDA);             // set SDA as output
    sbi(SCLDDR, SCL);             // set SCL as output
    I2C_STOP
}


//***********************************************************/
void i2cswSend(u08 device, u08 subAddr, u08 length, u08 *data)
//***********************************************************/
{
    I2C_START                     // do start transition
    i2cPutbyte(device);           // send DEVICE address
    i2cPutbyte(subAddr);          // and the sub-address

    // send the data
    while (length--)
        i2cPutbyte(*data++);

    I2C_SDL_LO;                   // clear data line and
    I2C_STOP                      // send STOP transition
}


//**************************************************************/
void i2cswReceive(u08 device, u08 subAddr, u08 length, u08 *data)
//**************************************************************/
{
    short j;

    I2C_START                     // do start transition
    i2cPutbyte(device);           // send DEVICE address
    i2cPutbyte(subAddr);          // and the sub-address
    HDEL
    I2C_SCL_HI;                   // do a repeated START
    I2C_START                     // transition
    i2cPutbyte(device | READ);    // re-send DEVICE, with READ bit set

    // receive data bytes
    for (j = 0; j < length; j++)
        data[j] = i2cGetbyte(j == (length - 1));

    I2C_SDL_LO;                   // clear data line and
    I2C_STOP                      // send STOP transition
}

