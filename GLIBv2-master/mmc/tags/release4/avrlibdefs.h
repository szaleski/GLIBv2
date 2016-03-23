//*****************************************************************************
// File Name : avrlibdefs.h
//
// Title        : AVR definitions
// Revision     : 1.1
// Notes        :
// Target MCU   : Atmel AVR series
//
// Author       : Unknown (maybe Pascal Stang)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//*****************************************************************************


#ifndef AVRLIBDEFS_H
#define AVRLIBDEFS_H

#include "avrlibtypes.h"


// Code compatibility to new AVR-libc
#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif

#ifndef inb
	#define	inb(addr)			(addr)
#endif

#ifndef outw
	#define	outw(addr, data)	addr = (data)
#endif

#ifndef inw
	#define	inw(addr)			(addr)
#endif

#ifndef BV
	#define BV(bit)			    (1 << (bit))
#endif

#ifndef cbi
	#define cbi(reg, bit)	    reg &= ~(BV(bit))
#endif

#ifndef sbi
	#define sbi(reg, bit)	    reg |= (BV(bit))
#endif

#ifndef cli
	#define cli()			    __asm__ __volatile__ ("cli" ::)
#endif

#ifndef sei
	#define sei()			    __asm__ __volatile__ ("sei" ::)
#endif

// support for individual port pin naming in the mega128
// see port128.h for details
#ifdef __AVR_ATmega128__
// not currently necessary due to inclusion of these defines in newest AVR-GCC
// do a quick test to see if include is needed
#ifndef PD0
	#include "port128.h"
#endif
#endif

// use this for packed structures
// (this is seldom necessary on an 8-bit architecture like AVR, but can assist in code portability to AVR)
#define GNUC_PACKED             __attribute__((packed))    //MJ: not used

// port address helpers
#define DDR(x)                  ((x) - 1)    // address of data direction register of port x   //MJ: not used
#define PIN(x)                  ((x) - 2)    // address of input register of port x            //MJ: not used

#define CRITICAL_SECTION_START	u08 _sreg = SREG; cli()   //MJ: Because _sreg is defined as a local variable this macro can only be called once in a function. If called twice the compiler complains
#define CRITICAL_SECTION_END	SREG = _sreg

// MIN/MAX/ABS macros
#define MIN(a,b)			    ((a < b)?(a):(b))    //MJ: not used
#define MAX(a,b)			    ((a > b)?(a):(b))    //MJ: not used
#define ABS(x)				    ((x > 0)?(x):(-x))   //MJ: not used

#endif
