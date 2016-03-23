//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: led.h
// 
// Title		: LED header file
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : LED control defiitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#ifndef LED_H
#define LED_H


#define BLUE_LED 0
#define RED_LED  1
#define GREEN_LED 2

#define BLUE  0x01
#define RED   0x02
#define GREEN 0x03


#define LED_ON 0xff
#define LED_OFF 0

#define LED_STATE_OFF	0
#define LED_STATE_ON	1

#define LOCAL_CONTROL_STATE 0x01
#define OVERRIDE_STATE      0x02

#define RESTORE_LOCAL_CONTROL 0xfc

typedef struct led_control
{
 unsigned char local_cntr_fnc;
 unsigned char fnc_off;
 unsigned char on_duration;
 unsigned char color;
 unsigned char control_state;
 unsigned char state;
 unsigned char cnt;
}leds;



void leds_init();

void control_leds();

void local_led_control( unsigned char led, unsigned char led_state);

unsigned char ipmi_set_fru_led_state(unsigned char LedId, unsigned char LedFn, unsigned char LedOn);

unsigned char ipmi_get_fru_led_state(unsigned char LedId, unsigned char *buf );

unsigned char ipmi_get_fru_led_properties(unsigned char *buf );

unsigned char ipmi_get_led_color_capabilities(unsigned char LedId, unsigned char *buf );

void led_control( unsigned char led_n, unsigned char led_state);



unsigned char state_led (unsigned char led_n);

#endif //LED_H
