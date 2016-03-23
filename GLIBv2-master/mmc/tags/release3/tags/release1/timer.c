#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#include "global.h"
#include "timer.h"



// Program ROM constants
// the prescale division values stored in order of timer control register index
// STOP, CLK, CLK/8, CLK/64, CLK/256, CLK/1024
unsigned short __attribute__ ((progmem)) TimerPrescaleFactor[] = {0,1,8,64,256,1024};


typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr TimerIntFunc;

// delay for a minimum of <us> microseconds 
// the time resolution is dependent on the time the loop takes 
// e.g. with 4Mhz and 5 cycles per loop, the resolution is 1.25 us 
void delay_us(unsigned short time_us) 
{
	unsigned short delay_loops;
	register unsigned short i;

	delay_loops = (time_us+3)/5*CYCLES_PER_US; // +3 for rounding up (dirty) 

	// one loop takes 5 cpu cycles 
	for (i=0; i < delay_loops; i++) {};
}
/*
void delay_ms(unsigned char time_ms)
{
	unsigned short delay_count = F_CPU / 4000;

	unsigned short cnt;
	asm volatile ("\n"
                  "L_dl1%=:\n\t"
                  "mov %A0, %A2\n\t"
                  "mov %B0, %B2\n"
                  "L_dl2%=:\n\t"
                  "sbiw %A0, 1\n\t"
                  "brne L_dl2%=\n\t"
                  "dec %1\n\t" "brne L_dl1%=\n\t":"=&w" (cnt)
                  :"r"(time_ms), "r"((unsigned short) (delay_count))
	);
}
*/
void timerInit(void)
{
	// detach all user functions from interrupts
    TimerIntFunc = 0;
	// initialize  timer 0
    outb(TCCR0, (inb(TCCR0) | CTC_MODE));    // set set CTC mode
	timerSetPrescaler( TIMER0PRESCALE );	// set prescaler
//	timerSetPrescaler( TIMER_CLK_DIV1024 );	// set prescaler
	outb(TCNT0, 0);							// reset TCNT0
    outb(OCR0, 0x8C);                       // set value to output compare register
	sbi(TIMSK, OCIE0);						// enable output compare match interrupt


	// enable interrupts
	//sei();
}




void timerSetPrescaler(u08 prescale)
{
	// set prescaler on timer 0
	outb(TCCR0, (inb(TCCR0) & ~TIMER_PRESCALE_MASK) | prescale);
}




void timerAttach( void (*userFunc)(void) )
{
		// set the interrupt function to run
		// the supplied user's function
		TimerIntFunc = userFunc;
}





//! Interrupt handler for tcnt0 overflow interrupt
TIMER_INTERRUPT_HANDLER(SIG_OUTPUT_COMPARE0)
{
	// if a user function is defined, execute it too
		TimerIntFunc();
}


