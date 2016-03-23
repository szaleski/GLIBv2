/*! \file rtm_mng.h \brief RTM management. */
//*****************************************************************************
//
// File Name	: 'rtm_mng.h'
// Title		: main management functions for RTM module
// Author		: Vahan Petrosyan
// Created		: 10/19/2010
// Target MCU	: Atmel AVR series
//*****************************************************************************



#ifndef RTM_MNG_H
#define RTM_MNG_H

#define HOT_SWAP_CLOSE_STATE     0x01
#define HOT_SWAP_OPEN_STATE     0x02
#define HOT_SWAP_QUISCED_STATE     0x04
#define RTM_PRESENT     0x20
#define RTM_ABSENT      0x40
#define RTM_COMPATIBLE  0x80
#define RTM_INCOMPATIBLE 0x81

#define RTM_HS_EVENT_PRESENT 0x05
#define RTM_HS_EVENT_ABSENT  0x06
#define RTM_HS_EVENT_COMPAT  0x07
#define RTM_HS_EVENT_INCOMPAT 0x08

#define RTM_3V3         0x01
#define RTM_12V         0x02

// pins on I/O Extender
#define RTM_HOT_SWAP_PIN 0
#define RTM_BLUE_LED_PIN 1
#define RTM_RED_LED_PIN  2
#define RTM_GREEN_LED_PIN    3
#define RTM_POWER_GOOD_PIN 4
#define RTM_RESET_PIN      5
#define RTM_ENABLE_PIN     6
#define RTM_EEPROM_WRITE_PR_PIN 7



//check module state
void check_rtm_state();

u08 rtm_get_fru_inventory_area_info( u08* buf );
u08 rtm_fru_data_read( u08* area, u08 len, u08* data );
u08 rtm_fru_data_write( u08* area,u08* data, u08 len );
void rtm_set_power_level(u08 lev);
void rtm_quiesce();

void rtm_leds_blinking();
void rtm_set_led(u08 ledonoff);

#endif
