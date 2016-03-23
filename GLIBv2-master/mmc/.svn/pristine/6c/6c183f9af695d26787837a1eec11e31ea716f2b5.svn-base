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
// Modified by  : Frédéric Bompard, CPPM (Centre Physique des Particules de Marseille)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : The main routine for MMC(Module Management Controller).
//
// This code is distributed under the GNU Public License
// which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************




//MJ: Files that are still to be reviewed:
// jtag.c & .h
// load_cern.c & .h



// Include Files
#include <avr/io.h>		    // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "global.h"	    	// include our global settings
#include "timer.h"		    // include timer function library
#include "ipmi_if.h"	    // ipmi interface
#include "led.h"
#include "fru.h"
#include "avrlibdefs.h"
#include "project.h"
//#include "eeprom_cppm.h"
#include "user_code_select.h"


// Constants   //MJ: there is no mmc_main.h file yet

#define HOTSWAP_EVENT 0x01
#define TIMER_EVENT   0x02

#define BLINK_LONG	  10
#define BLINK_SHORT	  2


// External variables
extern u08 ipmb_address;
extern sensor_t sens[NUM_SENSOR];
extern volatile ipmi_status status_bits;
extern leds led[NUM_OF_LED];


// Global variables
volatile u08 event;
u08 HotSwap, run_type, start_crate = 0, blink, flag_blink;
enum M_Status FRU_status;


// Function prototypes
void timer_event();
void init_port(void);


//***********/
int main(void)
//***********/
{
    // INIT
    event = 0;
    FRU_status = M0;

    // port pins inputs with pull-ups and outputs at startup
    init_port();

    if (0xff == EEPROM_read(0))
        Write_FRU_Info_default(); // Write default value < FRU_information > in EEPROM

    leds_init();

    if ((inb(ACSR) & BV(ACO)))    // check if the board is in the crate
    {
        run_type = INCRATE;
        ipmi_init();
        sdr_init(ipmb_address);
    }
    sbi(ACSR, ACD);

    sensor_init();                // init sensor <<<< file sdr >>>>
    timerInit();                  // init timer
    timerAttach(timer_event);
    wdt_enable(WDTO_1S);          // enable watchdog at 1 second
    sens[HOT_SWAP_SENSOR].value = HotSwap = get_hot_swap_status();   // check the hot swap
    if (HotSwap == HOT_SWAP_CLOSED)
        sens[HOT_SWAP_SENSOR].state = 0x01;
    else
        sens[HOT_SWAP_SENSOR].state = 0x02;
    sei();     // enable all interrupts
    // end INIT

    while(1)
    {
        wdt_reset();              // reset the watchdog
        ipmi_check_request();     // check if ipmi request <<data in from ipmi>>

        // State Machine Payload Management
        switch (FRU_status)
        {
        case M0:                  // Not Installed
        {
            local_led_control(GREEN_LED, LED_OFF);   // see page 13 "Figure 5: Insertion and extraction sequence state diagram"
            if (run_type == INCRATE)
                FRU_status = M1;
            break;
        }
        case M1:                  // Inactive
        {
            // LED management
            local_led_control(BLUE_LED, LED_ON);
            local_led_control(RED_LED, LED_ON);
            local_led_control(GREEN_LED, LED_OFF);

            ipmi_picmg_fru_control(POWER_DOWN);     // disable DCDC

            if (start_crate == 0x01)                // MCH board read the last sensor from the FRU. The crate is initialised
            {
                if (HotSwap != sens[HOT_SWAP_SENSOR].value || HotSwap == HOT_SWAP_CLOSED)
                {
                    sens[HOT_SWAP_SENSOR].value = HotSwap; // record the new value in the structure

                    if (HotSwap == HOT_SWAP_CLOSED)        //hot_swap is closed state FRU = M2
                    {
                        sens[HOT_SWAP_SENSOR].state = 0x01;
                        FRU_status = M2;
                        blink = BLINK_LONG;                // init blink time led
                    }
                    else
                        sens[HOT_SWAP_SENSOR].state = 0x02;

                    ipmi_event_send(HOT_SWAP_SENSOR, HotSwap); // send value hot_swap to the MCH board
                }
            }
            break;
        }
        case M2:                  // Activation request
        {
            if (flag_blink == 1)
            {
                if (state_led(BLUE_LED) == LED_ON)
                    local_led_control(BLUE_LED, LED_OFF);
                else
                    local_led_control(BLUE_LED, LED_ON);

                flag_blink = 0;
                blink = BLINK_LONG;
                ipmi_event_send(HOT_SWAP_SENSOR, HotSwap);    // send value hot_swap to the MCH board
            }

            if (payload_power_on() > 0)                       // check if 12V is OK
            {
                ipmi_picmg_fru_control(POWER_UP);             // enable DCDC
                FRU_status = M3;                              // change state <= M3
            }
            else if (HotSwap != sens[HOT_SWAP_SENSOR].value)  // if the 12V is not present and the user change the hotswap state <= M1
            {
                sens[HOT_SWAP_SENSOR].value = HotSwap;
                FRU_status = M1;
            }
            break;
        }
        case M3:                  // Activation in progress
        {
            // LED management
            local_led_control(BLUE_LED, LED_OFF);
            local_led_control(GREEN_LED, LED_OFF);

            if (inb( LOW_VOLTAGE_POK_PORT ) & BV( LOW_VOLTAGE_POK_PIN ))   // check if DCDC is enable
            {
                if ((inb( FPGA_INIT_MASTER_DONE_PORT ) & BV( FPGA_INIT_MASTER_DONE_PIN )))
                    FRU_status = M4; // state <= M4
                else
                    FRU_status = M3;
            }
            else
                FRU_status = M3;
            break;
        }
        case M4:                  //  Active
        {
            // LED management
            local_led_control(BLUE_LED, LED_OFF);
            local_led_control(RED_LED, LED_OFF);
            local_led_control(GREEN_LED, LED_ON);

            if (HotSwap != sens[HOT_SWAP_SENSOR].value)    // check hotswap
            {
                sens[HOT_SWAP_SENSOR].value = HotSwap;

                if (HotSwap == HOT_SWAP_CLOSED)            // close state <= state = M4 normal operation
                {
                    sens[HOT_SWAP_SENSOR].state = 0x01;
                    FRU_status = M4;
                }
                else                                       // open hotswap "board unpluged"
                {
                    sens[HOT_SWAP_SENSOR].state = 0x02;    // init state sensor hotswap
                    FRU_status = M5;                       // change state <= M5
                    blink = BLINK_SHORT;                   // init blink time
                    flag_blink = 0;
                }
                ipmi_event_send(HOT_SWAP_SENSOR, HotSwap); // send the state hotswap
            }
            if (payload_power_on() == 0)                   // if 12V is disable state <= M5
                FRU_status = M5;
            break;
        }
        case M5:                  // De-activation request
        {
            ipmi_picmg_fru_control(POWER_DOWN); // disable DCDC

            if (flag_blink == 1)                //Init LED blinking
            {
                if (state_led(BLUE_LED) == LED_ON)
                    local_led_control(BLUE_LED, LED_OFF);
                else
                    local_led_control(BLUE_LED, LED_ON);

                flag_blink = 0;
                blink = BLINK_SHORT;
            }

            FRU_status = M6; // state <= M6
            break;
        }
        case M6:                  // De-activation in progress
        {
            if (flag_blink == 1)  //LED blinking management
            {
                if (state_led(BLUE_LED) == LED_ON)
                    local_led_control(BLUE_LED, LED_OFF);
                else
                    local_led_control(BLUE_LED, LED_ON);

                flag_blink = 0;
                blink = BLINK_SHORT;
            }
            if (payload_power_on() == 0)  // wait until the 12V is disabled
                FRU_status = M1;          // state <= M1
            break;
        }
        default:
            FRU_status = M0;
        }

        if (event & TIMER_EVENT)          // all 100 ms
        {
            ipmi_check_event_respond();   // response to ipmi
            sensor_monitoring_user();      // check temperature sensor
            ///MJ to be activated: check_rtm_state();            // check rtm
            //			  current_monitoring();				// This is to be changed in a user specific function call. VB
        }

        event = 0;
        set_sleep_mode(SLEEP_MODE_IDLE); // put the MCU to sleep
        sleep_mode();
    }
    return 0;
}


//***************/
void timer_event()   //Only called once from mmc_main.c
//***************/
{
    static u08 timer = 0;

    //  payload_status_check();   			// Used to check the FPGA and the DCDC. To be made user specific. VB
    timer++;
    HotSwap = get_hot_swap_status(); // check value of hotswap
    if (timer == 100)                // LED blinking time management
    {
        event |= TIMER_EVENT;
        timer = 0;

        if (blink > 0)
        {
            blink--;
            flag_blink = 0;
        }
        else
            flag_blink = 1;
    }
}


//*****************/
void init_port(void)    //Only called once from mmc_main.c; could be in-lined
//*****************/
{
    //port pins inputs with pull-ups and outputs at startup

    //Port A
    DDRA  = 0x00;  // Was set to 0x02. Changed by VB for V3 of HW.
    PORTA = 0x00;

    //Port B
    PORTB = 0x40;  // Was set to 0x00. Changed by VB for V3 of HW.
    DDRB  = 0xE1;

    //Port C
    DDRC  = 0xCC;  // Was set to 0x49. Changed by VB for V3 of HW.
    PORTC = 0xCC;  // Was set to 0x09. Changed by VB for V3 of HW.

    //Port D
    DDRD  = 0x00;
    PORTD = 0x00;

    //Port E
    DDRE  = 0x00;
    PORTE = 0x00;

    //Port G
    DDRG  = 0x00;  // Was set to 0x10. Changed by VB for V3 of HW.
    PORTG = 0x00;

    if (USER_CODE == 2)
        init_port_user();
}
