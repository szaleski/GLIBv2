//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: fru.h
// 
// Title		: FRU device header file
// Revision		: 1.1
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : FRU information and payload control definitions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************
#ifndef FRU_H
#define FRU_H


#include "avrlibtypes.h"


// Constants
#define FRU_SIZE              296

#define FRU_COLD_RESET        0
#define FRU_WARM_RESET        1
#define FRU_REBOOT            2
#define FRU_QUIESCE           4
#define POWER_UP              5
#define POWER_DOWN            6

#define FRU_PROM_REVISION_CMD 1
#define JTAG_CTRL_SET_CMD     2
#define FPGA_JTAG_PLR_CMD     3

enum M_Status{ M0, M1, M2, M3, M4, M5, M6, M7 };


//Function prototypes
u08 ipmi_get_fru_inventory_area_info(u08* buf);
u08 ipmi_fru_data_read(u08* area, u08 len, u08* data);
u08 ipmi_fru_data_write(u08* area,u08* data, u08 len);
u08 ipmi_picmg_port_state_get(u08 port, u08* buf);
u08 ipmi_picmg_port_state_set(u08 port, u08 state);
u08 ipmi_picmg_properties(u08* buf);
u08 ipmi_prom_version_change(u08 revision);
void ipmi_jtag_ctrl(u08 ctrl_type);
void ipmi_fpga_jtag_plr_set(u08 ctrl_type);

#endif //FRU_H
