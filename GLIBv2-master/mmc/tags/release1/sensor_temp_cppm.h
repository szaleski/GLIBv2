/*
 * sensor_temperature.h
 *
 *  Created on: 12 mai 2010
 *      Author: bompard
 */


#include "avrlibdefs.h"
#include "avrlibtypes.h"



#ifndef SENSOR_TEMP_CPPM_H_
#define SENSOR_TEMP_CPPM_H_


#define FPGA_MASTER_LSB_MSB REMOTE_1_LSB_MSB		// Address temperature on FPGA MASTER 10 bits
#define FPGA_SLAVE_LSB_MSB	REMOTE_2_LSB_MSB		// Address temperature on FPGA SLAVE 10 bits

#define FPGA_MASTER_MSB		REMOTE_1_MSB			// Address temperature on FPGA MASTER 8 bits
#define FPGA_SLAVE_MSB		REMOTE_2_MSB			// Address temperature on FPGA SLAVE 8 bits


#define ADD_SENSOR				0x32				// ADDRESS SENSOR I2C


#define MODE_LSB_MSB			0x02
#define MODE_MSB				0x01


#define LOCAL_SENSOR_MSB		0x10	// Address temperature local on chip 8 bits
#define REMOTE_1_MSB			0x11	// Address temperature on remote 1 8 bits
#define REMOTE_2_MSB			0x12	// Address temperature on remote 2 8 bits

#define LOCAL_SENSOR_LSB_MSB	0x20	// Address temperature local on chip 10 bits
#define REMOTE_1_LSB_MSB		0x21	// Address temperature on FPGA MASTER 10 bits
#define REMOTE_2_LSB_MSB		0x22	// Address temperature on FPGA SLAVE 10 bits

#define CONFIG_REGISTER			0x03	// read only
#define STATUS_REGISTER			0x02	// read/Write
#define CONTROL_REGISTER		0x06	// read/Write
#define REMOTE_DIODE_MODEL		0x30	// read/Write
#define REMOTE_MODE_CONTROL		0x07	// read/Write


//*****************************************************
//*					public fonction					  *
//*****************************************************


void init_sensor_temperature (void);
u08 read_temperature ( u08 add_sensor,u08 *buf );
u08 read_all_temperature(u08 mode , u08 *buf);
void read_config(u08 config , u08 * buf);
void write_config (u08 config, u08 *buf);

#endif /* SENSOR_TEMPERATURE_H_ */
