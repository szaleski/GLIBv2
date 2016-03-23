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

// For the AMC slot definition
extern u08 ipmb_address;

u08 SDR0[] =
{
	0x01,           // record number, LSB -> SDR_Init
	0x00,           // record number, MSB -> SDR_Init
	0x51,			// IPMI protocol version
	0x12,			// record type: device locator
	24,			    // record length: remaining bytes -> SDR_Init
	/* record key bytes */
	0x00, 			// device slave address
	0x00, 			// channel number
	/* record body bytes */
	0x00, 			// power state notification, global initialization
//
	0x29, 			// device capabilities
	0x00,			// reserved
	0x00,			// reserved
	0x00,			// reserved
	0xc1,			// entityID
	0x00,			// entity instance
	0x00,			// OEM
	0xcd,			// device ID string type/length
//
	'C','E','R','N',' ','A','M','C','-','G','L','I','B'
};

//Hot Swap Sensor (Compact)
//u08 SDR1[] =
//{
    ////sensor record header
	//0x00,              // record number, LSB -> SDR_Init
	//0x00,              // record number, MSB -> SDR_Init
    //0x51,              //IPMI protocol version
    //0x02,              //record type: compact sensor
      //39,              //record length: remaining bytes -> SDR_Init
    ////record key bytes
    //0x00,              //i2c address -> SDR_Init
    //0x00,              //sensor owner LUN
    //HOT_SWAP_SENSOR,   //sensor number
////
    ////record body bytes
    //0xc1,              //entity id: AMC
    //0x00,              //entity instance, -> SDR_Init
    //0x03,              //initialization
    //0x42,              //capabilities
    //HOT_SWAP,          //type
    //0x08,              //event/read type: discrete
    //0x07,              //LSB assert event mask: 3 bit value
    //0x00,              //MSB assert event mask
////  
    //0x07,              //LSB deassert event mask: 3 bit value
    //0x00,              //MSB deassert event mask
    //0x07,              //LSB read event mask: 3 bit value
    //0x00,              //MSB read event mask
    //0xc0,              //sensor units 1 Rate: none, Mod: none
    //0x00,              //sensor units 2 Unspecified
    //0x00,              //sensor units 3 Unused
    //0x01,              //LSB sensor record sharing
////
    //0x00,              //MSB sensor record sharing
    //0x00,              //positive threshold hysteresis
    //0x00,              //negative threshold hysteresis
    //0x00,              //reserved
    //0x00,              //reserved
    //0x00,              //reserved
    //0x00,              //OEM reserved
	//0xcc,              // 8 bit ASCII, number of bytes
////
	//'G','L','I','B', ' ', 'H', 'o', 't', 'S', 'w', 'a', 'p' //sensor string
//};

//Hot Swap Sensor (Full)
u08 SDR1[] =
{
	//sensor record header
	0x03,            //record number, LSB  - filled by SDR_Init()
	0x00,            //record number, MSB  - filled by SDR_Init()
	0x51,            //IPMI protocol version
	0x01,            //record type: full sensor
	0x37,            //record length: remaining bytes -> SDR_Init
	//record key bytes
	0x00,            //i2c address, -> SDR_Init
	0x00,            //sensor owner LUN
	HOT_SWAP_SENSOR, //sensor number
    // 
	//record body bytes
	0xc1,            //entity id: AMC Module
	0x00,            //entity instance -> SDR_Init
	0x03,            //init: event generation + scanning enabled
	0xc1,            //capabilities: auto re-arm,
	HOT_SWAP,        //sensor type: HOT SWAP
	0x6f,            //sensor reading
	0x07,            //LSB assert event mask: 3 bit value
	0x00,            //MSB assert event mask
	//
	0x07,            //LSB deassert event mask: 3 bit value
	0x00,            //MSB deassert event mask
	0x00,            //LSB: readable Threshold mask: all thresholds are readable
	0x00,            //MSB: setable Threshold mask: all thresholds are setable
	0xc0,            //sensor units 1
	0x00,            //sensor units 2
	0x00,            //sensor units 3
	0x00,            //Linearization
	//
	0x00,            //M
	0x00,            //M - Tolerance
	0x00,            //B
	0x00,            //B - Accuracy
	0x00,            //Sensor direction
	0x00,            //R-Exp , B-Exp
	0x00,            //Analogue characteristics flags
	0x00,            //Nominal reading
	//
	0x00,            //Normal maximum
	0x00,            //Normal minimum
	0x00,            //Sensor Maximum reading
	0x00,            //Sensor Minimum reading
	0x00,            //Upper non-recoverable Threshold
	0x00,            //Upper critical Threshold
	0x00,            //Upper non critical Threshold
	0x00,            //Lower non-recoverable Threshold
	//
	0x00,            //Lower critical Threshold
	0x00,            //Lower non-critical Threshold
	0x00,            //positive going Threshold hysteresis value
	0x00,            //negative going Threshold hysteresis value
	0x00,            //reserved
	0x00,            //reserved
	0x00,            //OEM reserved
	0xcc,            // 8 bit ASCII, number of bytes
	//
	'G','L','I','B', ' ', 'H', 'o', 't', 'S', 'w', 'a', 'p' //sensor string
};

//Temperature 1 - Full Sensor
u08 SDR2[] =
{
	//sensor record header
	0x03,           // record number, LSB -> SDR_Init
	0x00,           // record number, MSB -> SDR_Init
	0x51,			// IPMI protocol version
	0x01,			// record type: full sensor
	56,			    // record length: remaining bytes -> SDR_Init
	//record key bytes
	0x00,			// i2c address, -> SDR_Init
	0x00,			// sensor owner LUN
	TEMPERATURE_SENSOR1, //sensor number
	//
	//record body bytes
	0xc1,			// entity id: AMC Module
	0x00,			// entity instance -> SDR_Init
	0x03,			// init: event generation + scanning enabled
	0x48,			// capabilities: auto re-arm, rw-threshold
	TEMPERATURE,    // sensor type: Temperature
	0x01,			// sensor reading: threshold
	0x80,			// LSB assert event mask: 3 bit value
	0x0A,			// MSB assert event mask
	//
	0x80,			// LSB deassert event mask: 3 bit value
	0x7A,			// MSB deassert event mask
	0x38,			// LSB: readabled Threshold mask: all thresholds are readable:
	0x38,           // MSB: setabled Threshold mask: all thresholds are setable  :
	0x80,			// sensor units 1 : Twos complement
	0x01,			// sensor units 2 : Temperature
	0x00,			// sensor units 3 : Modifiers
	0x00,           // Linearisation
	//
	0x01,           // M
	0x00,           // M - Tolerance
	0x00,           // B
	0x00,           // B - Accuracy
	0x00,           // Sensor direction
	0x00,           // R-Exp , B-Exp
	0x07,           // Analogue characteristics flags
	30,             // Nominal reading
	//
	40,             // Normal maximum
	20,             // Normal minimum
	100,            // Sensor Maximum reading
	0,              // Sensor Minimum reading
	70,             // Upper non-recoverable Threshold
	60,             // Upper critical Threshold
	45,             // Upper non critical Threshold
	0,				// Lower non-recoverable Threshold
	//
	0,				// Lower critical Threshold
	0,              // Lower non-critical Threshold
	0x02,           // positive going Threshold hysteresis value
	0x02,           // negative going Threshold hysteresis value
	0x00,           // reserved
	0x00,           // reserved
	0x00,           // OEM reserved
	0xcd,           //8 bit ASCII, number of bytes
	//
	'G','L','I','B', ' ','T','e','m','p','F', 'P', 'G', 'A'   //sensor string
};

//Temperature 2 - Full Sensor
u08 SDR3[] =
{
	//sensor record header
	0x04,           // record number, LSB -> SDR_Init
	0x00,           // record number, MSB -> SDR_Init
	0x51,           //IPMI protocol version
	0x01,           //record type: full sensor
	57,             //record length -> SDR_Init
	//record key bytes
	0x00,           //i2c address, -> SDR_Init
	0x00,           //sensor owner LUN
	TEMPERATURE_SENSOR2, //sensor number
	//
	//record body bytes
	0xc1,			// entity id: AMC Module
	0x00,			// entity instance -> SDR_Init
	0x03,			// init: event generation + scanning enabled
	0x48,			// capabilities: auto re-arm, rw-threshold
	TEMPERATURE,    // sensor type: Temperature
	0x01,			// sensor reading: threshold
	0x80,			// LSB assert event mask: 3 bit value
	0x0A,			// MSB assert event mask
	//
	0x80,			// LSB deassert event mask: 3 bit value
	0x7A,			// MSB deassert event mask
	0x38,			// LSB: readabled Threshold mask: all thresholds are readable:
	0x38,        	// MSB: setabled Threshold mask: all thresholds are setable  :
	0x80,			// sensor units 1 : Twos complement
	0x01,			// sensor units 2 : Temperature
	0x00,			// sensor units 3 : Modifiers
	0x00,           // Linearisation
	//
	0x01,           // M
	0x00,           // M - Tolerance
	0x00,           // B
	0x00,           // B - Accuracy
	0x00,           // Sensor direction
	0x00,           // R-Exp , B-Exp
	0x07,           // Analogue characteristics flags
	30,             // Nominal reading
	//
	40,             // Normal maximum
	20,             // Normal minimum
	100,            // Sensor Maximum reading
	0,              // Sensor Minimum reading
	70,             // Upper non-recoverable Threshold
	60,             // Upper critical Threshold
	45,             // Upper non critical Threshold
	0,				// Lower non-recoverable Threshold
	//
	0,				// Lower critical Threshold
	0,              // Lower non-critical Threshold
	0x02,           // positive going Threshold hysteresis value
	0x02,           // negative going Threshold hysteresis value
	0x00,           // reserved
	0x00,           // reserved
	0x00,           // OEM reserved
	0xce,           //8 bit ASCII, number of bytes
	//
	'G','L','I','B', ' ','T','e','m','p','F', 'r','o','n','t' //sensor string
};

//Temperature 3 - Full Sensor
u08 SDR4[] =
{
	//sensor record header
	0x05,           // record number, LSB -> SDR_Init
	0x00,           // record number, MSB -> SDR_Init
	0x51,           // IPMI protocol version
	0x01,           // record type: full sensor
	56,           // record length -> SDR_Init
	//record key bytes
	0x00,           // i2c address, -> SDR_Init
	0x00,           // sensor owner LUN
	TEMPERATURE_SENSOR3, //sensor number
	//
	//record body bytes
	0xc1,			// entity id: AMC Module
	0x00,			// entity instance -> SDR_Init
	0x03,			// init: event generation + scanning enabled
	0x48,			// capabilities: auto re-arm, rw-threshold
	TEMPERATURE,    // sensor type: Temperature
	0x01,			// sensor reading: threshold
	0x80,			// LSB assert event mask: 3 bit value
	0x0A,			// MSB assert event mask
	//
	0x80,			// LSB deassert event mask: 3 bit value
	0x7A,			// MSB deassert event mask
	0x38,			// LSB: readabled Threshold mask: all thresholds are readable:
	0x38, //0x00,	// MSB: setabled Threshold mask: all thresholds are setable  :
	0x80,			// sensor units 1 : Twos complement
	0x01,			// sensor units 2 : Temperature
	0x00,			// sensor units 3 : Modifiers
	0x00,           // Linearisation
	//
	0x01,           // M
	0x00,           // M - Tolerance
	0x00,           // B
	0x00,           // B - Accuracy
	0x00,           // Sensor direction
	0x00,           // R-Exp , B-Exp
	0x07,           // Analogue characteristics flags
	30,             // Nominal reading
	//
	40,             // Normal maximum
	20,             // Normal minimum
	100,            // Sensor Maximum reading
	0,              // Sensor Minimum reading
	70,             // Upper non-recoverable Threshold
	60,             // Upper critical Threshold
	45,             // Upper non critical Threshold
	0,				// Lower non-recoverable Threshold
	//
	0,				// Lower critical Threshold
	0,              // Lower non-critical Threshold
	0x02,           // positive going Threshold hysteresis value
	0x02,           // negative going Threshold hysteresis value
	0x00,           // reserved
	0x00,           // reserved
	0x00,           // OEM reserved
	0xcd,           // 8 bit ASCII, number of bytes
	//
	'G','L','I','B', ' ','T','e','m','p','R', 'e','a','r' //sensor string
};

//12V Payload power sensor
u08 SDR5[] =
{
	//sensor record header
	0x06,           // record number, LSB -> SDR_Init
	0x00,           // record number, MSB -> SDR_Init
	0x51,			// IPMI protocol version
	0x01,			// record type: full sensor
	  51,			// record length: remaining bytes -> SDR_Init
	//record key bytes
	0x00,           // i2c address, -> SDR_Init
	0x00,           // sensor owner LUN
	VOLT_12,        // sensor number
	//
	//record body bytes
	0xc1,           // entity id: AMC Module
	0x00,           // entity instance -> SDR_Init
	0x03,           // init: event generation + scanning enabled
	0x48,           // capabilities: auto re-arm, Threshold is readable and setable
	VOLTAGE,        // sensor type: Voltage
	0x01,           // sensor reading: Threshold
	0x80,           // LSB assert event mask: 3 bit value
	0x0a,           // MSB assert event mask
	//
	0xa8,           // LSB deassert event mask: 3 bit value
	0x7a,           // MSB deassert event mask
	0x3F,			// LSB: readabled Threshold mask: all thresholds are readable: */
	0x3F, 	        // MSB: setabled Threshold mask: all thresholds are setable  : */
	0x00,           // sensor units 1 : unsigned (NOT 2's complement)
	0x04,           // sensor units 2 : Voltage
	0x00,           // sensor units 3 : Modifier
	0x00,           // Linearization
	//
	0x4e,           // M
	0x02,           // M - Tolerance
	0x00,           // B
	0x04,           // B - Accuracy
	0x0c,           // Sensor direction
	0xd0,           // R-Exp , B-Exp
	0x07,           // Analogue characteristics flags
	0xa0,           // Nominal reading
	//
	0xb0,           // Normal maximum
	0x90,           // Normal minimum
	0xff,           // Sensor Maximum reading
	0x00,           // Sensor Minimum reading
	0xf7,           // Upper non-recoverable Threshold
	0xf2,           // Upper critical Threshold
	0xe9,           // Upper non critical Threshold
	0x60,           // Lower non-recoverable Threshold
	//
	0x70,           // Lower critical Threshold
	0x80,           // Lower non-critical Threshold
	0x02,           // positive going Threshold hysteresis value
	0x02,           // negative going Threshold hysteresis value
	0x00,           // reserved
	0x00,           // reserved
	0x00,           // OEM reserved
	0xc8,           // 8 bit ASCII, number of bytes
	//
	'G','L','I','B', ' ','1', '2', 'V' //sensor string
};
u08 *sdrPtr[NUM_SDR_AMC + NUM_SDR_RTM] = {SDR0, SDR1, SDR2, SDR3, SDR4, SDR5};
u08 sdrLen[NUM_SDR_AMC + NUM_SDR_RTM] =  {sizeof(SDR0), sizeof(SDR1), sizeof(SDR2), sizeof(SDR3), sizeof(SDR4), sizeof(SDR5)};


sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];


//*****************************/
u08 Board_information(u08 buf[])  //Called from Write_FRU_Info_default in this file
//*****************************/
{
    u08 length = 0, i;

    // Default values
    u08 Board[] =
    {
	    0x01,                                                   // Format version
	    0x00,                                                   // Board area length
	    0x19,                                                   // Language code default ( English )
	    0x60, 0x35, 0x88,                                       // Manufacturer date/time in minutes since 1/1/1996 ( for date 21/12/2012 = 8,926,560 minutes = 0x883560)
	    0xC4,                                                   // Board manufacturer type/length
	    'C', 'E', 'R', 'N',                                     // Board Manufacturer
	    0xCB,                                                   // Board product name type/length (11)
	    'A','M','C',' ','G','L','I','B',' ','v','3',			// Board product name
	    0xCC,                                                   // Board serial number type/length (12)
	    '0','0','0','0','0','0','0','0','0','0','0','0',        // Board serial number
	    0xC2,                                                   // Board part number type/length
	    '0', '3',												// Board part number
	    0xC0,                                                   // FRU file ID type/length
	    0xC1,                                                   // No more fields
	    0x00,                                                   // padding
	    0x00,                                                   // Board area checksum
    };

    for (i = 0; i < SIZE_INFO; i++)
        buf[i] = 0;

    length = sizeof(Board);
    Board[1] = SIZE_INFO / 8;
    Board[length - 2] = 0xC0 | (SIZE_INFO - length);            // size of padding for data = size_info bytes

    for (i = 0; i < length; ++i)
        buf[i] = Board[i];

    buf[SIZE_INFO - 1] = checksum_clc(buf, SIZE_INFO - 1);
    return SIZE_INFO; // return size_info
}


//*******************************/
u08 Product_information(u08 buf[])   //Called from Write_FRU_Info_default in this file
//*******************************/
{
    u08 length = 0, i;

    // Default values
    u08 Product[] =
   {
	   0x01,                                                        // Format version
	   0x00,                                                        // Product area length
	   0x19,                                                        // Language code default ( English )
	   0xC4,                                                        // Product manufacturer type/length
	   'C', 'E', 'R', 'N',                                          // Product Manufacturer
	   0xCF,                                                        // Product product name type/length
	   'A','M','C',' ','G','L','I','B',' ','d','e','v','k','i','t', // Product product name
	   0xCB,                                                        // Board part/model number type/length
	   'w','/',' ','M','M','C',' ','v','4','.','0',					// Board part/model
	   0xCE,                                                        // Product version type/length
	  'F','W',':','1','.','0','2','/','1','3','0','2','0','8',      // Product version
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

    length = sizeof(Product);
    Product[1] = SIZE_INFO / 8;
    Product[length - 2] = 0xC0 | (SIZE_INFO - length);

    for (i = 0; i < length; ++i)
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
        0x82, //0x02,     // End of list/version --> bypass Multirecord area P2P
        0x06,             // record length
        0x00,             // Record checksum
        0x00,             // Header checksum
        0x5A, 0x31, 0x00, // Manufacturer ID, least significant byte (LSB) first (PICMG ID = 12634)
        0x16,             // PICMG record ID
        0x00,             // Record format version
          30,             // Current drawn in multiples of 100 mA
		   0,             // padding  
		   0,             // padding  
		   0,             // padding   
		   0			  // padding  
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

//**********************/
void init_port_user(void)    //Called from init_port() in mmc_main.c
//**********************/
{
	// DDR bits [1]-> output, [0]-> input
	
	//Warning: For the shared ports the user has to look at init_port() in mmc_main and copy the definitions of the "G" bits to this block of code

	
	 //Port A: NNGGGGNG (RTM), NNUUUUNG (non-RTM)
	 DDRA  = 0x3C; // PA[5:2], (RTM lines) set as outputs
	 PORTA = 0x00;

	
	//Port C: UUUUUUUU
	DDRC  = 0x00;
	PORTC = 0x00;

	//Port E: UUUUGGNN
	DDRE  = 0x80; // PE7 -> i2c bridge enable pin -> set as output
	PORTE = 0x00; // PE7 -> i2c bridge enable pin -> force low (disconnect mmc/fpga i2c buses)

	//Port F: GGGGUUUG
	DDRF  = 0x00;
	PORTF = 0x00;

	//Port G: NNNNUUUU
	DDRG  = 0x00;
	PORTG = 0x00;
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

	sens[VOLT_12].value = a2dConvert8bit(0);
}


//***************************/
u08 tempSensorRead(u08 sensor)    //Called from sensor_monitoring_user in user_code.c
//***************************/
{
	u08 stemp;     //sensor temperature
	//    u08 saddr;     //temperature sensor address

	u08 dev_addr;     //temperature sensor address
	u16 reg_addr;
	
	if (sensor == TEMPERATURE_SENSOR1)
{dev_addr = 0x2A; reg_addr = 0x01;}
	if (sensor == TEMPERATURE_SENSOR2)
{dev_addr = 0x1A; reg_addr = 0x00;}
	if (sensor ==  TEMPERATURE_SENSOR3)
{dev_addr = 0x4E; reg_addr = 0x00;}
	
	CRITICAL_SECTION_START;
	i2cswReceive(dev_addr, reg_addr, 1, 1, &stemp);
	CRITICAL_SECTION_END;

	return stemp;
}


//**************************************/
//**************************************/
//**************************************/
//**************************************/
//**************************************/
//**************************************/
//**************************************/
//**************************************/



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
	
	//Port A: NNGGGGNG (RTM), NNUUUNG (non-RTM), 
	//DDRA  = 0x3C; // PA[5:2], (RTM lines) set as outputs
	u08 amc_slot = (((ipmb_address - 0x70) >> 1)) & 0x0f;
	PORTA = amc_slot << 2 ; // forwards the amc_slot to the FPGA through the CPLD (RTM[3:0])
	
	// check if 12 V is present
	if (sens[VOLT_12].value > 0x90)
	return(1);     //OK for M4
	else
	return(0);     //Not OK for M4
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
}
//*******************/
void load_rtm_eeprom() //Called from ..
//*******************/
{
}
//***************************************************/
void set_led_pattern(u08 bank, u08 pattern, u08 level) //Called from ..
//***************************************************/
{
}
//******************/
void leds_init_user()    //Called from leds_init in led.c
//******************/
{
}
//**************************/
u08 state_led_user(u08 led_n)   //Called from state_led in led.c
//**************************/
{
}
//**************************************************/
void local_led_control_user(u08 led_n, u08 led_state)  //Called from local_led_control in led.c
//**************************************8***********/
{
}
//********************************************/
void led_control_user(u08 led_n, u08 led_state)  //Called from led_control in led.c
//********************************************/
{
}

//////////////////////////////////////////////////////////

//*******************************************************************************/
u08 ipmi_oem_user(u08 command, u08 *iana, u08 *user_data, u08 data_size, u08 *buf)
//*******************************************************************************/
{
    //Note: You must not return more than 25 bytes

    //Check if this OEM command is supported by us.
    //The originator of the OEM command (see table 5.1 in the IPMI 1.5 spec will send a 3-byte IANA ID
    //If we recognise this ID we respond to the command. Otherwise we return an error code
	//The completion codes can be found in http://download.intel.com/design/servers/ipmi/IPMIv1_5rev1_1.pdf Table 5-2	

	u08 completion_code;
	u08 oem_reply_size ;
	
    if (iana[0] != 0x00 || iana[1] != 0x00 || iana[2] != 0x60)
    {
        return(0xff);           //Not for us
    }

	//===== RawI2cSend =======//
	if ((command==0x33) && (data_size>5) && (data_size<22))	// min_size =  6: 5-byte header +  1-byte payload
	{														// max_size = 21: 5-byte header + 16-byte payload		
		u08  slave_addr    =   user_data[0] & 0x7f; // 7-bit slave_addr  
		u08  sub_addr_size =   user_data[1];
		u16  sub_addr      =  (user_data[2] << 8) + user_data[3];	
		u08  wrBuffer_size =   user_data[4] ; // number of bytes to write
		u08 *wrBuffer      = &(user_data[5]); // define a pointer (address) and initialize it with the address of the 5th element of the array user_data
			
		CRITICAL_SECTION_START;
		i2cswSend(slave_addr, sub_addr, sub_addr_size, wrBuffer_size, wrBuffer);
		CRITICAL_SECTION_END;
				
		completion_code = 0x00;	// Command completed normally
		oem_reply_size  = 4;	// default reply = 1-byte completion code + 3-byte IANA number
	}		

	//===== RawI2cReceive ====//
	else if ((command==0x34) && (data_size==5)) // 5-byte header only
	{
		u08  slave_addr    =   user_data[0] & 0x7f; // 7-bit slave_addr
		u08  sub_addr_size =   user_data[1];
		u16  sub_addr      =  (user_data[2] << 8) + user_data[3];
		u08  rdBuffer_size =   user_data[4] ; // number of bytes to write
		u08 *rdBuffer;

		CRITICAL_SECTION_START;
		i2cswReceive(slave_addr, sub_addr, sub_addr_size, rdBuffer_size, rdBuffer);
		CRITICAL_SECTION_END;
			
		completion_code = 0x00;	// Command completed normally
		oem_reply_size  = 4+rdBuffer_size;	// 1-byte completion code + 3-byte IANA number + bytes read
			
		for (int i=0; i<rdBuffer_size;i++) {buf[i+4]=rdBuffer[i];} // read request reply
	}		

	//===== else =======//
	else
	{
		completion_code = 0xC1;	// Invalid/unrecognized/unsupported command
		oem_reply_size  = 4;	// default reply = 1-byte completion code + 3-byte IANA number
	}
	
	//=== OEM reply ===//
    buf[0] = completion_code;
	buf[1] = iana[0];   //We have to return our IANA ID as the first 3 bytes of the reply
    buf[2] = iana[1];
    buf[3] = iana[2];
	// + buf[4:] for read requests only
	
    return(oem_reply_size);  //4 because we are returning 4 bytes.
}
