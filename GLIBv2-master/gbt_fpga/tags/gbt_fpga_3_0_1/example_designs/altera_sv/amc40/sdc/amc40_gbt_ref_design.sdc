## Generated SDC file "M:/svn_repositories/GBT_project/svn_work/trunk/example_designs/altera_sv/amc40/sdc/amc40_gbt_ref_design.sdc"

## Copyright (C) 1991-2013 Altera Corporation
## Your use of Altera Corporation's design tools, logic functions 
## and other software and tools, and its AMPP partner logic 
## functions, and any output files from any of the foregoing 
## (including device programming or simulation files), and any 
## associated documentation or information are expressly subject 
## to the terms and conditions of the Altera Program License 
## Subscription Agreement, Altera MegaCore Function License 
## Agreement, or other applicable license agreement, including, 
## without limitation, that your use is for the sole purpose of 
## programming logic devices manufactured by Altera and sold by 
## Altera or its authorized distributors.  Please refer to the 
## applicable agreement for further details.


## VENDOR  "Altera"
## PROGRAM "Quartus II"
## VERSION "Version 13.1.0 Build 162 10/23/2013 SJ Full Version"

## DATE    "Thu Mar 27 13:32:54 2014"

##
## DEVICE  "5SGXEA7N2F45C3"
##


#**************************************************************
# Time Information
#**************************************************************

set_time_format -unit ns -decimal_places 3

#**************************************************************
# Create Clock
#**************************************************************

create_clock -name {REF_CLOCK_L4}        -period 8.333 -waveform { 0.000 4.166 } [get_ports {REF_CLOCK_L4}]
create_clock -name {REF_CLOCK_R5}        -period 8.000 -waveform { 0.000 4.000 } [get_ports {REF_CLOCK_R5}]

create_clock -name {GBTBANK1_RXRECCLK_1} -period 8.333 -waveform { 0.000 4.166 } [get_pins {*|gbtBank_1|*|*|*|*|gen_native_inst.xcvr_native_insts[0].gen_bonded_group_native.xcvr_native_inst|inst_sv_pcs|ch[0].inst_sv_pcs_ch|inst_sv_hssi_8g_rx_pcs|wys|rcvdclkpma}]

create_clock -name {GBTBANK2_RXRECCLK_1} -period 8.333 -waveform { 0.000 4.166 } [get_pins {*|gbtBank_2|*|*|*|*|gen_native_inst.xcvr_native_insts[0].gen_bonded_group_native.xcvr_native_inst|inst_sv_pcs|ch[0].inst_sv_pcs_ch|inst_sv_hssi_8g_rx_pcs|wys|rcvdclkpma}]
create_clock -name {GBTBANK2_RXRECCLK_2} -period 8.333 -waveform { 0.000 4.166 } [get_pins {*|gbtBank_2|*|*|*|*|gen_native_inst.xcvr_native_insts[0].gen_bonded_group_native.xcvr_native_inst|inst_sv_pcs|ch[1].inst_sv_pcs_ch|inst_sv_hssi_8g_rx_pcs|wys|rcvdclkpma}]
create_clock -name {GBTBANK2_RXRECCLK_3} -period 8.333 -waveform { 0.000 4.166 } [get_pins {*|gbtBank_2|*|*|*|*|gen_native_inst.xcvr_native_insts[0].gen_bonded_group_native.xcvr_native_inst|inst_sv_pcs|ch[2].inst_sv_pcs_ch|inst_sv_hssi_8g_rx_pcs|wys|rcvdclkpma}]

create_clock -name {altera_reserved_tck} -period 33.333 -waveform { 0.000 16.666 } [get_ports {altera_reserved_tck}]

#**************************************************************
# Create Generated Clock
#**************************************************************

derive_pll_clocks

#**************************************************************
# Set Clock Latency
#**************************************************************

#**************************************************************
# Set Clock Uncertainty
#**************************************************************

set_clock_uncertainty -rise_from [get_clocks {REF_CLOCK_L4}]        -rise_to [get_clocks {REF_CLOCK_L4}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {REF_CLOCK_L4}]        -fall_to [get_clocks {REF_CLOCK_L4}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {REF_CLOCK_L4}]        -rise_to [get_clocks {REF_CLOCK_L4}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {REF_CLOCK_L4}]        -fall_to [get_clocks {REF_CLOCK_L4}] 0.080 
set_clock_uncertainty -rise_from [get_clocks {REF_CLOCK_R5}]        -rise_to [get_clocks {REF_CLOCK_R5}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {REF_CLOCK_R5}]        -fall_to [get_clocks {REF_CLOCK_R5}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {REF_CLOCK_R5}]        -rise_to [get_clocks {REF_CLOCK_R5}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {REF_CLOCK_R5}]        -fall_to [get_clocks {REF_CLOCK_R5}] 0.080  

set_clock_uncertainty -rise_from [get_clocks {GBTBANK1_RXRECCLK_1}] -rise_to [get_clocks {GBTBANK1_RXRECCLK_1}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {GBTBANK1_RXRECCLK_1}] -fall_to [get_clocks {GBTBANK1_RXRECCLK_1}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK1_RXRECCLK_1}] -rise_to [get_clocks {GBTBANK1_RXRECCLK_1}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK1_RXRECCLK_1}] -fall_to [get_clocks {GBTBANK1_RXRECCLK_1}] 0.080

set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_1}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_1}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_1}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_1}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_1}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_1}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_1}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_1}] 0.080
set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_2}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_2}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_2}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_2}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_2}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_2}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_2}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_2}] 0.080
set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_3}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_3}] 0.080  
set_clock_uncertainty -rise_from [get_clocks {GBTBANK2_RXRECCLK_3}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_3}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_3}] -rise_to [get_clocks {GBTBANK2_RXRECCLK_3}] 0.080  
set_clock_uncertainty -fall_from [get_clocks {GBTBANK2_RXRECCLK_3}] -fall_to [get_clocks {GBTBANK2_RXRECCLK_3}] 0.080

#**************************************************************
# Set Input Delay
#**************************************************************

#**************************************************************
# Set Output Delay
#**************************************************************

#**************************************************************
# Set Clock Groups
#**************************************************************

set_clock_groups -asynchronous -group [get_clocks {REF_CLOCK_L4}] 
set_clock_groups -asynchronous -group [get_clocks {REF_CLOCK_R5}] 

set_clock_groups -asynchronous -group [get_clocks {GBTBANK1_RXRECCLK_1}] 

set_clock_groups -asynchronous -group [get_clocks {GBTBANK2_RXRECCLK_1}] 
set_clock_groups -asynchronous -group [get_clocks {GBTBANK2_RXRECCLK_2}] 
set_clock_groups -asynchronous -group [get_clocks {GBTBANK2_RXRECCLK_3}] 

set_clock_groups -asynchronous -group [get_clocks {altera_reserved_tck}] 

#**************************************************************
# Set False Path
#**************************************************************

set_false_path -to [get_ports {AMC_CLOCK_OUT}]
set_false_path -to [get_ports {AMC_CLOCK_OUT(n)}]
set_false_path -to [get_ports {RTM_CLOCK_OUT_P}]
set_false_path -to [get_ports {RTM_CLOCK_OUT_N}]
set_false_path -to [get_ports {RTM_ORBIT_OUT_P}]
set_false_path -to [get_ports {RTM_ORBIT_OUT_N}]
set_false_path -to [get_ports {THROTTLE_OUT_P}]
set_false_path -to [get_ports {THROTTLE_OUT_N}]
set_false_path -to [get_ports {RTM_TX_0_P}]
set_false_path -to [get_ports {RTM_TX_1_P}]
set_false_path -to [get_ports {RTM_TX_1_N}]
set_false_path -to [get_ports {RTM_TX_2_P}]
set_false_path -to [get_ports {RTM_TX_2_N}]
set_false_path -to [get_ports {RTM_TX_3_P}]
set_false_path -to [get_ports {RTM_TX_3_N}]
set_false_path -to [get_ports {altera_reserved_tdo}]

set_false_path -to [get_keepers {alt_sv_gbt_example_design:gbtExmplDsgn|alt_sv_mgt_txpll:gbtBank_2_mgtTxPll|alt_sv_gt_txpll:gtTxPll|sv_xcvr_plls:alt_sv_gt_txpll_inst|pll[0].avmm.stratixv_hssi_avmm_interface_inst~SYNC_DATA_REG*}]
set_false_path -to [get_registers {*alt_xcvr_resync*sync_r[0]}]
set_false_path -to [get_pins -nocase -compatibility_mode {*|alt_rst_sync_uq1|altera_reset_synchronizer_int_chain*|clrn}]

set_false_path -from [get_ports {altera_reserved_tdi}]
set_false_path -from [get_ports {altera_reserved_tms}]
set_false_path -hold -from [get_keepers {*|alt_xcvr_reconfig_basic:basic|sv_xcvr_reconfig_basic:s5|pif_interface_sel}] 

#**************************************************************
# Set Multicycle Path
#**************************************************************

set_multicycle_path -hold -end -from [get_clocks {fabricPll|alt_sv_fabric_pll_inst|altera_pll_i|general[0].gpll~PLL_OUTPUT_COUNTER|divclk}] -to [get_keepers {alt_sv_gbt_example_design:gbtExmplDsgn|gbt_bank:gbtBank_2|multi_gigabit_transceivers:mgt|mgt_latopt:\mgtLatOpt_gen:mgtLatOpt|alt_sv_mgt_latopt_txwordclkmon:\txWordClkMon_gen:txWordClkMon|sampledClk_from_txWordClkReg}] 1

#**************************************************************
# Set Maximum Delay
#**************************************************************

set_max_delay -from [get_registers {gbtExmplDsgn|gbtBank_1|\gbtRx_gen:1:gbtRx|rxGearbox|*}] -to [get_registers {gbtExmplDsgn|gbtBank_1|\gbtRx_gen:1:gbtRx|descrambler|*}] 20.000

set_max_delay -from [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:1:gbtTx|scrambler|*}] -to [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:1:gbtTx|txGearbox|*}] 20.000
set_max_delay -from [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:2:gbtTx|scrambler|*}] -to [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:2:gbtTx|txGearbox|*}] 20.000
set_max_delay -from [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:3:gbtTx|scrambler|*}] -to [get_registers {gbtExmplDsgn|gbtBank_2|\gbtTx_gen:3:gbtTx|txGearbox|*}] 20.000

#**************************************************************
# Set Minimum Delay
#**************************************************************

#**************************************************************
# Set Input Transition
#**************************************************************

#**************************************************************
# Set Max Skew
#**************************************************************

set_max_skew -from [get_clocks *] -to [get_clocks { gbtExmplDsgn|gbtBank_1_mgtTxPll|gtTxPll|alt_sv_gt_txpll_inst|pll[0].pll.atx_pll.tx_pll|clk010g }] 1.000 
