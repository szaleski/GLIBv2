/*
 * sensor_temperature.c
 *
 *  Created on: 12 mai 2010
 *      Author: bompard
 */

#include "sensor_temp_cppm.h"
#include "i2csw.h"
#include <avr/interrupt.h>	// include interrupt support







void init_sensor_temperature (void)
{
	u08 cmd=0;


	

	i2cswInit();


	cmd = 0x20;		// configure register for : < continuous conversion > < unsigned temperature  (0 to 255°C) >
	i2cswSend(ADD_SENSOR, CONFIG_REGISTER, 1, &cmd);

	cmd = 0x05;		// configure control register < activate filter on the diode >
	i2cswSend(ADD_SENSOR, CONTROL_REGISTER, 1, &cmd);

	cmd = 0x05;		// configure   < for mesure on procesor >
	i2cswSend(ADD_SENSOR, REMOTE_DIODE_MODEL, 1, &cmd);

	cmd = 0x77;		// configure   < mode enable >
	i2cswSend(ADD_SENSOR, REMOTE_MODE_CONTROL, 1, &cmd);

}


u08 read_temperature ( u08 add_sensor, u08 *buf )
{
	u08 mode = 1;

	if(add_sensor & 0x20) mode = 2;
	else mode = 1;
	CRITICAL_SECTION_START;
	i2cswReceive( ADD_SENSOR, add_sensor , mode , buf);
	CRITICAL_SECTION_END;
	return mode;
}

u08 read_all_temperature(u08 mode , u08 *buf)
{
	u08 address = 0;
	u08 length = 0;
	u08 i=0;
//---------------------------------------------------------------------------
	// read temperature local
		if(mode == MODE_LSB_MSB)
		{
			address = LOCAL_SENSOR_LSB_MSB;
			length = 2;
		}

		else
		{
			address = LOCAL_SENSOR_MSB;
			length = 2;
		}

	 CRITICAL_SECTION_START;
	i2cswReceive( ADD_SENSOR, address , length , &buf[i]); //
	
//---------------------------------------------------------------------------
	// read temperature remote 1
	if(mode == MODE_LSB_MSB)
	{
		address = REMOTE_1_LSB_MSB;
		i = 2;
	}
	else
	{
		address = REMOTE_1_MSB;
		i = 1;
	}

	i2cswReceive( ADD_SENSOR, address , length , &buf[i]); //

//---------------------------------------------------------------------------
	// read temperature remote 2
	if(mode == MODE_LSB_MSB)
	{
		address = REMOTE_2_LSB_MSB;
		i = 4;
	}
	else
	{
		address = REMOTE_2_MSB;
		i = 2;
	}

	i2cswReceive( ADD_SENSOR, address , length , &buf[i]); //
	CRITICAL_SECTION_END;
//---------------------------------------------------------------------------
	if(mode == MODE_LSB_MSB)
		return 6;
	else return 3;

}



void read_config(u08 config ,u08 *buf)
{
	CRITICAL_SECTION_START;
	i2cswReceive( ADD_SENSOR, config , 1 , buf); //
	CRITICAL_SECTION_END;
}

void write_config (u08 config, u08 *buf)
{
	CRITICAL_SECTION_START;
	i2cswSend(ADD_SENSOR, config, 1, buf);
	CRITICAL_SECTION_END;
}
