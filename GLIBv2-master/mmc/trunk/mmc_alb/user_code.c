//*****************************************************************************
// File Name    : user_code.c
//
// Author  : Markus Joos (markus.joos@cern.ch)
//
//*****************************************************************************

// User code for the ALB without RTM

// This file is the source file for user code. Users of the MMC S/W should only have to modify this file as well as user_code.h


// Header file
#include "user_code.h"


// Globals
//For the load groups
u08 current_IO_state = 0xff;

// For the LEDs
extern leds amc_led[NUM_OF_AMC_LED];

// For the sensors
extern u08 run_type;


u08 SDR0[] =
{
    0x01,           // record number, LSB -> SDR_Init
    0x00,           // record number,MSB -> SDR_Init
    0x51,           // IPMI protocol version
    0x12,           // record type: device locator
    0x16,           // record length: remaining bytes -> SDR_Init
    // record key bytes
    0x00,           // device slave address
    0x00,           // channel number
    // record body bytes
    0x00,           // power state notification, global initialisation
    0x29,           // device capabilities
    0x00,           // reserved
    0x00,           // reserved
    0x00,           // reserved
    0xc1,           // entityID
    0x00,           // entity instance
    0x00,           // OEM
    0xcd,           // device ID string type/length
    'A','M','C',' ','L','O','A','D',' ','C','A','R','D'
};


//Temperature 1 - Full Sensor
u08 SDR1[] =
{
    //sensor record header
    //Byte 1
    0x02,                //record number, LSB
    0x00,                //record number,MSB
    0x51,                //IPMI protocol version
    0x01,                //record type: full sensor
    0x33,                //record length: remaining bytes -> SDR_Init

    //record key bytes
    0x00,                //i2c address, -> SDR_Init
    0x00,                //sensor owner LUN
    //Byte 8
    TEMPERATURE_SENSOR1, //sensor number

    //record body bytes
    0xc1,                //entity id: AMC Module
    0x00,                //entity instance -> SDR_Init
    0x03,                //init: event generation + scanning enabled
    0x48,                //capabilities: auto re-arm, Threshold is readable and setable
    0x01,                //sensor type: Temperature
    0x01,                //sensor reading: Threshold
    0x80,                //LSB assert event mask: 3 bit value
    //Byte 16
    0x0a,                //MSB assert event mask
    0xa8,                //LSB deassert event mask: 3 bit value
    0x7a,                //MSB deassert event mask
    0x38,                //LSB: readable Threshold mask: all thresholds are readable
    0x38,                //MSB: setable Threshold mask: all thresholds are setable
    0x80,                //sensor units 1 : 2's complement
    0x01,                //sensor units 2 : Temperature
    0x00,                //sensor units 3 : Modifier
    // Byte 24
    0x00,                //Linearisation
    0x01,                //M
    0x01,                //M - Tolerance
    0x00,                //B
    0x25,                //B - Accuracy
    0x88,                //Sensor direction
    0x00,                //R-Exp , B-Exp
    0x07,                //Analogue characteristics flags
    //Byte 32
    25,                  //Nominal reading
    50,                  //Normal maximum
    20,                  //Normal minimum
    0xFF,                //Sensor Maximum reading
    0x00,                //Sensor Minimum reading
    85,                  //Upper non-recoverable Threshold
    75,                  //Upper critical Threshold
    65,                  //Upper non critical Threshold
    //Byte 40
    -20,                 //Lower non-recoverable Threshold
    -10,                 //Lower critical Threshold
    0,                   //Lower non-critical Threshold
    0x02,                //positive going Threshold hysteresis value
    0x02,                //negative going Threshold hysteresis value
    0x00,                //reserved
    0x00,                //reserved
    0x00,                //OEM reserved
    //Byte 48
    0xC8,                //8 bit ASCII, number of bytes
    'L','M','8','2',' ','I','C','1'   //sensor string
};

//Temperature 2 - Full Sensor
u08 SDR2[] =
{
    //sensor record header
    //Byte 1
    0x03,                //record number, LSB  - filled by SDR_Init()
    0x00,                //record number, MSB  - filled by SDR_Init()
    0x51,                //IPMI protocol version
    0x01,                //record type: full sensor
    0x33,                //record length: remaining bytes -> SDR_Init

    //record key bytes
    0x00,                //i2c address, -> SDR_Init
    0x00,                //sensor owner LUN
    //Byte 8
    TEMPERATURE_SENSOR2, //sensor number

    //record body bytes
    0xc1,                //entity id: AMC Module
    0x00,                //entity instance -> SDR_Init
    0x03,                //init: event generation + scanning enabled
    0x48,                //capabilities: auto re-arm, Threshold is readable and setable
    0x01,                //sensor type: Temperature
    0x01,                //sensor reading: Threshold
    0x80,                //LSB assert event mask: 3 bit value
    //Byte 16
    0x0a,                //MSB assert event mask
    0xa8,                //LSB deassert event mask: 3 bit value
    0x7a,                //MSB deassert event mask
    0x38,                //LSB: readable Threshold mask: all thresholds are readable
    0x38,                //MSB: setable Threshold mask: all thresholds are setable
    0x80,                //***** Modified by P.Vichoudis - it was: 0x00, //sensor units 1 : 2's complement
    0x01,                //sensor units 2 : Temperature
    0x00,                //sensor units 3 : Modifier
    //Byte 24
    0x00,                //Linearisation
    0x01,                //M
    0x01,                //M - Tolerance
    0x00,                //B
    0x25,                //B - Accuracy
    0x88,                //Sensor direction
    0x00,                //R-Exp , B-Exp
    0x07,                //Analogue characteristics flags
    //Byte 32
    25,                  //Nominal reading
    50,                  //Normal maximum
    20,                  //Normal minimum
    0xff,                //Sensor Maximum reading
    0x00,                //Sensor Minimum reading
    85,                 //Upper non-recoverable Threshold
    75,                  //Upper critical Threshold
    65,                  //Upper non critical Threshold
    //Byte 40
    -20,                 //Lower non-recoverable Threshold
    -10,                 //Lower critical Threshold
    0,                   //Lower non-critical Threshold
    0x02,                //positive going Threshold hysteresis value
    0x02,                //negative going Threshold hysteresis value
    0x00,                //reserved
    0x00,                //reserved
    0x00,                //OEM reserved
    //Byte 48
    0xc8,                //8 bit ASCII, number of bytes
    'L','M','8','2',' ','I','C','2' //sensor string
};

u08 SDR3[] =
{
    //sensor record header
    //Byte 1
    0x03,            //record number, LSB  - filled by SDR_Init()
    0x00,            //record number, MSB  - filled by SDR_Init()
    0x51,            //IPMI protocol version
    0x01,            //record type: full sensor
    0x37,            //record length: remaining bytes -> SDR_Init

    //record key bytes
    0x00,            //i2c address, -> SDR_Init
    0x00,            //sensor owner LUN
    //Byte 8
    HOT_SWAP_SENSOR, //sensor number

    //record body bytes
    0xc1,            //entity id: AMC Module
    0x00,            //entity instance -> SDR_Init
    0x03,            //init: event generation + scanning enabled
    0xc1,            //capabilities: auto re-arm,
    HOT_SWAP,        //sensor type: HOT SWAP
    0x6f,            //sensor reading
    0x07,            //LSB assert event mask: 3 bit value
    //Byte 16
    0x00,            //MSB assert event mask
    0x07,            //LSB deassert event mask: 3 bit value
    0x00,            //MSB deassert event mask
    0x00,            //LSB: readable Threshold mask: all thresholds are readable
    0x00,            //MSB: setable Threshold mask: all thresholds are setable
    0xc0,            //sensor units 1
    0x00,            //sensor units 2
    0x00,            //sensor units 3
    //Byte 24
    0x00,            //Linearization
    0x00,            //M
    0x00,            //M - Tolerance
    0x00,            //B
    0x00,            //B - Accuracy
    0x00,            //Sensor direction
    0x00,            //R-Exp , B-Exp
    0x00,            //Analogue characteristics flags
    //Byte 32
    0x00,            //Nominal reading
    0x00,            //Normal maximum
    0x00,            //Normal minimum
    0x00,            //Sensor Maximum reading
    0x00,            //Sensor Minimum reading
    0x00,            //Upper non-recoverable Threshold
    0x00,            //Upper critical Threshold
    0x00,            //Upper non critical Threshold
    //Byte 40
    0x00,            //Lower non-recoverable Threshold
    0x00,            //Lower critical Threshold
    0x00,            //Lower non-critical Threshold
    0x00,            //positive going Threshold hysteresis value
    0x00,            //negative going Threshold hysteresis value
    0x00,            //reserved
    0x00,            //reserved
    0x00,            //OEM reserved
    //Byte 48
    0xcc,            //8 bit ASCII, number of bytes
    'F', 'R', 'U', ' ', 'H', 'O', 'T', '_', 'S', 'W', 'A', 'P' //sensor string
};

u08 SDR4[] =
{
    //sensor record header
    0x04,              //record number, LSB  - filled by SDR_Init()
    0x00,              //record number, MSB  - filled by SDR_Init()
    0x51,              //IPMI protocol version
    0x02,              //record type: compact sensor
    0x23,              //record length: remaining bytes, SDR_Init
    //record key bytes
    0x00,              //i2c address, filled by SDR_Init
    0x00,              //sensor owner LUN
    VOLTAGE_OK_SENSOR, //sensor number
    //record body bytes
    //Byte 8
    0xc1,              //entity id: MCH
    0x00,              //entity instance, SDR_Init
    0x03,              //init: events + scanning enabled
    0xc2,              //capabilities: auto re-arm, global disable
    0x08,              //type: Power Supply(DC-to-DC converter)
    0x6f,              //sensor specific event/read
    0x01,              //LSB assert event mask: 1 bit value
    0x00,              //MSB assert event mask
    0x01,              //LSB deassert event mask: 1 bit value
    0x00,              //MSB deassert event mask
    0x01,              //LSB read event mask: 1 bit value
    0x00,              //MSB read event mask
    0xc0,              //sensor units 1
    0x00,              //sensor units 2
    0x00,              //sensor units 3
    0x01,              //sharing: 1 sensor
    0x00,              //no entity instance string
    0x00,              //no positive threshold hysteresis
    0x00,              //no negative threshold hysteresis
    0x00,              //reserved
    0x00,              //reserved
    0x00,              //reserved
    0x00,              //OEM reserved
    0xc8,              //8 bit ASCII, number of bytes
    'P', 'O', 'W', 'E', 'R', '_', 'O', 'K' //sensor string
};


//12V Payload power sensor
u08 SDR5[] =
{
    //sensor record header
    //Byte 1
    0x06,    //record number, LSB  - filled by SDR_Init()
    0x00,    //record number, MSB  - filled by SDR_Init()
    0x51,    //IPMI protocol version
    0x01,    //record type: full sensor
    0x2e,    //record length: remaining bytes -> SDR_Init
    //record key bytes
    0x00,    //i2c address, -> SDR_Init
    0x00,    //sensor owner LUN
    //Byte 8
    VOLT_12, //sensor number
    //record body bytes
    0xc1,    //entity id: AMC Module
    0x00,    //entity instance -> SDR_Init
    0x03,    //init: event generation + scanning enabled
    0x48,    //capabilities: auto re-arm, Threshold is readable and setable
    VOLTAGE, //sensor type: Voltage
    0x01,    //sensor reading: Threshold
    0x80,    //LSB assert event mask: 3 bit value
    //Byte 16
    0x0a,    //MSB assert event mask
    0xa8,    //LSB deassert event mask: 3 bit value
    0x7a,    //MSB deassert event mask
    0x38,    //LSB: readable Threshold mask: all thresholds are readable
    0x38,    //MSB: setable Threshold mask: all thresholds are setable
    0x00,    //sensor units 1 : 2's complement
    0x04,    //sensor units 2 : Temperature
    0x00,    //sensor units 3 : Modifier
    //Byte 24
    0x00,    //Linearization
    0x4e,    //M
    0x02,    //M - Tolerance
    0x00,    //B
    0x04,    //B - Accuracy
    0x0c,    //Sensor direction
    0xd0,    //R-Exp , B-Exp
    0x07,    //Analogue characteristics flags
    //Byte 32
    0x7f,    //Nominal reading
    0xef,    //Normal maximum
    0xab,    //Normal minimum
    0xff,    //Sensor Maximum reading
    0x00,    //Sensor Minimum reading
    0xf7,    //Upper non-recoverable Threshold
    0xf2,    //Upper critical Threshold
    0xe9,    //Upper non critical Threshold
    //Byte 40
    0xa2,    //Lower non-recoverable Threshold
    0xa7,    //Lower critical Threshold
    0xab,    //Lower non-critical Threshold
    0x02,    //positive going Threshold hysteresis value
    0x02,    //negative going Threshold hysteresis value
    0x00,    //reserved
    0x00,    //reserved
    0x00,    //OEM reserved
    //Byte 48
    0xc3,    //8 bit ASCII, number of bytes
    '1', '2', 'V' //sensor string
};


// Temperature 3 - Full Sensor
u08 SDR6[] = {
    // sensor record header
    0x03,           // record number, LSB  - filled by SDR_Init()
    0x00,           // record number, MSB  - filled by SDR_Init()
    0x51,           // IPMI protocol version
    0x01,           // record type: full sensor
    0x33,           // record length: remaining bytes -> SDR_Init
    // record key bytes
    0x00,           // i2c address, -> SDR_Init
    0x00,           // sensor owner LUN
    TEMPERATURE_SENSOR3,    // sensor number
    // record body bytes
    0xc1,           // entity id: AMC Module
    0x00,           // entity instance -> SDR_Init
    0x03,           // init: event generation + scanning enabled
    0x48,           // capabilities: auto re-arm, Threshold is readable and setable,
    0x01,           // sensor type: Temperature
    0x01,           // sensor reading: Threshold
    0x80,           // LSB assert event mask: 3 bit value
    0x0a,           // MSB assert event mask
    0xa8,           // LSB deassert event mask: 3 bit value
    0x7a,           // MSB deassert event mask
    0x38,           // LSB: readabled Threshold mask: all thresholds are readabled:
    0x38,           // MSB: setabled Threshold mask: all thresholds are setabled:
    0x80,           // sensor units 1 : 2's complement
    0x01,           // sensor units 2 : Temperature
    0x00,           // sensor units 3 : Modifier
    0x00,           // Linearization
    0x01,           // M
    0x01,           // M - Tolerance
    0x00,           // B
    0x25,           // B - Accuracy
    0x88,           // Sensor direction
    0x00,           // R-Exp , B-Exp
    0x07,           // Analogue characteristics flags
    25,             // Nominal reading
    50,             // Normal maximum
    20,             // Normal minimum
    127,            // Sensor Maximum reading
    -128,           // Sensor Minimum reading
    85,             // Upper non-recoverable Threshold
    75,             // Upper critical Threshold
    65,             // Upper non critical Threshold
    -20,            // Lower non-recoverable Threshold
    -10,            // Lower critical Threshold
    0,              // Lower non-critical Threshold
    0x02,           // positive going Threshold hysteresis value
    0x02,           // negative going Threshold hysteresis value
    0x00,           // reserved
    0x00,           // reserved
    0x00,           // OEM reserved
    0xc8,           // 8 bit ASCII, number of bytes
    'L','M','8','2',' ','I','C','3' // sensor string
};


// Temperature 4 - Full Sensor
u08 SDR7[] = {
    // sensor record header
    0x03,           // record number, LSB  - filled by SDR_Init()
    0x00,           // record number, MSB  - filled by SDR_Init()
    0x51,           // IPMI protocol version
    0x01,           // record type: full sensor
    0x33,           // record length: remaining bytes -> SDR_Init
    // record key bytes
    0x00,           // i2c address, -> SDR_Init
    0x00,           // sensor owner LUN
    TEMPERATURE_SENSOR4,    // sensor number
    // record body bytes
    0xc1,           // entity id: AMC Module
    0x00,           // entity instance -> SDR_Init
    0x03,           // init: event generation + scanning enabled
    0x48,           // capabilities: auto re-arm, Threshold is readable and setable,
    0x01,           // sensor type: Temperature
    0x01,           // sensor reading: Threshold
    0x80,           // LSB assert event mask: 3 bit value
    0x0a,           // MSB assert event mask
    0xa8,           // LSB deassert event mask: 3 bit value
    0x7a,           // MSB deassert event mask
    0x38,           // LSB: readabled Threshold mask: all thresholds are readabled:
    0x38,           // MSB: setabled Threshold mask: all thresholds are setabled:
    0x80,           // sensor units 1 : 2's complement
    0x01,           // sensor units 2 : Temperature
    0x00,           // sensor units 3 : Modifier
    0x00,           // Linearization
    0x01,           // M
    0x01,           // M - Tolerance
    0x00,           // B
    0x25,           // B - Accuracy
    0x88,           // Sensor direction
    0x00,           // R-Exp , B-Exp
    0x07,           // Analogue characteristics flags
    25,             // Nominal reading
    50,             // Normal maximum
    20,             // Normal minimum
    127,            // Sensor Maximum reading
    -128,           // Sensor Minimum reading
    85,             // Upper non-recoverable Threshold
    75,             // Upper critical Threshold
    65,             // Upper non critical Threshold
    -20,            // Lower non-recoverable Threshold
    -10,            // Lower critical Threshold
    0,              // Lower non-critical Threshold
    0x02,           // positive going Threshold hysteresis value
    0x02,           // negative going Threshold hysteresis value
    0x00,           // reserved
    0x00,           // reserved
    0x00,           // OEM reserved
    0xc8,           // 8 bit ASCII, number of bytes *
    'L','M','8','2',' ','I','C','4' // sensor string
};


// Payload power current 4 - Full Sensor
u08 SDR8[] = {
    // sensor record header
    0x03,           // record number, LSB  - filled by SDR_Init()
    0x00,           // record number, MSB  - filled by SDR_Init()
    0x51,           // IPMI protocol version
    0x01,           // record type: full sensor
    0x33,           // record length: remaining bytes -> SDR_Init
    // record key bytes
    0x00,           // i2c address, -> SDR_Init
    0x00,           // sensor owner LUN
    CURRENT_SENSOR, // sensor number
    // record body bytes
    0xc1,           // entity id: AMC Module
    0x00,           // entity instance -> SDR_Init
    0x03,           // init: event generation + scanning enabled
    0x48,           // capabilities: auto re-arm, Threshold is readable and setable,
    0x03,           // sensor type: Current
    0x01,           // sensor reading: Threshold
    0x80,           // LSB assert event mask: 3 bit value
    0x0a,           // MSB assert event mask *
    0xa8,           // LSB deassert event mask: 3 bit value
    0x7a,           // MSB deassert event mask
    0x38,           // LSB: readabled Threshold mask: all thresholds are readabled:
    0x38,           // MSB: setabled Threshold mask: all thresholds are setabled:
    0x00,           // sensor units 1 : unsigned
    0x05,           // sensor units 2 : Amps
    0x00,           // sensor units 3 : Modifier
    0x00,           // Linearization
    0x20,           // M
    0x10,           // M - Tolerance
    0x00,           // B
    0x04,           // B - Accuracy
    0x00,           // Sensor direction
    0xd0,           // R-Exp , B-Exp
    0x07,           // Analogue characteristics flags
    0x01,           // Nominal reading
    0xff,           // Normal maximum
    0,              // Normal minimum
    0xff,           // Sensor Maximum reading
    0,              // Sensor Minimum reading
    0xff,           // Upper non-recoverable Threshold
    0xfa,           // Upper critical Threshold
    0xda,           // Upper non critical Threshold
    0,              // Lower non-recoverable Threshold
    0,              // Lower critical Threshold
    0,              // Lower non-critical Threshold
    0x02,           // positive going Threshold hysteresis value
    0x02,           // negative going Threshold hysteresis value
    0x00,           // reserved
    0x00,           // reserved
    0x00,           // OEM reserved
    0xc8,           // 8 bit ASCII, number of bytes
    'C','U','R','R','E','N','T',' ' // sensor string
};

u08 *sdrPtr[NUM_SDR_AMC + NUM_SDR_RTM] = {SDR0, SDR1, SDR2, SDR3, SDR4, SDR5, SDR6, SDR7, SDR8};
u08 sdrLen[NUM_SDR_AMC + NUM_SDR_RTM] =  {sizeof(SDR0), sizeof(SDR1), sizeof(SDR2), sizeof(SDR3), sizeof(SDR4), sizeof(SDR5), sizeof(SDR6), sizeof(SDR7), sizeof(SDR8)};

sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];


//******************/
void leds_init_user()    //Called from leds_init in led.c
//******************/
{
    amc_led[LOAD_GROUP_0].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_0].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_0].on_duration    = 0;
    amc_led[LOAD_GROUP_0].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_1].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_1].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_1].on_duration    = 0;
    amc_led[LOAD_GROUP_1].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_2].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_2].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_2].on_duration    = 0;
    amc_led[LOAD_GROUP_2].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_3].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_3].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_3].on_duration    = 0;
    amc_led[LOAD_GROUP_3].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_4].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_4].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_4].on_duration    = 0;
    amc_led[LOAD_GROUP_4].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_5].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_5].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_5].on_duration    = 0;
    amc_led[LOAD_GROUP_5].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_6].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_6].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_6].on_duration    = 0;
    amc_led[LOAD_GROUP_6].control_state  = OVERRIDE_STATE;

    amc_led[LOAD_GROUP_7].local_cntr_fnc = LED_OFF;
    amc_led[LOAD_GROUP_7].fnc_off        = LED_OFF;
    amc_led[LOAD_GROUP_7].on_duration    = 0;
    amc_led[LOAD_GROUP_7].control_state  = OVERRIDE_STATE;

    current_IO_state = 0xff;
}


//**************************/
u08 state_led_user(u08 led_n)   //Called from state_led in led.c
//**************************/
{
    if (amc_led[led_n].control_state == OVERRIDE_STATE)
        return (0); //MJ: 0 = LED_OFF. Is this what we want in mmc_main.c? What is OVERRIDE_STATE used for?
    return (0xff);  //MJ: mmc_main.c does not process this error. How could it react?
}


//**************************************************/
void local_led_control_user(u08 led_n, u08 led_state)  //Called from local_led_control in led.c
//**************************************8***********/
{
    if (amc_led[led_n].control_state == OVERRIDE_STATE)
        return;
}


//********************************************/
void led_control_user(u08 led_n, u08 led_state)  //Called from led_control in led.c
//********************************************/
{
    if (amc_led[led_n].control_state == LOCAL_CONTROL_STATE)
        return;

    switch (led_n)
    {
    case LOAD_GROUP_0:
        if(led_state == LED_ON) current_IO_state = 0x7f & current_IO_state;
        else                    current_IO_state = 128 | current_IO_state;
        break;
    case LOAD_GROUP_1:
        if(led_state == LED_ON) current_IO_state = 0xbf & current_IO_state;
        else                    current_IO_state = 64 | current_IO_state;
        break;
    case LOAD_GROUP_2:
        if(led_state == LED_ON) current_IO_state = 0xdf & current_IO_state;
        else                    current_IO_state = 32 | current_IO_state;
        break;
    case LOAD_GROUP_3:
        if(led_state == LED_ON) current_IO_state = 0xef & current_IO_state;
        else                    current_IO_state = 16 | current_IO_state;
        break;
    case LOAD_GROUP_4:
        if(led_state == LED_ON) current_IO_state = 0xf7 & current_IO_state;
        else                    current_IO_state = 8 | current_IO_state;
        break;
    case LOAD_GROUP_5:
        if(led_state == LED_ON) current_IO_state = 0xfb & current_IO_state;
        else                    current_IO_state = 4 | current_IO_state;
        break;
    case LOAD_GROUP_6:
        if(led_state == LED_ON) current_IO_state = 0xfd & current_IO_state;
        else                    current_IO_state = 2 | current_IO_state;
        break;
    case LOAD_GROUP_7:
        if(led_state == LED_ON) current_IO_state = 0xfe & current_IO_state;
        else                    current_IO_state = 1 | current_IO_state;
        break;
    default:
        break;   //MJ Should not be here. How to signal error?
    }
    Set_IOexpander_state(IO_EXPANDER_ADD, current_IO_state);
}


//**********************************************************/
void Set_IOexpander_state (u08 IO_I2C_Adress, u08 ByteToSend)
//**********************************************************/
{
    CRITICAL_SECTION_START;
    i2cswSend(IO_I2C_Adress, 0, 0, 1, &ByteToSend);
    CRITICAL_SECTION_END;
}


//*******************************************************************/
u08 ipmi_get_led_color_capabilities_user(u08 fru, u08 LedId, u08 *buf)   //Called from ipmi_get_led_color_capabilities in led.c
//*******************************************************************/
{
    u08 len = 0;

    //We treat fru=0 and fru=1 in the same way: All LEDs are RED.

    if (fru == 0 && LedId >= NUM_OF_AMC_LED)  // value out of range
        return(0xff);
    if (fru == 1 && LedId >= NUM_OF_RTM_LED)  // value out of range
        return(0xff);

    buf[len++] = 1 << RED;    // capability is RED only
    buf[len++] = RED;         // default colour is RED
    buf[len++] = RED;         // default colour in override state is RED

    return(len);
}


//**********************/
void init_port_user(void)    //Called from init_port() in mmc_main.c
//**********************/
{
    //Warning: For the shared ports the user has to look at init_port() in mmc_main and copy the definitions of the "G" bits to this block of code

    //Port C: UUUUUUUU
    DDRC  = 0x00;
    PORTC = 0x00;

    //Port E: UUUUGGNN
    DDRE  = 0x00;
    PORTE = 0x00;

    //Port F: GGGGUUUG
    DDRF  = 0x00;
    PORTF = 0x00;

    //Port G: NNNNUUUU
    DDRG  = 0x00;
    PORTG = 0x00;
}


//**************************************/
void rtm_sensor_set_state_user(u08 state)
//**************************************/
{
}


//********************/
void sensor_init_user()   //Called from sensor_init() in sdr.c
//********************/
{
    //Nothing to be done for the ALB
}


/***************************/
void sensor_monitoring_user()   //Called from mmc_main.c
/***************************/
{
    sens[TEMPERATURE_SENSOR1].value = tempSensorRead(TEMPERATURE_SENSOR1);   //MJMJ: code missing?
    if (run_type == INCRATE)
        check_temp_event(TEMPERATURE_SENSOR1);

    sens[TEMPERATURE_SENSOR2].value = tempSensorRead(TEMPERATURE_SENSOR2);
    if (run_type == INCRATE)
        check_temp_event(TEMPERATURE_SENSOR2);

    sens[TEMPERATURE_SENSOR3].value  = tempSensorRead(TEMPERATURE_SENSOR3);
    if(run_type == INCRATE)
       check_temp_event(TEMPERATURE_SENSOR3);

    sens[TEMPERATURE_SENSOR4].value  = tempSensorRead(TEMPERATURE_SENSOR4);
    if(run_type == INCRATE)
       check_temp_event(TEMPERATURE_SENSOR4);

    sens[CURRENT_SENSOR].value = CurrentSensorRead(CURRENT_SENSOR);
    sens[VOLT_12].value = a2dConvert8bit(0);
}


//***************************/
u08 tempSensorRead(u08 sensor)    //Called from sensor_monitoring_user in user_code.c
//***************************/
{
    u08 stemp;     //sensor temperature
    u08 saddr;     //temperature sensor address

    if (sensor == TEMPERATURE_SENSOR1)
        saddr = 0x2A; // this sensor is supplied by the MP.
    if (sensor == TEMPERATURE_SENSOR2)
        saddr = 0x1A; // this sensor is supplied via the PP (only accessible when in M4).
    if (sensor ==  TEMPERATURE_SENSOR3)
        saddr = 0x2B;
    if (sensor ==  TEMPERATURE_SENSOR4)
        saddr = 0x4E;

    CRITICAL_SECTION_START;
    i2cswReceive(saddr, 0, 1, 1, &stemp);
    CRITICAL_SECTION_END;

    return stemp;
}


//*******************************/
u08 CurrentSensorRead (u08 sensor)   //Called from user_code.c
//*******************************/
{
    static u16 sResult = 0;        //MJ: why static?
    static float Result;
    u08 saddr, scmd, bytes[2];

    scmd  = 0xA0;
    if(sensor == CURRENT_SENSOR)
        saddr = 0x22;   // VB: This is for the use of the AD7997 BRUZ-0
    else
    {
        return(0);
        //MJ: What else??? How could we flag an error?
    }

    CRITICAL_SECTION_START;
    i2cswReceive(saddr, scmd, 1, 2, &bytes[0]);
    CRITICAL_SECTION_END;

    sResult = (bytes[0] << 8) + bytes[1];
    sResult = 0x3ff & (sResult >> 2);       // VB: This is when the converter is 10 bits (7997).
    Result = (float)(sResult) * 3 / 1024;   // VB: This is when the converter is 10 bits (7997).
    if (Result < 2.5)
        Result = 2.5;
    Result = 500 * (Result - 2.5);
    Result = (u08)(Result);                 //MJ: Let's hope this cast is correct
    return(Result);
}


//*****************************/
u08 Board_information(u08 buf[])  //Called from Write_FRU_Info_default in this file
//*****************************/
{
    u08 lenght = 0, i;

    // Default values
    u08 Board[] =
    {
        0x01,                                                   // Format version
        0x00,                                                   // Board area length
        0x19,                                                   // Language code default ( English )
        0x50, 0x4A, 0x74,                                       // Manufacturer date/time
        0xC4,                                                   // Board manufacturer type/length
        'C', 'E', 'R', 'N',                                     // Board Manufacturer
        0xCD,                                                   // Board product name type/lenght
        'A','M','C','-','L','o','a','d','B','o','a','r','d',    // Board product name
        0xC9,                                                   // Board serial number type/length
        '0', '0', '0', '0', '0', '0', '0', '0', '1',            // Board serial number
        0xC5,                                                   // Board part number type/length
        'A', 'M', 'C', '-', 'L',                                // Board part number
        0xC0,                                                   // FRU file ID type/length
        0xC1,                                                   // No more fields
        0x00,                                                   // padding
        0x00,                                                   // Board area checksum
    };

    for (i = 0; i < SIZE_INFO; i++)
        buf[i] = 0;

    lenght = sizeof(Board);
    Board[1] = SIZE_INFO / 8;
    Board[lenght - 2] = 0xC0 | (SIZE_INFO - lenght);            // size of padding for data = size_info bytes

    for (i = 0; i < lenght; ++i)
        buf[i] = Board[i];

    buf[SIZE_INFO - 1] = checksum_clc(buf, SIZE_INFO - 1);
    return SIZE_INFO; // return size_info
}


//*******************************/
u08 Product_information(u08 buf[])   //Called from Write_FRU_Info_default in this file
//*******************************/
{
    u08 lenght = 0, i;

    // Default values
    u08 Product[] =
    {
        0x01,                                                        // Format version
        0x00,                                                        // Product area length
        0x19,                                                        // Language code default ( English )
        0xC4,                                                        // Product manufacturer type/length
        'C', 'E', 'R', 'N',                                          // Product Manufacturer
        0xC8,                                                        // Product product name type/length
        'A','M','C','-','L','o','a','d',                             // Product product name
        0xC5,                                                        // Board part/model number type/length
        'A', 'M', 'C', '-', 'L',                                     // Board part/model
        0xC2,                                                        // Product version type/length
        '0', '2',                                                    // Product version
        0xCC,                                                        // Product serial number type/length
        '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',  // Product serial number
        0x00,                                                        // Asset tag type/length
        0xC0,                                                        // FRU file ID type/length
        0xC1,                                                        // End of fields
        0x00,                                                        // Padding
        0x00                                                         // Product area checksum
    };

    for (i = 0; i < SIZE_INFO; i++)
        buf[i] = 0;

    lenght = sizeof(Product);
    Product[1] = SIZE_INFO / 8;
    Product[lenght - 2] = 0xC0 | (SIZE_INFO - lenght);

    for (i = 0; i < lenght; ++i)
        buf[i] = Product[i];

    buf[SIZE_INFO - 1] = checksum_clc(buf, SIZE_INFO - 1);
    return SIZE_INFO;
}


//***********************************/
u08 Multirecord_area_Module(u08 buf[])  //Called from Write_FRU_Info_default in this file
//***********************************/
{
    u08 i;

    u08 Module[] =
    {
        0xC0,             // Record type ID
        0x02,             // End of list/version
        0x06,             // record length
        0x00,             // Record checksum
        0x00,             // Header checksum
        0x5A, 0x31, 0x00, // Manufacturer ID, least significant byte (LSB) first (PICMG ID = 12634)
        0x16,             // PICMG record ID
        0x00,             // Record format version
        0x41              // Current drawn in multiples of 100 mA
    };

    u08 RecordLength = Module[2];
    u08 HeaderLength = 4;
    u08 Record[RecordLength];
    u08 Header[4];
    u08 TotalLength = RecordLength + HeaderLength + 1;

    // calculate record checksum
    for (i = 0; i < RecordLength; i++)
        Record[i] = Module[5 + i];

    Module[3] = checksum_clc(Record, RecordLength);

    // calculate header checksum
    for (i = 0; i < HeaderLength; i++)
        Header[i] = Module[i];

    Module[4] = checksum_clc(Header, HeaderLength);
    // copy buffer
    for (i = 0; i < TotalLength; i++)
        buf[i] = Module[i];

    return TotalLength;
}


//*******************************************/
u08 Multirecord_area_Point_to_point(u08 buf[])  //Called from Write_FRU_Info_default in this file
//*******************************************/
{
    u08 i;     //MJMJ: Review if this is needed for the ALB

    u08 Point_to_point[] =
    {
        0xC0,                        // Record type ID
        0x82,                        // End of list/version
        0x10,                        // record length
        0x00,                        // Record checksum
        0x00,                        // Header checksum
        0x5A, 0x31, 0x00,            // Manufacturer ID, least significant byte (LSB) first (PICMG ID = 12634)
        0x19,                        // PICMG record ID (Point to Point Record)
        0x00,                        // Record format version
        0x00,                        // OEM GUID count
        0x80,                        // Record type
        0x01,                        // AMC channel descriptor count
        0xA4, 0x98, 0xF3,            // AMC channel descriptor, LSB first (Channel ID = 0)
        0x00, 0x00, 0x00, 0x00, 0x00 // AMC link descriptor, LSB first
    };

    u08 RecordLength = Point_to_point[2];
    u08 HeaderLength = 4;
    u08 Record[RecordLength];
    u08 Header[4];
    u08 TotalLength = RecordLength + HeaderLength + 1;

    // calculate record checksum
    for (i = 0; i < RecordLength; i++)
        Record[i] = Point_to_point[5 + i];

    Point_to_point[3] = checksum_clc(Record, RecordLength);
    // calculate header checksum
    for (i = 0; i < HeaderLength; i++)
        Header[i] = Point_to_point[i];

    Point_to_point[4] = checksum_clc(Header, HeaderLength);
    // copy buffer
    for (i = 0; i < TotalLength; i++)
        buf[i] = Point_to_point[i];

    return TotalLength;
}

//*****************************************/
u08 ipmi_picmg_fru_control(u08 control_type)  //Called from various places
//*****************************************/
{
    switch (control_type)
    {
    case FRU_COLD_RESET:
        cbi(DCDC_ENABLE_PORT, DCDC_ENABLE_PIN);
        delay(100);
        sbi(DCDC_ENABLE_PORT, DCDC_ENABLE_PIN);
        break;
    case FRU_WARM_RESET:
        cbi(FPGA_RST_PORT, FPGA_RST_PIN);
        delay(100);
        sbi(FPGA_RST_PORT, FPGA_RST_PIN);
        break;
    case FRU_REBOOT:
        cbi(RELOAD_FPGAS_PORT, RELOAD_FPGAS_PIN);
        delay(100);
        sbi(RELOAD_FPGAS_PORT, RELOAD_FPGAS_PIN);
        break;
    case FRU_QUIESCE:
        break;
    case POWER_UP:
        sbi(DCDC_ENABLE_PORT, DCDC_ENABLE_PIN);
        sbi(REG_ENABLE_PORT, REG_ENABLE_PIN);
        delay(10);                                   //MJ: Do we really need a delay here?
        break;
    case POWER_DOWN:
        cbi(DCDC_ENABLE_PORT, DCDC_ENABLE_PIN);
        cbi(REG_ENABLE_PORT, REG_ENABLE_PIN);
        break;
    default:
        return(0xff);
        break;
    }

    return(0);
}


//*********************/
u08 user_ok_for_m4(void)    //Called from mmc_main.c
//*********************/
{
    // check if 12 V is present
    if (sens[VOLT_12].value > 0x82)
        return(1);     //OK for M4
    else
        return(0);     //Not OK for M4
}


//MJ-VB: Check implementation with VB

// It is not the job of the MMC to load the EEPROM.
// This should be done once during production.
// As the RTM load was delivered with an empty EEPROM we program it via the MMC anyway
//*******************/
void load_rtm_eeprom()
//*******************/
{
}


//***************************************************/
void set_led_pattern(u08 bank, u08 pattern, u08 level)
//***************************************************/
{
}

//*******************************************************************************************/
u08 ipmi_oem_user(u08 command, u08 *iana, u08 *user_data, u08 data_size, u08 *buf, u08 *error)
//*******************************************************************************************/
{
    //Note: You must not return more than 25 bytes

    //Check if this OEM command is supported by us.
    //The originator of the OEM command (see table 5.1 in the IPMI 1.5 spec will send a 3-byte IANA ID
    //If we recognise this ID we respond to the command. Otherwise we return an error code

    *error = IPMI_CC_OK;

    if (iana[0] != 0x00 || iana[1] != 0x00 || iana[2] != 0x60)
    {
        *error = IPMI_CC_PARAM_OUT_OF_RANGE;
        return(0);             //Not for us
    }

    buf[0] = iana[0];   //We have to return our IANA ID as the first 3 bytes of the reply
    buf[1] = iana[1];
    buf[2] = iana[2];

    //Check if we are supporting the command
    //if (command == 0x01)       //supported
    //...do something
    //else if (command == 0x02)  //not supported
    //{
    //    *error = IPMI_CC_INV_CMD;
    //    return(0);
    //}

    //Put here the additional data you want to return
    buf[3] = command + data_size;   //Just a test

    return(4);  //4 because we are returning 4 bytes.
}


