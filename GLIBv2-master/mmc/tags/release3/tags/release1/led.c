//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: led.c
// 
// Title		: LED control file
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : Control LEDs on front panel
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

//*****************************************************************************
// Date			: 2010 CPPM ( Centre Pysique des Particules de Marseille ) 
// File Name	: mmc_main.c
// 
// Title		: LED control file
// Revision		: 2.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// modify       : Bompard frédéric 
// 				: CPPM ( Centre Physique des Particules de Marseille ) 
//
// Description :  Control LEDs on front panel
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#include <avr/io.h>
#include "avrlibdefs.h"
#include "led.h"
#include "project.h"


leds led[NUM_OF_LED];


void leds_init()
{
    


     led[BLUE_LED].local_cntr_fnc = LED_ON;
     led[BLUE_LED].fnc_off        = LED_ON;
     led[BLUE_LED].on_duration    = 0;
     led[BLUE_LED].color          = BLUE;
     led[BLUE_LED].control_state  = LOCAL_CONTROL_STATE;
     local_led_control( BLUE_LED, LED_ON);


     led[RED_LED].local_cntr_fnc = LED_OFF;
     led[RED_LED].fnc_off        = LED_OFF;
     led[RED_LED].on_duration    = 0;
     led[RED_LED].color          = RED;
     led[RED_LED].control_state  = LOCAL_CONTROL_STATE;
     local_led_control( RED_LED, LED_OFF);



     led[GREEN_LED].local_cntr_fnc = LED_OFF;
     led[GREEN_LED].fnc_off        = LED_OFF;
     led[GREEN_LED].on_duration    = 0;
     led[GREEN_LED].color          = GREEN;
     led[GREEN_LED].control_state  = LOCAL_CONTROL_STATE;
     local_led_control( GREEN_LED, LED_OFF);

	return;

}
//*************************************************************
//** 			Added by Bompard Frédéric ( CPPM )			 **
//*************************************************************

unsigned char state_led (unsigned char led_n)
{


	if( led[led_n].control_state == OVERRIDE_STATE )
        return 0;

	switch( led_n )
    {
      case BLUE_LED:
            
			 if( inb( LED_BLEU_IN ) & BV( LED_BLEU_PIN ))
			 		return LED_OFF;
             else 
			 		return LED_ON;
             break;
      case RED_LED:
             if(inb( LED_RED_IN ) & BV( LED_RED_PIN ))	return LED_OFF;
             else return LED_ON;
             break;
      case GREEN_LED:
             if(inb( LED_GREEN_IN ) & BV( LED_GREEN_PIN ))	return LED_OFF;
             else return LED_ON;
             break;
      default:
	  		break;
     }
	 return -1;
}

//*************************************************************
//**		End Added by Frédéric Bompard ( CPPM )			 **
//*************************************************************



void control_leds()
{

    unsigned char ledNum;

	for(ledNum = 0; ledNum < NUM_OF_LED; ledNum++)
    {
        if( led[ledNum].control_state == OVERRIDE_STATE )
        {
	       switch (led[ledNum].fnc_off) 
           {
		      case LED_OFF:
                   if( !led[ledNum].cnt-- )
                        led_control( ledNum, LED_OFF ); 
			       break;
		      case LED_ON:
                   if( !led[ledNum].cnt-- )
                        led_control( ledNum, LED_ON ); 
			       break;
		      default:
                   if( --led[ledNum].cnt == 0 )
                   {
				      if(led[ledNum].state == LED_OFF)
                      {
					     led[ledNum].state = LED_ON;
					     led[ledNum].cnt = led[ledNum].on_duration;
                         led_control( ledNum, LED_ON ); 
				      } 
                      else 
                      {
					     led[ledNum].state = LED_OFF;
					     led[ledNum].cnt = led[ledNum].fnc_off;
                         led_control( ledNum, LED_OFF ); 
				      }
                    }
			       break;
            }
		}
	}

}











void local_led_control( unsigned char led_n, unsigned char led_state)
{

    if( led[led_n].control_state == OVERRIDE_STATE )
        return;

    switch( led_n )
    {
      case BLUE_LED:
             if( led_state == LED_ON )
                 cbi(LED_BLEU_PORT, LED_BLEU_PIN);
             else
                 sbi(LED_BLEU_PORT, LED_BLEU_PIN);
             break;
      case RED_LED:
             if( led_state == LED_ON )
                 cbi(LED_RED_PORT, LED_RED_PIN);
             else
                 sbi(LED_RED_PORT, LED_RED_PIN);
             break;
      case GREEN_LED:
             if( led_state == LED_ON )
                 cbi(LED_GREEN_PORT, LED_GREEN_PIN);
             else
                 sbi(LED_GREEN_PORT, LED_GREEN_PIN);
             break;
      default:
             break;
     }

    led[led_n].local_cntr_fnc = led_state;
	
}

void led_control( unsigned char led_n, unsigned char led_state)
{

    if( led[led_n].control_state == LOCAL_CONTROL_STATE )
        return;

    switch( led_n )
    {
      case BLUE_LED:
             if( led_state == LED_ON )
                 cbi(LED_BLEU_PORT, LED_BLEU_PIN);
             else
                 sbi(LED_BLEU_PORT, LED_BLEU_PIN);
             break;
      case RED_LED:
             if( led_state == LED_ON )
                 cbi(LED_RED_PORT, LED_RED_PIN);
             else
                 sbi(LED_RED_PORT, LED_RED_PIN);
             break;
      case GREEN_LED:
             if( led_state == LED_ON )
                 cbi(LED_GREEN_PORT, LED_GREEN_PIN);
             else
                 sbi(LED_GREEN_PORT, LED_GREEN_PIN);
             break;
      default:
             break;
     }

    led[led_n].state = led_state;


}
unsigned char ipmi_set_fru_led_state(unsigned char LedId, unsigned char LedFn, unsigned char LedOn)
{
	if(LedId >= NUM_OF_LED)	/* value out of range */
		return(-1);

    if( LedFn >= 1 && LedFn <= 0xfa )
    {
    	led[LedId].fnc_off = LedFn;           /* ON/OFF/Off time */
    	led[LedId].on_duration = LedOn;           /* ON time	*/
    	led[LedId].control_state = OVERRIDE_STATE;	/* initial state */
        if ( led[LedId].state == LED_ON )
        { 
            led_control( LedId, LED_OFF );
            led[LedId].cnt = LedFn;
        }
        else
        {
            led_control( LedId, LED_ON );
            led[LedId].cnt = LedOn;
        }
    }
    else if( LedFn == LED_OFF || LedFn == LED_ON ) 
    {
    	led[LedId].fnc_off = LedFn;           /* ON/OFF/Off time */
    	led[LedId].on_duration = 0x00;           /* ON time	*/
    //	led[LedId].control_state = OVERRIDE_STATE;	/* initial state */
		led[LedId].control_state = LOCAL_CONTROL_STATE;	/* initial state */
    }
    else if( LedFn == RESTORE_LOCAL_CONTROL )
    {
        local_led_control( LedId, led[LedId].local_cntr_fnc);
    	led[LedId].control_state = LOCAL_CONTROL_STATE;	/* initial state */
    }
    else return -1;


	return(0);
}


unsigned char ipmi_get_fru_led_state(unsigned char LedId, unsigned char *buf )
{
	unsigned char len = 0;

	if(LedId >= NUM_OF_LED)	/* value out of range */
		return(-1);

	buf[len++] = led[LedId].control_state; 		/* LED States - override state has been enabled*/
	buf[len++] = led[LedId].local_cntr_fnc;  	/* Local Control Function - not supported */
	buf[len++] = 0x00;		/* Local control ON duration */
	buf[len++] = led[LedId].color;	/* Local color: Blue or Red */
    if( led[LedId].control_state == OVERRIDE_STATE )
    {
	buf[len++] = led[LedId].fnc_off;  /* Override state function - ON/OFF/Off time */
	buf[len++] = led[LedId].on_duration;	/* Override control ON duration */
	buf[len++] = led[LedId].color;	/* Override color: Blue or Red */
    }

	return(len);
}


unsigned char ipmi_get_fru_led_properties( unsigned char *buf )
{
	unsigned char len = 0;

	buf[len++] = 0x07;	/* support the control of the leds */
	buf[len++] = 0x00;	/* number of leds under control */

	return(len);
}


unsigned char ipmi_get_led_color_capabilities( unsigned char LedId, unsigned char *buf )
{
	unsigned char len = 0;

	if(LedId >= NUM_OF_LED)	/* value out of range */
		return(-1);

	switch (LedId) {
	case BLUE_LED:
		buf[len++] = 1<<BLUE;	/* capability is BLUE only */
		buf[len++] = BLUE;	/* default color is BLUE */
		buf[len++] = BLUE;	/* default color in override state is BLUE */
		break;
	case RED_LED:
		buf[len++] = 1<<RED;	/* capability is RED only */
		buf[len++] = RED;	/* default color is RED */
		buf[len++] = RED;	/* default color in override state is RED */
		break;
    case GREEN_LED:
		buf[len++] = 1<<GREEN;	/* capability is RED only */
		buf[len++] = GREEN;	/* default color is RED */
		buf[len++] = GREEN;	/* default color in override state is RED */
		break;
	default:
		break;
	}

	return(len);
}
