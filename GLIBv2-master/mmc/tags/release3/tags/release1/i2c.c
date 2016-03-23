/*! \file i2c.c \brief I2C interface using AVR Two-Wire Interface (TWI) hardware. */
//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
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
//static u08 I2cReceiveDataLength;

// function pointer to i2c receive routine
//! I2cSlaveReceive is called when this processor
// is addressed as a slave for writing
static void (*i2cSlaveReceive)(u08 receiveDataLength, u08* recieveData);




void i2cInit(void)
{
	// set pull-up resistors on I2C bus pins
	#if defined (__AVR_ATmega32__)
	sbi(PORTC, 0);	// i2c SCL on ATmega163,323,16,32,etc
	sbi(PORTC, 1);	// i2c SDA on ATmega163,323,16,32,etc
	#else //defined (__AVR_ATmega128__)
	sbi(PORTD, 0);	// i2c SCL on ATmega128,64
	sbi(PORTD, 1);	// i2c SDA on ATmega128,64
	#endif
	// clear SlaveReceive and SlaveTransmit handler to null
	i2cSlaveReceive = 0;
	// set i2c bit rate to 100KHz
	i2cSetBitrate(20);
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);
	// set state
	I2cState = I2C_IDLE;
	// enable TWI interrupt and slave address ACK
	sbi(TWCR, TWIE);
	sbi(TWCR, TWEA);
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	// enable interrupts
	//sei();
}

void i2cSetBitrate(u16 bitrateKHz)
{
	u08 bitrate_div = 0;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	#ifdef TWPS0
		// for processors with additional bitrate division (mega128)
		// SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
		// set TWPS to zero
		cbi(TWSR, TWPS0);
		cbi(TWSR, TWPS1);
	#endif
	// calculate bitrate division

	bitrate_div = ((F_CPU/10000)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div-16)/2;
	outb(TWBR, bitrate_div);
}

void i2cSetLocalDeviceAddr(u08 deviceAddr, u08 genCallEn)
{
	// set local device address (used in slave mode only)
	outb(TWAR, ((deviceAddr&0xFE) | (genCallEn?1:0)) );
}

void i2cSetSlaveReceiveHandler(void (*i2cSlaveRx_func)(u08 receiveDataLength, u08* recieveData))
{
	i2cSlaveReceive = i2cSlaveRx_func;
}
/*
void i2cSetSlaveTransmitHandler(u08 (*i2cSlaveTx_func)(u08 transmitDataLengthMax, u08* transmitData))
{
	i2cSlaveTransmit = i2cSlaveTx_func;
}
*/
inline void i2cSendStart(void)
{
	// send start condition
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
}

inline void i2cSendStop(void)
{
	// transmit stop condition
	// leave with TWEA on for slave receiving
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
}

inline void i2cWaitForComplete(void)
{
	// wait for i2c interface to complete operation
	while( !(inb(TWCR) & BV(TWINT)) );
}

inline void i2cSendByte(u08 data)
{
	// save data to the TWDR
	outb(TWDR, data);
	// begin send
	outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
}

inline void i2cReceiveByte(u08 ackFlag)
{
	// begin receive over i2c
	if( ackFlag )
	{
		// ackFlag = TRUE: ACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
	}
	else
	{
		// ackFlag = FALSE: NACK the recevied data
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
	}
}

inline u08 i2cGetReceivedByte(void)
{
	// retieve received data byte from i2c TWDR
	return( inb(TWDR) );
}

inline u08 i2cGetStatus(void)
{
	// retieve current i2c status from i2c TWSR
	return( inb(TWSR) );
}

void i2cMasterSend(u08 deviceAddr, u08 length, u08* data)
{
	u08 i;
// wait for interface to be ready
	while(I2cState);
	// set state
	I2cState = I2C_MASTER_TX;
	// save data
	I2cDeviceAddrRW = (deviceAddr & 0xFE);	// RW cleared: write operation

    for( i=0; i<length; i++)
	     I2cSendData[i] = *data++;

    I2cSendDataIndex = 0;
	I2cSendDataLength = length;	
	// send start condition
	i2cSendStart();
}




//! I2C (TWI) interrupt service routine
SIGNAL(SIG_2WIRE_SERIAL)
{
	// read status bits
	u08 status = inb(TWSR) & TWSR_STATUS_MASK;

	switch(status)
	{
	// Master General
	case TW_START:						// 0x08: Sent start condition
	case TW_REP_START:					// 0x10: Sent repeated start condition
		// send device address
		i2cSendByte(I2cDeviceAddrRW);
		break;
	
	// Master Transmitter & Receiver status codes
	case TW_MT_SLA_ACK:					// 0x18: Slave address acknowledged
	case TW_MT_DATA_ACK:				// 0x28: Data acknowledged
	case TW_MT_SLA_NACK:				// 0x20: Slave address not acknowledged
	case TW_MT_DATA_NACK:				// 0x30: Data not acknowledged
		if(I2cSendDataIndex < I2cSendDataLength)
		{
			// send data
			i2cSendByte( I2cSendData[I2cSendDataIndex++] );
		}
		else
		{
			// transmit stop condition, enable SLA ACK
			i2cSendStop();
			// set state
			I2cState = I2C_IDLE;
		}
		break;
	case TW_MT_ARB_LOST:				// 0x38: Bus arbitration lost
		// release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		// set state
		I2cState = I2C_IDLE;
		// release bus and transmit start when bus is free
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
		break;

		//Master Receive is not used in ipmb
	case TW_MR_DATA_NACK:				// 0x58: Data received, NACK reply issued
	case TW_MR_SLA_NACK:				// 0x48: Slave address not acknowledged	case TW_MR_DATA_ACK:				// 0x50: Data acknowledged
	case TW_MR_SLA_ACK:					// 0x40: Slave address acknowledged
		break;

	// Slave Receiver status codes
	case TW_SR_SLA_ACK:					// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:		// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_GCALL_ACK:				// 0x70:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		// 0x78:     GCA+W has been received, ACK has been returned
		// we are being addressed as slave for writing (data will be received from master)
		// set state
		I2cState = I2C_SLAVE_RX;
		// prepare buffer
		I2cReceiveDataIndex = 0;
		// receive data byte and return ACK
   		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		break;
	case TW_SR_DATA_ACK:				// 0x80: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:			// 0x90: data byte has been received, ACK has been returned
		// get previously received data byte
		I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		// check receive buffer status
		if(I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE)
		{
			// receive data byte and return ACK
			i2cReceiveByte(TRUE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		}
		else
		{
			// receive data byte and return NACK
			i2cReceiveByte(FALSE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		}
		break;
	case TW_SR_DATA_NACK:				// 0x88: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:			// 0x98: data byte has been received, NACK has been returned
		// receive data byte and return NACK
		i2cReceiveByte(FALSE);
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		break;
	case TW_SR_STOP:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		// switch to SR mode with SLA ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// i2c receive is complete, call i2cSlaveReceive
		if(i2cSlaveReceive) i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
		// set state
		I2cState = I2C_IDLE;
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
		// reset internal hardware and release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
		// set state
		I2cState = I2C_IDLE;
		break;
	}
}

eI2cStateType i2cGetState(void)
{
	return I2cState;
}








