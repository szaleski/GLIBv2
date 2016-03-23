-- megafunction wizard: %Transceiver Reconfiguration Controller v13.1%
-- GENERATION: XML
-- alt_sv_gt_reconfctrl_x2.vhd

-- Generated using ACDS version 13.1 162 at 2014.04.06.21:54:04

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity alt_sv_gt_reconfctrl_x2 is
	generic (
		device_family                 : string  := "Stratix V";
		number_of_reconfig_interfaces : integer := 3;
		enable_offset                 : integer := 1;
		enable_lc                     : integer := 1;
		enable_dcd                    : integer := 0;
		enable_dcd_power_up           : integer := 1;
		enable_analog                 : integer := 1;
		enable_eyemon                 : integer := 0;
		enable_ber                    : integer := 0;
		enable_dfe                    : integer := 0;
		enable_adce                   : integer := 0;
		enable_mif                    : integer := 0;
		enable_pll                    : integer := 0
	);
	port (
		reconfig_busy             : out std_logic;                                         --   reconfig_busy.reconfig_busy
		mgmt_clk_clk              : in  std_logic                      := '0';             --    mgmt_clk_clk.clk
		mgmt_rst_reset            : in  std_logic                      := '0';             --  mgmt_rst_reset.reset
		reconfig_mgmt_address     : in  std_logic_vector(6 downto 0)   := (others => '0'); --   reconfig_mgmt.address
		reconfig_mgmt_read        : in  std_logic                      := '0';             --                .read
		reconfig_mgmt_readdata    : out std_logic_vector(31 downto 0);                     --                .readdata
		reconfig_mgmt_waitrequest : out std_logic;                                         --                .waitrequest
		reconfig_mgmt_write       : in  std_logic                      := '0';             --                .write
		reconfig_mgmt_writedata   : in  std_logic_vector(31 downto 0)  := (others => '0'); --                .writedata
		ch0_1_to_xcvr             : out std_logic_vector(139 downto 0);                    --   ch0_1_to_xcvr.reconfig_to_xcvr
		ch0_1_from_xcvr           : in  std_logic_vector(91 downto 0)  := (others => '0'); -- ch0_1_from_xcvr.reconfig_from_xcvr
		ch2_2_to_xcvr             : out std_logic_vector(69 downto 0);                     --   ch2_2_to_xcvr.reconfig_to_xcvr
		ch2_2_from_xcvr           : in  std_logic_vector(45 downto 0)  := (others => '0')  -- ch2_2_from_xcvr.reconfig_from_xcvr
	);
end entity alt_sv_gt_reconfctrl_x2;

architecture rtl of alt_sv_gt_reconfctrl_x2 is
	component alt_xcvr_reconfig is
		generic (
			device_family                 : string  := "Stratix V";
			number_of_reconfig_interfaces : integer := 2;
			enable_offset                 : integer := 1;
			enable_lc                     : integer := 1;
			enable_dcd                    : integer := 0;
			enable_dcd_power_up           : integer := 1;
			enable_analog                 : integer := 1;
			enable_eyemon                 : integer := 0;
			enable_ber                    : integer := 0;
			enable_dfe                    : integer := 0;
			enable_adce                   : integer := 0;
			enable_mif                    : integer := 0;
			enable_pll                    : integer := 0
		);
		port (
			reconfig_busy             : out std_logic;                                         -- reconfig_busy
			mgmt_clk_clk              : in  std_logic                      := 'X';             -- clk
			mgmt_rst_reset            : in  std_logic                      := 'X';             -- reset
			reconfig_mgmt_address     : in  std_logic_vector(6 downto 0)   := (others => 'X'); -- address
			reconfig_mgmt_read        : in  std_logic                      := 'X';             -- read
			reconfig_mgmt_readdata    : out std_logic_vector(31 downto 0);                     -- readdata
			reconfig_mgmt_waitrequest : out std_logic;                                         -- waitrequest
			reconfig_mgmt_write       : in  std_logic                      := 'X';             -- write
			reconfig_mgmt_writedata   : in  std_logic_vector(31 downto 0)  := (others => 'X'); -- writedata
			reconfig_to_xcvr          : out std_logic_vector(209 downto 0);                    -- reconfig_to_xcvr
			reconfig_from_xcvr        : in  std_logic_vector(137 downto 0) := (others => 'X'); -- reconfig_from_xcvr
			tx_cal_busy               : out std_logic;                                         -- tx_cal_busy
			rx_cal_busy               : out std_logic;                                         -- tx_cal_busy
			cal_busy_in               : in  std_logic                      := 'X';             -- cal_busy_in
			reconfig_mif_address      : out std_logic_vector(31 downto 0);                     -- address
			reconfig_mif_read         : out std_logic;                                         -- read
			reconfig_mif_readdata     : in  std_logic_vector(15 downto 0)  := (others => 'X'); -- readdata
			reconfig_mif_waitrequest  : in  std_logic                      := 'X'              -- waitrequest
		);
	end component alt_xcvr_reconfig;

	signal alt_sv_gt_reconfctrl_x2_inst_reconfig_to_xcvr : std_logic_vector(209 downto 0); -- port fragment

begin

	alt_sv_gt_reconfctrl_x2_inst : component alt_xcvr_reconfig
		generic map (
			device_family                 => device_family,
			number_of_reconfig_interfaces => number_of_reconfig_interfaces,
			enable_offset                 => enable_offset,
			enable_lc                     => enable_lc,
			enable_dcd                    => enable_dcd,
			enable_dcd_power_up           => enable_dcd_power_up,
			enable_analog                 => enable_analog,
			enable_eyemon                 => enable_eyemon,
			enable_ber                    => enable_ber,
			enable_dfe                    => enable_dfe,
			enable_adce                   => enable_adce,
			enable_mif                    => enable_mif,
			enable_pll                    => enable_pll
		)
		port map (
			reconfig_busy                     => reconfig_busy,                                 --   reconfig_busy.reconfig_busy
			mgmt_clk_clk                      => mgmt_clk_clk,                                  --    mgmt_clk_clk.clk
			mgmt_rst_reset                    => mgmt_rst_reset,                                --  mgmt_rst_reset.reset
			reconfig_mgmt_address             => reconfig_mgmt_address,                         --   reconfig_mgmt.address
			reconfig_mgmt_read                => reconfig_mgmt_read,                            --                .read
			reconfig_mgmt_readdata            => reconfig_mgmt_readdata,                        --                .readdata
			reconfig_mgmt_waitrequest         => reconfig_mgmt_waitrequest,                     --                .waitrequest
			reconfig_mgmt_write               => reconfig_mgmt_write,                           --                .write
			reconfig_mgmt_writedata           => reconfig_mgmt_writedata,                       --                .writedata
			reconfig_to_xcvr                  => alt_sv_gt_reconfctrl_x2_inst_reconfig_to_xcvr, --   ch0_1_to_xcvr.reconfig_to_xcvr
			reconfig_from_xcvr(91 downto 0)   => ch0_1_from_xcvr(91 downto 0),                  -- ch0_1_from_xcvr.reconfig_from_xcvr
			reconfig_from_xcvr(137 downto 92) => ch2_2_from_xcvr(45 downto 0),                  -- ch0_1_from_xcvr.reconfig_from_xcvr
			tx_cal_busy                       => open,                                          --     (terminated)
			rx_cal_busy                       => open,                                          --     (terminated)
			cal_busy_in                       => '0',                                           --     (terminated)
			reconfig_mif_address              => open,                                          --     (terminated)
			reconfig_mif_read                 => open,                                          --     (terminated)
			reconfig_mif_readdata             => "0000000000000000",                            --     (terminated)
			reconfig_mif_waitrequest          => '0'                                            --     (terminated)
		);

	ch2_2_to_xcvr <= alt_sv_gt_reconfctrl_x2_inst_reconfig_to_xcvr(209 downto 140);

	ch0_1_to_xcvr <= alt_sv_gt_reconfctrl_x2_inst_reconfig_to_xcvr(139 downto 0);

end architecture rtl; -- of alt_sv_gt_reconfctrl_x2
-- Retrieval info: <?xml version="1.0"?>
--<!--
--	Generated by Altera MegaWizard Launcher Utility version 1.0
--	************************************************************
--	THIS IS A WIZARD-GENERATED FILE. DO NOT EDIT THIS FILE!
--	************************************************************
--	Copyright (C) 1991-2014 Altera Corporation
--	Any megafunction design, and related net list (encrypted or decrypted),
--	support information, device programming or simulation file, and any other
--	associated documentation or information provided by Altera or a partner
--	under Altera's Megafunction Partnership Program may be used only to
--	program PLD devices (but not masked PLD devices) from Altera.  Any other
--	use of such megafunction design, net list, support information, device
--	programming or simulation file, or any other related documentation or
--	information is prohibited for any other purpose, including, but not
--	limited to modification, reverse engineering, de-compiling, or use with
--	any other silicon devices, unless such use is explicitly licensed under
--	a separate agreement with Altera or a megafunction partner.  Title to
--	the intellectual property, including patents, copyrights, trademarks,
--	trade secrets, or maskworks, embodied in any such megafunction design,
--	net list, support information, device programming or simulation file, or
--	any other related documentation or information provided by Altera or a
--	megafunction partner, remains with Altera, the megafunction partner, or
--	their respective licensors.  No other licenses, including any licenses
--	needed under any third party's intellectual property, are provided herein.
---->
-- Retrieval info: <instance entity-name="alt_xcvr_reconfig" version="13.1" >
-- Retrieval info: 	<generic name="device_family" value="Stratix V" />
-- Retrieval info: 	<generic name="number_of_reconfig_interfaces" value="3" />
-- Retrieval info: 	<generic name="gui_split_sizes" value="2,1" />
-- Retrieval info: 	<generic name="enable_offset" value="1" />
-- Retrieval info: 	<generic name="enable_dcd" value="0" />
-- Retrieval info: 	<generic name="enable_dcd_power_up" value="1" />
-- Retrieval info: 	<generic name="enable_analog" value="1" />
-- Retrieval info: 	<generic name="enable_eyemon" value="0" />
-- Retrieval info: 	<generic name="ber_en" value="0" />
-- Retrieval info: 	<generic name="enable_dfe" value="0" />
-- Retrieval info: 	<generic name="enable_adce" value="0" />
-- Retrieval info: 	<generic name="enable_mif" value="0" />
-- Retrieval info: 	<generic name="gui_enable_pll" value="0" />
-- Retrieval info: 	<generic name="gui_cal_status_port" value="false" />
-- Retrieval info: 	<generic name="AUTO_MGMT_CLK_CLK_CLOCK_RATE" value="-1" />
-- Retrieval info: </instance>
-- IPFS_FILES : alt_sv_gt_reconfctrl_x2.vho
-- RELATED_FILES: alt_sv_gt_reconfctrl_x2.vhd, altera_xcvr_functions.sv, sv_xcvr_h.sv, alt_xcvr_resync.sv, alt_xcvr_reconfig_h.sv, sv_xcvr_dfe_cal_sweep_h.sv, alt_xcvr_reconfig.sv, alt_xcvr_reconfig_sv.sv, alt_xcvr_reconfig_cal_seq.sv, alt_xreconf_cif.sv, alt_xreconf_uif.sv, alt_xreconf_basic_acq.sv, alt_xcvr_reconfig_analog.sv, alt_xcvr_reconfig_analog_sv.sv, alt_xreconf_analog_datactrl.sv, alt_xreconf_analog_rmw.sv, alt_xreconf_analog_ctrlsm.sv, alt_xcvr_reconfig_offset_cancellation.sv, alt_xcvr_reconfig_offset_cancellation_sv.sv, alt_xcvr_reconfig_eyemon.sv, alt_xcvr_reconfig_eyemon_sv.sv, alt_xcvr_reconfig_eyemon_ctrl_sv.sv, alt_xcvr_reconfig_eyemon_ber_sv.sv, ber_reader_dcfifo.v, step_to_mon_sv.sv, mon_to_step_sv.sv, alt_xcvr_reconfig_dfe.sv, alt_xcvr_reconfig_dfe_sv.sv, alt_xcvr_reconfig_dfe_reg_sv.sv, alt_xcvr_reconfig_dfe_cal_sv.sv, alt_xcvr_reconfig_dfe_cal_sweep_sv.sv, alt_xcvr_reconfig_dfe_cal_sweep_datapath_sv.sv, alt_xcvr_reconfig_dfe_oc_cal_sv.sv, alt_xcvr_reconfig_dfe_pi_phase_sv.sv, alt_xcvr_reconfig_dfe_step_to_mon_en_sv.sv, alt_xcvr_reconfig_dfe_adapt_tap_sv.sv, alt_xcvr_reconfig_dfe_ctrl_mux_sv.sv, alt_xcvr_reconfig_dfe_local_reset_sv.sv, alt_xcvr_reconfig_dfe_cal_sim_sv.sv, alt_xcvr_reconfig_dfe_adapt_tap_sim_sv.sv, alt_xcvr_reconfig_adce.sv, alt_xcvr_reconfig_adce_sv.sv, alt_xcvr_reconfig_adce_datactrl_sv.sv, alt_xcvr_reconfig_dcd.sv, alt_xcvr_reconfig_dcd_sv.sv, alt_xcvr_reconfig_dcd_cal.sv, alt_xcvr_reconfig_dcd_control.sv, alt_xcvr_reconfig_dcd_datapath.sv, alt_xcvr_reconfig_dcd_pll_reset.sv, alt_xcvr_reconfig_dcd_eye_width.sv, alt_xcvr_reconfig_dcd_align_clk.sv, alt_xcvr_reconfig_dcd_get_sum.sv, alt_xcvr_reconfig_dcd_cal_sim_model.sv, alt_xcvr_reconfig_mif.sv, sv_xcvr_reconfig_mif.sv, sv_xcvr_reconfig_mif_ctrl.sv, sv_xcvr_reconfig_mif_avmm.sv, alt_xcvr_reconfig_pll.sv, sv_xcvr_reconfig_pll.sv, sv_xcvr_reconfig_pll_ctrl.sv, alt_xcvr_reconfig_soc.sv, alt_xcvr_reconfig_cpu_ram.sv, alt_xcvr_reconfig_direct.sv, sv_xrbasic_l2p_addr.sv, sv_xrbasic_l2p_ch.sv, sv_xrbasic_l2p_rom.sv, sv_xrbasic_lif_csr.sv, sv_xrbasic_lif.sv, sv_xcvr_reconfig_basic.sv, alt_arbiter_acq.sv, alt_xcvr_reconfig_basic.sv, alt_xcvr_arbiter.sv, alt_xcvr_m2s.sv, altera_wait_generate.v, alt_xcvr_csr_selector.sv, sv_reconfig_bundle_to_basic.sv, alt_xcvr_reconfig_cpu.v, alt_xcvr_reconfig_cpu_reconfig_cpu.v, alt_xcvr_reconfig_cpu_reconfig_cpu_test_bench.v, alt_xcvr_reconfig_cpu_mm_interconnect_0.v, alt_xcvr_reconfig_cpu_irq_mapper.sv, altera_reset_controller.v, altera_reset_synchronizer.v, altera_merlin_master_translator.sv, altera_merlin_slave_translator.sv, altera_merlin_master_agent.sv, altera_merlin_slave_agent.sv, altera_merlin_burst_uncompressor.sv, altera_avalon_sc_fifo.v, alt_xcvr_reconfig_cpu_mm_interconnect_0_addr_router.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_addr_router_001.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_id_router.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_id_router_001.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_cmd_xbar_demux.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_cmd_xbar_demux_001.sv, altera_merlin_arbitrator.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_cmd_xbar_mux.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_cmd_xbar_mux_001.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_rsp_xbar_mux.sv, alt_xcvr_reconfig_cpu_mm_interconnect_0_rsp_xbar_mux_001.sv
