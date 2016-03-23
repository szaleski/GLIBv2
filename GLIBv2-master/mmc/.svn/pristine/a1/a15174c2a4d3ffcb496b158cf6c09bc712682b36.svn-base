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
#include "rtm_mng.h"
#include "user_code_select.h"


// External variables
extern u08 run_type;
extern u08 rtm_sensor_enable;
extern u08 rtm_hs_enable;
extern u08 *sdrPtr[NUM_SDR_AMC + NUM_SDR_RTM];
extern u08 sdrLen[NUM_SDR_AMC + NUM_SDR_RTM];
extern sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];
extern u08 SDR0[];


//**********************/
void sdr_init(u08 ipmiID)   //Called from mmc_main.c
//**********************/
{
    u08 *ptrSDR, i;

    for (i = 0; i < NUM_SDR_AMC + NUM_SDR_RTM; i++)
    {
        ptrSDR = sdrPtr[i];
        ptrSDR[4] = sdrLen[i] - 5;                          //set length field in sensor
        if (ptrSDR[3] == 0x02 || ptrSDR[3] == 0x01)         //full / compact sensor: fill Entity instance. See Table 37-1 of IPMI1.5
            ptrSDR[9] = 0x60 | ((ipmiID - 0x70) >> 1);      //VB: This should be checked more carefully according to chapter 33.1 of IPMIv1.5
        else if (ptrSDR[3] == 0x12)                         //Device Locator record: fill Entity instance. See Table 37-5 of IPMI1.5
            ptrSDR[13] = 0x60 | ((ipmiID - 0x70) >> 1);     //MJ: according to MTCA.4 REQ 3-62 the "entity instance" should be "AMC's site number + 60h". Does this code do that?

        ptrSDR[0] = i; 		//fill record ID
        ptrSDR[1] = 0x00;   //MSB is always 0               //MJ: why do we have to fix this here? Can it not be done in the definition of the SDRx[] arrays above?
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
        for (i = 0; i < (NUM_SENSOR_AMC + NUM_SENSOR_RTM); i++)
        {
            for (j = 0; j < (NUM_SDR_AMC + NUM_SDR_RTM); j++)
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


//*******************/
u08 payload_power_on()   //Called from mmc_main.c
//*******************/
{
    if (sens[VOLT_12].value > 0x82)
    {
        ipmi_picmg_fru_control(POWER_UP);
        return(1);
    }
    else
        return(0);
}


//***********************************/
void check_temp_event(u08 temp_sensor)    //Called from user_code.c   //MJ: Was declared "static". Why?
//***********************************/
{
    // Check sensor Threshold limits

    switch (sens[temp_sensor].state)
    {
    case TEMP_STATE_LOW_NON_REC:
        if (sens[temp_sensor].value >= (sdrPtr[sens[temp_sensor].sdr_ptr][39] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        {   //higher than lower non-recoverable
            sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNR_GH); //lower non recoverable going high
        }
        break;

    case TEMP_STATE_LOW_CRIT:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][39])
        {   //temp is below lower non-recoverable
            sens[temp_sensor].state = TEMP_STATE_LOW_NON_REC;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNR_GL); //lower non recoverable going low
        }
        if (sens[temp_sensor].value > (sdrPtr[sens[temp_sensor].sdr_ptr][40] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        {   //higher than lower critical
            sens[temp_sensor].state = TEMP_STATE_LOW;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LC_GH); //lower critical going high
        }
        break;

    case TEMP_STATE_LOW:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][40])
        {   //temp is below lower critical
            sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LC_GL); //lower critical going low
        }
        if (sens[temp_sensor].value > (sdrPtr[sens[temp_sensor].sdr_ptr][41] + sdrPtr[sens[temp_sensor].sdr_ptr][43]))
        {   //lower non critical
            sens[temp_sensor].state = TEMP_STATE_NORMAL;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNC_GH); //lower non critical going high
        }
        break;

    case TEMP_STATE_NORMAL:
        if (sens[temp_sensor].value <= sdrPtr[sens[temp_sensor].sdr_ptr][41])
        {   //temp is below lower non-critical
            sens[temp_sensor].state = TEMP_STATE_LOW;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_LNC_GL); //lower non critical going low
        }
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][38])
        {   //higher than upper non-critical
            sens[temp_sensor].state = TEMP_STATE_HIGH;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNC_GH); //upper non critical going high
        }
        break;

    case TEMP_STATE_HIGH:
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][37])
        {   //higher than upper critical
            sens[temp_sensor].state = TEMP_STATE_HIGH_CRIT;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UC_GH); //upper critical going high
        }
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][38] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        {   //lower than upper non-critical
            sens[temp_sensor].state = TEMP_STATE_NORMAL;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNC_GL); //upper non critical going low
        }
        break;
    case TEMP_STATE_HIGH_CRIT:
        if (sens[temp_sensor].value >= sdrPtr[sens[temp_sensor].sdr_ptr][36])
        {   //higher than upper non-recoverable
            sens[temp_sensor].state = TEMP_STATE_HIGH_NON_REC;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UNR_GH); //upper non-recoverable going high
        }
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][37] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        {   //lower than upper critical
            sens[temp_sensor].state = TEMP_STATE_HIGH;
            ipmi_event_send(temp_sensor, IPMI_THRESHOLD_UC_GL); //upper critical going low
        }
        break;
    case TEMP_STATE_HIGH_NON_REC:
        if (sens[temp_sensor].value < (sdrPtr[sens[temp_sensor].sdr_ptr][36] - sdrPtr[sens[temp_sensor].sdr_ptr][42]))
        {   //below upper non-recoverable
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
    if (rtm_sensor_enable)
    {
        if (sdr_sensor)
            buf[0] = NUM_SDR_AMC + NUM_SDR_RTM;       //Number of sdr records
        else
            buf[0] = NUM_SENSOR_AMC + NUM_SENSOR_RTM; //Number of sensors
    }
    else if (rtm_hs_enable)
    {
        if (sdr_sensor)
            buf[0] = NUM_SDR_AMC + 2;                 //Number of sdr records, just device locator and HS for the RTM
        else
            buf[0] = NUM_SENSOR_AMC + 1;              //Number of sensors, just HS for the RTM
    }
    else
    {
        if (sdr_sensor)
            buf[0] = NUM_SDR_AMC;                     //Number of sdr records
        else
            buf[0] = NUM_SENSOR_AMC;                  //Number of sensors
    }

    buf[1] = 0x01;                                    //static sensor population
    return 2;
}


//****************************************************/
u08 ipmi_sdr_get(u08* id, u08 offs, u08 size, u08* buf)  //Called from ipmi_if.c
//****************************************************/
{
    u16 recordID;
    u08 len = 0, i;
    u08 max_sens;

    if (rtm_sensor_enable)
        max_sens = NUM_SDR_AMC + NUM_SDR_RTM;
    else if (rtm_hs_enable)
        max_sens = NUM_SDR_AMC + 2;                    //Only device locator and hotSwap sensor enabled so far (waiting for payload power)
    else
        max_sens = NUM_SDR_AMC;

    recordID = id[0] | (id[1] << 8);

    if (recordID > max_sens || size > MAX_BYTES_READ || offs > MAX_RECORD_SIZE)
        return(0xff);

    if (offs > sdrLen[recordID] || (offs + size) > sdrLen[recordID])   //MJ: The first condition seems to be included in the second
        return(0xff);

    if ((recordID + 1) < max_sens)
    {
        buf[len++] = (recordID + 1) & 0xff; //next record ID
        buf[len++] = (recordID + 1) >> 8;   //next record ID
    }
    else
    {
        buf[len++] = 0xff;                  //last record ID
        buf[len++] = 0xff;                  //last record ID
    }

    for (i = 0; i < size; i++)
        buf[len++] = sdrPtr[recordID][i + offs];

    return(len);
}


//*******************************************************/
u08 ipmi_get_sensor_threshold(u08 sensor_number, u08* buf)  //Called from ipmi_if.c
//*******************************************************/
{
    u08 max_sens;

    if (rtm_sensor_enable)
        max_sens = NUM_SENSOR_AMC + NUM_SENSOR_RTM;
    else if (rtm_hs_enable)
        max_sens = NUM_SENSOR_AMC + 1;
    else
        max_sens = NUM_SENSOR_AMC;

    if (sensor_number > (max_sens - 1))
        return(0xff);

    //Only full sensors have limits. Therefore we have to check what sensor we are dealing with
    if(sdrPtr[sens[sensor_number].sdr_ptr][3] == 0x1)        //If it is a full sensor it has to have thresholds
    {
        buf[0] = 0x3f;    //MJ: We claim that all 6 thresholds are readable. This may not be true for all sensors. Therefore this hard coded value
                          //MJ: should be replaced by a sensor specific value. It is not yet clear how to implement this
                          //MJ: In any case the function should (partially) be moved into the user code section

        buf[1] = sdrPtr[sens[sensor_number].sdr_ptr][41];    //Lower non critical
        buf[2] = sdrPtr[sens[sensor_number].sdr_ptr][40];    //Lower critical
        buf[3] = sdrPtr[sens[sensor_number].sdr_ptr][39];    //Lower non recoverable
        buf[4] = sdrPtr[sens[sensor_number].sdr_ptr][38];    //Upper non critical
        buf[5] = sdrPtr[sens[sensor_number].sdr_ptr][37];    //Upper critical
        buf[6] = sdrPtr[sens[sensor_number].sdr_ptr][36];    //Upper non recoverable
    }
    else
    {
        buf[0] = 0x0;    //There are no thresholds
        buf[1] = 0x0;    //Dummy (Lower non critical)
        buf[2] = 0x0;    //Dummy (Lower critical)
        buf[3] = 0x0;    //Dummy (Lower non recoverable)
        buf[4] = 0x0;    //Dummy (Upper non critical)
        buf[5] = 0x0;    //Dummy (Upper critical)
        buf[6] = 0x0;    //Dummy (Upper non recoverable)
    }
    return(7);
}


//*************************************************************************************************************/
u08 ipmi_set_sensor_threshold(u08 sensor_number, u08 mask, u08 lnc, u08 lcr, u08 lnr, u08 unc, u08 ucr, u08 unr)  //Called from ipmi_if.c
//*************************************************************************************************************/
{
    u08 max_sens;

    //Note: Bits in the bytes 19 & 20 in the full sensor SDR define which thresholds can be modified
    //This function does not evaluate these bits. I assume the MCH takes care of that

    if (rtm_sensor_enable)
        max_sens = NUM_SENSOR_AMC + NUM_SENSOR_RTM;
    else if (rtm_hs_enable)
        max_sens = NUM_SENSOR_AMC + 1;
    else
        max_sens = NUM_SENSOR_AMC;

    if (sensor_number > (max_sens - 1))
        return(0xff);

    if(sdrPtr[sens[sensor_number].sdr_ptr][3] == 0x1)                     //If it is a full sensor it has to have thresholds
    {
        if (mask & 0x01) sdrPtr[sens[sensor_number].sdr_ptr][41] = lnc;   //Lower non critical
        if (mask & 0x02) sdrPtr[sens[sensor_number].sdr_ptr][40] = lcr;   //Lower critical
        if (mask & 0x04) sdrPtr[sens[sensor_number].sdr_ptr][39] = lnr;   //Lower non recoverable
        if (mask & 0x08) sdrPtr[sens[sensor_number].sdr_ptr][38] = unc;   //Upper non critical
        if (mask & 0x10) sdrPtr[sens[sensor_number].sdr_ptr][37] = ucr;   //Upper critical
        if (mask & 0x20) sdrPtr[sens[sensor_number].sdr_ptr][36] = unr;   //Upper non recoverable
    }

    return(0);
}


//*****************************************************/
u08 ipmi_get_sensor_reading(u08 sensor_number, u08* buf)  //Called from ipmi_if.c
//*****************************************************/
{
    u08 max_sens;

    if (rtm_sensor_enable)
        max_sens = NUM_SENSOR_AMC + NUM_SENSOR_RTM;
    else if (rtm_hs_enable)
        max_sens = NUM_SENSOR_AMC + 1;
    else
        max_sens = NUM_SENSOR_AMC;

    if (sensor_number > (max_sens - 1))
        return(0xff);

    buf[0] = sens[sensor_number].value;           //raw value of sensor. For the RTM hot swap sensor this is just "0"
#ifdef USE_RTM
    if (sensor_number >= FIRST_RTM_SENSOR)
        buf[1] = sens[sensor_number].evnt_scan;   //events and scanning only enabled if RTM is active
    else
        buf[1] = 0xc0;                            //all events and scanning enabled
#else
      buf[1] = 0xc0;                            //all events and scanning enabled
#endif
    buf[2] = sens[sensor_number].state;

    //Section 35.14 of IPMI 1.5 suggests that we should return "buf[3] = 0". When reading a sensor Natview shows
    //State = 0xrr00 with "rr" being a random value if "buf[3] = 0" is missing
    buf[3] = 0;

#ifdef USE_RTM
    if(sensor_number == RTM_HOT_SWAP_SENSOR)      //See MTCA.4, Table 3-2, Response data(5)
    {
        if (sens[RTM_HOT_SWAP_SENSOR].state & RTM_COMPATIBLE)
            buf[3] = 0x80;
        else
            buf[3] = 0x81;
    }
#endif

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

