//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: project.h
// 
// Title		: project definitions
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : Pins configurations and main project definitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************


#ifndef PROJECT_H
#define PROJECT_H





/* firmware release */
#define MMC_FW_REL_MAJ		1	/* major release, 7 bit */
#define MMC_FW_REL_MIN		0	/* minor release, 8 bit */


#define STANDALONE   0x01
#define INCRATE   0x02

/*
 * common configuration
 */
#define NUM_SENSOR 		8	/* Number of sensors */
#define NUM_SDR         9   /* Number of SDRs */


//#define NUM_SENSOR 		6	/* Number of sensors */
//#define NUM_SDR         7   /* Number of SDRs */




//EEPROM size
//#define FRU_SIZE 1024
#define FRU_SIZE 296

#define MAX_BYTES_READ 22


/*
 * LEDs
 */
#define NUM_OF_LED 3



#define LED_BLEU_PORT	 PORTB
#define LED_BLEU_DDR	 DDRB
#define LED_BLEU_PIN	 PB7
#define LED_BLEU_IN		 PINB

#define LED_RED_PORT	 PORTB
#define LED_RED_DDR		 DDRB
#define LED_RED_PIN		 PB5
#define LED_RED_IN		 PINB


#define LED_GREEN_PORT	 PORTB
#define LED_GREEN_DDR	 DDRB
#define LED_GREEN_PIN	 PB6
#define LED_GREEN_IN	 PINB




/*
 * HOT SWAP handle
 */

#define HOT_SWAP_HANDLE_PORT PORTD
#define HOT_SWAP_HANDLE_PPIN PIND
#define HOT_SWAP_HANDLE_DDR DDRD
#define HOT_SWAP_HANDLE_PIN PD2


/*
 * PS0 and PS1
 */

#define PS0 PE3
#define PS1 PE2
#define PS01_PORT PINE 



/*
 * Geographical address detection pins
 */

#define GA0_PORT PINB
#define GA0_DDR DDRB
#define GA0_PORT_PIN PB3

#define GA1_PORT PINB
#define GA1_DDR DDRB
#define GA1_PORT_PIN PB2

#define GA2_PORT PINB
#define GA2_DDR DDRB
#define GA2_PORT_PIN PB1

#define GAPU_PORT PORTB
#define GAPU_DDR DDRB
#define GAPU_PORT_PIN PB0






/*
 * I2C configuration
 */
/* support for master/slave mode */

#define SCLPORT	PORTD	// i2c clock port
#define SCLDDR	DDRD	// i2c clock port direction
// data line port
#define SDAPORT	PORTD	// i2c data port
#define SDADDR	DDRD	// i2c data port direction
#define SDAPIN	PIND	// i2c data port input
// pin assignments
#define SCL		PD4		// i2c clock pin
#define SDA		PD5		// i2c data pin

/* I2C slave addresss */
#define I2C_SLAVE_ADDR_SENSOR_TEMP	0x19	/* EEPROM */












/*
 * SMBus configuration
 */

#define SMBCLKPORT	PORTD	// SMBus clock port
#define SMBCLKDDR	DDRD	// SMBus clock port direction
// data line port
#define SMBDATAPORT	PORTD	// SMBus data port
#define SMBDATADDR	DDRD	// SMBus data port direction
#define SMBDATAPIN	PIND	// SMBus data port input
// pin assignments
#define SMBCLK		PD4		// SMBus clock pin
#define SMBDATA		PD5		// SMBus data pin


#define SMB_SLAVE_ADDR_STEMP	0x19	/* temperature sensor */





// Interrupt from temeperature sensor

#define TEMP_INT1_PORT PINE
#define TEMP_INT1_DDR DDRE
#define TEMP_INT1_PIN PE4

/*
 * FPGA connection configuration
 */

// power good indicator

#define LOW_VOLTAGE_POK_PORT  PINA			// ok
#define LOW_VOLTAGE_POK_DDR  DDRA
#define LOW_VOLTAGE_POK_PIN      PA0


// run and soft-start control of LTM4600EV

#define DCDC_ENABLE_PORT    PORTA
#define DCDC_ENABLE_DDR     DDRA
#define DCDC_ENABLE_PIN    PA1

// FPGA PROM revision selection pins
#define SEL0_PORT PORTC
#define SEL0_DDR  DDRC
#define SEL0_PIN  PC4

#define SEL1_PORT PORTC
#define SEL1_DDR  DDRC
#define SEL1_PIN  PC5

// FPGA internal register reset pin

#define FPGA_RST_PORT PORTC				// OK
#define FPGA_RST_DDR  DDRC
#define FPGA_RST_PIN  PC2


// REG_ENABLE pin
#define REG_ENABLE_PORT PORTC
#define REG_ENABLE_DDR  DDRC
#define REG_ENABLE_PIN  PC6

// JTAG controller pin

#define JTAG_CNT_PORT PORTG
#define JTAG_CNT_DDR  DDRG
#define JTAG_CNT_PIN  PG4

// Reboot FPGA pin 

#define RELOAD_FPGAS_PORT PORTC			// ok
#define RELOAD_FPGAS_DDR  DDRC
#define RELOAD_FPGAS_PIN  PC3


// FPGA Master booting process done pin


#define FPGA_INIT_MASTER_DONE_PORT PINC
#define FPGA_INIT_MASTER_DONE_DDR  DDRC
#define FPGA_INIT_MASTER_DONE_PIN  PC5

// FPGA Slave booting process done pin


#define FPGA_INIT_SLAVE_DONE_PORT PINC
#define FPGA_INIT_SLAVE_DONE_DDR  DDRC
#define FPGA_INIT_SLAVE_DONE_PIN  PC4

//SEL_GTP pins

#define SEL_GTP_PORT PORTA
#define SEL_GTP_DDR  DDRA
#define SEL_GTP1  PA4
#define SEL_GTP2  PA5
#define SEL_GTP3  PA6
#define SEL_GTP4  PA6



// JTAG interface pins

#define JTAG_INTF_PORT PORTG
#define JTAG_INTF_DDR  DDRG
#define JTAG_INTF_TDI  PG0
#define JTAG_INTF_TMS  PG1
#define JTAG_INTF_TCK  PG2
#define JTAG_INTF_TDO  PG3


// JTAG player select

#define SEL_JTAG_PLR_PORT PORTE
#define SEL_JTAG_PLR_DDR  DDRE
#define SEL_JTAG_PLR_PIN  PE5




/* supported IPMI release, nibble swapped */
#define MMC_IPMI_REL		0x51	/* V1.5 */



/*
 * manufacturer specific identifier codes
 *
 * NOTE: Manufacturer identification is handled by
 *       http://www.iana.org/assignments/enterprise-numbers
 */
#define IPMI_CPPM_ID            0xbead  /* IANA  manufacturer id */
#define IPMI_CPPM_AMC_ID		0xbead

/* additional device support */
#define IPMI_MSG_ADD_DEV_SUPP	0x29	/* event receiver, accept sensor cmds */

/*
 * IPMI message module
 */
/* hardware type */
#define IPMI_MSG_AMC					/* AMC card */
#define IPMI_MSG_MANU_ID	IPMI_CPPM_ID		/* manufacturer id */
#define IPMI_MSG_PROD_ID	IPMI_CPPM_AMC_ID	/* product id */








#endif

