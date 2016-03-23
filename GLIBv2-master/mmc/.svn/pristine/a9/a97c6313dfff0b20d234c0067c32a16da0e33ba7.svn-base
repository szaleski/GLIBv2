//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: sdr.h
// 
// Title		: SDR header file
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description  : Sensor Data Records definitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#ifndef SDR_H
#define SDR_H

#include "avrlibtypes.h"


// Constants

// Analogue or discrete sensors
#define ANALOG_SENSOR           1          //MJ: not used
#define DISCRETE_SENSOR         2          //MJ: not used

// Sensor unit type
#define DEGREES_C               1          //MJ: not used
#define VOLTS                   2          //MJ: not used

// Temperature States
#define TEMP_STATE_NORMAL	    0x00	   // temperature is in normal range
#define TEMP_STATE_LOW		    0x01	   // temperature is below lower non critical
#define TEMP_STATE_LOW_CRIT	    0x02	   // temperature is below lower critical
#define TEMP_STATE_LOW_NON_REC	0x04	   // temperature is below lower non recoverable
#define TEMP_STATE_HIGH		    0x08	   // temperature is higher upper non critical
#define TEMP_STATE_HIGH_CRIT	0x10	   // temperature is higher upper critical
#define TEMP_STATE_HIGH_NON_REC	0x20	   // temperature is higher high non recoverable

// Module handle sensor status
#define HOT_SWAP_CLOSED         0
#define HOT_SWAP_OPENED         1
#define HOT_SWAP_QUIESCED       2

#define MAX_RECORD_SIZE         64

// registers for temperature sensor
#define WCRW                    0x0a       // write conversion rate
#define RLTS                    0x00       // local temperature
#define RRTE                    0x01       // remote temperature


// Types
typedef struct sensor
{
    u08 value;
    u08 state;
    u08 type;
    u08 evnt_scan;   //MJ: This flag indicates if a RTM sensor is active (i.e. if the RTM is present)
    u08 event_dir;
    u08 sdr_ptr;
} sensor_t;


// Function prototypes
void sdr_init(u08 address);    //initialisation of the SDRs
void sensor_init();            //initialisation of the sensors
void payload_status_check();   //reading pay-load status
void check_temp_event(u08 temp_sensor);
u08 payload_power_on();
u08 get_hot_swap_status();     //reading module handle status
u08 ipmi_sdr_info(u08* buf, u08 sdr_sensor);
u08 ipmi_sdr_get(u08* id, u08 offs, u08 size, u08* buf);
u08 ipmi_get_sensor_reading(u08 sensor_number, u08* buf);
u08 ipmi_picmg_get_device_locator(u08* buf);
u08 ipmi_get_sensor_threshold(u08 sensor_number, u08* buf);
u08 ipmi_set_sensor_threshold(u08 sensor_number, u08 mask, u08 lnc, u08 lcr, u08 lnr, u08 unc, u08 ucr, u08 unr);



#endif //SDR_H
