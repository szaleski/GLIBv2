//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: sdr.h
// 
// Title		: SDR header file
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : Sensor Data Recoerds definitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#ifndef SDR_H
#define SDR_H

#include "avrlibtypes.h"
#include "project.h"


//analog or discrete sensors
#define ANALOG_SENSOR   0x01
#define DISCRETE_SENSOR 0x02





#define HOT_SWAP_SENSOR     0
#define TEMPERATURE_SENSOR1 1
#define TEMPERATURE_SENSOR2 2
#define VOLTAGE_OK_SENSOR   3
#define FPGA_MASTER        4
#define VOLT_12       		5
#define FPGA_SLAVE          6
#define TEMPERATURE_SENSOR3 7




#define TEMPERATURE_SENSORS 2

//sensor type codes
#define TEMPERATURE     0x01
#define VOLTAGE         0x02
#define HOT_SWAP        0xF2

//sensor unit type 
#define DEGREES_C 0x01
#define VOLTS     0x02


/*
 * Temperature States
 */
#define TEMP_STATE_NORMAL	    0x00	   /* temp is in normal range */
#define TEMP_STATE_LOW		    0x01	   /* temp is below lower non critical */
#define TEMP_STATE_LOW_CRIT	    0x02	   /* temp is below lower critical */
#define TEMP_STATE_LOW_NON_REC	0x04	   /* temp is below lower non recoverable */
#define TEMP_STATE_HIGH		    0x08	   /* temp is higher upper non critical */
#define TEMP_STATE_HIGH_CRIT	0x10	   /* temp is higher upper critical */
#define TEMP_STATE_HIGH_NON_REC	0x20	   /* temp is higher high non recoverable */




//module handle sensor status
#define HOT_SWAP_CLOSED   0x00
#define HOT_SWAP_OPENED   0x01
#define HOT_SWAP_QUIESCED 0x02




typedef struct sensor
{
    unsigned char value;
    unsigned char state;
    unsigned char type;
    unsigned char event_dir;
    unsigned char sdr_ptr;
} sensor_t;





//initialization of the SDRs
void sdr_init(u08 address);

//initialization of the sensors
void sensor_init();


//reading payload status                 
void payload_status_check();

int payload_power_on();


//reading temperature
void temp_monitoring();

//reading module handle status
u08 get_hot_swap_status();

u08 ipmi_sdr_info(u08* buf, u08 sdr_sensor);

u08 ipmi_sdr_get(u08* id, u08 offs, u08 size, u08* buf);

u08 ipmi_get_sensor_reading( u08 sensor_number, u08* buf );

u08 ipmi_picmg_get_device_locator( u08* buf );








#endif //SDR_H
