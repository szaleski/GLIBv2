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




// Include Files
#include <avr/io.h>		    // include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "global.h"	    	// include our global settings
#include "timer.h"		    // include timer function library
#include "ipmi_if.h"	    // ipmi interface
#include "led.h"
#include "rtm_mng.h"
#include "fru.h"
#include "avrlibdefs.h"
#include "project.h"
#include "user_code_select.h"


// Constants   //MJ: there is no mmc_main.h file yet

#define HOTSWAP_EVENT 0x01
#define TIMER_EVENT   0x02

#define BLINK_LONG	  10
#define BLINK_SHORT	  2


// External variables
extern u08 ipmb_address;
extern sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];
extern volatile ipmi_status status_bits;
//extern leds led[NUM_OF_LED];


// Global variables
volatile u08 event;
u08 rtm_sensor_enable = 0;
u08 rtm_hs_enable = 0;
u08 HotSwap, run_type, start_crate = 0, blink, flag_blink, event_request_possible = 0;
enum M_Status FRU_status;     //MJ: This variable reflects the M-status of the AMC. The corresponding variable for the RTM is called RTM_M_Status. Maybe the names could be harmonised

//MJ debug hack
u08 b1, b2, b3, b4;
u16 s1, s2, s3, s4;


// Function prototypes
void timer_event(void);
void init_port(void);


//***********/
int main(void)
//***********/
{
    u08 initial_state_sent = 0;

    // INIT
    event = 0;
	
    FRU_status = M0;

    // port pins inputs with pull-ups and outputs at startup
    init_port();

    if (EEPROM_read(0) == 0xff)   // MJ: Should we really load the EEPROM in the MMC code or should this be done once at production time?
        Write_FRU_Info_default(); // Write default value < FRU_information > in EEPROM

    i2cswInit();
    leds_init();

    if ((inb(ACSR) & BV(ACO)))    // check if the board is in the crate
    {
        run_type = INCRATE;
        ipmi_init();
        sdr_init(ipmb_address);
    }
    sbi(ACSR, ACD);

    sensor_init();                // init sensor
    timerInit();                  // init timer
    timerAttach(timer_event);
    wdt_enable(WDTO_1S);          // enable watchdog at 1 second
    sens[HOT_SWAP_SENSOR].value = HotSwap = get_hot_swap_status();   // check the hot swap
    if (HotSwap == HOT_SWAP_CLOSED)
        sens[HOT_SWAP_SENSOR].state = 0x01;  //See MTCA.4, table 3-2, Response data, byte 4, bit 0
    else
        sens[HOT_SWAP_SENSOR].state = 0x02;  //See MTCA.4, table 3-2, Response data, byte 4,  bit 1

    sei();                        // enable all interrupts
    // end INIT

    while(1)
    {
        wdt_reset();              // reset the watchdog
        ipmi_check_request();     // check if ipmi request <<data in from ipmi>>

        event_request_possible = get_event_request_ok();

        //set_led_pattern(0, 0x10 + event_request_possible, 1);
        if ((initial_state_sent == 0) && (event_request_possible == 1))
        {
            set_led_pattern(0, 0x70 + event_request_possible, 1);
            ipmi_event_send(HOT_SWAP_SENSOR, HotSwap); // send initial value of the hot_swap to the MCH board
            initial_state_sent = 1;
        }

        // State Machine Payload Management
        switch (FRU_status)
        {
        case M0:                  // Not Installed
        {
            //set_led_pattern(0, 0x00, 1);
            local_led_control(GREEN_LED, LED_OFF);   // see page 13 "Figure 5: Insertion and extraction sequence state diagram"
            if (run_type == INCRATE)
                FRU_status = M1;
            break;
        }
        case M1:                  // Inactive
        {
            //set_led_pattern(0, 0x10, 1);
            // LED management
            local_led_control(RED_LED, LED_ON);
            local_led_control(GREEN_LED, LED_OFF);
            local_led_control(BLUE_LED, LED_ON);

            ipmi_picmg_fru_control(POWER_DOWN);     // disable DCDC

            ///MJ: do we need this? if (start_crate == 0x01)                // MCH board read the last sensor from the FRU. The crate is initialised
            ///{

            //set_led_pattern(0xf0f0 + (HotSwap << 8) + sens[HOT_SWAP_SENSOR].value);
                if(((HotSwap != sens[HOT_SWAP_SENSOR].value) || (HotSwap == HOT_SWAP_CLOSED)))
                {
                    sens[HOT_SWAP_SENSOR].value = HotSwap; // record the new value in the structure

                    if (HotSwap == HOT_SWAP_CLOSED)        // hot_swap is closed state FRU = M2
                    {
                        sens[HOT_SWAP_SENSOR].state = 0x01;
                        FRU_status = M2;
                        blink = BLINK_LONG;                // init blink time led
                    }
                    else
                        sens[HOT_SWAP_SENSOR].state = 0x02;

                    ipmi_event_send(HOT_SWAP_SENSOR, HotSwap); // send value hot_swap to the MCH board
                }
            ///}
            break;
        }
        case M2:                  // Activation request
        {
            //set_led_pattern(0, 0x20, 1);
            if (flag_blink == 1)
            {
                //set_led_pattern(0, 0x28, 1);
                if (state_led(BLUE_LED) == LED_ON)
                    local_led_control(BLUE_LED, LED_OFF);
                else
                    local_led_control(BLUE_LED, LED_ON);

                flag_blink = 0;
                blink = BLINK_LONG;

                //if(event_request_possible)   
                //{
                    ipmi_event_send(HOT_SWAP_SENSOR, HotSwap);    // send value hot_swap to the MCH board   //MJ: This message will be sent periodically. Is that intended? MDC: fix for vadatech MCH. Yes, the command will be sent periodically until the MCH activates the PP
                    //first_send = 0;
                //}
            }
            if (payload_power_on() > 0)                       // check if 12V is OK
            {
                //set_led_pattern(0, 0x21, 1);
                ipmi_picmg_fru_control(POWER_UP);             // enable DCDC  //MJ: This seems to be a duplication. It is already done in payload_power_on()
                FRU_status = M3;                              // change state <= M3
            }
            else if (HotSwap != sens[HOT_SWAP_SENSOR].value)  // if the 12V is not present and the user change the hotswap state <= M1
            {
                //set_led_pattern(0, 0x22, 1);
                sens[HOT_SWAP_SENSOR].value = HotSwap;
                FRU_status = M1;
				ipmi_event_send(HOT_SWAP_SENSOR, HotSwap);
            }
            //set_led_pattern(0, 0x24, 1);
            break;
        }
        case M3:                  // Activation in progress
        {
            //set_led_pattern(0, 0x30, 1);
            // LED management
            local_led_control(BLUE_LED, LED_OFF);
            local_led_control(GREEN_LED, LED_OFF);

            b1 = inb(LOW_VOLTAGE_POK_PORT);
            b2 = BV(LOW_VOLTAGE_POK_PIN);
            b3 = b1;

            if (user_ok_for_m4())
                FRU_status = M4;
        }
        case M4:                  //  Active
        {
            //set_led_pattern(0, 0x40, 1);
            // LED management
            local_led_control(BLUE_LED, LED_OFF);
            local_led_control(RED_LED, LED_OFF);
            local_led_control(GREEN_LED, LED_ON);
			//sbi(RTMENBUFPORT, RTMENBUF);

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
            //set_led_pattern(0, 0x50, 1);
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
            //set_led_pattern(0, 0x60, 1);
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
		
        if (event & TIMER_EVENT)           // all 100 ms
        {
            ipmi_check_event_respond();    // respond to ipmi
            sensor_monitoring_user();      // check sensors
            if(FRU_status == M4)
			{
			    check_rtm_state();             // check RTM
                rtm_leds_blinking();           // MJ: This is unnecessary for AMCs without RTM. How can we make it optional?
            }
		}

        event = 0;
        set_sleep_mode(SLEEP_MODE_IDLE);   // put the MCU to sleep
        sleep_mode();
    }
    return 0;
}


//***************/
void timer_event()   //Only called once from mmc_main.c
//***************/
{
    static u08 timer = 0;

    //payload_status_check();        // Used to check the FPGA and the DCDC. To be made user specific. VB
    timer++;

    HotSwap = get_hot_swap_status(); // check value of hot swap
    if (timer == 100)                // LED blinking time management  //MJ: This could be exported to a separate function and added to the list of periodic actions in the main loop
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
void init_port(void)    //Only called once from mmc_main.c; could be in-lined  //MJ-VB: How much of this is user code?
//*****************/
{
    //Definition of DDR (Data Direction Register)
    //bit = 0: This line is an input line
    //bit = 1: This line is an output line
    //
    //Definition of PORT
    //If line is output and PORT bit = 0: Send logical 0
    //If line is output and PORT bit = 1: Send logical 1
    //If line is input and PORT bit = 0:  Disable the pull-up of this line
    //If line is input and PORT bit = 1:  Enable the pull-up of this line
    //
    //Ownership of the ports and their lines
    //G = This bit is part of the generic infrastructure
    //U = This bit is user defined
    //N = This bit is not used or does not exist
    //Example: GGGUUUNN
      //Bits 7-5 are "G"
      //Bits 4-2 are "U"
      //Bits 1-0 are "N"

    //The code below initialises the ports that contain "G" bits. User bits in these ports will be defined later in the user code section

    //Port A: GGGGGGGG
    DDRA  = 0x38;
    PORTA = 0x00;

    //Port B: GGGGGGGG
    DDRB  = 0xE1;
    PORTB = 0x00;

    //Port C: UUUUUUUU

    //Port D: GGGGGGGG
    DDRD  = 0x00;
    PORTD = 0x00;

    //Port E: UUUUGGNN
    DDRE  = 0x00;
    PORTE = 0x00;

    //Port F: GGGGUUUG
    DDRF  = 0x00;
    PORTF = 0x00;

    //Port G: NNNNUUUU

    if (USER_CODE == 2)
        init_port_user();  //Now the user can overlay his requirements but should respect the "G" bits
}
