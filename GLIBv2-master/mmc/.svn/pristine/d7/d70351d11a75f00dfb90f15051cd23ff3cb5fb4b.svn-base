//*****************************************************************************
// File Name	: user_code.h
// 
// Author  : Markus Joos (markus.joos@cern.ch)
//
//*****************************************************************************

// This file is the header file for user code. Users of the MMC S/W should only have to modify this file as well as user_code.c
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



//********************************
//Constants for the IPMI DEVICE ID   //See ipmi_get_device_id in ipmi_if.c
//********************************
// firmware release
#define MMC_FW_REL_MAJ      2                   // major release, 7 bit
#define MMC_FW_REL_MIN      2                   // minor release, 8 bit   //MJ Update to "1" once MMC S/W has been fully revised
// manufacturer specific identifier codes
// NOTE: Manufacturer identification is handled by http://www.iana.org/assignments/enterprise-numbers
#define IPMI_MSG_MANU_ID_LSB 0x60    //CERN IANA ID = 0x000060
#define IPMI_MSG_MANU_ID_B2  0x00
#define IPMI_MSG_MANU_ID_MSB 0x00
#define IPMI_MSG_PROD_ID_LSB 0x34
#define IPMI_MSG_PROD_ID_MSB 0x12
//#define IPMI_CPPM_ID        0xbead            // IANA  manufacturer id
//#define IPMI_CERN_ID        96                // IANA  manufacturer id


//******************************
//Constants for the FRU / EEPROM
//******************************
#define SIZE_INFO 128  // size data for BOARD and PRODUCT Information
#define USER_MAX_FRU        0    // 0 for normal AMCs, 1 for MTCA.4 modules


//******************
//Constants for LEDs
//******************
#define NUM_OF_LED            (3 + 1)  //Note: the first 3 LEDs are mandatory
#define USER_LED              3        //Note: IDs 0, 1 and 2 are already used (see led.h)

//User LED
#define LED_USER_PORT         PORTF
#define LED_USER_DDR          DDRF
#define LED_USER_PIN          PF1
#define LED_USER_IN           PINF

//*******************
// Constants for SDRs
//*******************
#define HOT_SWAP_SENSOR         0   //Mandatory
#define TEMPERATURE_SENSOR1     1
#define TEMPERATURE_SENSOR2     2
#define VOLTAGE_OK_SENSOR       3   //MJ: This sensor may be obsolete
#define FPGA_MASTER             4
#define VOLT_12                 5   //Mandatory
#define FPGA_SLAVE              6

// Sensor type codes
#define TEMPERATURE             0x01       //MJ: not used
#define VOLTAGE                 0x02
#define HOT_SWAP                0xF2

#define NUM_SENSOR              7
#define NUM_SDR                 8
#define FIRST_RTM_SENSOR        (FPGA_SLAVE + 1)     //This number has to be the number of the first RTM sensor. For AMCs without RTM: (Last AMC sensor + 1)


//**********************************
// Constants for temperature sensors
//**********************************
#define FPGA_MASTER_LSB_MSB     REMOTE_1_LSB_MSB        // Address temperature on FPGA MASTER 10 bits  //MJ: not used??
#define FPGA_SLAVE_LSB_MSB      REMOTE_2_LSB_MSB        // Address temperature on FPGA SLAVE 10 bits  //MJ: not used??
#define FPGA_MASTER_MSB         REMOTE_1_MSB            // Address temperature on FPGA MASTER 8 bits
#define FPGA_SLAVE_MSB          REMOTE_2_MSB            // Address temperature on FPGA SLAVE 8 bits
#define ADD_SENSOR              0x32                   // ADDRESS SENSOR I2C  //MJ: not used??
#define MODE_LSB_MSB            0x02  //MJ: not used??
#define MODE_MSB                0x01  //MJ: not used??
#define LOCAL_SENSOR_MSB        0x10    // Address temperature local on chip 8 bits
#define REMOTE_1_MSB            0x11    // Address temperature on remote 1 8 bits
#define REMOTE_2_MSB            0x12    // Address temperature on remote 2 8 bits
#define LOCAL_SENSOR_LSB_MSB    0x20    // Address temperature local on chip 10 bits  //MJ: not used??
#define REMOTE_1_LSB_MSB        0x21    // Address temperature on FPGA MASTER 10 bits  //MJ: not used??
#define REMOTE_2_LSB_MSB        0x22    // Address temperature on FPGA SLAVE 10 bits  //MJ: not used??
#define CONFIG_REGISTER         0x03    // read only  //MJ: not used??
#define STATUS_REGISTER         0x02    // read/Write  //MJ: not used??
#define CONTROL_REGISTER        0x06    // read/Write  //MJ: not used??
#define REMOTE_DIODE_MODEL      0x30    // read/Write  //MJ: not used??
#define REMOTE_MODE_CONTROL     0x07    // read/Write  //MJ: not used??


//*****************************
//Mandatory function prototypes
//*****************************
//NOTE: Users must not change the API of the functions listed below
void leds_init_user();
void local_led_control_user(u08 led_n, u08 led_state);
void led_control_user(u08 led_n, u08 led_state);
void init_port_user(void);
void sensor_init_user();
void sensor_monitoring_user();
u08 state_led_user(u08 led_n);
u08 ipmi_get_led_color_capabilities_user(u08 LedId, u08 *buf);
u08 Board_information(u08 buf[]);
u08 Product_information(u08 buf[]);
u08 Multirecord_area_Module(u08 buf[]);
u08 Multirecord_area_Point_to_point(u08 buf[]);
u08 ipmi_picmg_fru_control(u08 control_type);
u08 ipmi_oem_user(u08 command, u08 *iana, u08 *user_data, u08 data_size, u08 *buf, u08 *error);


//********************************
//User defined function prototypes
//********************************
//NOTE: Users are free to change the API of the functions listed below or to add and delete functions
u08 tempSensorRead(u08 sensor);



#endif
