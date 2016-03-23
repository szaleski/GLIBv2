//*****************************************************************************
// Copyright (C) 2007 DESY(Deutsches-Elektronen Synchrotron) 
//
// File Name	: ipmi_if.c
// 
// Title		: IPMI interface
// Revision		: 1.0
// Notes		:	
// Target MCU	: Atmel AVR series
//
// Author       : Vahan Petrosyan (vahan_petrosyan@desy.de)
// Modified by  : Markus Joos (markus.joos@cern.ch)
//
// Description : The IPMI interface initialization and functions
//					
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************

#include <avr/io.h>
#include "project.h"
#include "timer.h"
#include "i2c.h"
#include "ipmi_if.h"
#include "sdr.h"
#include "fru.h"
#include "led.h"
#include "rtm_mng.h"
#include "user_code_select.h"


//Globals
extern sensor_t sens[NUM_SENSOR_AMC + NUM_SENSOR_RTM];
extern enum M_Status FRU_status;
extern enum M_Status RTM_M_Status;
extern int start_crate;                          // Variable declaration for snooping

volatile ipmi_status status_bits;
static LocalBuffer eventBuffer, readBuffer;
static ipmi_msg_t rqs;
static u08 rsp_data[25], rsp_data_len, rqSAddr, rqSeq, rsSAddr, seq_no, retry;
static u16 reservationID;
u08 endofsdr = 0, endoffru = 0, i = 0, event_receiver, ipmb_address;
char temp_net[80], temp_cmd[80];
u08 event_request_ok = 0;

//*************/
void ipmi_init()
//*************/
{
    i2cInit();                                  // initialize i2c function library
    ipmb_address = get_address();
    i2cSetLocalDeviceAddr(ipmb_address, TRUE);  // set local device address and allow response to general call
    i2cSetSlaveReceiveHandler(ipmb_get);        // slave operation
    event_receiver = IPMC_ADDRESS;              // set default event receiver address
    seq_no = 1;
    retry = 0;
}


//**************************************/
void ipmi_set_event_receiver(u08 address)    //Called from ipmi_response_send in this file
//**************************************/
{
    event_receiver = address;

    if (event_receiver == 0xff)
        event_request_ok = 0;
	else
        event_request_ok = 1;
    //set_led_pattern(1, 0x10 + event_request_ok, 1);
}   


//***********************/
u08 get_event_request_ok()
//***********************/
{
  return(event_request_ok);
}


//**********************/
void ipmi_check_request()
//**********************/
{
    u08 i, j;

    if (status_bits.ipmb_rqs == 0)
	    return;

    if (checsum_verify(readBuffer.Buffer, 2, readBuffer.Buffer[2]) != 0)                                             // first checksum
        return; //ignore request

    if ((checsum_verify(readBuffer.Buffer, readBuffer.Length - 1, readBuffer.Buffer[readBuffer.Length - 1])) != 0)   // second checksum
        return; //ignore request

    //fill request
    //For a definition of the format of the request see paragraph 2.11.1 of "Intelligent Platform Management Bus Communications Protocol Specification"

    rsSAddr = readBuffer.Buffer[0];
    rqs.netfn = readBuffer.Buffer[1] >> 2;  //Seems we are not interested in the bottom 2 bits of this byte which contain the responder's LUN
    rqSAddr = readBuffer.Buffer[3];         //Address of the requester (i.e. MCH)
    rqSeq = readBuffer.Buffer[4] >> 2;
    rqs.lun = readBuffer.Buffer[4] & 0x03;
    rqs.cmd = readBuffer.Buffer[5];

    if (rqs.netfn & 0x01)  //MJ: In this branch we handle responses to messages sent by the MMC (I guess this can only be event requests)
    {
        if ((rqSAddr == event_receiver) && (readBuffer.Buffer[6] == IPMI_CC_OK))  //Acknowledgement for an event request
        {
            status_bits.event_pending = 0;
            status_bits.ipmb_rqs = 0;
            retry = 0;
        }
        //MJ: In case of any other completion code (especially "IPMI_CC_NODE_BUSY") status_bits.event_pending and the message will be re-sent up to 3 times.
        return;
    }

    if (readBuffer.Length > 7)
    {
        rqs.data_len = readBuffer.Length - 7;
        for (i = 6, j = 0; i < readBuffer.Length - 1; i++, j++)
            rqs.data[j] = readBuffer.Buffer[i];
    }
    else
        rqs.data_len = 0;

    ipmi_response_send();
}


//**********************/
void ipmi_response_send()
//**********************/
{
    u08 user_error_code, error = IPMI_CC_OK;
    rsp_data_len = 0;

    temp_net[i] = rqs.netfn; // snooping of queries
    temp_cmd[i] = rqs.cmd;
    i++;

    if (i >= 79)
        i = 0;

    switch (rqs.netfn)
    {
    case IPMI_APP_NETFN:
        if ((rqs.cmd == IPMI_GET_DEVICE_ID_CMD) || (rqs.cmd == IPMI_BROADCAST_GET_DEVICE_ID_CMD))
            rsp_data_len = ipmi_get_device_id();
        else
            error = IPMI_CC_INV_CMD;
        break;

    case IPMI_SENSOR_EVENT_NETFN:
        switch (rqs.cmd)
        {
        case IPMI_SET_EVENT_RECEIVER_CMD:
            ipmi_set_event_receiver(rqs.data[0]);   //MJ: unclear: According to IPMI 1.5 there should be a second byte with the LUN. Why don't we need it?
            break;

        case IPMI_GET_EVENT_RECEIVER_CMD:
            rsp_data[0] = event_receiver;   //MJ: unclear: According to IPMI 1.5 there should be a second byte with the LUN. Why don't we need it?
            rsp_data_len = 1;
            break;

        case IPMI_GET_DEVICE_SDR_INFO_CMD:
            if (rsp_data_len > 0)
                rsp_data_len = ipmi_sdr_info(rsp_data, rqs.data[0]);
            else
                rsp_data_len = ipmi_sdr_info(rsp_data, 0);
            break;

        case IPMI_GET_DEVICE_SDR_CMD:
            if (rqs.data[4] != 0)
            {
                if ((rqs.data[0] != (reservationID & 0xff)) || (rqs.data[1] != (reservationID >> 8)))
                    error = IPMI_CC_RES_CANCELED;
                else if ((rsp_data_len = ipmi_sdr_get(&rqs.data[2], rqs.data[4], rqs.data[5], rsp_data)) == 0xff)
                    error = IPMI_CC_PARAM_OUT_OF_RANGE;
                break;
            }
            if ((rsp_data_len = ipmi_sdr_get(&rqs.data[2], rqs.data[4], rqs.data[5], rsp_data)) == 0xff)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            else
                endofsdr = 1; // activate SDR flag on last query
            break;

        case IPMI_GET_SENSOR_THRESHOLD_CMD:             //For data format see IPMI1.5, Table 29-9
            if ((rsp_data_len = ipmi_get_sensor_threshold(rqs.data[0], rsp_data)) == 0xff)
                error = IPMI_CC_ILL_SENSOR_OR_RECORD;
            break;

        case IPMI_SET_SENSOR_THRESHOLD_CMD:             //For data format see IPMI1.5, Table 29-8
            if ((rsp_data_len = ipmi_set_sensor_threshold(rqs.data[0], rqs.data[1], rqs.data[2], rqs.data[3], rqs.data[4], rqs.data[5], rqs.data[6], rqs.data[7])) == 0xff)
                error = IPMI_CC_ILL_SENSOR_OR_RECORD;
            break;

        case IPMI_RESERVE_DEVICE_SDR_REPOSITORY_CMD:
            reservationID++;
            if (reservationID == 0)
                reservationID = 1;
            rsp_data[0] = reservationID & 0xff;
            rsp_data[1] = reservationID >> 8;
            rsp_data_len = 2;
            break;

        case IPMI_GET_SENSOR_READING_CMD:
            if ((rsp_data_len = ipmi_get_sensor_reading(rqs.data[0], rsp_data)) == 0xff)
                error = IPMI_CC_ILL_SENSOR_OR_RECORD;
            //	else start_crate = 0x01; // waiting initialisation NAT board
            break;

        default:
            error = IPMI_CC_INV_CMD;
            break;
        }
        break;

    case IPMI_STORAGE_NETFN:
        switch (rqs.cmd)
        {
        case IPMI_GET_FRU_INVENTORY_AREA_INFO_CMD:
            if(rqs.data[0] == 1)  //RTM
               rsp_data_len = rtm_get_fru_inventory_area_info(rsp_data);
            else
               rsp_data_len = ipmi_get_fru_inventory_area_info(rsp_data);
            break;

        case IPMI_READ_FRU_DATA_CMD:
            if(rqs.data[0] == 1)   //RTM
               rsp_data_len = rtm_fru_data_read(&rqs.data[1], rqs.data[3], rsp_data);
            else
            {
                if ((rsp_data_len = ipmi_fru_data_read(&rqs.data[1], rqs.data[3], rsp_data)) == 0xff)
                    error = IPMI_CC_PARAM_OUT_OF_RANGE;
                else
                    endoffru = 1; // activate Read FRU flag on last query
            }
            break;

        case IPMI_WRITE_FRU_DATA_CMD:
            if(rqs.data[0] == 1)
                rsp_data_len = rtm_fru_data_write(&rqs.data[1], &rqs.data[3], rqs.data_len - 3);
            else
                rsp_data_len = ipmi_fru_data_write(&rqs.data[1], &rqs.data[3], rqs.data_len - 3);
            if(rsp_data_len == 0xff)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            break;

        default:
            error = IPMI_CC_INV_CMD;
            break;
        }
        break;

    case IPMI_GROUP_EXTENSION_NETFN + IPMI_PICMG_GRP_EXT:
        switch (rqs.cmd)
        {
        case IPMI_PICMG_CMD_GET_PROPERTIES:
            //set_led_pattern(0, 0xff, 2);
            rsp_data_len = ipmi_picmg_properties(rsp_data);
            break;

        case IPMI_PICMG_CMD_FRU_CONTROL:
            if (rqs.data[1] == 1 && rqs.data[2] == FRU_QUIESCE)   //MTCA.4, 3.5.2-6 OK
            {
                ipmb_send_rsp(error);
#ifdef USE_RTM
                RTM_M_Status = M6;
#endif
                return;
            }
            else
            {
                if ((ipmi_picmg_fru_control(rqs.data[2])) != 0xff)
                {
                    if (rqs.data[2] == FRU_QUIESCE)
                    {
                        ipmb_send_rsp(error);
#ifdef USE_RTM
                        if (RTM_M_Status > M1)
                        {
                            rtm_quiesce();              //MJ: Should this be done here or in the state machine in rtm_mng.c?
                            rtm_set_power_level(0);
                        }
#endif						
                        ipmi_picmg_fru_control(POWER_DOWN);
                        ipmi_event_send(HOT_SWAP_SENSOR, HOT_SWAP_QUIESCED);
                        FRU_status = M1;
                        return;
                    }
                }
                else
                    error = IPMI_CC_INV_DATA_FIELD_IN_REQ;
            }
            break;

        case IPMI_PICMG_CMD_GET_FRU_LED_PROPERTIES:
            rsp_data_len = ipmi_get_fru_led_properties(rqs.data[1], rsp_data);
            break;

        case IPMI_PICMG_CMD_GET_LED_COLOR_CAPABILITIES:
            if ((rsp_data_len = ipmi_get_led_color_capabilities(rqs.data[1], rqs.data[2], rsp_data)) == 0xff)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            break;

        case IPMI_PICMG_CMD_SET_FRU_LED_STATE:  //Defined in Table 3-31 of PICMG 3.0 R3.0
            if (rqs.data[1] == 0) //This is the FRU number. 0=AMC, 1=RTM
            {
                if (rqs.data[2] == BLUE_LED)
                {
                    if (rqs.data[3] == LED_OFF)
                    {
                        if ((FRU_status != M3) && (FRU_status != M4))
                            FRU_status = M3;
                    }
                    if (rqs.data[3] == LED_ON)
                        FRU_status = M1;
                }
            }
            //set_led_pattern(0, 0x81, 1);

            if ((rsp_data_len = ipmi_set_fru_led_state(rqs.data[1], rqs.data[2], rqs.data[3], rqs.data[4])) == 0xff)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            break;
        case IPMI_PICMG_CMD_GET_FRU_LED_STATE:
            if ((rsp_data_len = ipmi_get_fru_led_state(rqs.data[1], rqs.data[2], rsp_data)) == 0xff)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            break;

        case IPMI_PICMG_CMD_GET_DEVICE_LOCATOR_RECORD:
            if (rqs.data[2] != 0)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
            else
                rsp_data_len = ipmi_picmg_get_device_locator(rsp_data);
            break;

        case IPMI_PICMG_CMD_SET_AMC_PORT_STATE:
            rsp_data_len = ipmi_picmg_port_state_set(0, 0);
            break;

        case IPMI_PICMG_CMD_GET_AMC_PORT_STATE:
            rsp_data_len = ipmi_picmg_port_state_get(rqs.data[0], rsp_data);
            break;

        case IPMI_PICMG_CMD_SET_POWER_LEVEL:  //MTCA.4, 3.5.1-16 or 3.5.2-9 OK
            if (rqs.data[1] == 1)   //We are only interested in FRU1 (RTM) //MJ: Do we not have to care about FRU0 (AMC)?
                rtm_set_power_level(rqs.data[2]);
            break;

        default:
            error = IPMI_CC_INV_CMD;
        }
        break;

    case IPMI_OEM_GROUP_NETFN:
        // For information on how OEM commands work see:
        // table 5-1 of the IPMI 1.5 Spec
        // Example: http://docs.oracle.com/cd/E19536-01/820-0457-11/oemipmi.html

        //We pass all OEM commands directly to the user code. Therefore there is no switch{} in this section
        rsp_data_len = ipmi_oem_user(rqs.cmd, &rqs.data[0], &rqs.data[3], rqs.data_len - 3, rsp_data, &user_error_code);
        error = user_error_code;
        break;

    case IPMI_CONTROLLER_SPECIFIC:
        if (rqs.cmd == FRU_PROM_REVISION_CMD)
        {
            if (ipmi_prom_version_change(rqs.data[0]) != 0)
                error = IPMI_CC_PARAM_OUT_OF_RANGE;
        }
        else if (rqs.cmd == JTAG_CTRL_SET_CMD)
            ipmi_jtag_ctrl(rqs.data[0]);
        else if (rqs.cmd == FPGA_JTAG_PLR_CMD)
            ipmi_fpga_jtag_plr_set(rqs.data[0]);
        else
            error = IPMI_CC_INV_CMD;
        break;

    default:
        error = IPMI_CC_INV_CMD;
        break;
    }

    if (endofsdr == 1 && endoffru == 1)
        start_crate = 1; // Both flags activated : build Atart_crate flag

    ipmb_send_rsp(error);
}


//****************************/
void ipmi_check_event_respond()    //Called from the main
//****************************/
{
    if (status_bits.event_pending == 0)
        return;

    if (retry < 3)
    {
        ipmb_send_event();    //MJ: Unclear: Are we sending the same event 3 times? The retry does not seem to depend on the success or failure of sending the event
        retry++;
    }
    else
    {
        status_bits.event_pending = 0;
        retry = 0;
    }
}


//*********************/
u08 ipmi_get_device_id()  //Called from ipmi_response_send in this file
//*********************/
{
    u08 len = 0;

    rsp_data[len++] = 0x00;                            // Device ID
    rsp_data[len++] = 0x80;                            // Device revision and SDRs
    rsp_data[len++] = MMC_FW_REL_MAJ;                  // Major Firmware revision
    rsp_data[len++] = MMC_FW_REL_MIN;                  // Minor Firmware revision
    rsp_data[len++] = MMC_IPMI_REL;                    // IPMI version 1.5
    rsp_data[len++] = IPMI_MSG_ADD_DEV_SUPP;           // Additional device support(commands and functions)
    rsp_data[len++] = IPMI_MSG_MANU_ID_LSB;            // Manufacturer ID LSB
    rsp_data[len++] = IPMI_MSG_MANU_ID_B2;
    rsp_data[len++] = IPMI_MSG_MANU_ID_MSB;
    rsp_data[len++] = IPMI_MSG_PROD_ID_LSB;            // Product ID LSB
    rsp_data[len++] = IPMI_MSG_PROD_ID_MSB;

    return len;
}


//***********************************/
u08 checksum_clc(u08* buf, u08 length)
//***********************************/
{
    u08 crc = 0x00;

    while (length--)
        crc += buf[length];

    return (0 - crc);
}


//***************************************************/
u08 checsum_verify(u08* buf, u08 length, u08 checksum)
//***************************************************/
{
    return (checksum - checksum_clc(buf, length));
}


//get message from ipmb 
//***************************************************/
void ipmb_get(u08 receiveDataLength, u08* receiveData)  //Installed as asynchronous handler in ipmi_init in this file
//***************************************************/
{
    u08 i;

    status_bits.connected = 1;

    if (status_bits.ipmb_rqs == 1)
        return;

    for (i = 0; i < receiveDataLength; i++)   // copy the received data to a local buffer
        readBuffer.Buffer[i] = *receiveData++;

    readBuffer.Length = receiveDataLength;
    status_bits.ipmb_rqs = 1;
}


//master write message
//*****************************/
void ipmb_send_rsp(u08 cc_error)
//*****************************/
{
    LocalBuffer writeBuffer;
    u08 i = 0, j;

    writeBuffer.Buffer[i++] = rqSAddr;
    writeBuffer.Buffer[i++] = ((rqs.netfn + 1) << 2) | (rqs.lun & 0x03);
    writeBuffer.Buffer[i++] = checksum_clc(writeBuffer.Buffer, 2);
    writeBuffer.Buffer[i++] = rsSAddr;
    writeBuffer.Buffer[i++] = (rqSeq << 2);
    writeBuffer.Buffer[i++] = rqs.cmd;
    writeBuffer.Buffer[i++] = cc_error;

    if (rqs.netfn == IPMI_GROUP_EXTENSION_NETFN)
        writeBuffer.Buffer[i++] = IPMI_PICMG_GRP_EXT;

    if (!cc_error && (rsp_data_len > 0))
    {
        for (j = 0; j < rsp_data_len; j++)
            writeBuffer.Buffer[i++] = rsp_data[j];
    }

    writeBuffer.Buffer[i] = checksum_clc(writeBuffer.Buffer, i);
    i2cMasterSend(rqSAddr, i, &writeBuffer.Buffer[1]);
    status_bits.ipmb_rqs = 0;
}


//send event message
//************************************************/
void ipmi_event_send(u08 sensor_number, u08 evData)   //Called from multiple places
//************************************************/
{
    if (status_bits.event_pending == 1)    //If the previous message has not yet gone out we refuse to accept another one. MJ: What are the consequences?
        return;

    //IPMI message header
    eventBuffer.Buffer[0] = event_receiver;
    eventBuffer.Buffer[1] = IPMI_SENSOR_EVENT_NETFN << 2;       // send netfn
    eventBuffer.Buffer[2] = checksum_clc(eventBuffer.Buffer, 2);
    eventBuffer.Buffer[3] = ipmb_address;
    eventBuffer.Buffer[4] = (++seq_no << 2);
    eventBuffer.Buffer[5] = IPMI_PLATFORM_EVENT_CMD;
    //IPMI event request message. See table 23-5 of IPMI 1.5
    eventBuffer.Buffer[6] = 0x04;                               // Event Message revision
    eventBuffer.Buffer[7] = sens[sensor_number].type;
    eventBuffer.Buffer[8] = sensor_number;
    eventBuffer.Buffer[9] = sens[sensor_number].event_dir;
    eventBuffer.Buffer[10] = evData;
    eventBuffer.Buffer[11] = 0xff;
    eventBuffer.Buffer[12] = 0xff;
    eventBuffer.Buffer[13] = checksum_clc(eventBuffer.Buffer, 13);

    i2cMasterSend(event_receiver, 13, &eventBuffer.Buffer[1]);

    status_bits.event_pending = 1;  //Pending does not mean "message not yet sent" but "response not yet received"
}


//*******************/
void ipmb_send_event()    //Called from ipmi_check_event_respond in this file
//*******************/
{
    i2cMasterSend(event_receiver, 13, &eventBuffer.Buffer[1]);
}


//get IPMB_L address 
//**************/
u08 get_address()
//**************/
{
    u08 geog_addr = 0, GAddr1 = 0, GAddr2 = 0, GAddr3 = 0;

    // configure GAx as high impedance input
    cbi(GA0_DDR, GA0_PORT_PIN);
    cbi(GA1_DDR, GA1_PORT_PIN);
    cbi(GA2_DDR, GA2_PORT_PIN);

    sbi(GAPU_DDR, GAPU_PORT_PIN);      // configure GA pull-up as low output
    cbi(GAPU_PORT, GAPU_PORT_PIN);     // set pin to low
    SFIOR |= 0x04;                     // disable internal pull-up resistors
    GAPU_PORT &= ~GAPU_PORT_PIN;       // set Pullup to low
    delay(100);                        // give Pin a chance to change

    GAddr1 = (inb(GA0_PORT) & BV(GA0_PORT_PIN)) ? 1 : 0;
    GAddr2 = (inb(GA1_PORT) & BV(GA1_PORT_PIN)) ? 1 : 0;
    GAddr3 = (inb(GA2_PORT) & BV(GA2_PORT_PIN)) ? 1 : 0;

    sbi(GAPU_PORT, GAPU_PORT_PIN);     // set pin to high
    delay(100);

    //read lines and compare
    if (GAddr1 != ((inb(GA0_PORT) & BV(GA0_PORT_PIN)) ? 1 : 0))
        GAddr1 = UNCONNECTED;
    if (GAddr2 != ((inb(GA1_PORT) & BV(GA1_PORT_PIN)) ? 1 : 0))
        GAddr2 = UNCONNECTED;
    if (GAddr3 != ((inb(GA2_PORT) & BV(GA2_PORT_PIN)) ? 1 : 0))
        GAddr3 = UNCONNECTED;

    switch (GAddr1)
    {
    case GROUNDED:
    {
        switch (GAddr2)
        {
        case GROUNDED:
        {
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x72;
                break;
            }
            else
                break;
        }
        case UNCONNECTED:
        {
            if (GAddr3 == GROUNDED)
            {
                geog_addr = 0x74;
                break;
            }
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x76;
                break;
            }
            else
                break;
        }
        default:
            break;
        }
        break;
    }
    case UNCONNECTED:
    {
        switch (GAddr2)
        {
        case GROUNDED:
        {
            if (GAddr3 == GROUNDED)
            {
                geog_addr = 0x78;
                break;
            }
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x7A;
                break;
            }
            else
                break;
        }
        case UNCONNECTED:
        {
            if (GAddr3 == GROUNDED)
            {
                geog_addr = 0x7C;
                break;
            }
            if (GAddr3 == POWERED)
            {
                geog_addr = 0x7E;
                break;
            }
            else
                break;
        }
        case POWERED:
        {
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x80;
                break;
            }
            if (GAddr3 == POWERED)
            {
                geog_addr = 0x82;
                break;
            }
            else
                break;
        }
        default:
            break;
        }
        break;
    }
    case POWERED:
    {
        switch (GAddr2)
        {
        case UNCONNECTED:
        {
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x84;
                break;
            }
            if (GAddr3 == POWERED)
            {
                geog_addr = 0x86;
                break;
            }
            else
                break;
        }
        case POWERED:
        {
            if (GAddr3 == UNCONNECTED)
            {
                geog_addr = 0x88;
                break;
            }
            else
                break;
        }
        default:
            break;
        }
        break;
    }

    default:
        break;
    }

    SFIOR &= ~0x04;  // re-enable internal pull-up resistors
    return(geog_addr ? geog_addr : 0x70);
}

