//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: led.h
// 
// Title		: LED header file
// Revision		: 1.1
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : LED control definitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

//MJ to be done:
//- The files uses constants such as "PINB" without having a direct "#include" for the respective header


#ifndef LED_H
#define LED_H

#include "avrlibtypes.h"


//Constants
#define LED_BLUE_PORT         PORTB
#define LED_BLUE_DDR          DDRB
#define LED_BLUE_PIN          PB7
#define LED_BLUE_IN           PINB

#define LED_RED_PORT          PORTB
#define LED_RED_DDR           DDRB
#define LED_RED_PIN           PB5
#define LED_RED_IN            PINB

#define LED_GREEN_PORT        PORTB
#define LED_GREEN_DDR         DDRB
#define LED_GREEN_PIN         PB6
#define LED_GREEN_IN          PINB

#define BLUE_LED              0
#define RED_LED               1
#define GREEN_LED             2

#define BLUE                  0x01
#define RED                   0x02
#define GREEN                 0x03

#define LED_ON                0xff
#define LED_OFF               0
#define LED_BLINK             1

#define LED_STATE_ON	      1
#define LED_STATE_OFF	      0

#define LOCAL_CONTROL_STATE   0x01
#define OVERRIDE_STATE        0x02
#define RESTORE_LOCAL_CONTROL 0xfc


//Types
typedef struct led_control
{
  u08 local_cntr_fnc;
  u08 fnc_off;
  u08 on_duration;
  u08 color;
  u08 control_state;
  u08 state;
  u08 cnt;
} leds;


//Function prototypes
void leds_init();
void control_leds();
void local_led_control(u08 led, u08 led_state);
void led_control(u08 led_n, u08 led_state);
u08 ipmi_get_fru_led_properties(u08 fru, u08 *buf);
u08 ipmi_get_led_color_capabilities(u08 fru, u08 LedId, u08 *buf);
u08 ipmi_set_fru_led_state(u08 fru, u08 LedId, u08 LedFn, u08 LedOn);
u08 ipmi_get_fru_led_state(u08 fru, u08 LedId, u08 *buf);
u08 state_led (u08 led_n);

#endif //LED_H
