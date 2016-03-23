//*****************************************************************************
// Copyright (C): 2010 CPPM ( Centre Pysique des Particules de Marseille ) 
// File Name	: mmc_main.c
// Date			:
// Title		: eeprom_cppm control file
// Revision		: 2.0
// Notes		: 7/04/2010
// Target MCU	: Atmel AVR series
//
// Author       : Bompard frédéric 
// 				: CPPM ( Centre Physique des Particules de Marseille ) 
//
// Description : EEPROM management 
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************




#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include"eeprom_cppm.h"
#include"ipmi_if.h"
#include"fru.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"


#define size_info 128  // size data for BOAR and PRODUCT Information


//*************************************************************************************
//**			 Write default value in epprom for Board information area.			 **
//** 			 Contains information about the board on which the FRU 				 **
//**		       information device is located as shown in table 4.				 **
//**			 Specification Module Management Controler mezzanine board	    	 **
//*************************************************************************************



u08 Board_information(u08 buf[])
{

u08 lenght=0;
int i;


//			  Default valeur											
	u08 Board[] = {
				0x01,											// Format version
				0x00,											// Board area lenght
				0x19,											// Language code default ( English )
				0x50,0x4A,0x74,									// Manufacturer date/time
				0xC4,											// Board manufacturer type/lenght
				'C','P','P','M',								// Board Manufacturer
				0xCB,											// Board product name type/lenght
				'A','M','C','-','S','o','r','m','i','o','u',	// Board product name
				0xC9,											// Board serial number type/lenght
				'1','1','1','1','1','1','1','1','1',			// Board serial number
				0xC5,											// Board part number type/lenght
				'A','M','C','-','S',							// Board part number
				0xC0,											// FRU file ID type/lenght
				0xC1,											// No more fields
				0x00,											// padding
				0x00,											// Board area checksum
				};


	
	
	for(i=0;i<size_info;i++) buf[i] = 0;
		


	lenght = sizeof (Board);

	Board[1] = size_info/8;

	Board[lenght - 2] = 0xC0 | ( size_info - lenght );  // size of padding for data = size_info bytes

	

	for(i=0;i<lenght;++i)
			buf[i] = Board[i];

	buf[size_info-1] = checksum_clc( buf,size_info-1);
	
	return size_info;		// return size_info
}

//******************************************************************************************
//******************************************************************************************






u08 Product_information(u08 buf[])
{

u08 lenght=0;
int i;


//			  Default valeur											
	u08 Product[] = {
				0x01,											// Format version
				0x00,											// Product area lenght
				0x19,											// Language code default ( English )
				0xC4,											// Product manufacturer type/lenght
				'C','P','P','M',								// Product Manufacturer
				0xCB,											// Product product name type/lenght
				'A','M','C','-','S','u','g','i','t','o','n',	// Product product name
				0xC5,											// Board part/model number type/lenght
				'A','M','C','-','S',							// Board part/model
				0xC2,											// Product version type/lenght
				'0','2',										// Product version
				0xCC,											// Product serial number type/lenght
				'1','1','1','1','1','1','1','1','2','1','1','2',// Product serial number
				0x01,											// Asset tag type/lenght
				0xC0,											// FRU file ID type/lenght
				0xC1,											// End of fields
				0x00,											// Padding
				0x00											// Product area checksum
				};



	for(i=0;i<size_info;i++) buf[i] = 0;

	lenght = sizeof (Product);

	Product[1] = size_info/8;

	Product[lenght - 2] = 0xC0 | ( size_info - lenght );

	

	for(i=0;i<lenght;++i)
			buf[i] = Product[i];

	buf[size_info-1] = checksum_clc( buf,size_info-1);
	
	return size_info;






}



u08 Multirecord_area_Module(u08 buf[])
{

u08 lenght=0;
int i;

u08 Module[] = {
				0xC0,							// Record type ID
				0x02,							// End of liste/version
				0x06,							// record lenght
				0x00,							// Record checksum
				0x00,							// Header checksum
				0x5A,0x31,0x00,					// Manufactuer ID, least significant byte (LSB) first ( PICMG ID = 12634) 
				0x16,							// PICMG record ID 
				0x00,							// Record format version          //***** Modified by P.Vichoudis - it was: 0x2F,// Current draw ( in 100 mA) value default (15A = 0xF8) ( 9.5A = 0x2F )
				0x2F							// Current draw 0x2F*100mA = 4.7A //***** Modified by P.Vichoudis - it was: 0x00 // Record format version
				};



u08 RecordLength = Module[2];							//***** Modified by P.Vichoudis - it was: 	for(i=0;i<size_info;i++) buf[i] = 0;
u08 HeaderLength = 4;									//***** Modified by P.Vichoudis - it was: 
u08 Record[RecordLength];								//***** Modified by P.Vichoudis - it was: 	lenght = sizeof (Module);
u08 Header[4];											//***** Modified by P.Vichoudis - it was: 	Module[1] = lenght;
u08 TotalLength = RecordLength+HeaderLength+1;			//***** Modified by P.Vichoudis - it was: 
// calculate record checksum							//***** Modified by P.Vichoudis - it was: 	Module[lenght-1] = checksum_clc( Module,lenght-1);
for(i=0;i<RecordLength;i++) {Record[i]=Module[5+i]; }	//***** Modified by P.Vichoudis - it was: 
Module[3] = checksum_clc( Record,RecordLength);			//***** Modified by P.Vichoudis - it was: 	for(i=0;i<lenght;++i)
// calculate header checksum							//***** Modified by P.Vichoudis - it was: 			buf[i] = Module[i];
for(i=0;i<HeaderLength;i++)	{Header[i]=Module[i];   }	//***** Modified by P.Vichoudis - it was: 
Module[4] = checksum_clc( Header,HeaderLength);			//***** Modified by P.Vichoudis - it was: 
// copy buffer											//***** Modified by P.Vichoudis - it was: 
for(i=0;i<TotalLength;i++) {buf[i] = Module[i];		}	//***** Modified by P.Vichoudis - it was: 	
return TotalLength;										//***** Modified by P.Vichoudis - it was: 	return lenght;
}



u08 Multirecord_area_Point_to_point(u08 buf[])
{

u08 lenght=0;
int i;

u08 Point_to_point[] = {
				0xC0,							// Record type ID
				0x82,							// End of liste/version
				0x10,							// record lenght
				0x00,							// Record checksum
				0x00,							// Header checksum
				0x5A,0x31,0x00,					// Manufactuer ID, least significant byte (LSB) first ( PICMG ID = 12634) 
				0x19,							// PICMG record ID (Point to Point Record)
				0x00,							// Record format version
				0x00,							// OEM GUID count
				0x80,							// Record type
				0x01,							// AMC channel descriptor count
				0xA4,0x98,0xF3,					// AMC channel descriptor, LSB first (Channel ID = 0)
				0x00,0x00,0x00,0x00,0x00		// AMC link descriptor, LSB first
				};
u08 RecordLength = Point_to_point[2];							//***** Modified by P.Vichoudis - it was:
u08 HeaderLength = 4; 											//***** Modified by P.Vichoudis - it was:
u08 Record[RecordLength];										//***** Modified by P.Vichoudis - it was:	for(i=0;i<size_info;i++) buf[i] = 0;
u08 Header[4];													//***** Modified by P.Vichoudis - it was:
u08 TotalLength = RecordLength+HeaderLength+1;					//***** Modified by P.Vichoudis - it was:
// calculate record checksum									//***** Modified by P.Vichoudis - it was:	lenght = sizeof (Point_to_point);
for(i=0;i<RecordLength;i++) {Record[i]=Point_to_point[5+i]; }	//***** Modified by P.Vichoudis - it was:
Point_to_point[3] = checksum_clc( Record,RecordLength);			//***** Modified by P.Vichoudis - it was:
// calculate header checksum									//***** Modified by P.Vichoudis - it was:	Point_to_point[lenght-1] = checksum_clc( Point_to_point,lenght-1);
for(i=0;i<HeaderLength;i++)	{Header[i]=Point_to_point[i];   }	//***** Modified by P.Vichoudis - it was:
Point_to_point[4] = checksum_clc( Header,HeaderLength);			//***** Modified by P.Vichoudis - it was:	for(i=0;i<lenght;++i)
// copy buffer													//***** Modified by P.Vichoudis - it was:			buf[i] = Point_to_point[i];
for(i=0;i<TotalLength;i++)  {buf[i] = Point_to_point[i];	}	//***** Modified by P.Vichoudis - it was:
return TotalLength;												//***** Modified by P.Vichoudis - it was:	return lenght;
}



void Write_FRU_Info_default(void)
{

	u16 size=0;
	static u16 size_M=0;
	u08 buf[512];
	u08  Header[8];
	int i;
	


	
	Header[0] = 0x01;
	Header[1] = 0x00;	// area Internal is empty 	
	Header[2] = 0x00;	// area Chassis is empty




	Header[3] = 0x08/8;	// Offset area Board start
	size = 8;
	size_M = size;

	size += Board_information(buf);

	Header[4] = size/8;	// offset area Product info


	CRITICAL_SECTION_START;
      //  eeprom_write_block(buf,&size,size_M);
	for(i=0;i<(size-size_M);i++)
	{
		EEPROM_write(i+size_M, buf[i]);
	}
   
    

	size_M = size;


	size += Product_information(buf);

	for(i=0;i<(size-size_M);i++)
	{
		EEPROM_write(i+size_M, buf[i]);
	}





	Header[5] = size/8;	// offset area Multirecord


	size_M = size;

	size += Multirecord_area_Module(buf);

	for(i=0;i<(size-size_M);i++)
	{
		EEPROM_write(i+size_M, buf[i]);
	}
	
	size_M = size;
    size += Multirecord_area_Point_to_point(buf);
	
	for(i=0;i<(size-size_M);i++)
	{
		EEPROM_write(i+size_M, buf[i]);
	}

	size_M = size;

	Header[6] = 0x00;	// PAD


	Header[7] =  checksum_clc( Header,7);


	for(i=0;i<8;i++)
	{
		EEPROM_write(i, Header[i]);
	}

	CRITICAL_SECTION_END;
}







void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{

//	CRITICAL_SECTION_START;
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
		
	/* Set up address and data registers */
	
	EEARL = uiAddress & 0xFF;
	EEARH = uiAddress>>8;
	EEDR = ucData;

	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
//	CRITICAL_SECTION_END;
}



unsigned char EEPROM_read(unsigned int uiAddress)
{
	/* Wait for completion of previous write */
	while(EECR & (1<<EEWE));
	
	/* Set up address register */
	EEARL = uiAddress & 0xFF;
	EEARH = uiAddress>>8;
	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);
	/* Return data from data register */
	return EEDR;
}



