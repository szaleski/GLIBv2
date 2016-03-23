// Author: unknown
// Modified by  : Markus Joos (markus.joos@cern.ch)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "global.h"
#include "timer.h"


// Program ROM constants
// the pre-scale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) const TimerPrescaleFactor[] = {0,1,8,64,256,1024};

typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc;

// Delay for a minimum of <us> microseconds
// The time resolution is dependent on the time the loop takes
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
//***********************/
void delay_us(u16 time_us)     //Called from ipmi_if.c and fru.c
//***********************/
{
	u16 delay_loops;
	register u16 i;     

	// one loop takes 5 cpu cycles 
	delay_loops = (time_us + 3) / 22 * 3 * CYCLES_PER_US; // +3 for rounding up (dirty)  //MJ: "22 * 3" is the result of a manual calibration of the delay

	for (i=0; i < delay_loops; i++)
	    asm volatile("nop");          //MJ: Without the "nop" there is the risk that the loop gets removed by the optimizer
}


//*****************/
void timerInit(void)     //Called from mmc_main.c
//*****************/
{
    TimerIntFunc = 0;                       // detach all user functions from interrupts

    // Initialise timer 0
    outb(TCCR0, (inb(TCCR0) | CTC_MODE));   // set set CTC mode
	timerSetPrescaler(TIMER0PRESCALE);	    // set pre-scaler
	outb(TCNT0, 0);							// reset TCNT0
    outb(OCR0, 0x8C);                       // set value to output compare register
	sbi(TIMSK, OCIE0);						// enable output compare match interrupt
}


//*********************************/
void timerSetPrescaler(u08 prescale)   //Only used locally in this file  //MJ: Can this function be in-lined?
//*********************************/
{
	// set pre-scaler on timer 0
	outb(TCCR0, (inb(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale);
}


//*************************************/
void timerAttach(void (*userFunc)(void))     //Called from mmc_main.c
//*************************************/
{
    // Set the interrupt function to run the supplied user function
	TimerIntFunc = userFunc;
}


// Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it too
	TimerIntFunc();
}
