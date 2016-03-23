
// Author: unknown
// Modified by  : Markus Joos (markus.joos@cern.ch)

#ifndef TIMER_H
#define TIMER_H

#include "global.h"

// Constants, macros and typdefs

// Timer/clock prescaler values and timer overflow rates
// tics = rate at which the timer counts up
// 8bitoverflow = rate at which the timer overflows 8bits (or reaches 256)
// 16bit [overflow] = rate at which the timer overflows 16bits (65536)
// 
// overflows can be used to generate periodic interrupts
//
// for 8MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 8MHz			8bitoverflow= 31250Hz		16bit= 122.070Hz
// 2 = CLOCK/8		tics= 1MHz			8bitoverflow= 3906.25Hz		16bit=  15.259Hz
// 3 = CLOCK/64		tics= 125kHz		8bitoverflow=  488.28Hz		16bit=   1.907Hz
// 4 = CLOCK/256	tics= 31250Hz		8bitoverflow=  122.07Hz		16bit= 	 0.477Hz
// 5 = CLOCK/1024	tics= 7812.5Hz		8bitoverflow=   30.52Hz		16bit=   0.119Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 4MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 4MHz			8bitoverflow= 15625Hz		16bit=  61.035Hz
// 2 = CLOCK/8		tics= 500kHz		8bitoverflow= 1953.125Hz	16bit=   7.629Hz
// 3 = CLOCK/64		tics= 62500Hz		8bitoverflow=  244.141Hz	16bit=   0.954Hz
// 4 = CLOCK/256	tics= 15625Hz		8bitoverflow=   61.035Hz	16bit=   0.238Hz
// 5 = CLOCK/1024	tics= 3906.25Hz		8bitoverflow=   15.259Hz	16bit=   0.060Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 3.69MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics=     3.69MHz	8bitoverflow=    14414Hz	16bit=  56.304Hz
// 2 = CLOCK/8		tics=    461250Hz	8bitoverflow= 1801.758Hz	16bit=   7.038Hz
// 3 = CLOCK/64		tics=  57625.25Hz	8bitoverflow=  225.220Hz	16bit=   0.880Hz
// 4 = CLOCK/256	tics= 14414.063Hz	8bitoverflow=   56.305Hz	16bit=   0.220Hz
// 5 = CLOCK/1024	tics=  3603.516Hz	8bitoverflow=   14.076Hz	16bit=   0.055Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 32.768KHz crystal on timer 2 (use for real-time clock)
// 0 = STOP
// 1 = CLOCK		tics= 32.768kHz		8bitoverflow= 128Hz
// 2 = CLOCK/8		tics= 4096kHz		8bitoverflow=  16Hz
// 3 = CLOCK/32		tics= 1024kHz		8bitoverflow=   4Hz
// 4 = CLOCK/64		tics= 512Hz			8bitoverflow=   2Hz
// 5 = CLOCK/128	tics= 256Hz			8bitoverflow=   1Hz
// 6 = CLOCK/256	tics= 128Hz			8bitoverflow=   0.5Hz
// 7 = CLOCK/1024	tics= 32Hz			8bitoverflow=   0.125Hz

#define TIMER_CLK_STOP			0x00	// Timer Stopped
#define TIMER_CLK_DIV1			0x01	// Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	// Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	// Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	// Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	// Timer clocked at F_CPU/1024
#define TIMER_CLK_T_FALL		0x06	// Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	// Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	// Timer Pre-scaler Bit-Mask


// Default pre-scale settings for the timers
// These settings are applied when you call timerInit or any of the timer<x>Init
#define TIMER0PRESCALE		    TIMER_CLK_DIV64		///< timer 0 pre-scaler default


// Interrupt macros for attaching user functions to timer interrupts. Use these with timerAttach(intNum, function)
#define TIMER0OVERFLOW_INT			0
#define TIMER1OVERFLOW_INT			1
#define TIMER1OUTCOMPAREA_INT		2
#define TIMER1OUTCOMPAREB_INT		3
#define TIMER1INPUTCAPTURE_INT		4
#define TIMER2OVERFLOW_INT			5
#define TIMER2OUTCOMPARE_INT		6

#ifdef OCR0	                           // for processors that support output compare on Timer0
  #define TIMER0OUTCOMPARE_INT		7
  #define TIMER_NUM_INTERRUPTS		8
#else
  #define TIMER_NUM_INTERRUPTS		7
#endif

#define CTC_MODE                    0x08

// Default type of interrupt handler to use for timers. Do not change unless you know what you're doing.
// Value may be SIGNAL or INTERRUPT
#ifndef TIMER_INTERRUPT_HANDLER
  #define TIMER_INTERRUPT_HANDLER	SIGNAL
#endif

// Functions
#define delay		delay_us
#define delay_ms	timerPause
void delay_us(u16 time_us);

// Initialise timing system (all timers). Runs all timer init functions
// Sets all timers to default pre-scale values #defined in systimer.c
void timerInit(void);

// Clock pre-scaler set/get commands for each timer/counter.
// For setting the pre-scaler, you should use one of the #defines above like TIMER_CLK_DIVx, where [x] is the division rate you want.
// When getting the current pre-scaler setting, the return value will be the [x] division value currently set.
void timerSetPrescaler(u08 prescale);		///< set timer0 pre-scaler

// Attach a user function to a timer interrupt
void timerAttach(void (*userFunc)(void));

void timer2enable(void);
void timer2disable(void);

#endif
