//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: led.c
// 
// Title		: LED control file
// Revision		: 1.1
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Frédéric Bompard CPPM ( Centre Physique des Particules de Marseille )
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : Control LEDs on front panel
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

// Header file
#include <avr/io.h>
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "led.h"
#include "user_code_select.h"


//Globals
leds led[NUM_OF_LED];


//*************/
void leds_init()    //Called from mmc_main.c
//*************/
{
    led[BLUE_LED].local_cntr_fnc  = LED_ON;
    led[BLUE_LED].fnc_off         = LED_ON;
    led[BLUE_LED].on_duration     = 0;
    led[BLUE_LED].color           = BLUE;
    led[BLUE_LED].control_state   = LOCAL_CONTROL_STATE;
    local_led_control(BLUE_LED, LED_ON);

    led[RED_LED].local_cntr_fnc   = LED_OFF;
    led[RED_LED].fnc_off          = LED_OFF;
    led[RED_LED].on_duration      = 0;
    led[RED_LED].color            = RED;
    led[RED_LED].control_state    = LOCAL_CONTROL_STATE;
    local_led_control(RED_LED, LED_OFF);

    led[GREEN_LED].local_cntr_fnc = LED_OFF;
    led[GREEN_LED].fnc_off        = LED_OFF;
    led[GREEN_LED].on_duration    = 0;
    led[GREEN_LED].color          = GREEN;
    led[GREEN_LED].control_state  = LOCAL_CONTROL_STATE;
    local_led_control(GREEN_LED, LED_OFF);

    if (USER_CODE == 2)
        leds_init_user();
}


//*********************/
u08 state_led(u08 led_n)   //Called from mmc_main.c
//*********************/
{
    if (led[led_n].control_state == OVERRIDE_STATE)
        return (0); //MJ: 0 = LED_OFF. Is this what we want in mmc_main.c? What is OVERRIDE_STATE used for?

    switch (led_n)
    {
    case BLUE_LED:
        if (inb(LED_BLUE_IN) & BV(LED_BLUE_PIN))    return LED_OFF;
        else                                        return LED_ON;
        break;
    case RED_LED:
        if (inb(LED_RED_IN) & BV(LED_RED_PIN))      return LED_OFF;
        else                                        return LED_ON;
        break;
    case GREEN_LED:
        if (inb(LED_GREEN_IN) & BV(LED_GREEN_PIN))  return LED_OFF;
        else                                        return LED_ON;
        break;
    default:
        if (USER_CODE == 2)
            return(state_led_user(led_n));
        break;
    }
    return (0xff); //MJ: mmc_main.c does not process this error. How could it react?
}


//*********************************************/
void local_led_control(u08 led_n, u08 led_state)   //Called from led.c only
//*********************************************/
{
    if (led[led_n].control_state == OVERRIDE_STATE)
        return;

    switch (led_n)
    {
    case BLUE_LED:
        if (led_state == LED_ON)  cbi(LED_BLUE_PORT, LED_BLUE_PIN);
        else                      sbi(LED_BLUE_PORT, LED_BLUE_PIN);
        break;
    case RED_LED:
        if (led_state == LED_ON)  cbi(LED_RED_PORT, LED_RED_PIN);
        else                      sbi(LED_RED_PORT, LED_RED_PIN);
        break;
    case GREEN_LED:
        if (led_state == LED_ON)  cbi(LED_GREEN_PORT, LED_GREEN_PIN);
        else                      sbi(LED_GREEN_PORT, LED_GREEN_PIN);
        break;
    default:
        if (USER_CODE == 2)
            local_led_control_user(led_n, led_state);
        break;
    }

    led[led_n].local_cntr_fnc = led_state;
}


//***************************************/
void led_control(u08 led_n, u08 led_state)  //Called from various places
//***************************************/
{
    if (led[led_n].control_state == LOCAL_CONTROL_STATE)
        return;

    switch (led_n)
    {
    case BLUE_LED:
        if (led_state == LED_ON)  cbi(LED_BLUE_PORT, LED_BLUE_PIN);
        else                      sbi(LED_BLUE_PORT, LED_BLUE_PIN);
        break;
    case RED_LED:
        if (led_state == LED_ON)  cbi(LED_RED_PORT, LED_RED_PIN);
        else                      sbi(LED_RED_PORT, LED_RED_PIN);
        break;
    case GREEN_LED:
        if (led_state == LED_ON)  cbi(LED_GREEN_PORT, LED_GREEN_PIN);
        else                      sbi(LED_GREEN_PORT, LED_GREEN_PIN);
        break;
    default:
        if (USER_CODE == 2)
            led_control_user(led_n, led_state);
        break;
    }

    led[led_n].state = led_state;
}


//********************************************************/
u08 ipmi_set_fru_led_state(u08 LedId, u08 LedFn, u08 LedOn)   //Called from ipmi_if.c
//********************************************************/
{
    if (LedId >= NUM_OF_LED) // value out of range
        return (0xff);

    if (LedFn >= 1 && LedFn <= 0xfa)
    {
        led[LedId].fnc_off       = LedFn;          // ON/OFF/Off time
        led[LedId].on_duration   = LedOn;          // ON time
        led[LedId].control_state = OVERRIDE_STATE; // initial state
        if (led[LedId].state == LED_ON)
        {
            led_control(LedId, LED_OFF);
            led[LedId].cnt = LedFn;                //MJ: cnt does not seem to be used for anything
        }
        else
        {
            led_control(LedId, LED_ON);
            led[LedId].cnt = LedOn;
        }
    }
    else if (LedFn == LED_OFF || LedFn == LED_ON)
    {
        led[LedId].fnc_off       = LedFn;          // ON/OFF/Off time
        led[LedId].on_duration   = 0x00;           // ON time
        led[LedId].control_state = OVERRIDE_STATE; // initial state
        led_control(LedId, LedFn);
    }
    else if (LedFn == RESTORE_LOCAL_CONTROL)
    {
        local_led_control(LedId, led[LedId].local_cntr_fnc);
        led[LedId].control_state = LOCAL_CONTROL_STATE; // initial state
    }
    else
        return (0xff);

    return (0);
}


//********************************************/
u08 ipmi_get_fru_led_state(u08 LedId, u08 *buf)   //Called from ipmi_if.c
//********************************************/
{
    u08 len = 0;

    if (LedId >= NUM_OF_LED)                 // value out of range
        return (0xff);

    buf[len++] = led[LedId].control_state;   // LED States - override state has been enabled
    buf[len++] = led[LedId].local_cntr_fnc;  // Local Control Function - not supported
    buf[len++] = 0x00;                       // Local control ON duration
    buf[len++] = led[LedId].color;           // Local colour: Blue or Red  //MJ: Where is green?
    if (led[LedId].control_state == OVERRIDE_STATE)
    {
        buf[len++] = led[LedId].fnc_off;     // Override state function - ON/OFF/Off time
        buf[len++] = led[LedId].on_duration; // Override control ON duration
        buf[len++] = led[LedId].color;       // Override colour: Blue or Red
    }

    return (len);
}


//**************************************/
u08 ipmi_get_fru_led_properties(u08 *buf)  //Called from ipmi_if.c
//**************************************/
{
    u08 len = 0;

    buf[len++] = 0x07; // support the control of the LEDs
    buf[len++] = 0x00; // number of LEDs under control
    return (len);
}


//*****************************************************/
u08 ipmi_get_led_color_capabilities(u08 LedId, u08 *buf)   //Called from ipmmi_if.c
//*****************************************************/
{
    u08 len = 0;

    if (LedId >= NUM_OF_LED)     // value out of range
        return(0xff);

    switch (LedId)
    {                            //The data returned by this function is defined in table 3-30 of the ATCA spec.
    case BLUE_LED:
        buf[len++] = 1 << BLUE;  // capability is BLUE only
        buf[len++] = BLUE;       // default colour is BLUE
        buf[len++] = BLUE;       // default colour in override state is BLUE
        break;
    case RED_LED:
        buf[len++] = 1 << RED;   // capability is RED only
        buf[len++] = RED;        // default colour is RED
        buf[len++] = RED;        // default colour in override state is RED
        break;
    case GREEN_LED:
        buf[len++] = 1 << GREEN; // capability is RED only
        buf[len++] = GREEN;      // default colour is RED
        buf[len++] = GREEN;      // default colour in override state is RED
        break;
    default:
        if (USER_CODE == 2)
            return(ipmi_get_led_color_capabilities_user(LedId, buf));
    }

    return (len);
}
