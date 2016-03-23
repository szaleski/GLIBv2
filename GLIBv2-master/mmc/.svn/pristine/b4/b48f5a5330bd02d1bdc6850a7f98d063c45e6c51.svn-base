//*****************************************************************************
// File Name    : user_code.h
//
// User code for the ALB & RTM
//
// Author  : Markus Joos (markus.joos@cern.ch)
//
//*****************************************************************************

//User code for the ALB-RTM

// This file is the header file for user code.
// Users of the MMC S/W should only have to modify this file as well as user_code.c

#ifndef USER_CODE_H
#define USER_CODE_H

#include <avr/io.h>
#include "../avrlibtypes.h"
#include "../avrlibdefs.h"
#include "../a2d.h"
#include "../led.h"
#include "../sdr.h"
#include "../i2csw.h"
#include "../ipmi_if.h"
#include "../eeprom.h"
#include "../fru.h"
#include "../timer.h"
#include "../rtm_mng.h"
#include "../i2crtm.h"

#define USE_RTM

//********************************
//Constants for the IPMI DEVICE ID   //See ipmi_get_device_id in ipmi_if.c
//********************************
// firmware release
#define MMC_FW_REL_MAJ       1                  // major release, 7 bit
#define MMC_FW_REL_MIN       0                  // minor release, 8 bit
// manufacturer specific identifier codes
// NOTE: Manufacturer identification is handled by http://www.iana.org/assignments/enterprise-numbers
#define IPMI_MSG_MANU_ID_LSB 0x60               //CERN IANA ID = 0x000060
#define IPMI_MSG_MANU_ID_B2  0x00
#define IPMI_MSG_MANU_ID_MSB 0x00
#define IPMI_MSG_PROD_ID_LSB 0x35
#define IPMI_MSG_PROD_ID_MSB 0x12


//******************************
//Constants for the FRU / EEPROM
//******************************
#define SIZE_INFO           128  // size data for BOARD and PRODUCT Information
#define RTM_FRU_SIZE        66
#define USER_MAX_FRU        1    // 0 for normal AMCs, 1 for MTCA.4 modules

// Other constants for the RTM
#define RTMPSPIN            PINA
#define RTMPS               PA2
#define RTMEN3V3PORT        PORTA
#define RTMEN3V3            PA4
#define RTMEN12VPORT        PORTA
#define RTMEN12V            PA3
#define RTMEN12VPIN         PINA
#define RTMENBUFPORT        PORTA
#define RTMENBUF            PA5
#define RTM_IO_PORTS_ADDR   0x26
#define RTM_LED_PORTS_ADDR  0x20
#define RTM_EEPROM_ADDR     0x50


//**********************
//Constants for AMC LEDs
//**********************
#define NUM_OF_AMC_LED  12       //Note: the first 3 LEDs are mandatory //MJ: 3 mandatory, 8 user, 1 (LED3) not implemented
#define LOAD_GROUP_0    11       //Note: IDs 0, 1 and 2 are already used (see led.h). "LED 3" not used because it is not a user LED (see PICMG3.0, table 3-29)
#define LOAD_GROUP_1    10
#define LOAD_GROUP_2    9
#define LOAD_GROUP_3    8
#define LOAD_GROUP_4    7
#define LOAD_GROUP_5    6
#define LOAD_GROUP_6    5
#define LOAD_GROUP_7    4

//**********************
//Constants for RTM LEDs
//**********************
#define NUM_OF_RTM_LED      11  //Note: the first 3 LEDs are mandatory //MJ: 3 mandatory, 7 user, 1 (LED3) not implemented
#define RTM_LOAD_GROUP_0    4   //Note: IDs 0, 1 and 2 are already used by BLUE, GREEN and RED LED.  "LED 3" not used
#define RTM_LOAD_GROUP_1    5
#define RTM_LOAD_GROUP_2    6
#define RTM_LOAD_GROUP_3    7
#define RTM_LOAD_GROUP_4    8
#define RTM_LOAD_GROUP_5    9
#define RTM_LOAD_GROUP_6    10

//The definitions below are for the IO expander at address "RTM_IO_PORTS_ADDR"
#define BLUE_LED_BIT      1
#define RED_LED_BIT       2
#define GREEN_LED_BIT     3
#define RTM_HOT_SWAP_PIN  0

//Other definitions
#define RTM_EEPROM_WP     7
#define RTM_LED_ON        0
#define RTM_LED_OFF       1
#define IO_EXPANDER_ADD   0x3A

//*******************
// Constants for SDRs
//*******************

//Sensor numbers
//NOTE: First declare all AMC then all RTM sensors
//AMC sensors
#define HOT_SWAP_SENSOR     0
#define TEMPERATURE_SENSOR1 1
#define TEMPERATURE_SENSOR2 2
#define VOLTAGE_OK_SENSOR   3  //MJ: This sensor may be obsolete
#define VOLT_12             4
#define TEMPERATURE_SENSOR3 5
#define TEMPERATURE_SENSOR4 6
#define CURRENT_SENSOR      7
//RTM sensors
#define RTM_HOT_SWAP_SENSOR 8
#define RTM_CURRENT_SENSOR  9
#define TEMPERATURE_RTM1    10
#define TEMPERATURE_RTM2    11

#define NUM_SENSOR_AMC      8   //The device locators for AMC does not count
#define NUM_SENSOR_RTM      4   //The device locators for RTM does not count

#define NUM_SDR_AMC         9
#define NUM_SDR_RTM         5
#define FIRST_RTM_SENSOR    RTM_HOT_SWAP_SENSOR     //This number has to be the number of the first RTM sensor. For AMCs without RTM: (Last AMC sensor + 1)

// Sensor type codes
#define VOLTAGE             0x02
#define HOT_SWAP            0xF2

// Pins for a DCDC. //MJ-VB: Check if this is OK
#define DCDC_ENABLE_PORT PORTC
#define DCDC_ENABLE_PIN  PC7
#define REG_ENABLE_PORT PORTC
#define REG_ENABLE_PIN  PC6

//Constants for checkcompat (rtm_mng.c)
#define RTM_MULTI_RECORD_OFFSET 0x30
#define RTM_MFG_B1              0x60 // Manufacturer IANA ID (LSB)
#define RTM_MFG_B2              0x00 // Manufacturer IANA ID
#define RTM_MFG_B3              0x00 // Manufacturer IANA ID (MSB)
#define RTM_DEV_B1              0x00 // Device designator (LSB)
#define RTM_DEV_B2              0x00 // Device designator
#define RTM_DEV_B3              0x02 // Device designator
#define RTM_DEV_B4              0x08 // Device designator (MSB)


//*****************************
//Mandatory function prototypes
//*****************************
//NOTE: Users must not change the API of the functions listed below
void leds_init_user();
void local_led_control_user(u08 led_n, u08 led_state);
void led_control_user(u08 led_n, u08 led_state);
void rtm_led_control_user(u08 led, u08 onoff);
void init_port_user(void);
void sensor_init_user();
void sensor_monitoring_user();
void rtm_sensor_set_state_user(u08 state);
u08 state_led_user(u08 led_n);
u08 ipmi_get_led_color_capabilities_user(u08 fru, u08 LedId, u08 *buf);
u08 Board_information(u08 buf[]);
u08 Product_information(u08 buf[]);
u08 Multirecord_area_Module(u08 buf[]);
u08 Multirecord_area_Point_to_point(u08 buf[]);
u08 ipmi_picmg_fru_control(u08 control_type);
u08 user_ok_for_m4(void);
u08 ipmi_oem_user(u08 command, u08 *iana, u08 *user_data, u08 data_size, u08 *buf, u08 *error);


//********************************
//User defined function prototypes
//********************************
//NOTE: Users are free to change the API of the functions listed below or to add and delete functions
u08 tempSensorRead(u08 sensor);
void Set_IOexpander_state (u08 IO_I2C_Adress, u08 ByteToSend);
u08 CurrentSensorRead (u08 sensor);
void load_rtm_eeprom();

//Just for debugging purposes
void set_led_pattern(u08 bank, u08 pattern, u08 level);


#endif
