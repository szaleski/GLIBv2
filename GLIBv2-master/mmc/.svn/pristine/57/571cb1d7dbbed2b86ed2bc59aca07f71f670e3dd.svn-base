/*! \file rtm_mng.c \brief Software I2C interface using port pins. */
//*****************************************************************************
//
// File Name	: 'rtm_mng.c'
// Title		: main management functions for RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
// Modified by  : Markus Joos (markus.joos@cern.ch)
//*****************************************************************************

#include "project.h"
#include <avr/io.h>
#include "i2crtm.h"
#include "rtm_mng.h"
#include "fru.h"
#include "sdr.h"
#include "ipmi_if.h"
#include "timer.h"
#include "led.h"

//Global variables
enum M_Status RTM_M_Status = M0;
extern enum M_Status FRU_status;
extern sensor_t sens[NUM_SENSOR];

volatile u08 leds_blinking = 0;
u08 mch_is_mtca4_comp = 0;
u08 compat_checked = 0;


//*******************/
void check_rtm_state()
//*******************/
{
    u08 ios;
    static u08 mch_reply_timeout = 0;

    if (bit_is_set(RTMPSPIN, RTMPS))
    {
        if (RTM_M_Status > M0)
        {
            rtm_power(RTM_12V, POWER_DOWN);
            rtm_power(RTM_3V3, POWER_DOWN);
            ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_ABSENT);
            RTM_M_Status = M0;
        }
        sens[RTM_HOT_SWAP_SENSOR].state = RTM_ABSENT;
    }
    else
    {
        if (sens[RTM_HOT_SWAP_SENSOR].state == RTM_ABSENT)
        {
            rtm_power(RTM_3V3, POWER_UP);
            i2crtmInit();
            sens[RTM_HOT_SWAP_SENSOR].state = RTM_PRESENT | HOT_SWAP_OPEN_STATE;
            compat_checked = 0;
        }
        else
        {
            switch (RTM_M_Status)
            {
            case M0:
                ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_PRESENT);
                write_io_ports(0x0D);
                RTM_M_Status = M1;
                break;
            case M1:
                if (!compat_checked)
                {
                    if (checkcompat())
                    {
                        sens[RTM_HOT_SWAP_SENSOR].state |= RTM_COMPATIBLE;
                        ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_COMPAT);
                    }
                    else
                    {
                        sens[RTM_HOT_SWAP_SENSOR].state |= RTM_INCOMPATIBLE;
                        ipmi_event_send(RTM_HOT_SWAP_SENSOR, RTM_HS_EVENT_INCOMPAT);
                    }
                    compat_checked = 1;
                    break;
                }
                if (FRU_status == M4 && (sens[RTM_HOT_SWAP_SENSOR].state & RTM_COMPATIBLE))
                {
                    ios = read_io_ports();
                    if (!(ios & (1 << RTM_HOT_SWAP_PIN)))
                    {
                        sens[RTM_HOT_SWAP_SENSOR].state &= 0xFD;
                        sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_CLOSE_STATE;
                        ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_CLOSED);
                        RTM_M_Status = M2;
                    }
                }
                break;
            case M2:
                ios = read_io_ports();
                if (ios & (1 << RTM_HOT_SWAP_PIN))
                {
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_OPENED);
                    sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_OPEN_STATE;
                    sens[RTM_HOT_SWAP_SENSOR].state &= 0xFE;
                    RTM_M_Status = M1;
                }
                break;
            case M3:
                if (!(bit_is_set(RTMEN12VPIN, RTMEN12V)))
                {
                    RTM_M_Status = M4;
                    break;
                }

                ios = read_io_ports();
                if (ios & (1 << RTM_HOT_SWAP_PIN))
                {
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_OPENED);
                    sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_OPEN_STATE;
                    sens[RTM_HOT_SWAP_SENSOR].state &= 0xFE;
                    RTM_M_Status = M5;
                }
                break;
            case M4:
                ios = read_io_ports();
                if (ios & (1 << RTM_HOT_SWAP_PIN))
                {
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_OPENED);
                    sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_OPEN_STATE;
                    sens[RTM_HOT_SWAP_SENSOR].state &= 0xFE;
                    RTM_M_Status = M5;
                    break;
                }
                if (!(inb(RTMPG12VPIN) & RTM_PG_MASK))
                    write_io_ports(0x07);
                else
                    write_io_ports(0x0B);
                break;
            case M5:
                ios = read_io_ports();
                if (!(ios & (1 << RTM_HOT_SWAP_PIN)))
                {
                    sens[RTM_HOT_SWAP_SENSOR].state &= 0xFD;
                    sens[RTM_HOT_SWAP_SENSOR].state |= HOT_SWAP_CLOSE_STATE;
                    ipmi_event_send(RTM_HOT_SWAP_SENSOR, HOT_SWAP_CLOSED);
                    RTM_M_Status = M2;
                }
                break;
            case M6:
                if (bit_is_set(RTMEN12VPIN, RTMEN12V))
                {
                    write_io_ports(0x0D);
                    RTM_M_Status = M1;
                }
                break;
            default:
                break;
            }
        }
    }
}


//******************************/
void rtm_set_power_level(u08 lev)
//******************************/
{
    mch_is_mtca4_comp = 1;

    if (lev)
    {
        rtm_power(RTM_12V, POWER_UP);
        RTM_M_Status = M3;
    }
    else
    {
        rtm_power(RTM_12V, POWER_DOWN);
        RTM_M_Status = M6;
    }
}


//***************************/
void rtm_set_led(u08 ledonoff)
//***************************/
{
    if (ledonoff == LED_ON)
    {
        write_io_ports(0x0D);
        timer2disable();
    }
    else if (ledonoff == LED_OFF)
    {
        write_io_ports(0x0F);
        timer2disable();
    }
    else
    {
        leds_blinking = 100;
        timer2enable();
    }
}


//***************/
void rtm_quiesce()
//***************/
{
    // RTM_M_Status = M6;
    // TODO some actions to prepare power down
}


//*************************************************/
static void rtm_power(u08 r_power, u08 on_off_power)
//*************************************************/
{
    switch (r_power)
    {
    case RTM_3V3:
        if (on_off_power == POWER_UP)
            cbi(RTMEN3V3PORT, RTMEN3V3);
        else
            sbi(RTMEN3V3PORT, RTMEN3V3);
        break;
    case RTM_12V:
        if (on_off_power == POWER_UP)
            cbi(RTMEN12VPORT, RTMEN12V);
        else
            sbi(RTMEN12VPORT, RTMEN12V);
        break;
    default:
        break;
    }
}


//***********************/
static u08 read_io_ports()
//***********************/
{
    u08 stemp;    // sensor temperature
    u08 addr_cmd; // temperature sensor command

    // write register address
    addr_cmd = (RTM_IO_PORTS_ADDR << 1) | 0x01; // I2C slave address + Read

    CRITICAL_SECTION_START;
    i2crtmReceive(addr_cmd, 0, 1, &stemp);
    CRITICAL_SECTION_END;

    return stemp;
}


//**********************************/
static void write_io_ports(u08 value)
//**********************************/
{
    u08 addr_cmd;

    // write register address
    addr_cmd = (RTM_IO_PORTS_ADDR << 1); // I2C slave address + WRITE

    CRITICAL_SECTION_START;
    i2crtmSend(addr_cmd, 0, 1, &value);
    CRITICAL_SECTION_END;
}


//******************************************/
u08 rtm_get_fru_inventory_area_info(u08* buf)
//******************************************/
{
    u08 len = 0;

    buf[len++] = FRU_SIZE & 0xff;
    buf[len++] = FRU_SIZE >> 8;
    buf[len++] = 0x00; //byte access type

    return len;
}


//*************************************************/
u08 rtm_fru_data_read(u08* area, u08 len, u08* data)
//*************************************************/
{
    u16 address;
    u08 addr_cmd;

    address = area[0] | (area[1] << 8);

    if (address >= FRU_SIZE)
        return(-1);                    //MJ: function is declared unsigned

    if (len > MAX_BYTES_READ)
        len = MAX_BYTES_READ;

    if ((address + len) > FRU_SIZE)
        len = FRU_SIZE - address;

    // write register address
    addr_cmd = (RTM_EEPROM_ADDR << 1); // I2C slave address + WRITE

    CRITICAL_SECTION_START;
    *data++ = len;
    i2crtmReceive(addr_cmd, address, len, data);
    CRITICAL_SECTION_END;

    return(len + 1);
}


//**************************************************/
u08 rtm_fru_data_write(u08* area, u08* data, u08 len)
//**************************************************/
{
    u16 address;
    u08 addr_cmd, rest_bytes;

    address = area[0] | (area[1] << 8);

    if (address >= FRU_SIZE)
        return(-1);      //MJ: function is declared unsigned

    if ((address + len) > FRU_SIZE)
        len = FRU_SIZE - address;

    // write register address
    addr_cmd = (RTM_EEPROM_ADDR << 1); // I2C slave address + WRITE

    rest_bytes = 32 - address % 32;

    CRITICAL_SECTION_START;
    if (rest_bytes < len)
    {
        i2crtmSend(addr_cmd, address, rest_bytes, data);
        i2crtmSend(addr_cmd, address + rest_bytes, len - rest_bytes, data);
    }
    else
        i2crtmSend(addr_cmd, address, len, data);
    CRITICAL_SECTION_END;

    return len;
}


//*********************/
static u08 checkcompat()
//*********************/
{
    u16 address, reclen;
    u08 addr_cmd, endoflist = 0, data[10];

    addr_cmd = (RTM_EEPROM_ADDR << 1);
    address = 0;

    CRITICAL_SECTION_START;
    i2crtmReceive(addr_cmd, address, 8, data);
    CRITICAL_SECTION_END;

    if (data[5] > 0 && data[5] != 0xff)
    {
        address = data[5] * 8;
        do
        {
            CRITICAL_SECTION_START;
            i2crtmReceive(addr_cmd, address, 10, data);
            CRITICAL_SECTION_END;

            endoflist = data[1] & 0x80;
            reclen = data[2] + 5;

            if ((data[0] == 0xc0) && (data[5] == 0x5A) && (data[6] == 0x31) && (data[8] == 0x30))
            {
                CRITICAL_SECTION_START;
                i2crtmReceive(addr_cmd, address + 10, 8, data);
                CRITICAL_SECTION_END;

                if ((data[0] == 0x03) && (data[1] == 0x3F) && (data[2] == 0x05) && (data[3] == 0x00) && (data[4] == 0)
                        && (data[5] == 0) && (data[6] == 0x02) && (data[7] == 0x08))
                    return(1);
            }

            address += reclen;
        }
        while (!endoflist);
    }

    return(0);
}


//*********************/
void rtm_leds_blinking()
//*********************/
{
    if (leds_blinking == 0)
        leds_blinking = 100;

    if (leds_blinking == 100)
        write_io_ports(0x0f);

    if (leds_blinking == 50)
        write_io_ports(0x0d);

    leds_blinking--;
}
