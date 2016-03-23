//*****************************************************************************
// File Name    : user_code.h
//
// User code for the AMT & RTM emulator
//
// Author  : Markus Joos (markus.joos@cern.ch)
//
//*****************************************************************************

// User code for the AMT-RTM

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
#define MMC_FW_REL_MAJ      1                   // major release, 7 bit
#define MMC_FW_REL_MIN      0                   // minor release, 8 bit
// manufacturer specific identifier codes
// NOTE: Manufacturer identification is handled by http://www.iana.org/assignments/enterprise-numbers
#define IPMI_MSG_MANU_ID_LSB 0x60               //CERN IANA ID = 0x000060
#define IPMI_MSG_MANU_ID_B2  0x00
#define IPMI_MSG_MANU_ID_MSB 0x00
#define IPMI_MSG_PROD_ID_LSB 0x35
#define IPMI_MSG_PROD_ID_MSB 0x12


//Constants for checkcompat (rtm_mng.c)
#define RTM_MULTI_RECORD_OFFSET 0x30
#define RTM_MFG_B1              0x60 // Manufacturer IANA ID (LSB)
#define RTM_MFG_B2              0x00 // Manufacturer IANA ID
#define RTM_MFG_B3              0x00 // Manufacturer IANA ID (MSB)
#define RTM_DEV_B1              0x00 // Device designator (LSB)
#define RTM_DEV_B2              0x00 // Device designator
#define RTM_DEV_B3              0x02 // Device designator
#define RTM_DEV_B4              0x08 // Device designator (MSB)

//******************************
//Constants for the FRU / EEPROM
//******************************
#define SIZE_INFO         128  // size data for BOARD and PRODUCT Information
#define RTM_FRU_SIZE      66
#define USER_MAX_FRU      1    // 0 for normal AMCs, 1 for MTCA.4 modules


//**********************
//Constants for AMC LEDs
//**********************
#define NUM_OF_AMC_LED  12       //Note: the first 3 LEDs are mandatory. LED3 (the 4th LED) is reserved
#define LED_A_1         4        //Note: IDs 0, 1 and 2 are already used (see led.h) and 3 is reserved
#define LED_A_2         5
#define LED_A_3         6
#define LED_A_4         7
#define LED_A_5         8
#define LED_A_6         9
#define LED_A_7         10
#define LED_A_8         11

//**********************
//Constants for RTM LEDs
//**********************
#define NUM_OF_RTM_LED  12       //Note: the first 3 LEDs are mandatory. LED3 (the 4th LED) is reserved
#define LED_B_1         4
#define LED_B_2         5
#define LED_B_3         6
#define LED_B_4         7
#define LED_B_5         8
#define LED_B_6         9
#define LED_B_7         10
#define LED_B_8         11

//The definitions below are for the IO expander at address "RTM_IO_PORTS_ADDR"
#define BLUE_LED_BIT      2
#define RED_LED_BIT       4
#define GREEN_LED_BIT     3
#define RTM_HOT_SWAP_PIN  1



#define IO_EXPANDER_IC12_ADD 0x3A
#define IO_EXPANDER_IC13_ADD 0x23


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
#define RTM_IO_PORTS_ADDR   0x3B
#define RTM_LED_ON          0
#define RTM_LED_OFF         1


//*******************
// Constants for SDRs
//*******************

//Sensor numbers
//NOTE: First declare all AMC then all RTM sensors
//AMC sensors
#define HOT_SWAP_SENSOR     0
#define TEMPERATURE_SENSOR1 1
#define TEMPERATURE_SENSOR2 2
#define VOLT_12             3
//RTM sensors
#define RTM_HOT_SWAP_SENSOR 4
#define TEMPERATURE_RTM1    5

#define NUM_SENSOR_AMC      4  //The device locator of the AMC does not count
#define NUM_SDR_AMC         5
#define NUM_SENSOR_RTM      2  //The device locator of the RTM does not count
#define NUM_SDR_RTM         3


#define FIRST_RTM_SENSOR    RTM_HOT_SWAP_SENSOR     //This number has to be the number of the first RTM sensor. For AMCs without RTM: (Last AMC sensor + 1)

// Sensor type codes
//#define TEMPERATURE             0x01       //MJ: not used
#define VOLTAGE                 0x02
#define HOT_SWAP                0xF2

//**********************************
// Constants for temperature sensors
//**********************************
//#define FPGA_MASTER_LSB_MSB     REMOTE_1_LSB_MSB        // Address temperature on FPGA MASTER 10 bits  //MJ: not used??
//#define FPGA_SLAVE_LSB_MSB      REMOTE_2_LSB_MSB        // Address temperature on FPGA SLAVE 10 bits  //MJ: not used??
//#define FPGA_MASTER_MSB         REMOTE_1_MSB            // Address temperature on FPGA MASTER 8 bits
//#define FPGA_SLAVE_MSB          REMOTE_2_MSB            // Address temperature on FPGA SLAVE 8 bits
//#define ADD_SENSOR              0x32                    // ADDRESS SENSOR I2C  //MJ-VB: Check address and purpose
//#define MODE_LSB_MSB            0x02    //MJ: not used??
//#define MODE_MSB                0x01    //MJ: not used??
#define LOCAL_SENSOR_MSB        0x10    // Address temperature local on chip 8 bits
#define REMOTE_1_MSB            0x11    // Address temperature on remote 1 8 bits
#define REMOTE_2_MSB            0x12    // Address temperature on remote 2 8 bits
//#define LOCAL_SENSOR_LSB_MSB    0x20    // Address temperature local on chip 10 bits  //MJ: not used??
//#define REMOTE_1_LSB_MSB        0x21    // Address temperature on FPGA MASTER 10 bits  //MJ: not used??
//#define REMOTE_2_LSB_MSB        0x22    // Address temperature on FPGA SLAVE 10 bits  //MJ: not used??
#define CONFIG_REGISTER         0x03    // read only
//#define STATUS_REGISTER         0x02    // read/Write  //MJ: not used??
#define CONTROL_REGISTER        0x06    // read/Write
#define REMOTE_DIODE_MODEL      0x30    // read/Write
#define REMOTE_MODE_CONTROL     0x07    // read/Write



// Pins for a DCDC. //MJ: Not used in the AMT. Declarations were in project.h but got moved here because they are user code. May be required for the GLIB
//#define DCDC_ENABLE_PORT PORTC      // Was set to PORTA. Changed by VB for new V3 HW compatibility.
//#define DCDC_ENABLE_DDR  DDRC       // Was set to DDRA. Changed by VB for new V3 HW compatibility.
//#define DCDC_ENABLE_PIN  PC7        // Was set to PA1. Changed by VB for new V3 HW compatibility.
//#define REG_ENABLE_PORT PORTC
//#define REG_ENABLE_DDR  DDRC
//#define REG_ENABLE_PIN  PC6



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
void load_rtm_eeprom();

//Just for debugging purposes
void set_led_pattern(u08 bank, u08 pattern, u08 level);


#endif
