//*****************************************************************************
//
// File Name	: 'a2d.c'
// Title		: Analog-to-digital converter functions
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002-04-08
// Revised		: 2002-09-30
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include "a2d.h"


// initialize a2d converter
//***************/
void a2dInit(void)    //Called from sdr.c
//***************/
{
	sbi(ADCSR, ADEN);				        // enable ADC (turn on ADC power)
	sbi(ADCSR, ADFR);				        // free running mode
	a2dSetPrescaler(ADC_PRESCALE);	        // set default prescaler
	a2dSetReference(ADC_REFERENCE_AREF);	// set default reference
	sbi(ADMUX, ADLAR);				        // set to left-adjusted result
}


// turn off a2d converter
//**************/
void a2dOff(void)   //MJ: not used
//**************/
{
	cbi(ADCSR, ADEN);	// disable ADC (turn off ADC power)
}


// configure A2D converter clock division (prescaling)
//*******************************/
void a2dSetPrescaler(u08 prescale)     //Called from a2d.c  //MJ: Does this have to be a separate function?
//*******************************/
{
	outb(ADCSR, ((inb(ADCSR) & ~ADC_PRESCALE_MASK) | prescale));
}


// configure A2D converter voltage reference
//**************************/
void a2dSetReference(u08 ref)  //Called from a2d.c  //MJ: Does this have to be a separate function?
//**************************/
{
	outb(ADMUX, ((inb(ADMUX) & ~ADC_REFERENCE_MASK) | (ref<<6)));
}


// sets the a2d input channel
//***********************/
void a2dSetChannel(u08 ch)  //Called from sdr.c
//***********************/
{
	outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK));	// set channel
}


// start a conversion on the current a2d input channel
//***********************/
void a2dStartConvert(void)  //Called from sdr.c
//***********************/
{
	sbi(ADCSR, ADIF);	// clear hardware "conversion complete" flag 
	sbi(ADCSR, ADSC);	// start conversion
}


// return TRUE if conversion is complete
//********************/
u08 a2dIsComplete(void)    //MJ: not used
//********************/
{
	return bit_is_set(ADCSR, ADSC);
}


// Perform a 10-bit conversion
// starts conversion, waits until conversion is done, and returns result
//************************/
u16 a2dConvert10bit(u08 ch)    //MJ: not used
//************************/
{
	outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | (ch & ADC_MUX_MASK));	// set channel
	sbi(ADCSR, ADIF);						                            // clear hardware "conversion complete" flag
	sbi(ADCSR, ADSC);						                            // start conversion
	while(bit_is_set(ADCSR, ADSC)); 		                            // wait until conversion complete

	// CAUTION: MUST READ ADCL BEFORE ADCH!!!
	return(inb(ADCL) | (inb(ADCH) << 8));	                            // read ADC (full 10 bits);
}


// Perform a 8-bit conversion.
// starts conversion, waits until conversion is done, and returns result
//***********************/
u08 a2dConvert8bit(u08 ch)   //Called from sdr.c
//***********************/
{
    return (inb(ADCH));	// read ADC
}
