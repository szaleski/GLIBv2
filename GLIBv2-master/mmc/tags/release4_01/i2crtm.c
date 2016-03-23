/*! \file i2rtm.c \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'i2rtm.c'
// Title		: Software I2C interface to RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************





//Note: User code still to be extracted




#include <avr/io.h>
#include "i2crtm.h"
#include "avrlibtypes.h"
#include "project.h"
#include "timer.h"
#include "rtm_mng.h"
#include "user_code_select.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed


//************************
//* I2C public functions *
//************************


//! Initialise I2C communication
//******************/
void i2crtmInit(void)    //Called from rtm_mng.c
//******************/
{
#ifdef USE_RTM
    sbi(RTMENBUFPORT, RTMENBUF);    // In our case activating the RTM I2C only consist into enabling the I2C buffer. Then the same I2C pins are used as for the rest.
#endif
}


//************************************************************************************/
void write_rtm_eeprom_page(u08 base_addr, u08 length, u08 *data, u16 after_write_delay)   //called from load_rtm_eeprom in this file
//************************************************************************************/
{
  u08 ack_ok = 0;

  CRITICAL_SECTION_START;
  i2cswSend(RTM_EEPROM_ADDR, base_addr, 2, length, data);

  //Poll for the EEPROM to be ready for the next write
  while (!ack_ok)
  {
      I2C_START
      ack_ok = i2cPutbyte(RTM_EEPROM_ADDR << 1);
      I2C_SDL_LO;
      I2C_STOP
  }

  CRITICAL_SECTION_END;
}





