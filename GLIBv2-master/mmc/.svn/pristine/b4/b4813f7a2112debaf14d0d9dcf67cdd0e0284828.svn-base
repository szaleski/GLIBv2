//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: fru.c
// 
// Title		: FRU device
// Revision		: 1.1
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Markus Joos (markus.joos@cern.ch)
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
#include "eeprom.h"
#include "user_code_select.h"

//*******************************************/
u08 ipmi_get_fru_inventory_area_info(u08* buf) //Called from ipmi_if.c
//*******************************************/
{
    u08 len = 0;

    buf[len++] = FRU_SIZE & 0xff;
    buf[len++] = FRU_SIZE >> 8;
    buf[len++] = 0x00; //byte access type

    return (len);
}

//**************************************************/
u08 ipmi_fru_data_read(u08* area, u08 len, u08* data) //Called from ipmi_if.c
//**************************************************/
{
    u16 address = 0;
    u08 i;

    if (len > MAX_BYTES_READ)
        return (0xff);

    address = area[0] | (area[1] << 8);

    if ((address + len) > FRU_SIZE)
        return (0xff);

    *data++ = len;

    for (i = 0; i < len; i++)
        *(data + i) = EEPROM_read(address + i);

    return(len + 1);   //MJ ipmi_if.c only looks at errors (0xff). Maybe it would be cleaner to return 0
}


//***************************************************/
u08 ipmi_fru_data_write(u08* area, u08* data, u08 len) //Called from ipmi_if.c
//***************************************************/
{
    u16 address;
    u08 i;

    address = area[0] | (area[1] << 8);

    if ((address + len) > FRU_SIZE)
        return(0xff);

    CRITICAL_SECTION_START;

    for (i = 0; i < len; i++)
        EEPROM_write(address + i, data[i]);

    CRITICAL_SECTION_END;
    return(len);    //MJ ipmi_if.c only looks at errors (0xff). Maybe it would be cleaner to return 0
}


//**********************************************/
u08 ipmi_picmg_port_state_get(u08 port, u08* buf) //Called from ipmi_if.c
//**********************************************/
{
    return 0;
}


//***********************************************/
u08 ipmi_picmg_port_state_set(u08 port, u08 state) //Called from ipmi_if.c
//***********************************************/
{
    return 0;
}

//The PICMG properties are defined in ATCA 3.0, Table 3-11 (see errata)
//MJ: For AMCs the definitions in the AMC standard override ATCA 3.0. Therefore table 3-1 on page 3-4 of AMC R2.0 applies
//MJ: Finally the "Maximum FRU Device ID" has to be "1" according to MTCA.4
//********************************/
u08 ipmi_picmg_properties(u08* buf) //Called from ipmi_if.c
//********************************/
{
    u08 len = 0;
    //set_led_pattern(1, 0xff, 2);

    buf[len++] = 0x14;  // version of the PICMG extensions
    buf[len++] = USER_MAX_FRU;  // Maximum FRU Device ID
    buf[len++] = 0x00;  // FRU Device ID

    return(len);
}


//***************************************/
u08 ipmi_prom_version_change(u08 revision)   //Called from ipmi_if.c
//***************************************/
{
    switch (revision)
    {
    case 0:
        sbi(SEL0_PORT, SEL0_PIN);
        sbi(SEL1_PORT, SEL1_PIN);
        ipmi_picmg_fru_control(FRU_REBOOT);
        break;
    case 1:
        cbi(SEL0_PORT, SEL0_PIN);
        sbi(SEL1_PORT, SEL1_PIN);
        ipmi_picmg_fru_control(FRU_REBOOT);
        break;
    case 2:
        sbi(SEL0_PORT, SEL0_PIN);
        cbi(SEL1_PORT, SEL1_PIN);
        ipmi_picmg_fru_control(FRU_REBOOT);
        break;
    case 3:
        cbi(SEL0_PORT, SEL0_PIN);
        cbi(SEL1_PORT, SEL1_PIN);
        ipmi_picmg_fru_control(FRU_REBOOT);
        break;
    default:
        return(0xff);
        break;
    }

    return(0);
}


//MJ: Is this a FRU related function? If not: Is there a better place for it?
//*******************************/
void ipmi_jtag_ctrl(u08 ctrl_type)   //Called from ipmi_if.c
//*******************************/
{
    if (!bit_is_set(JTAG_CNT_DDR, JTAG_CNT_PIN))
    {
        sbi(JTAG_CNT_DDR, JTAG_CNT_PIN);
        delay(10);
    }

    if (ctrl_type > 0)
        sbi(JTAG_CNT_PORT, JTAG_CNT_PIN);
    else
        cbi(JTAG_CNT_PORT, JTAG_CNT_PIN);
}


//MJ: Is this a FRU related function? If not: Is there a better place for it?
//***************************************/
void ipmi_fpga_jtag_plr_set(u08 ctrl_type)   //Called from ipmi_if.c
//***************************************/
{
    if (!bit_is_set(SEL_JTAG_PLR_DDR, SEL_JTAG_PLR_PIN))
    {
        sbi(SEL_JTAG_PLR_DDR, SEL_JTAG_PLR_PIN);
        delay(10);
    }

    if (ctrl_type > 0)
        sbi(SEL_JTAG_PLR_PORT, SEL_JTAG_PLR_PIN);
    else
        cbi(SEL_JTAG_PLR_PORT, SEL_JTAG_PLR_PIN);
}
