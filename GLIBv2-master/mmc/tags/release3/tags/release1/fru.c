//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
//File Name	: fru.c
// 
// Title		: FRU device
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
//
// Description : FRU(Field Replaceable Unit) information and payload control.
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************



#include <avr/eeprom.h>
#include "avrlibdefs.h"
#include "timer.h"
#include "fru.h"
#include "project.h"
#include "ipmi_if.h"
#include "eeprom_cppm.h"





u08 ipmi_get_fru_inventory_area_info( u08* buf )
{
    u08 len = 0;

    buf[len++] = FRU_SIZE & 0xff; 
    buf[len++] = FRU_SIZE >> 8; 
    buf[len++] = 0x00; //byte access type

    return len;
}



//*********************************************************************



u08 ipmi_fru_data_read( u08* area, u08 len, u08* data )
{

  u16 address=0;
  int i;
  
  
  if( len > MAX_BYTES_READ )
      return -1;
  
  address = area[0] | ( area[1] << 8);


  if( (address + len) > FRU_SIZE )
      return -1;

  //  CRITICAL_SECTION_START;

     *data++ = len;
     

	 for(i=0;i<len;i++)
			*(data + i) = EEPROM_read(address+i);
    
	
	// eeprom_read_block (data,&address,len);

    
  //  CRITICAL_SECTION_END;
   return len+1;
}



//*********************************************************************

u08 ipmi_fru_data_write( u08* area,u08* data, u08 len )
{
  u16 address;
  int i;
  

  address = area[0] | ( area[1] << 8);


  if( (address + len) > FRU_SIZE )
      return -1;

    CRITICAL_SECTION_START;

       // eeprom_write_block(data,&address,len);
 	for(i=0;i<len;i++)
			EEPROM_write(address+i,data[i]);

   
    CRITICAL_SECTION_END;
   return len;
}



//*********************************************************************


u08 ipmi_picmg_port_state_get( u08 port, u08* buf )
{

    

    return 0;
}



//*********************************************************************


u08 ipmi_picmg_port_state_set( u08 port, u08 state )
{
return 0;
}



//*********************************************************************



u08 ipmi_picmg_properties( u08* buf )
{
    u08 len = 0;

    buf[len++] = 0x04;// version of the PICMG extensions
    buf[len++] = 0x00;// Maximum FRU Device ID 
    buf[len++] = 0x00;// FRU Device ID

    return len;

}


//*********************************************************************


u08 ipmi_picmg_fru_control(u08 control_type)
{
    u08 cc = 0;

    switch( control_type )
    {
        case FRU_COLD_RESET: 
            cbi( DCDC_ENABLE_PORT, DCDC_ENABLE_PIN );
            delay(100);
            sbi( DCDC_ENABLE_PORT, DCDC_ENABLE_PIN );
            break;
        case FRU_WARM_RESET:
            cbi( FPGA_RST_PORT, FPGA_RST_PIN );
            delay(100);
            sbi( FPGA_RST_PORT, FPGA_RST_PIN );
            break;
        case FRU_REBOOT:
            cbi( RELOAD_FPGAS_PORT, RELOAD_FPGAS_PIN );
            delay(100);
            sbi( RELOAD_FPGAS_PORT, RELOAD_FPGAS_PIN );
            break;
        case FRU_QUIESCE:
            break;
        case POWER_UP:
            //if( !bit_is_set(DCDC_ENABLE_DDR, DCDC_ENABLE_PIN ) )
           // {
           //    sbi( DCDC_ENABLE_DDR, DCDC_ENABLE_PIN );
           //    delay(10);
           // }
            sbi( DCDC_ENABLE_PORT, DCDC_ENABLE_PIN );
			sbi( REG_ENABLE_PORT, REG_ENABLE_PIN );
            delay(10);
            break;
        case POWER_DOWN:
          //  if( !bit_is_set(DCDC_ENABLE_DDR, DCDC_ENABLE_PIN ) )
           // {
           //    sbi( DCDC_ENABLE_DDR, DCDC_ENABLE_PIN );
           //    delay(100);
            //}
            cbi( DCDC_ENABLE_PORT, DCDC_ENABLE_PIN );
			cbi( REG_ENABLE_PORT, REG_ENABLE_PIN );
            break;
        default:
            cc = -1;
            break;
    }


    return cc;
}



//*********************************************************************



u08 ipmi_prom_version_change(u08 revision)
{

    u08 cc = 0;

    switch(revision)
    {
        case 0:
            sbi(SEL0_PORT, SEL0_PIN);
            sbi(SEL1_PORT, SEL1_PIN);
            ipmi_picmg_fru_control( FRU_REBOOT );
            break;
        case 1:
            cbi(SEL0_PORT, SEL0_PIN);
            sbi(SEL1_PORT, SEL1_PIN);
            ipmi_picmg_fru_control( FRU_REBOOT );
            break;
        case 2:
            sbi(SEL0_PORT, SEL0_PIN);
            cbi(SEL1_PORT, SEL1_PIN);
            ipmi_picmg_fru_control( FRU_REBOOT );
            break;
        case 3:
            cbi(SEL0_PORT, SEL0_PIN);
            cbi(SEL1_PORT, SEL1_PIN);
            ipmi_picmg_fru_control( FRU_REBOOT );
            break;
        default:
            cc = -1;
            break;
         
    }

    return cc;

}



//*********************************************************************


void ipmi_jtag_ctrl( u08 ctrl_type)
{



    if( !bit_is_set(JTAG_CNT_DDR, JTAG_CNT_PIN ) )
    {
           sbi(JTAG_CNT_DDR, JTAG_CNT_PIN );
           delay(10);
    }

    if( ctrl_type > 0 )
        sbi( JTAG_CNT_PORT, JTAG_CNT_PIN );
    else
        cbi( JTAG_CNT_PORT, JTAG_CNT_PIN );



}




//*********************************************************************


void ipmi_fpga_jtag_plr_set( u08 ctrl_type)
{

    if( !bit_is_set(SEL_JTAG_PLR_DDR, SEL_JTAG_PLR_PIN ) )
    {
           sbi(SEL_JTAG_PLR_DDR, SEL_JTAG_PLR_PIN );
           delay(10);
    }

    if( ctrl_type > 0 )
        sbi( SEL_JTAG_PLR_PORT, SEL_JTAG_PLR_PIN );
    else
        cbi( SEL_JTAG_PLR_PORT, SEL_JTAG_PLR_PIN );
    


}

//*********************************************************************


// ********************* end of file ***********************************
