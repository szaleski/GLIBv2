
//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: mmc_main.c
// 
// Title		: mmc main file
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : The main routine for MMC(Module Management Controller).
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

//*****************************************************************************

//File Name		: mmc_main.c
// 
// Title		: mmc main file
// Revision		: 2.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// modify       : Bompard frédéric 
// 				: CPPM ( Centre Physique des Particules de Marseille ) 
//
// Description : The main routine for MMC(Module Management Controller).
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************


 
//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "global.h"		// include our global settings
#include "timer.h"		// include timer function library
#include "ipmi_if.h"	// ipmi interface
#include "sdr.h"
#include "led.h"
#include "fru.h"
#include "avrlibdefs.h"
#include "project.h"
#include "eeprom_cppm.h"



//************************ Defines ***************************

#define HOTSWAP_EVENT 0x01
#define TIMER_EVENT   0x02

#define BLINK_LONG	  10
#define BLINK_SHORT	  2


//******************* External varriables ********************
extern u08 ipmb_address;
extern sensor_t sens[NUM_SENSOR];
extern volatile  ipmi_status status_bits;
extern leds led[NUM_OF_LED];

//*********************** Varriables *************************
volatile u08 event;
u08 HotSwap;
enum M_Status FRU_status;
u08 run_type;
int start_crate = 0;
int blink;
int flag_blink;


//*********************** fonctions  *************************

void timer_event();
void init_port(void);


//************************************************************
//**				Programme main							**
//************************************************************


int main(void)
{
//************************  INIT	***************************
    event = 0;
    FRU_status = M0;
	    
//*************************************************************
//** 			Add by Bompard Frédéric ( CPPM )			 **
//************************************************************* 
    //port pins inputs with pullups and outputs at startup
	init_port();

	if( 0xff == EEPROM_read(0))	
			Write_FRU_Info_default();		// Write default value < FRU_information > in EEPROM 
   


//*************************************************************
//**		End Add by Frédéric Bompard ( CPPM )			 **
//*************************************************************
    leds_init();
	
	if( (inb(ACSR) & BV(ACO))) // check if the board in the crate
	{
	 	run_type = INCRATE;
        ipmi_init();
        sdr_init(ipmb_address);
	}
    sbi(ACSR, ACD );

    sensor_init();								// init sensor <<<< file sdr >>>>
 	timerInit();								// init timer 
	timerAttach( timer_event );
	wdt_enable(WDTO_1S);						// enable watchdog at 1 second
	sens[HOT_SWAP_SENSOR].value = HotSwap = get_hot_swap_status(); 		// check the hotswap
    if( HotSwap == HOT_SWAP_CLOSED ) 
            sens[HOT_SWAP_SENSOR].state = 0x01;
    else 
            sens[HOT_SWAP_SENSOR].state = 0x02;
    sei(); 										// enable all interupt 


//***********************************************************************
//**************************** end INIT	*********************************
//***********************************************************************

	while(1)
	{

		wdt_reset(); 									// reset the watchdog 
	    ipmi_check_request();							// check if ipmi request << data in from ipmi>>

//*************************************************************
//** 			Add by Bompard Frédéric ( CPPM )			 **
//************************************************************* 
	
		// State Machine Payload Management
		switch ( FRU_status )
		{
		// *********************************************************************************
		// ************************** Not Installed ****************************************
		// *********************************************************************************
			case M0:	
			{
				local_led_control( GREEN_LED, LED_OFF);			// see page 13 "Figure 5: Insertion and extraction sequence state diagram"
				if(run_type == INCRATE)
						FRU_status = M1;

				break;
			}
		// *********************************************************************************
		// ****************************** Inactive *****************************************
		// *********************************************************************************
			case M1:		
			{
				// management all led
				local_led_control( BLUE_LED, LED_ON);
				local_led_control( RED_LED, LED_ON);
				local_led_control( GREEN_LED, LED_OFF);

				ipmi_picmg_fru_control(POWER_DOWN); 					// desable DCDC
				
				if( start_crate == 0x01 ) 	// MCH board read the last sensor from the FRU. The crate is initialised 
				{
						if( HotSwap != sens[HOT_SWAP_SENSOR].value || HotSwap == HOT_SWAP_CLOSED  )
				    	{
					          sens[HOT_SWAP_SENSOR].value = HotSwap; // record the new value in the struct
        		  
					          if( HotSwap == HOT_SWAP_CLOSED ) {         //hot_swap is closed state FRU = M2
			          			  	sens[HOT_SWAP_SENSOR].state = 0x01;
									FRU_status = M2;						
									blink = BLINK_LONG;					// init blink time led	
					 		  }
							  else sens[HOT_SWAP_SENSOR].state = 0x02;

							  ipmi_event_send( HOT_SWAP_SENSOR, HotSwap );   // send value hot_swap to the MCH board
						}
			          	
				}
				break;
			}
		// ***********************************************************************************
		// ************************* Activation request **************************************
		// ***********************************************************************************
			case M2:		
			{
				if(flag_blink == 1){
						if(state_led (BLUE_LED) == LED_ON)
								local_led_control( BLUE_LED, LED_OFF);

						else local_led_control( BLUE_LED, LED_ON);
										
						flag_blink = 0;
						blink = BLINK_LONG;
						ipmi_event_send( HOT_SWAP_SENSOR, HotSwap );   // send value hot_swap to the MCH board					
				}

				if( payload_power_on() > 0 ){			  				// check if 12V is ok
	            		ipmi_picmg_fru_control(POWER_UP);				// enable DCDC
						FRU_status = M3;				 				// change state <= M3
				}
				else if( HotSwap != sens[HOT_SWAP_SENSOR].value ){		// if the 12V is not present and the user change the hotswap state <= M1
			          sens[HOT_SWAP_SENSOR].value = HotSwap;
					  FRU_status = M1;
				}
				break;
			}
		// ***********************************************************************************
		// ************************* Activation in progress **********************************
		// ***********************************************************************************
			case M3: 
			{
				// management all led
				local_led_control( BLUE_LED, LED_OFF);
				local_led_control( GREEN_LED, LED_OFF);

				if( inb( LOW_VOLTAGE_POK_PORT ) & BV( LOW_VOLTAGE_POK_PIN ))  // check if DCDC is enable
 				{		
						if( (inb( FPGA_INIT_MASTER_DONE_PORT ) & BV( FPGA_INIT_MASTER_DONE_PIN )))// &&   
						//	( inb( FPGA_INIT_SLAVE_DONE_PORT ) & BV( FPGA_INIT_SLAVE_DONE_PIN )) ) // check if FPGA are initialise 
 								 FRU_status = M4;													// state <= M4
						else FRU_status = M3;
				}
				else 	
						FRU_status = M3;
				break;
			}
		// ***********************************************************************************	
		// ****************************** Active *********************************************
		// ***********************************************************************************
			case M4:		
			{
				// management all led
				local_led_control(BLUE_LED,LED_OFF);
				local_led_control(RED_LED,LED_OFF);
				local_led_control(GREEN_LED,LED_ON);

				if( HotSwap != sens[HOT_SWAP_SENSOR].value ){     // check hotswap 
	         		 sens[HOT_SWAP_SENSOR].value = HotSwap;
          
	         		 if( HotSwap == HOT_SWAP_CLOSED ) 				// close state <= state = M4 normal operation
			  		 {
	          		  	sens[HOT_SWAP_SENSOR].state = 0x01;
						FRU_status = M4;
					 }
	        		 else 											// open hotswap "board unpluged"
					 {
	         			    sens[HOT_SWAP_SENSOR].state = 0x02;		// init state sensor hotswap
						   	FRU_status = M5;						// change state <= M5
							blink = BLINK_SHORT;					// init blink time
							flag_blink = 0;
			 		 }
	         	 	ipmi_event_send( HOT_SWAP_SENSOR, HotSwap );	// send the state hotswap
	    		}
				if (payload_power_on() == 0){						// if 12V is desable state <= M5
						FRU_status = M5;
				}
				break;
			}
        // ***********************************************************************************
		// ***************************** Deactivation request ********************************
		// ***********************************************************************************
			case M5: 
			{
				ipmi_picmg_fru_control(POWER_DOWN); // desable DCDC
				//*********************************************************
				//********************* Init blink led  *******************
				if(flag_blink == 1){
						if(state_led (BLUE_LED) == LED_ON)
								local_led_control( BLUE_LED, LED_OFF);
						else	local_led_control( BLUE_LED, LED_ON);
						
						flag_blink = 0;
						blink = BLINK_SHORT;
				}
				
				FRU_status = M6;				// state <= M6
				break;
			}
		// ***********************************************************************************
		// ***************************** Deactivation in progress ****************************
		// ***********************************************************************************
			case M6:	
			{
				//*******************************************************
				//*************** Management blink led  *****************
				if(flag_blink == 1)	{
						if(state_led (BLUE_LED) == LED_ON)
								local_led_control( BLUE_LED, LED_OFF);
						else	local_led_control( BLUE_LED, LED_ON);
						
						flag_blink = 0;
						blink = BLINK_SHORT;
				}
				if( payload_power_on() == 0 ){// wait until the 12V is desable
				    FRU_status = M1;			// state <= M1
				}
				break;
			}
			default:
				FRU_status = M0;
		}

//*************************************************************
//**		End Add by Frédéric Bompard ( CPPM )			 **
//*************************************************************
		if( event & TIMER_EVENT) {				// all 100 ms 
	          ipmi_check_event_respond();		// response to ipmi
	          temp_monitoring();				// check temperature sensor
	    }
	
		event = 0;
	    //***********************************************
		//**************** go to sleep	*****************	
	    set_sleep_mode(SLEEP_MODE_IDLE);		// sleep the MCU
	    sleep_mode();							
	}
	return 0;
}

//******************************************************************
//**				   end of programme main					  **
//******************************************************************









//*****************************************************************
//**					fonction irq Timer						 **
//*****************************************************************


void timer_event()
{
    static u08 timer = 0;

    payload_status_check();   			// check the FPGA and the DCDC
    timer++;
	HotSwap = get_hot_swap_status();	// checkvalue hotswap
	if( timer == 100 )					// management time blink led
    {
        event |= TIMER_EVENT;
        timer = 0;

//** 			Add by Bompard Frédéric ( CPPM )			 **
		// for blink the led
		if(blink > 0){
			blink--;
			flag_blink=0;
		}
		else	flag_blink = 1;
    }

//**		End Add by Frédéric Bompard ( CPPM )			 **
}


//*************************************************************
//** 			Added by Bompard Frédéric ( CPPM )			 **
//*************************************************************
//**				  init port input or output		 	     **
//*************************************************************
void init_port(void)
{
    //port pins inputs with pullups and outputs at startup

    //Port A
    DDRA  = 0x02;
    PORTA = 0x00;

                                         
    //Port B
    DDRB  = 0xE1;
    PORTB = 0x00;

    //Port C
	PORTC = 0x09;
	DDRC  = 0x49;

    //Port D
    DDRD  = 0x00;
    PORTD = 0x00;

    //Port E
    DDRE  = 0x00;
    PORTE = 0x00;

    //Port F
 //   DDRF  = 0x80;
//    PORTF = 0x00;

    //Port G
    DDRG  = 0x10;
    PORTG = 0x00;

}
//*******************************************************************************
//**					End Added by Frédéric Bompard ( CPPM )			       **
//*******************************************************************************
//**							end of file									   **
//*******************************************************************************

