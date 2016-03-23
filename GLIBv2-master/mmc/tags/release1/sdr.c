//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: sdr_main.c
// 
// Title		: SDR records
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : Sensor Data Records, temperature and voltage management of the AMC module
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

//*****************************************************************************
// Copyright (C) 2010 CPPM ( Centre physiques des Particules de MArseille )
//
// File Name	: sdr_main.c
// 
// Title		: SDR records
// Revision		: 2.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Modify       : Bompard Frédéric
//				: CPPM
//
// Description : Sensor Data Records, temperature and voltage management of the AMC module
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include "avrlibdefs.h"
#include "sdr.h"
#include "led.h"
#include "i2csw.h"
#include "ipmi_if.h"
#include "fru.h"
#include "a2d.h"

#include"sensor_temp_cppm.h"


#define MAX_RECORD_SIZE 64

extern u08 run_type;

/* regisers for temperature sensor*/
#define WCRW 0x0a //write conversion rate
#define RLTS 0x00 //local temperature
#define RRTE 0x01 //remote temperature





//************************************************************************
//**			Modified by Frédéric Bompard ( CPPM )					**
//************************************************************************


unsigned char SDR0[] = {0x00,0x00,0x51,0x12,0x14,0x00,0x00,0x00, 		/* Device Locator Record */
			0x29,0x00,0x00,0x00,0xc1,0x00,0x00,0xcb,
			'A','M','C','-','S','U','G','I','T','O','N'};


/* Temperature 1 - Full Sensor */
unsigned char SDR1[] = {
	/* sensor record header */
/*1*/	0x02,			/* record number, LSB */
	0x00,			/* record number,MSB */
	0x51,			/* IPMI protocol version */
	0x01,			/* record type: full sensor */
	0x33,			/* record length: remaining bytes -> SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, -> SDR_Init */
	0x00,			/* sensor owner LUN */
/*8*/	TEMPERATURE_SENSOR1,	/* sensor number */

	/* record body bytes */
	0xc1,			/* entity id: AMC Module */
	0x00,			/* entity instance -> SDR_Init */
	0x03,			/* init: event generation + scanning enabled */
	0x48,			/* capabilities: auto re-arm, Threshold is readable and setable, */
	0x01,			/* sensor type: Temperature */
	0x01,			/* sensor reading: Threshold */
	0x80,			/* LSB assert event mask: 3 bit value */
/*16*/	0x0a,			/* MSB assert event mask */
	0xa8,			/* LSB deassert event mask: 3 bit value */
	0x7a,			/* MSB deassert event mask */
	0x38,			/* LSB: readabled Threshold mask: all thresholds are readabled:  */
	0x38,			/* MSB: setabled Threshold mask: all thresholds are setabled: */
	0x80, 			//***** Modified by P.Vichoudis - it was: 0x00, /* sensor units 1 : 2's complement*/
	0x01,			/* sensor units 2 : Temperature*/
	0x00,			/* sensor units 3 : Modifier*/
/*24*/	0x00,			/* Linearization */
	0x01,			/* M */
	0x01,			/* M - Tolerance */
	0x00,			/* B */
	0x25,			/* B - Accuracy */ 
	0x88,			/* Sensor direction */
	0x00,			/* R-Exp , B-Exp */
	0x07,			/* Analogue characteristics flags */
/*32*/	30,			/* Nominal reading */
	50,				/* Normal maximum */
	25,				/* Normal minimum */
	0xFF,			/* Sensor Maximum reading */
	0x00,			/* Sensor Minimum reading */
	85,			/* Upper non-recoverable Threshold *///////////////////////////////////////////////////
	50,				/* Upper critical Threshold */
	75,				/* Upper non critical Threshold */
/*40*/	-20,			/* Lower non-recoverable Threshold */
	-10,			/* Lower critical Threshold */
	0,				/* Lower non-critical Threshold */
	0x02,			/* positive going Threshold hysteresis value */
	0x02,			/* negative going Threshold hysteresis value */
	0x00,			/* reserved */
	0x00,			/* reserved */
	0x00, 			/* OEM reserved */
/*48*/	0xD0,			/* 8 bit ASCII, number of bytes */  // D0
	'T','e','m','p','_','F','P','G','A','_','M','A','S','T','E','R'	/* sensor string */
};


/* Temperature 2 - Full Sensor */
unsigned char SDR2[] = {
	/* sensor record header */
/*1*/	0x03,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x01,			/* record type: full sensor */
	0x33,			/* record length: remaining bytes -> SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, -> SDR_Init */
	0x00,			/* sensor owner LUN */
/*8*/	TEMPERATURE_SENSOR2,	/* sensor number */

	/* record body bytes */
	0xc1,			/* entity id: AMC Module */
	0x00,			/* entity instance -> SDR_Init */
	0x03,			/* init: event generation + scanning enabled */
	0x48,			/* capabilities: auto re-arm, Threshold is readable and setable, */
	0x01,			/* sensor type: Temperature */
	0x01,			/* sensor reading: Threshold */
	0x80,			/* LSB assert event mask: 3 bit value */
/*16*/	0x0a,			/* MSB assert event mask */
	0xa8,			/* LSB deassert event mask: 3 bit value */
	0x7a,			/* MSB deassert event mask */
	0x38,			/* LSB: readabled Threshold mask: all thresholds are readabled:  */
	0x38,			/* MSB: setabled Threshold mask: all thresholds are setabled: */
	0x80, 			//***** Modified by P.Vichoudis - it was: 0x00, /* sensor units 1 : 2's complement*/
	0x01,			/* sensor units 2 : Temperature*/
	0x00,			/* sensor units 3 : Modifier*/
/*24*/	0x00,			/* Linearization */
	0x01,			/* M */
	0x01,			/* M - Tolerance */
	0x00,			/* B */
	0x25,			/* B - Accuracy */
	0x88,			/* Sensor direction */
	0x00,			/* R-Exp , B-Exp */
	0x07,			/* Analogue characteristics flags */
/*32*/	30,			/* Nominal reading */
	50,			/* Normal maximum */
	25,			/* Normal minimum */
	0xff,			/* Sensor Maximum reading */
	0x00,			/* Sensor Minimum reading */
	110,			/* Upper non-recoverable Threshold */
	80,			/* Upper critical Threshold */
	75,			/* Upper non critical Threshold */
/*40*/	-20,			/* Lower non-recoverable Threshold */
	-10,			/* Lower critical Threshold */
	0,			/* Lower non-critical Threshold */
	0x02,			/* positive going Threshold hysteresis value */
	0x02,			/* negative going Threshold hysteresis value */
	0x00,			/* reserved */
	0x00,			/* reserved */
	0x00, 			/* OEM reserved */
/*48*/	0xc8,			/* 8 bit ASCII, number of bytes */
	'T','e','m','p',' ','I','/','O'	/* sensor string */
};




unsigned char SDR3[] = {
	/* sensor record header */
/*1*/	0x03,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x01,			/* record type: full sensor */
	0x37,			/* record length: remaining bytes -> SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, -> SDR_Init */
	0x00,			/* sensor owner LUN */
/*8*/	HOT_SWAP_SENSOR,	/* sensor number */

	/* record body bytes */
	0xc1,			/* entity id: AMC Module */
	0x00,			/* entity instance -> SDR_Init */
	0x03,			/* init: event generation + scanning enabled */
	0xc1,			/* capabilities: auto re-arm,*/
	HOT_SWAP,			/* sensor type: HOT SWAP*/
	0x6f,			/* sensor reading*/
	0x07,			/* LSB assert event mask: 3 bit value */
/*16*/	0x00,			/* MSB assert event mask */
	0x07,			/* LSB deassert event mask: 3 bit value */
	0x00,			/* MSB deassert event mask */
	0x00,			/* LSB: readabled Threshold mask: all thresholds are readabled:  */
	0x00,			/* MSB: setabled Threshold mask: all thresholds are setabled: */
	0xc0,			/* sensor units 1 :*/
	0x00,			/* sensor units 2 :*/
	0x00,			/* sensor units 3 :*/
/*24*/	0x00,			/* Linearization */
	0x00,			/* M */
	0x00,			/* M - Tolerance */
	0x00,			/* B */
	0x00,			/* B - Accuracy */
	0x00,			/* Sensor direction */
	0x00,			/* R-Exp , B-Exp */
	0x00,			/* Analogue characteristics flags */
/*32*/	0x00,			/* Nominal reading */
	0x00,			/* Normal maximum */
	0x00,			/* Normal minimum */
	0x00,			/* Sensor Maximum reading */
	0x00,			/* Sensor Minimum reading */
	0x00,			/* Upper non-recoverable Threshold */
	0x00,			/* Upper critical Threshold */
	0x00,			/* Upper non critical Threshold */
/*40*/	0x00,			/* Lower non-recoverable Threshold */
	0x00,			/* Lower critical Threshold */
	0x00,			/* Lower non-critical Threshold */
	0x00,			/* positive going Threshold hysteresis value */
	0x00,			/* negative going Threshold hysteresis value */
	0x00,			/* reserved */
	0x00,			/* reserved */
	0x00, 			/* OEM reserved */
/*48*/	0xcc,			/* 8 bit ASCII, number of bytes */
	'F','R','U',' ','H','O','T','_','S','W','A','P'	/* sensor string */
};



unsigned char SDR4[] = {
	/* sensor record header */
	0x04,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x02,			/* record type: compact sensor */
	0x23,			/* record length: remaining bytes, SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, filled by SDR_Init */
	0x00,			/* sensor owner LUN */
	VOLTAGE_OK_SENSOR,	/* sensor number */

	/* record body bytes */
/*8*/	0xc1,			/* entity id: MCH */
	0x00,			/* entity instance, SDR_Init */
	0x03,			/* init: events + scanning enabled */
	0xc2,			/* capabilities: auto re-arm, global disable */
	0x08,			/* type: Power Supply(DC-to-DC converter) */
	0x6f,			/* sensor specific event/read */
	0x01,			/* LSB assert event mask: 1 bit value */
	0x00,			/* MSB assert event mask */
	0x01,			/* LSB deassert event mask: 1 bit value */
	0x00,			/* MSB deassert event mask */
	0x01,			/* LSB read event mask: 1 bit value */
	0x00,			/* MSB read event mask */
	0xc0,			/* sensor units 1 */
	0x00,			/* sensor units 2 */
	0x00,			/* sensor units 3 */
	0x01,			/* sharing: 1 sensor */
	0x00,			/* no entity instance string */
	0x00, 			/* no positive threshold hysteresis */
	0x00, 			/* no negative threshold hysteresis */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* OEM reserved */
	0xc8,			/* 8 bit ASCII, number of bytes */
	'P','O','W','E','R',' ','O', 'K'	/* sensor string */
};

unsigned char SDR5[] = {
	/* sensor record header */
	0x05,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x02,			/* record type: compact sensor */
	0x23,			/* record length: remaining bytes, SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, filled by SDR_Init */
	0x00,			/* sensor owner LUN */
	FPGA_MASTER,	/* sensor number */

	/* record body bytes */
/*8*/	0xc1,			/* entity id: MCH */
	0x00,			/* entity instance, SDR_Init */
	0x03,			/* init: events + scanning enabled */
	0xc2,			/* capabilities: auto re-arm, global disable */
	0x07,			/* type: Power Supply(DC-to-DC converter) */
	0x0a,			/* sensor discrete event/read */
	0x01,			/* LSB assert event mask: 1 bit value */
	0x00,			/* MSB assert event mask */
	0x01,			/* LSB deassert event mask: 1 bit value */
	0x00,			/* MSB deassert event mask */
	0x01,			/* LSB read event mask: 1 bit value */
	0x00,			/* MSB read event mask */
	0xc0,			/* sensor units 1 */
	0x00,			/* sensor units 2 */
	0x00,			/* sensor units 3 */
	0x01,			/* sharing: 1 sensor */
	0x00,			/* no entity instance string */
	0x00, 			/* no positive threshold hysteresis */
	0x00, 			/* no negative threshold hysteresis */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* OEM reserved */
	0xcb,			/* 8 bit ASCII, number of bytes */
	'F','P','G','A','_','M','A','S','T','E','R'	/* sensor string */
};



/* 12V Payload power sensor */
unsigned char SDR6[] = {
	/* sensor record header */
/*1*/	0x06,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x01,			/* record type: full sensor */
	0x2e,			/* record length: remaining bytes -> SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, -> SDR_Init */
	0x00,			/* sensor owner LUN */
/*8*/	VOLT_12,	/* sensor number */

	/* record body bytes */
	0xc1,			/* entity id: AMC Module */
	0x00,			/* entity instance -> SDR_Init */
	0x03,			/* init: event generation + scanning enabled */
	0x48,			/* capabilities: auto re-arm, Threshold is readable and setable, */
	VOLTAGE,			/* sensor type: Voltage */
	0x01,			/* sensor reading: Threshold */
	0x80,			/* LSB assert event mask: 3 bit value */
/*16*/	0x0a,			/* MSB assert event mask */
	0xa8,			/* LSB deassert event mask: 3 bit value */
	0x7a,			/* MSB deassert event mask */
	0x38,			/* LSB: readabled Threshold mask: all thresholds are readabled:  */
	0x38,			/* MSB: setabled Threshold mask: all thresholds are setabled: */
	0x00,			/* sensor units 1 : 2's complement*/ //***** Modified by P.Vichoudis: The value is correct, the comment is wrong 
	0x04,			/* sensor units 2 : Temperature*/
	0x00,			/* sensor units 3 : Modifier*/
/*24*/	0x00,			/* Linearization */
	0x4e,			/* M */
	0x02,			/* M - Tolerance */
	0x00,			/* B */
	0x04,			/* B - Accuracy */
	0x0c,			/* Sensor direction */
	0xd0,			/* R-Exp , B-Exp */
	0x07,			/* Analogue characteristics flags */
/*32*/	0x7f,			/* Nominal reading */
	0xef,			/* Normal maximum */
	0xab,			/* Normal minimum */
	0xff,			/* Sensor Maximum reading */
	0x00,			/* Sensor Minimum reading */
	0xf7,			/* Upper non-recoverable Threshold */
	0xf2,			/* Upper critical Threshold */
	0xe9,			/* Upper non critical Threshold */
/*40*/	0xa2,			/* Lower non-recoverable Threshold */
	0xa7,			/* Lower critical Threshold */
	0xab,			/* Lower non-critical Threshold */
	0x02,			/* positive going Threshold hysteresis value */
	0x02,			/* negative going Threshold hysteresis value */
	0x00,			/* reserved */
	0x00,			/* reserved */
	0x00, 			/* OEM reserved */
/*48*/	0xc3,			/* 8 bit ASCII, number of bytes */
	'1','2','V'	/* sensor string */
};



/* Temperature 1 - Full Sensor */
unsigned char SDR7[] = {
	/* sensor record header */
/*1*/	0x02,			/* record number, LSB */
	0x00,			/* record number,MSB */
	0x51,			/* IPMI protocol version */
	0x01,			/* record type: full sensor */
	0x33,			/* record length: remaining bytes -> SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, -> SDR_Init */
	0x00,			/* sensor owner LUN */
/*8*/	TEMPERATURE_SENSOR3,	/* sensor number */

	/* record body bytes */
	0xc1,			/* entity id: AMC Module */
	0x00,			/* entity instance -> SDR_Init */
	0x03,			/* init: event generation + scanning enabled */
	0x48,			/* capabilities: auto re-arm, Threshold is readable and setable, */
	0x01,			/* sensor type: Temperature */
	0x01,			/* sensor reading: Threshold */
	0x80,			/* LSB assert event mask: 3 bit value */
/*16*/	0x0a,			/* MSB assert event mask */
	0xa8,			/* LSB deassert event mask: 3 bit value */
	0x7a,			/* MSB deassert event mask */
	0x38,			/* LSB: readabled Threshold mask: all thresholds are readabled:  */
	0x38,			/* MSB: setabled Threshold mask: all thresholds are setabled: */
	0x80, 			//***** Modified by P.Vichoudis - it was: 0x00, /* sensor units 1 : 2's complement*/
	0x01,			/* sensor units 2 : Temperature*/
	0x00,			/* sensor units 3 : Modifier*/
/*24*/	0x00,			/* Linearization */
	0x01,			/* M */
	0x01,			/* M - Tolerance */
	0x00,			/* B */
	0x25,			/* B - Accuracy */ 
	0x88,			/* Sensor direction */
	0x00,			/* R-Exp , B-Exp */
	0x07,			/* Analogue characteristics flags */
/*32*/	30,			/* Nominal reading */
	50,			/* Normal maximum */
	25,			/* Normal minimum */
	0xff,			/* Sensor Maximum reading */
	0x00,			/* Sensor Minimum reading */
	110,			/* Upper non-recoverable Threshold */
	80,			/* Upper critical Threshold */
	75,			/* Upper non critical Threshold */
/*40*/	-20,			/* Lower non-recoverable Threshold */
	-10,			/* Lower critical Threshold */
	0,			/* Lower non-critical Threshold */
	0x02,			/* positive going Threshold hysteresis value */
	0x02,			/* negative going Threshold hysteresis value */
	0x00,			/* reserved */
	0x00,			/* reserved */
	0x00, 			/* OEM reserved */
/*48*/	0xcf,		//***** Modified by P.Vichoudis - it was: 0xce,			/* 8 bit ASCII, number of bytes */
	'T','e','m','p','_','F','P','G','A','_','S','L','A','V','E'	/* sensor string */
};




unsigned char SDR8[] = {
	/* sensor record header */
	0x05,			/* record number, LSB  - filled by SDR_Init()*/
	0x00,			/* record number, MSB  - filled by SDR_Init()*/
	0x51,			/* IPMI protocol version */
	0x02,			/* record type: compact sensor */
	0x23,			/* record length: remaining bytes, SDR_Init */

	/* record key bytes */
	0x00,			/* i2c address, filled by SDR_Init */
	0x00,			/* sensor owner LUN */
	FPGA_SLAVE,	/* sensor number */

	/* record body bytes */
/*8*/	0xc1,			/* entity id: MCH */
	0x00,			/* entity instance, SDR_Init */
	0x03,			/* init: events + scanning enabled */
	0xc2,			/* capabilities: auto re-arm, global disable */
	0x07,			/* type: Power Supply(DC-to-DC converter) */
	0x0a,			/* sensor discrete event/read */
	0x01,			/* LSB assert event mask: 1 bit value */
	0x00,			/* MSB assert event mask */
	0x01,			/* LSB deassert event mask: 1 bit value */
	0x00,			/* MSB deassert event mask */
	0x01,			/* LSB read event mask: 1 bit value */
	0x00,			/* MSB read event mask */
	0xc0,			/* sensor units 1 */
	0x00,			/* sensor units 2 */
	0x00,			/* sensor units 3 */
	0x01,			/* sharing: 1 sensor */
	0x00,			/* no entity instance string */
	0x00, 			/* no positive threshold hysteresis */
	0x00, 			/* no negative threshold hysteresis */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* reserved */
	0x00, 			/* OEM reserved */
	0xca,			/* 8 bit ASCII, number of bytes */
	'F','P','G','A','_','S','L','A','V','E'	/* sensor string */
};







unsigned char *sdrPtr[NUM_SDR] = {SDR0,SDR1,SDR2,SDR3,SDR4,SDR5,SDR6,SDR7,SDR8};
unsigned char sdrLen[NUM_SDR] = {
	sizeof(SDR0),
	sizeof(SDR1),
	sizeof(SDR2),
	sizeof(SDR3),
    sizeof(SDR4),
    sizeof(SDR5),
    sizeof(SDR6),
	sizeof(SDR7),
    sizeof(SDR8)
};

sensor_t sens[NUM_SENSOR];



void sdr_init( u08 ipmiID)
{ 
	unsigned char *ptrSDR;
	unsigned char i;

	for (i=0; i < NUM_SDR; i++){
		ptrSDR = sdrPtr[i];
		ptrSDR[4] = sdrLen[i]-5;		/* set length field in sensor */
		if(ptrSDR[3] == 0x02 || ptrSDR[3] == 0x01)		/* full / compact sensor: fill Entity ID */
			ptrSDR[9] = 0x60 | ((ipmiID-0x70) >>1);
		else if(ptrSDR[3] == 0x12)				/* Device Locator record: fill Entity ID */
			ptrSDR[13] = 0x60 | ((ipmiID-0x70) >>1);

		ptrSDR[0] = i; 			//***** Modified by P.Vichoudis - it was: ptrSDR[0] = i+1;		/* fill record ID */
		ptrSDR[1] = 0x00;		/* MSB is allways 0 */
		ptrSDR[5] = ipmiID;		/* Sensor Owner ID */
	}


}



//************************************************************************
//**		    End modify by Frédéric Bompard ( CPPM )					**
//************************************************************************





void sensor_init()
{

	u08 i,j;


//*************************************************************
//** 			Add by Bompard Frédéric ( CPPM )			 **
//************************************************************* 
		init_sensor_temperature();

//*************************************************************
//**		End Add by Frédéric Bompard ( CPPM )			 **
//*************************************************************


   if( run_type == INCRATE)
   {
	    for( i = 0; i < NUM_SENSOR; i++ )
	    {
	        for( j = 0; j< NUM_SDR; j++ )
	        {
	            if( (sdrPtr[j][3] == 0x02 || sdrPtr[j][3] == 0x01) && sdrPtr[j][7] == i )
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

  a2dSetChannel( 0 );
  a2dStartConvert();



}



//************************************************************************
//**			Modify by Frédéric Bompard ( CPPM )						**
//************************************************************************

unsigned char tempSensorRead(unsigned char sensor)
{
	u08 stemp;				       /* sensor temperature */
	u08 scmd;			      /* temperature sensor command */


	if(sensor == TEMPERATURE_SENSOR1)					// Temperature FPGA MASTER 
		scmd = FPGA_MASTER_MSB;			
	else if(sensor == TEMPERATURE_SENSOR3)				// Temperature FPGA SLAVE
		scmd = FPGA_SLAVE_MSB;			
	else												// Temperature LOCAL
		scmd = LOCAL_SENSOR_MSB; 



	read_temperature ( scmd, &stemp );



	return stemp;
}

//************************************************************************
//**		    End modify by Frédéric Bompard ( CPPM )					**
//************************************************************************




//reading sensors's values                  
void payload_status_check()
{
  //static u08 power_retry = 0;


  
 sens[VOLTAGE_OK_SENSOR].value    = inb( LOW_VOLTAGE_POK_PORT ) & BV( LOW_VOLTAGE_POK_PIN ) ? 1 : 0;
  sens[VOLTAGE_OK_SENSOR].state    = (inb( LOW_VOLTAGE_POK_PORT ) & BV( LOW_VOLTAGE_POK_PIN )) ? 0 : 1;
 
  if( sens[VOLTAGE_OK_SENSOR].value )  
        local_led_control(RED_LED, LED_OFF);
  else
  {
       local_led_control(RED_LED, LED_ON);
       local_led_control(GREEN_LED, LED_OFF);
       return;
  }


  if( inb( FPGA_INIT_MASTER_DONE_PORT ) & BV( FPGA_INIT_MASTER_DONE_PIN )) 
 			 sens[FPGA_MASTER].value  =  1 ;
	else sens[FPGA_MASTER].value  =  0 ;


	if	( inb( FPGA_INIT_SLAVE_DONE_PORT ) & BV( FPGA_INIT_SLAVE_DONE_PIN ) )
				sens[FPGA_SLAVE].value  =  1 ;
	else sens[FPGA_SLAVE].value  =  0 ;

//*********************************************************************
// works only on CPPM board
/*

	// check if FPGA master is DONE
  	if( sens[FPGA_MASTER].value )
    	    local_led_control(GREEN_LED, LED_ON);
 	else 
        local_led_control(GREEN_LED, LED_OFF);


	// check if FPGA slave is DONE
	if( sens[FPGA_SLAVE].value )
    	    local_led_control(GREEN_LED, LED_ON);
 	else 
        local_led_control(GREEN_LED, LED_OFF);

*/
//**************************************************************************


}


int payload_power_on()
{
  
    if( sens[VOLT_12].value > 0x90 )
    {
        ipmi_picmg_fru_control(POWER_UP);
        return 1;

    }
    else
        return 0;
}




static void check_temp_event(u08 temp_sensor)
{


    	/*
    	 * check sensor Threshold limits
    	 */
    	switch(sens[temp_sensor].state) 
        {
    	case TEMP_STATE_LOW_NON_REC:
    		if(sens[temp_sensor].value >=  (sdrPtr[sens[temp_sensor].sdr_ptr][39] + sdrPtr[sens[temp_sensor].sdr_ptr][43])) { /* higher than lower non-recoverable */
    			sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LNR_GH);	/* lower non recoverable going high */
    		}
    		break;

    	case TEMP_STATE_LOW_CRIT:
    		if(sens[temp_sensor].value <=  sdrPtr[sens[temp_sensor].sdr_ptr][39]) {			/* temp is below lower non-recoverable */
    			sens[temp_sensor].state = TEMP_STATE_LOW_NON_REC;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LNR_Gl);	/* lower non recoverable going low */
    		}
    		if(sens[temp_sensor].value >  (sdrPtr[sens[temp_sensor].sdr_ptr][40]  + sdrPtr[sens[temp_sensor].sdr_ptr][43])) { /* higher than lower critical*/
    			sens[temp_sensor].state = TEMP_STATE_LOW;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LC_GH);	/* lower critical going high */
    		}
    		break;

    	case TEMP_STATE_LOW:
    		if(sens[temp_sensor].value <=  sdrPtr[sens[temp_sensor].sdr_ptr][40]) {			/* temp is below lower critical */
    			sens[temp_sensor].state = TEMP_STATE_LOW_CRIT;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LC_GL);	/* lower critical going low */
    		}
    		if(sens[temp_sensor].value >  (sdrPtr[sens[temp_sensor].sdr_ptr][41]  + sdrPtr[sens[temp_sensor].sdr_ptr][43])) { /* lower non critical*/
    			sens[temp_sensor].state = TEMP_STATE_NORMAL;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LNC_GH);	/* lower non critical going high */
    		//	ledFn[LED1]=LED_OFF;		/* RED Led off */
    		}
    		break;

    	case TEMP_STATE_NORMAL:
    		if(sens[temp_sensor].value <=  sdrPtr[sens[temp_sensor].sdr_ptr][41]) {			/* temp is below  lower non-critical */
    			sens[temp_sensor].state = TEMP_STATE_LOW;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_LNC_GL);	/* lower non critical going low */
    		//	ledFn[LED1]=LED_ON;		/* RED Led on */
    		}
    		if(sens[temp_sensor].value >=  sdrPtr[sens[temp_sensor].sdr_ptr][38]) {		 	/* higher than upper non-critical*/
    			sens[temp_sensor].state = TEMP_STATE_HIGH;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UNC_GH);	/* upper non critical going high */
    		//	ledFn[LED1]=LED_ON;		/* RED Led on */
    		}
    		break;

    	case TEMP_STATE_HIGH:
    		if(sens[temp_sensor].value >=  sdrPtr[sens[temp_sensor].sdr_ptr][37]) {		 	/* higher than upper critical*/
    			sens[temp_sensor].state = TEMP_STATE_HIGH_CRIT;

						ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UC_GH);	/* upper critical going high */

	   		}
    		if(sens[temp_sensor].value <  (sdrPtr[sens[temp_sensor].sdr_ptr][38] - sdrPtr[sens[temp_sensor].sdr_ptr][42])) {	/* lower than upper non-critical */
    			sens[temp_sensor].state = TEMP_STATE_NORMAL;

			 			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UNC_GL);	/* upper non critical going low */
    		//	ledFn[LED1]=LED_OFF;		/* RED Led off */
    		}
    		break;
    	case TEMP_STATE_HIGH_CRIT:
    		if(sens[temp_sensor].value >=  sdrPtr[sens[temp_sensor].sdr_ptr][36]) {		 	/* higher than upper non-recoverable */
    			sens[temp_sensor].state = TEMP_STATE_HIGH_NON_REC;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UNR_GH);	/* upper non-recoverable going high */
    		}
    		if(sens[temp_sensor].value <  (sdrPtr[sens[temp_sensor].sdr_ptr][37] - sdrPtr[sens[temp_sensor].sdr_ptr][42])) {	/* lower than upper critical */
    			sens[temp_sensor].state = TEMP_STATE_HIGH;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UC_GL);	/* upper critical going low */
    		}
    		break;
    	case TEMP_STATE_HIGH_NON_REC:
    		if(sens[temp_sensor].value <  (sdrPtr[sens[temp_sensor].sdr_ptr][36] - sdrPtr[sens[temp_sensor].sdr_ptr][42])) {	/* below upper non-recoverable */
    			sens[temp_sensor].state = TEMP_STATE_HIGH_CRIT;
    			ipmi_event_send(temp_sensor,  IPMI_THRESHOLD_UNR_GL);		/* upper non-recoverable going low */
    		}
    		break;
    	default:
    		break;
    	}


}

u08 get_hot_swap_status()
{


 return (inb( HOT_SWAP_HANDLE_PPIN ) & BV( HOT_SWAP_HANDLE_PIN )) ? 1 : 0;


}

void temp_monitoring()
{



  sens[TEMPERATURE_SENSOR1].value  = tempSensorRead( TEMPERATURE_SENSOR1 );
  if( run_type == INCRATE)
     check_temp_event(TEMPERATURE_SENSOR1);

  sens[TEMPERATURE_SENSOR2].value  = tempSensorRead( TEMPERATURE_SENSOR2 );
  if( run_type == INCRATE)
     check_temp_event(TEMPERATURE_SENSOR2);

	sens[TEMPERATURE_SENSOR3].value  = tempSensorRead( TEMPERATURE_SENSOR3 );
  if( run_type == INCRATE)
     check_temp_event(TEMPERATURE_SENSOR3);



  sens[VOLT_12].value  = a2dConvert8bit(0);
}





u08 ipmi_sdr_info( u08* buf, u08 sdr_sensor )
{

  if( sdr_sensor )
    buf[0]  = NUM_SDR; //Number of sdr
  else
    buf[0]  = NUM_SENSOR; //Number of sensors

  buf[1]  = 0x01;           //static sensor population
  return 2;
  
}

u08 ipmi_sdr_get(u08* id, u08 offs, u08 size, u08* buf)
{
    u16 recordID;
    u08 len = 0, i;

    recordID = id[0] | (id[1] <<8);

    if( recordID > NUM_SDR || size > MAX_BYTES_READ ||  offs > MAX_RECORD_SIZE)
         return -1;

    if( offs > sdrLen[recordID] || (offs + size) > sdrLen[recordID] )
        return -1;

    if( (recordID+1) < NUM_SDR )
    {
		buf[len++] = (recordID+1) & 0xff;	/* next record ID */
		buf[len++] = (recordID+1) >> 8;	/* next record ID */
	} 
    else 
    {
		buf[len++] = 0xff;	/* last record ID */
		buf[len++] = 0xff;	/* last record ID */
	}

    for( i = 0; i < size; i++ )
    {
        buf[len++] = sdrPtr[recordID][i+offs];
    }

   
    return len;

}

u08 ipmi_get_sensor_reading( u08 sensor_number, u08* buf )
{
    if( sensor_number > NUM_SENSOR )
    return -1;


  buf[0] = sens[sensor_number].value; //raw value of sensor
  buf[1] = 0xc0; //all events and scanning enabled 
  buf[2] = sens[sensor_number].state;

  return 3;
  

}

u08 ipmi_picmg_get_device_locator(u08* buf)
{
    unsigned char len = 0;


	buf[len++] = SDR0[0];	/* record ID LSB */
	buf[len++] = SDR0[1];	/* record ID MSB */

	return len;

}















