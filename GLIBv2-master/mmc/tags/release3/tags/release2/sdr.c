//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: sdr_main.c
// 
// Title		: SDR records
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Frédéric Bompard (CPPM)
// Modified by  : Paschalis Vichoudis (CERN)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : Sensor Data Records, temperature and voltage management of the AMC module
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

//MJ: check for obsolete includes
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include "avrlibdefs.h"
#include "sdr.h"
#include "led.h"
#include "i2csw.h"
#include "ipmi_if.h"
#include "fru.h"
#include "a2d.h"
#include "user_code_select.h"


// External variables
extern u08 run_type;
extern u08 *sdrPtr[NUM_SDR];
extern u08 sdrLen[NUM_SDR];
extern sensor_t sens[NUM_SENSOR];
extern u08 SDR0[];


//**********************/
void sdr_init(u08 ipmiID)   //Called from mmc_main.c
//**********************/
{
    u08 *ptrSDR, i;

    for (i = 0; i < NUM_SDR; i++)
    {
        ptrSDR = sdrPtr[i];
        ptrSDR[4] = sdrLen[i] - 5;                   //set length field in sensor
        if (ptrSDR[3] == 0x02 || ptrSDR[3] == 0x01)  //full / compact sensor: fill Entity ID
            ptrSDR[9] = 0x60 | ((ipmiID - 0x70) >> 1);
        else if (ptrSDR[3] == 0x12)                  //Device Locator record: fill Entity ID
            ptrSDR[13] = 0x60 | ((ipmiID - 0x70) >> 1);

        ptrSDR[0] = i; 		//fill record ID
        ptrSDR[1] = 0x00;   //MSB is always 0        //MJ: why do we have to fix this here? Can it not be done in the definition of the SDRx[] arrays above?
        ptrSDR[5] = ipmiID; //Sensor Owner ID
    }
}


//***************/
void sensor_init()   //Called from mmc_main.c
//***************/
{
    u08 i, j;

    sensor_init_user();

    if (run_type == INCRATE)
    {
        for (i = 0; i < NUM_SENSOR; i++)
        {
            for (j = 0; j < NUM_SDR; j++)
            {
                if ((sdrPtr[j][3] == 0x02 || sdrPtr[j][3] == 0x01) && sdrPtr[j][7] == i)
                    break;
            }
            sens[i].type      = sdrPtr[j][12];
            sens[i].event_dir = sdrPtr[j][13];
            sens[i].value     = 0;
            sens[i].state     = 0;
            sens[i].sdr_ptr   = j;
        }
    }

    a2dInit();
    a2dSetChannel(0);
    a2dStartConvert();
}


//reading sensors's values                  
//************************/
void payload_status_check()   //MJ: This function seems to be unused
//************************/
{
    sens[VOLTAGE_OK_SENSOR].value = inb(LOW_VOLTAGE_POK_PORT) & BV(LOW_VOLTAGE_POK_PIN) ? 1 : 0;   //MJ: Are we happy with ternary operations?
    sens[VOLTAGE_OK_SENSOR].state = (inb(LOW_VOLTAGE_POK_PORT) & BV(LOW_VOLTAGE_POK_PIN)) ? 0 : 1;

    if (sens[VOLTAGE_OK_SENSOR].value)
        local_led_control(RED_LED, LED_OFF);
    else
    {
        local_led_control(RED_LED, LED_ON);
        local_led_control(GREEN_LED, LED_OFF);
        return;
    }

    if (inb(FPGA_INIT_MASTER_DONE_PORT) & BV(FPGA_INIT_MASTER_DONE_PIN))
        sens[FPGA_MASTER].value = 1;
    else
        sens[FPGA_MASTER].value = 0;

    if (inb(FPGA_INIT_SLAVE_DONE_PORT) & BV(FPGA_INIT_SLAVE_DONE_PIN))
        sens[FPGA_SLAVE].value = 1;
    else
        sens[FPGA_SLAVE].value = 0;
}


//*******************/
u08 payload_power_on()   //Called from mmc_main.c
//*******************/
{
    if (sens[VOLT_12].value > 0x90)
    {
        ipmi_picmg_fru_control(POWER_UP);
        return(1);
    }
    else
        return(0);
}


//***********************************/
void check_temp_event(u08 temp_sensor)    //Called from sdr.c   //MJ: Was declared "static". Why?
//***********************************/
{
    // Check sensor Threshold limits

    switch (sens[temp_sensor].state)
    {
    case TEMP_STATE_LOW_NON_REC:
        if (sens[temp_sensor].value >= (sdrPtr[sens[temp_sensor].sdr_ptr][39] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        { //higher than lower non-recoverable
            sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNR_GH); //lower non recoverable going high
        }
        break;

    case TEMP_STATE_LOW_CRIT:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][39])
        { //temp is below lower non-recoverable
            sens[temp_sensor].state = TEMP_STATE_LOW_NON_REC;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNR_GL); //lower non recoverable going low
        }
        if (sens[temp_sensor].value > (sdrPtr[sens[temp_sensor].sdr_ptr][40] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        { //higher than lower critical
            sens[temp_sensor].state = TEMP_STATE_LOW;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LC_GH); //lower critical going high
        }
        break;

    case TEMP_STATE_LOW:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][40])
        { //temp is below lower critical
            sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LC_GL); //lower critical going low
        }
        if (sens[temp_sensor].value > (sdrPtr[sens[temp_sensor].sdr_ptr][41] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        { //lower non critical
            sens[temp_sensor].state = TEMP_STATE_NORMAL;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNC_GH); //lower non critical going high
        }
        break;

    case TEMP_STATE_NORMAL:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][41])
        { //temp is below lower non-critical
            sens[temp_sensor].state = TEMP_STATE_LOW;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNC_GL); //lower non critical going low
        }
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][38])
        { //higher than upper non-critical
            sens[temp_sensor].state = TEMP_STATE_HIGH;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNC_GH); //upper non critical going high
        }
        break;

    case TEMP_STATE_HIGH:
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][37])
        { //higher than upper critical
            sens[temp_sensor].state = TEMP_STATE_HIGH_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UC_GH); //upper critical going high
        }
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][38] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        { //lower than upper non-critical
            sens[temp_sensor].state = TEMP_STATE_NORMAL;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNC_GL); //upper non critical going low
        }
        break;
    case TEMP_STATE_HIGH_CRIT:
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][36])
        { //higher than upper non-recoverable
            sens[temp_sensor].state = TEMP_STATE_HIGH_NON_REC;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNR_GH); //upper non-recoverable going high
        }
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][37] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        { //lower than upper critical
            sens[temp_sensor].state = TEMP_STATE_HIGH;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UC_GL); //upper critical going low
        }
        break;
    case TEMP_STATE_HIGH_NON_REC:
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][36] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        { //below upper non-recoverable
            sens[temp_sensor].state = TEMP_STATE_HIGH_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNR_GL); //upper non-recoverable going low
        }
        break;
    default:   //MJ: should we signal an error if we end up here?
        break;
    }
}


//**********************/
u08 get_hot_swap_status()   //Called from mmc_main.c
//**********************/
{
    return (inb(HOT_SWAP_HANDLE_PPIN) & BV(HOT_SWAP_HANDLE_PIN)) ? 1 : 0;   //MJ: Ternary operator
}


//****************************************/
u08 ipmi_sdr_info(u08* buf, u08 sdr_sensor)  //Called from ipmi_if.c
//****************************************/
{
    if (sdr_sensor)
        buf[0] = NUM_SDR;    //Number of sdr
    else
        buf[0] = NUM_SENSOR; //Number of sensors

    buf[1] = 0x01;           //static sensor population
    return 2;
}


//****************************************************/
u08 ipmi_sdr_get(u08* id, u08 offs, u08 size, u08* buf)  //Called from ipmi_if.c
//****************************************************/
{
    u16 recordID;
    u08 len = 0, i;

    recordID = id[0] | (id[1] << 8);

    if (recordID > NUM_SDR || size > MAX_BYTES_READ || offs > MAX_RECORD_SIZE)
        return(0xff);

    if (offs > sdrLen[recordID] || (offs + size) > sdrLen[recordID])   //MJ: The first condition seems to be included in the second
        return(0xff);

    if ((recordID + 1) < NUM_SDR)
    {
        buf[len++] = (recordID + 1) & 0xff; //next record ID
        buf[len++] = (recordID + 1) >> 8;   //next record ID
    }
    else
    {
        buf[len++] = 0xff; //last record ID
        buf[len++] = 0xff; //last record ID
    }

    for (i = 0; i < size; i++)
        buf[len++] = sdrPtr[recordID][i + offs];

    return(len);
}


//*****************************************************/
u08 ipmi_get_sensor_reading(u08 sensor_number, u08* buf)  //Called from ipmi_if.c
//*****************************************************/
{
    if (sensor_number > NUM_SENSOR)      //MJ: Should this be "if(sensor_number > (NUM_SENSOR - 1)"?
        return(0xff);

    buf[0] = sens[sensor_number].value; //raw value of sensor
    buf[1] = 0xc0;                      //all events and scanning enabled
    buf[2] = sens[sensor_number].state;
    buf[3] = 0;
    //MJ: section 35.14 of IPMI 1.5 suggests that we should return "buf[3] = 0". When reading a sensor Natview shows
    //State = 0xrr00 with "rr" being a random value if "buf[3] = 0" is missing

    return(4);
}


//****************************************/
u08 ipmi_picmg_get_device_locator(u08* buf)  //Called from ipmi_if.c
//****************************************/
{
    u08 len = 0;

    buf[len++] = SDR0[0]; //record ID LSB
    buf[len++] = SDR0[1]; //record ID MSB

    return(len);
}

