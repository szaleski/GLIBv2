/*! \file rtm_mng.c \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'rtm_mng.c'
// Title		: main management functions for RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//*****************************************************************************



//Note: there is user code lfet in this file

#include "project.h"
#include <avr/io.h>
#include "i2csw.h"
#include "i2crtm.h"
#include "rtm_mng.h"
#include "fru.h"
#include "sdr.h"
#include "ipmi_if.h"
#include "timer.h"
#include "led.h"
#include "user_code_select.h"

#ifdef USE_RTM

//Global variables
enum M_Status RTM_M_Status = M0;
extern enum M_Status FRU_status;
extern u08 rtm_sensor_enable;
extern u08 rtm_hs_enable;
extern sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];

volatile u08 leds_blinking = 10;    //MJ: Why volatile?
u08 compat_checked = 0, blink_blue = 0;
u16 s1;


//*******************/
void check_rtm_state()  //Called from mmc_main.c
//*******************/
{
    u08 first_byte;
	
    if (bit_is_set(RTMPSPIN, RTMPS))    // Step MTCA.4, 3.5.2-14 OK  //Negative logic. Pin is set if RTM is absent
    {
        //set_led_pattern(1, 0xf0, 1);
        if (RTM_M_Status > M0)
        {
            rtm_sensor_enable = 0;
            rtm_hs_enable = 0;
            rtm_power(RTM_12V, POWER_DOWN);
            rtm_power(RTM_3V3, POWER_DOWN);
            rtm_sensor_set_state_user(0);
            cbi(RTMENBUFPORT, RTMENBUF);       //Disable the I2C buffer towards the RTM.
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_ABSENT);
            sens[RTM_HOT_SWAP_SENSOR].evnt_scan = 0;
            RTM_M_Status = M0;
        }
        sens[RTM_HOT_SWAP_SENSOR].state = RTM_ABSENT;
    }
    else       //RTM is present  Step MTCA.4, 3.5.1-1 OK
    {
        switch (RTM_M_Status)
        {
        case M0:
            //set_led_pattern(1, 0x00, 1);
            rtm_power(RTM_3V3, POWER_UP);                          //Enable management power. Step MTCA.4, 3.5.1-2) OK
            i2crtmInit();

            rtm_led_onoff(BLUE_LED, LED_ON);                       //Turn RTM blue LED on, Step MTCA.4, 3.5.1-3) OK

            i2cswReceive(RTM_EEPROM_ADDR, 0, 2, 1, &first_byte);   //Read the first byte in the EEPROM. If it is not 0xff the EEPROM has already been loaded
            if(first_byte == 0xff)
            {
                CRITICAL_SECTION_START;
                load_rtm_eeprom();
                CRITICAL_SECTION_END;        
            }
			//set_led_pattern(1, 0x01, 1);

            rtm_hs_enable = 1;  //Enable the HS sensor of the RTM. MTCA.4, 3.5.1-4 OK
            sens[RTM_HOT_SWAP_SENSOR].state = RTM_PRESENT | HOT_SWAP_OPEN_STATE;
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_PRESENT);            //MTCA.4, 3.5.1-4) OK (I hope)

            //load_rtm_eeprom();  // VB: This was added so that the checkcompat works OK
            //The EEPROM should come programmed from the "factory". Therefore the MMC S/W should not contain the EEPROM (FRU) data.
            //The IPMI Write_FRU_Data command should be supported for in-situ updates of the EEPROM data

            compat_checked = 0;

            RTM_M_Status = M1;    //According to MTCA.4, 3.5.1-5) the CM(MCH) should now internally advance the state of the RTM to M1
            break;
        case M1:
            //set_led_pattern(1, 0x10, 1);

            if (!compat_checked)
            {
                if (checkcompat())  //MTCA.4, 3.5.1-6a) OK
                {
					rtm_sensor_enable = 1;
					sens[RTM_HOT_SWAP_SENSOR].state |= RTM_COMPATIBLE;
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_COMPAT); //MTCA.4, 3.5.1-6b) OK
					
                }
                else
                {
                    sens[RTM_HOT_SWAP_SENSOR].state &= RTM_INCOMPATIBLE_MASK;
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_INCOMPAT);
                }
                compat_checked = 1;
                break;
            }

            if (FRU_status == M4 && (sens[RTM_HOT_SWAP_SENSOR].state & RTM_COMPATIBLE))  //MTCA.4, 3.5.1-7) OK because we will only go to M2 if the RTM is compatible
            //MJ: In the latest version Desy does not check the RTM_COMPATIBLE any more
            {
                //set_led_pattern(1, 0x11, 1);
                if (hs_switch_state(0) == 1) //Hot swap switch closed. MTCA.4, 3.5.1-8 and MTCA.4, 3.5.1-9 OK
                    RTM_M_Status = M2;
                //We should now expect a "blue LED long blink" command from the CM (3.5.1-10)) for FRU1
            }
            break;
        case M2:
            //set_led_pattern(1, 0x20, 1);
            //Do we have to mask the sensors in the SDR before we get to this point?
            //rtm_sensor_enable = 1;
            //MTCA.4, 3.5.1-12 OK

            sens[RTM_HOT_SWAP_SENSOR].evnt_scan = 0xc0;
            rtm_sensor_set_state_user(0xc0);

            //MJ: How to interpret 3.5.1-13) and 14)?

            RTM_M_Status = M3;  //MTCA.4, 3.5.1-15 OK
            break;
        case M3:
            //set_led_pattern(1, 0x30, 1);
            //Now we are waiting for a IPMI_PICMG_CMD_SET_POWER_LEVEL from the MCH. When it arrives rtm_set_power_level will do the job

            //Did the HS handle get opened?
            if (hs_switch_state(1) == 0)
                RTM_M_Status = M6;   //It is up to the MCH to send a power down command for the RTM
            break;
        case M4:
            //set_led_pattern(1, 0x40, 1);
            //Now we should get a IPMI_PICMG_CMD_SET_FRU_LED_STATE (LED OFF) from the MCH.

            //Did the HS handle get opened?
            if (hs_switch_state(1) == 0)    //MTCA.4, 3.5.1-1 and 3.5.1-1 (via hs_switch_state) OK
                RTM_M_Status = M5;

            break;
        case M5:
            //set_led_pattern(1, 0x50, 1);
            //MJ: What exactly happens in //MTCA.4, 3.5.2-5 ?
            //MJ: Do we also go to M4 if we receive a FRU activation from the MCH?

            //Now we are waiting for a IPMI_PICMG_CMD_FRU_CONTROL (FRU_QUIESCE) command from the MCH. This will take us to M6

            //If we internally detect that the HS handle is closed we go back to M4
            if (hs_switch_state(0) == 1)   //Switch has been closed again
                RTM_M_Status = M4;
            break;
        case M6:
            //set_led_pattern(1, 0x60, 1);
            rtm_quiesce();                                            //MTCA.4, 3.5.2-7 OK
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_QUIESCED);  //MTCA.4, 3.5.2-8 OK

            //Now we are waiting for a IPMI_PICMG_CMD_SET_POWER_LEVEL (power off) command from the MCH. This will take us to M1
            break;
        default:
            break;
        }
    }
}


//*******************************/
u08 hs_switch_state(u08 ref_state)   //Called from rtm_mng.c
//*******************************/
{
    //"ref_stat" is the expected state. If the actual state differs from it the switch has
    // been operated and a message has to be sent to the MCH
    // Definition of the switch states:
    // 1 = closed
    // 0 = opened

    u08 ios;

    ios = read_rtm_io_port();
    if (ios & (1 << RTM_HOT_SWAP_PIN)) //Hot swap is open
    {
        if (ref_state == 1)            //Switch was closed before
        {
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_OPENED);
            sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_OPEN_STATE;
            sens[RTM_HOT_SWAP_SENSOR].state &= 0xFE;
        }
        return(0); //Switch was opened
    }
    else                               //Hot swap is closed
    {
        if (ref_state == 0)            //Switch was open before
        {
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_CLOSED);
            sens[RTM_HOT_SWAP_SENSOR].state &= 0xFD;                   //Remove the "Module Handle Open" bit. See MTCA.4, table 3-2
            sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_CLOSE_STATE;
        }
        return(1);     //Switch is closed
    }
}


//******************************/
void rtm_set_power_level(u08 lev)   //Called from ipmi_if.c
//******************************/
{
    if (lev)
    {
        rtm_power(RTM_12V, POWER_UP);      //MTCA.4, 3.5.1-17 OK
        RTM_M_Status = M4;                 //MTCA.4, 3.5.1-18 OK
    }
    else
    {
        rtm_power(RTM_12V, POWER_DOWN);    //MTCA.4, 3.5.2-10 OK
        RTM_M_Status = M1;                 //MTCA.4, 3.5.2-11a OK
    }
}


//***********************************/
void rtm_led_onoff(u08 led, u08 onoff)      //Called from rtm_mng.c and ipmi_if.c
//***********************************/
{
   //set_led_pattern(0, 0x87, 1);
   if (led >= NUM_OF_RTM_LED)               // value out of range
       return;

   if (onoff == LED_ON || onoff == LED_OFF) //MTCA.4, 3.5.2-12 OK
   {
       rtm_led_control(led, onoff);
       blink_blue = 0;
   }
   else //blink                             //MTCA.4, 3.5.1-11 and 3.5.2-4 OK
   {
       blink_blue = 1;
   }
}


//*************************************/
void rtm_led_control(u08 led, u08 onoff)    //Called from rtm_mng.c
//*************************************/
{
   u08 reg_data;

   //set_led_pattern(0, 0x8f, 1);
   if (led <= GREEN_LED)   //Handle BLUE, RED and GREEN
   {
       reg_data = read_rtm_io_port();
       if (onoff == LED_ON)
       {
           if      (led == BLUE_LED) reg_data &= ~(1 << BLUE_LED_BIT);
           else if (led == RED_LED)  reg_data &= ~(1 << RED_LED_BIT);
           else                      reg_data &= ~(1 << GREEN_LED_BIT);
       }
       else if(onoff == LED_OFF)
       {
           if      (led == BLUE_LED) reg_data |= (1 << BLUE_LED_BIT);
           else if (led == RED_LED)  reg_data |= (1 << RED_LED_BIT);
           else                      reg_data |= (1 << GREEN_LED_BIT);
       }

       write_rtm_io_port(reg_data);
   }
   else
       rtm_led_control_user(led, onoff);
}


//*******************/
u08 read_rtm_io_port()    //Called from rtm_mng.c and i2crtm.c
//*******************/
{
    u08 reg_val;

    CRITICAL_SECTION_START;
    i2cswReceive(RTM_IO_PORTS_ADDR, 0, 0, 1, &reg_val);     //CERN special: AMC and RTM use the same I2C bus
    CRITICAL_SECTION_END;

    return reg_val;
}


//******************************/
void write_rtm_io_port(u08 value)    //Called from rtm_mng.c
//******************************/
{
    // write register address
    CRITICAL_SECTION_START;
    value |= 1 << RTM_HOT_SWAP_PIN;                    //This bit has to stay at "1". If it is "0" we cannot read the status of the HS switch any more
    i2cswSend(RTM_IO_PORTS_ADDR, 0, 0, 1, &value);     //CERN special: AMC and RTM use the same I2C bus
    CRITICAL_SECTION_END;
}


//*********************/
void rtm_leds_blinking()  //Called from mmc_main.c
//*********************/
{
    if(blink_blue)
    {
        if (leds_blinking == 0)
            leds_blinking = 10;

        if (leds_blinking == 10)
            rtm_led_control(BLUE_LED, LED_ON);

        if (leds_blinking == 5)
            rtm_led_control(BLUE_LED, LED_OFF);

        leds_blinking--;
    }
}


//***************/
void rtm_quiesce()   //Called from ipmi_if.c and rtm_nmg.c
//***************/
{
    //MJ: we should call a user function to allow the user to disable the Zone 3
    sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_QUISCED_STATE;
    sens[RTM_HOT_SWAP_SENSOR].state &= 0xFC;
}


//******************************************/
void rtm_power(u08 r_power, u08 on_off_power)    //Called from rtm_mng.c
//******************************************/
{
    switch(r_power)
    {
    case RTM_3V3:
        if (on_off_power == POWER_UP)
            sbi(RTMEN3V3PORT, RTMEN3V3);
		else
            cbi(RTMEN3V3PORT, RTMEN3V3);
        break;
    case RTM_12V:
        if (on_off_power == POWER_UP)
            sbi(RTMEN12VPORT, RTMEN12V);
        else
            cbi(RTMEN12VPORT, RTMEN12V);
        break;
    default:
        break;
    }
}


//******************************************/
u08 rtm_get_fru_inventory_area_info(u08* buf)   //Called from ipmi_if.c
//******************************************/
{
    u08 len = 0;

    buf[len++] = RTM_FRU_SIZE & 0xff;
    buf[len++] = RTM_FRU_SIZE >> 8;
    buf[len++] = 0x00; //byte access type

    return len;
}


//*************************************************/
u08 rtm_fru_data_read(u08* area, u08 len, u08* data)   //Called from ipmi_if.c
//*************************************************/
{
    u16 address;

    address = area[0] | (area[1] << 8);

    if (address >= RTM_FRU_SIZE)
        return(0xff);

    if (len > MAX_BYTES_READ)
        len = MAX_BYTES_READ;

    if ((address + len) > RTM_FRU_SIZE)
        len = RTM_FRU_SIZE - address;

    // write register address

    CRITICAL_SECTION_START;
    *data++ = len;
    i2cswReceive(RTM_EEPROM_ADDR, address, 2, len, data);      //MJ: CERN special: AMC and RTM use the same I2C bus
    CRITICAL_SECTION_END;

    return(len + 1);
}


//**************************************************/
u08 rtm_fru_data_write(u08* area, u08* data, u08 len)   //Called from ipmi_if.c
//**************************************************/
{
    u16 address;
    u08 rest_bytes;

    address = area[0] | (area[1] << 8);

    if (address >= RTM_FRU_SIZE)
        return(0xff);      //MJ: how does the calling function process this error?

    if ((address + len) > RTM_FRU_SIZE)
        len = RTM_FRU_SIZE - address;

    // write register address
    rest_bytes = 32 - address % 32;     //MJ: What is this limit of "32" about?

    CRITICAL_SECTION_START;
    if (rest_bytes < len)
    {
        i2cswSend(RTM_EEPROM_ADDR, address, 2, rest_bytes, data);                           //MJ: CERN special: AMC and RTM use the same I2C bus
        i2cswSend(RTM_EEPROM_ADDR, address + rest_bytes, 2, len - rest_bytes, data);
    }
    else
        i2cswSend(RTM_EEPROM_ADDR, address, 2, len, data);
    CRITICAL_SECTION_END;

    return len;
}


//**************/
u08 checkcompat()     //Called from rtm_mng.c
//**************/
{
    u16 address, reclen;
    u08 endoflist = 0, data[10];

    address = 0;

    CRITICAL_SECTION_START;
    i2cswReceive(RTM_EEPROM_ADDR, address, 2, 8, data);   //At this level we just need to read byte[5] which is an address offset //MJ: I guess this can be simplified...
    CRITICAL_SECTION_END;

    if (data[5] > 0 && data[5] != 0xff)
    {
        //set_led_pattern(0xf080);
        address = data[5] * 8;
        do
        {
            CRITICAL_SECTION_START;
            i2cswReceive(RTM_EEPROM_ADDR, address, 2, 10, data);
            CRITICAL_SECTION_END;

            endoflist = data[1] & 0x80;
            reclen = data[2] + 5;

            if ((data[0] == 0xc0) && (data[5] == 0x5A) && (data[6] == 0x31) && (data[8] == 0x30))
            {
                CRITICAL_SECTION_START;
                i2cswReceive(RTM_EEPROM_ADDR, address + 10, 2, 8, data);
                CRITICAL_SECTION_END;
                //We are comparing data from the EEPROM of the RTM with hard-coded information about the AMC.
                //MJ: where is the data format documented in the standard?  //Have a look at MTCA.4 Table 3-3 for a start
                if ((data[0] == 0x03) &&       // Type of Interface Identifier = OEM (See MTCA.4 table 3-7)
                    (data[1] == RTM_MFG_B1) && // Manufacturer IANA ID (LSB) //MJ: DESY IANA LSB
                    (data[2] == RTM_MFG_B2) && // Manufacturer IANA ID       //MJ: DESY IANA MSB
                    (data[3] == RTM_MFG_B3) && // Manufacturer IANA ID (MSB)
                    (data[4] == RTM_DEV_B1) && // Device designator (LSB)
                    (data[5] == RTM_DEV_B2) && // Device designator
                    (data[6] == RTM_DEV_B3) && // Device designator
                    (data[7] == RTM_DEV_B4))   // Device designator (MSB)
                    return(1);                 // RTM is compatible
            }

            address += reclen;
        }
        while (!endoflist);
    }

    //MJ: How to handle possible errors?
    return(0);   // RTM is not compatible
}
#else

//*******************/
void check_rtm_state()  //Called from mmc_main.c
//*******************/
{
}


//*******************************/
u08 hs_switch_state(u08 ref_state)   //Called from rtm_mng.c
//*******************************/
{
    return(0);
}


//******************************/
void rtm_set_power_level(u08 lev)   //Called from ipmi_if.c
//******************************/
{
}


//***********************************/
void rtm_led_onoff(u08 led, u08 onoff)      //Called from rtm_mng.c and ipmi_if.c
//***********************************/
{
}


//*************************************/
void rtm_led_control(u08 led, u08 onoff)    //Called from rtm_mng.c
//*************************************/
{
}


//*******************/
u08 read_rtm_io_port()    //Called from rtm_mng.c and i2crtm.c
//*******************/
{
    return(0);
}


//******************************/
void write_rtm_io_port(u08 value)    //Called from rtm_mng.c
//******************************/
{
}


//*********************/
void rtm_leds_blinking()  //Called from mmc_main.c
//*********************/
{
}


//***************/
void rtm_quiesce()   //Called from ipmi_if.c and rtm_nmg.c
//***************/
{
}


//******************************************/
void rtm_power(u08 r_power, u08 on_off_power)    //Called from rtm_mng.c
//******************************************/
{
}


//******************************************/
u08 rtm_get_fru_inventory_area_info(u08* buf)   //Called from ipmi_if.c
//******************************************/
{
    return(0);
}


//*************************************************/
u08 rtm_fru_data_read(u08* area, u08 len, u08* data)   //Called from ipmi_if.c
//*************************************************/
{
    return(0);
}


//**************************************************/
u08 rtm_fru_data_write(u08* area, u08* data, u08 len)   //Called from ipmi_if.c
//**************************************************/
{
    return(0);
}


//**************/
u08 checkcompat()     //Called from rtm_mng.c
//**************/
{
    return(0);   // RTM is not compatible
}

#endif




