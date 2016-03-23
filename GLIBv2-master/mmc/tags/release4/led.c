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
#include "rtm_mng.h"
#include "led.h"
#include "user_code_select.h"


//Globals
leds amc_led[NUM_OF_AMC_LED];
leds rtm_led[NUM_OF_RTM_LED];


//*************/
void leds_init()    //Called from mmc_main.c
//*************/
{
    //AMC LEDs
    amc_led[BLUE_LED].local_cntr_fnc  = LED_ON;
    amc_led[BLUE_LED].fnc_off         = LED_ON;
    amc_led[BLUE_LED].on_duration     = 0;
    amc_led[BLUE_LED].color           = BLUE;
    amc_led[BLUE_LED].control_state   = LOCAL_CONTROL_STATE;
    local_led_control(BLUE_LED, LED_ON);

    amc_led[RED_LED].local_cntr_fnc   = LED_OFF;
    amc_led[RED_LED].fnc_off          = LED_OFF;
    amc_led[RED_LED].on_duration      = 0;
    amc_led[RED_LED].color            = RED;
    amc_led[RED_LED].control_state    = LOCAL_CONTROL_STATE;
    local_led_control(RED_LED, LED_OFF);

    amc_led[GREEN_LED].local_cntr_fnc = LED_OFF;
    amc_led[GREEN_LED].fnc_off        = LED_OFF;
    amc_led[GREEN_LED].on_duration    = 0;
    amc_led[GREEN_LED].color          = GREEN;
    amc_led[GREEN_LED].control_state  = LOCAL_CONTROL_STATE;
    local_led_control(GREEN_LED, LED_OFF);

    //RTM LEDs
#ifdef USE_RTM
    rtm_led[BLUE_LED].local_cntr_fnc  = LED_ON;
    rtm_led[BLUE_LED].fnc_off         = LED_ON;
    rtm_led[BLUE_LED].on_duration     = 0;
    rtm_led[BLUE_LED].color           = BLUE;
    rtm_led[BLUE_LED].control_state   = LOCAL_CONTROL_STATE;

    rtm_led[RED_LED].local_cntr_fnc   = LED_OFF;
    rtm_led[RED_LED].fnc_off          = LED_OFF;
    rtm_led[RED_LED].on_duration      = 0;
    rtm_led[RED_LED].color            = RED;
    rtm_led[RED_LED].control_state    = LOCAL_CONTROL_STATE;

    rtm_led[GREEN_LED].local_cntr_fnc = LED_OFF;
    rtm_led[GREEN_LED].fnc_off        = LED_OFF;
    rtm_led[GREEN_LED].on_duration    = 0;
    rtm_led[GREEN_LED].color          = GREEN;
    rtm_led[GREEN_LED].control_state  = LOCAL_CONTROL_STATE;
#endif

    if (USER_CODE == 2)
        leds_init_user();
}


//*********************/
u08 state_led(u08 led_n)   //Called from mmc_main.c
//*********************/
{
    if (amc_led[led_n].control_state == OVERRIDE_STATE)
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
    if (amc_led[led_n].control_state == OVERRIDE_STATE)
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

    amc_led[led_n].local_cntr_fnc = led_state;
}


//***************************************/
void led_control(u08 led_n, u08 led_state)  //Called from led.c only
//***************************************/
{
    if (amc_led[led_n].control_state == LOCAL_CONTROL_STATE)
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

    amc_led[led_n].state = led_state;
}


//*****************************************************************/
u08 ipmi_set_fru_led_state(u08 fru, u08 LedId, u08 LedFn, u08 LedOn)   //Called from ipmi_if.c
//*****************************************************************/
{
    if (fru == 0)
    {
        if (LedId >= NUM_OF_AMC_LED)                       // value out of range
            return (0xff);

        if (LedFn >= 1 && LedFn <= 0xfa)
        {
            amc_led[LedId].fnc_off       = LedFn;          // ON/OFF/Off time
            amc_led[LedId].on_duration   = LedOn;          // ON time
            amc_led[LedId].control_state = OVERRIDE_STATE; // initial state
            if (amc_led[LedId].state == LED_ON)
            {
                led_control(LedId, LED_OFF);
                amc_led[LedId].cnt = LedFn;                //MJ: cnt does not seem to be used for anything
            }
            else
            {
                led_control(LedId, LED_ON);
                amc_led[LedId].cnt = LedOn;
            }
        }
        else if (LedFn == LED_OFF || LedFn == LED_ON)
        {
            amc_led[LedId].fnc_off       = LedFn;            // ON/OFF/Off time
            amc_led[LedId].on_duration   = 0x00;             // ON time
            amc_led[LedId].control_state = OVERRIDE_STATE;   // initial state
            led_control(LedId, LedFn);
        }
        else if (LedFn == RESTORE_LOCAL_CONTROL)
        {
            local_led_control(LedId, amc_led[LedId].local_cntr_fnc);
            amc_led[LedId].control_state = LOCAL_CONTROL_STATE; // initial state
        }
        else
            return (0xff);
    }
    else if (fru == 1)
    {
        rtm_led[LedId].fnc_off       = LedFn;          // ON/OFF/Off time
        rtm_led[LedId].on_duration   = LedOn;          // ON time
        rtm_led[LedId].control_state = OVERRIDE_STATE; // initial state

        //set_led_pattern(0, 0x83, 1);
        if (LedFn == LED_OFF)
            rtm_led_onoff(LedId, LED_OFF);     //Step MTCA.4, 3.5.1-20 OK if rqs.data[2] refers to the BLUE LED
        else if (LedFn == LED_ON)
            rtm_led_onoff(LedId, LED_ON);      //Step MTCA.4, 3.5.2-11b OK if rqs.data[2] refers to the BLUE LED
        else //Blink the LED //MJ: This is a simplified implementation. The PICMG3.0 standard requires more sophistication...
            rtm_led_onoff(LedId, LED_BLINK);  //Step MTCA.4, 3.5.1-11 or 3.5.2-3 OK (sort of...)
            //According to MTCA.4, 3.5.1-10 the blue LED of the RTM has to do a long blink
            //According to MTCA.4, 3.5.2-3 the blue LED of the RTM has to do a short blink
    }

    return (0);
}


//*****************************************************/
u08 ipmi_get_fru_led_state(u08 fru, u08 LedId, u08 *buf)   //Called from ipmi_if.c
//*****************************************************/
{
    u08 len = 0;

    if (fru == 0)
    {
        if (LedId >= NUM_OF_AMC_LED)                 // value out of range
            return (0xff);

        buf[len++] = amc_led[LedId].control_state;   // LED States - override state has been enabled
        buf[len++] = amc_led[LedId].local_cntr_fnc;  // Local Control Function - not supported
        buf[len++] = 0x00;                           // Local control ON duration
        buf[len++] = amc_led[LedId].color;           // Local colour: Blue or Red  //MJ: Where is green?
        if (amc_led[LedId].control_state == OVERRIDE_STATE)
        {
            buf[len++] = amc_led[LedId].fnc_off;     // Override state function - ON/OFF/Off time
            buf[len++] = amc_led[LedId].on_duration; // Override control ON duration
            buf[len++] = amc_led[LedId].color;       // Override colour: Blue or Red
        }
    }
#ifdef USE_RTM
    else if (fru == 1)
    {
        if (LedId >= NUM_OF_RTM_LED)                 // value out of range
            return (0xff);

        buf[len++] = rtm_led[LedId].control_state;   // LED States - override state has been enabled
        buf[len++] = rtm_led[LedId].local_cntr_fnc;  // Local Control Function - not supported
        buf[len++] = 0x00;                           // Local control ON duration
        buf[len++] = rtm_led[LedId].color;           // Local colour: Blue or Red  //MJ: Where is green?
        if (rtm_led[LedId].control_state == OVERRIDE_STATE)
        {
            buf[len++] = rtm_led[LedId].fnc_off;     // Override state function - ON/OFF/Off time
            buf[len++] = rtm_led[LedId].on_duration; // Override control ON duration
            buf[len++] = rtm_led[LedId].color;       // Override colour: Blue or Red
        }
    }
#endif

    return (len);
}


//***********************************************/
u08 ipmi_get_fru_led_properties(u08 fru, u08 *buf)  //Called from ipmi_if.c
//***********************************************/
{
    u08 len = 0;

    buf[len++] = 0x07;           // support the control of 3 standard LEDs (BLUE LED, LED1, LED2). LED3 is not supported

    if(fru == 0)
        buf[len++] = NUM_OF_AMC_LED; // number of AMC LEDs under control
    else if(fru == 1)
        buf[len++] = NUM_OF_RTM_LED; // number of RTM LEDs under control
    else
        buf[len++] = 0; // Just in case....


    return (len);
}


//**************************************************************/
u08 ipmi_get_led_color_capabilities(u08 fru, u08 LedId, u08 *buf)   //Called from ipmmi_if.c
//**************************************************************/
{
    u08 len = 0;

    if (fru == 0 && LedId >= NUM_OF_AMC_LED)     // value out of range
        return(0xff);
    if (fru == 1 && LedId >= NUM_OF_RTM_LED)     // value out of range
        return(0xff);

    switch (LedId)               //For the 3 standard LEDs we do not have to distinguish between fru=0 and fru=1
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
            return(ipmi_get_led_color_capabilities_user(fru, LedId, buf));
    }

    return (len);
}
