//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
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
#include <avr/interrupt.h>
#include "project.h"
#include "i2c.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// Globals
// I2C state and address variables
static volatile eI2cStateType I2cState;
static u08 I2cDeviceAddrRW;

// send/transmit buffer (outgoing data)
static u08 I2cSendData[I2C_SEND_DATA_BUFFER_SIZE];
static u08 I2cSendDataIndex;
static u08 I2cSendDataLength;

// receive buffer (incoming data)
static u08 I2cReceiveData[I2C_RECEIVE_DATA_BUFFER_SIZE];
static u08 I2cReceiveDataIndex;

// function pointer to i2c receive routine
// I2cSlaveReceive is called when this processor is addressed as a slave for writing
static void (*i2cSlaveReceive)(u08 receiveDataLength, u08* recieveData);


//***************/
void i2cInit(void)   //Called from ipmi_if.c
//***************/
{
	// set pull-up resistors on I2C bus pins
	#if defined (__AVR_ATmega32__)
  	  sbi(PORTC, 0);	    // i2c SCL on ATmega163,323,16,32,etc
	  sbi(PORTC, 1);	    // i2c SDA on ATmega163,323,16,32,etc
	#else //defined (__AVR_ATmega128__)
	  sbi(PORTD, 0);	    // i2c SCL on ATmega128,64
	  sbi(PORTD, 1);	    // i2c SDA on ATmega128,64
	#endif

	i2cSlaveReceive = 0;    // clear SlaveReceive and SlaveTransmit handler to null
	i2cSetBitrate(20);      // set i2c bit rate to 100KHz
	sbi(TWCR, TWEN);        // enable TWI (two-wire interface)
	I2cState = I2C_IDLE;    // set state
	sbi(TWCR, TWIE);        // enable TWI interrupt and slave address ACK
	sbi(TWCR, TWEA);        // enable TWI interrupt and slave address ACK
}


//*******************************/
void i2cSetBitrate(u16 bitrateKHz)   //Called from i2c.c
//*******************************/
{
	u08 bitrate_div = 0;
	// set i2c bit rate
	// SCL freq = F_CPU / (16 + 2 * TWBR))
	#ifdef TWPS0
		// for processors with additional bit rate division (mega128)
		// SCL freq = F_CPU / (16 + 2 * TWBR * 4^TWPS)
		// set TWPS to zero
		cbi(TWSR, TWPS0);
		cbi(TWSR, TWPS1);
	#endif
	// calculate bit rate division

	bitrate_div = ((F_CPU / 10000) / bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div - 16) / 2;
	outb(TWBR, bitrate_div);
}


//******************************************************/
void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)   //Called from ipmi_if.c
//******************************************************/
{
	outb(TWAR, ((deviceAddr & 0xFE) | (genCallEn ? 1 : 0)));  // set local device address (used in slave mode only)
}


//*********************************************************************************************/
void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* recieveData))   //Called from ipmi_if.c
//*********************************************************************************************/
{
	i2cSlaveReceive = i2cSlaveRx_func;
}


//***************************/
inline void i2cSendStart(void)   //Called from i2c.c
//***************************/
{
	outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTA));   // send start condition
}


//**************************/
inline void i2cSendStop(void)   //Called from i2c.c
//**************************/
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA) | BV(TWSTO));
}


//*********************************/
inline void i2cWaitForComplete(void)    //MJ: not used
//*********************************/
{
	// wait for i2c interface to complete operation
	while(!(inb(TWCR) & BV(TWINT)));
}


//******************************/
inline void i2cSendByte(u08 data)   //Called from i2c.c
//******************************/
{
	outb(TWDR, data);                                       // save data to the TWDR
	outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));    // begin send
}


//************************************/
inline void i2cReceiveByte(u08 ackFlag)   //Called from i2c.c
//************************************/
{
	// begin receive over i2c
	if(ackFlag)
		outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));  // ackFlag = TRUE: ACK the received data
	else
		outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT));             // ackFlag = FALSE: NACK the received data
}


//********************************/
inline u08 i2cGetReceivedByte(void)    //MJ: Not used
//********************************/
{
	// retrieve received data byte from i2c TWDR
	return(inb(TWDR));
}


//**************************/
inline u08 i2cGetStatus(void)    //MJ: Not used
//**************************/
{
	// retrieve current i2c status from i2c TWSR
	return(inb(TWSR));
}


//******************************************************/
void i2cMasterSend(u08 deviceAddr, u08 length, u08* data)   //Called from ipmi_if.c
//******************************************************/
{
	u08 i;

	while(I2cState);                        // wait for interface to be ready
	I2cState = I2C_MASTER_TX;               // set state

	// save data
	I2cDeviceAddrRW = (deviceAddr & 0xFE);	// RW cleared: write operation

    for(i = 0; i < length; i++)
	     I2cSendData[i] = *data++;

    I2cSendDataIndex = 0;
	I2cSendDataLength = length;	
	i2cSendStart();                         // send start condition
}


//! I2C (TWI) interrupt service routine
//*********************/
SIGNAL(SIG_2WIRE_SERIAL)
//*********************/
{
	u08 status = inb(TWSR) & TWSR_STATUS_MASK;  // read status bits

	switch(status)
	{
	// Master General
	case TW_START:						// 0x08: Sent start condition
	case TW_REP_START:					// 0x10: Sent repeated start condition
		i2cSendByte(I2cDeviceAddrRW);   // send device address
		break;
	
	// Master Transmitter & Receiver status codes
	case TW_MT_SLA_ACK:					// 0x18: Slave address acknowledged
	case TW_MT_DATA_ACK:				// 0x28: Data acknowledged
	case TW_MT_SLA_NACK:				// 0x20: Slave address not acknowledged
	case TW_MT_DATA_NACK:				// 0x30: Data not acknowledged
		if(I2cSendDataIndex < I2cSendDataLength)
			i2cSendByte(I2cSendData[I2cSendDataIndex++]);  // send data
		else
		{
			i2cSendStop();             // transmit stop condition, enable SLA ACK
			I2cState = I2C_IDLE;       // set state
		}
		break;
	case TW_MT_ARB_LOST:			   // 0x38: Bus arbitration lost
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)); // release bus
		I2cState = I2C_IDLE;                             // set state
		break;

	//Master Receive is not used in ipmb
	case TW_MR_DATA_NACK:				// 0x58: Data received, NACK reply issued
	case TW_MR_SLA_NACK:				// 0x48: Slave address not acknowledged	case TW_MR_DATA_ACK:	// 0x50: Data acknowledged
	case TW_MR_SLA_ACK:					// 0x40: Slave address acknowledged
		break;

	// Slave Receiver status codes
	case TW_SR_SLA_ACK:					// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:		// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_GCALL_ACK:				// 0x70:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		// 0x78:     GCA+W has been received, ACK has been returned
	    // we are being addressed as slave for writing (data will be received from master)
		I2cState = I2C_SLAVE_RX;                            // set state
		I2cReceiveDataIndex = 0;                            // prepare buffer
   		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);  // receive data byte and return ACK
		outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));
		break;

	case TW_SR_DATA_ACK:				// 0x80: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:			// 0x90: data byte has been received, ACK has been returned
		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);      // get previously received data byte
		if(I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE)  // check receive buffer status
			i2cReceiveByte(TRUE);       // receive data byte and return ACK
		else
			i2cReceiveByte(FALSE);      // receive data byte and return NACK
		break;

	case TW_SR_DATA_NACK:				// 0x88: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:			// 0x98: data byte has been received, NACK has been returned
		i2cReceiveByte(FALSE);          // receive data byte and return NACK
		break;

	case TW_SR_STOP:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA));   // switch to SR mode with SLA ACK
		if(i2cSlaveReceive)
		    i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);         // i2c receive is complete, call i2cSlaveReceive
		I2cState = I2C_IDLE;                                              // set state
		break;

	// Slave Transmitter is not used in ipmb
	case TW_ST_SLA_ACK:					// 0xA8: own SLA+R has been received, ACK has been returned
	case TW_ST_ARB_LOST_SLA_ACK:		// 0xB0:     GCA+R has been received, ACK has been returned
	case TW_ST_DATA_ACK:				// 0xB8: data byte has been transmitted, ACK has been received
	case TW_ST_DATA_NACK:				// 0xC0: data byte has been transmitted, NACK has been received
	case TW_ST_LAST_DATA:				// 0xC8:
		break;

	// Misc
	case TW_NO_INFO:					// 0xF8: No relevant state information
		// do nothing
		break;
	case TW_BUS_ERROR:					// 0x00: Bus error due to illegal start or stop condition
		outb(TWCR, (inb(TWCR) & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTO) | BV(TWEA));  // reset internal hardware and release bus
		I2cState = I2C_IDLE;                                                         // set state
		break;
	}
}


//****************************/
eI2cStateType i2cGetState(void)  //MJ: not used
//****************************/
{
	return I2cState;
}








