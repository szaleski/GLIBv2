--=================================================================================================--
--##################################   Package Information   ######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Xilinx Virtex 6 - Multi Gigabit Transceivers latency-optimized
--                                                                                                 
-- Language:              VHDL'93                                                                 
--                                                                                                   
-- Target Device:         Xilinx Virtex 6                                                         
-- Tool version:          ISE 14.5                                                               
--                                                                                                   
-- Revision:              3.0                                                                      
--
-- Description:           
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        20/06/2013   3.0       M. Barros Marin   First .vhd module definition           
--
-- Additional Comments:
--
-- * Note!! The GTX TX and RX PLLs reference clocks frequency is 240 MHz
--
-- * Note!! The Elastic buffers are bypassed in this latency-optimized GTX (reduces the latency as well
--          as ensures deterministic latency within the GTX)
--
-- * Note!! The phase of the recovered clock is shifted during bitslip. This is done to achieve
--          deterministic phase when crossing from serial clock (2.4Ghz DDR) to RX_RECCLK (240MHz SDR)                                                                               
--
-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-- !!                                                                                           !!
-- !! * The different parameters of the GBT Bank are set through:                               !!  
-- !!   (Note!! These parameters are vendor specific)                                           !!                    
-- !!                                                                                           !!
-- !!   - The MGT control ports of the GBT Bank module (these ports are listed in the records   !!
-- !!     of the file "<vendor>_<device>_gbt_bank_package.vhd").                                !! 
-- !!     (e.g. xlx_v6_gbt_bank_package.vhd)                                                    !!
-- !!                                                                                           !!  
-- !!   - By modifying the content of the file "<vendor>_<device>_gbt_bank_user_setup.vhd".     !!
-- !!     (e.g. xlx_v6_gbt_bank_user_setup.vhd)                                                 !! 
-- !!                                                                                           !! 
-- !! * The "<vendor>_<device>_gbt_bank_user_setup.vhd" is the only file of the GBT Bank that   !!
-- !!   may be modified by the user. The rest of the files MUST be used as is.                  !!
-- !!                                                                                           !!  
-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
--                                                                                                   
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--

-- IEEE VHDL standard library:
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- Xilinx devices library:
library unisim;
use unisim.vcomponents.all;

-- Custom libraries and packages:
use work.gbt_bank_package.all;
use work.vendor_specific_gbt_bank_package.all;
use work.gbt_banks_user_setup.all;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity mgt_latopt is  
   generic (   
      GBT_BANK_ID                               : integer := 1   
   );
   port (
   
      --===============--
      -- Clocks scheme --
      --===============--      
      
      MGT_CLKS_I                                : in  gbtBankMgtClks_i_R;         
      MGT_CLKS_O                                : out gbtBankMgtClks_o_R;        
 
      --=========--
      -- MGT I/O --
      --=========--
      
      MGT_I                                     : in  mgt_i_R;
      MGT_O                                     : out mgt_o_R;
      
      --=============-- 
      -- GBT Control -- 
      --=============--  

      GBT_RX_MGT_RDY_O                          : out std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      GBT_RX_RXWORDCLK_READY_O                  : out std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);      
      ------------------------------------------
      GBT_RX_HEADER_LOCKED_I                    : in  std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      GBT_RX_BITSLIP_NBR_I                      : in  gbtRxSlideNbr_mxnbit_A(1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
     
      --=======-- 
      -- Words -- 
      --=======-- 
      
      GBT_TX_WORD_I                             : in  word_mxnbit_A         (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);     
      GBT_RX_WORD_O                             : out word_mxnbit_A         (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS)     

   );
end mgt_latopt;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of mgt_latopt is    
   
   --================================ Signal Declarations ================================--
   
   --===============--
   -- Resets scheme --      
   --===============--       
  
   signal txResetDone_from_gtx                  : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxResetDone_from_gtx                  : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   -- TX reset done synchronization registers:
   -------------------------------------------
   
   signal txResetDone_r                         : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txResetDone_r2_from_gtxTxRstDoneSync  : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   -- RX reset done synchronization registers:
   -------------------------------------------      

   signal rxResetDone_r_from_gtxRxRstDoneSync1  : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxResetDone_r2                        : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   ---------------------------------------------
   signal rxResetDone_r3                        : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxResetDone_r4_from_gtxRxRstDoneSync2 : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   --==============================--
   -- MGT internal phase alignment --
   --==============================--

   -- TX synchronizer:
   -------------------
   
   signal txEnPmaPhaseAlign_from_txSync         : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txPmaSetPhase_from_txSync             : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txDlyAlignDisable_from_txSync         : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txDlyAlignReset_from_txSync           : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txSyncDone_from_txSync                : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   ---------------------------------------------
   signal reset_to_txSync                       : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);

   -- RX synchronizer:
   -------------------  
   
   signal rxEnPmaPhaseAlign_from_rxSync         : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS); 
   signal rxPmaSetPhase_from_rxSync             : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDlyAlignDisable_from_rxSync         : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDlyAlignOverride_from_rxSync        : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDlyAlignReset_from_rxSync           : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxSyncDone_from_rxSync                : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   ---------------------------------------------
   signal reset_to_rxSync                       : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   --========================--
   -- RX phase alignment --
   --========================--

   signal bitSlipNbr_to_bitSlipControl          : gbtRxSlideNbr_mxnbit_A  (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlipRun_to_bitSlipControl          : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlip_from_bitSlipControl           : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlip_to_gtx                        : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal done_from_bitSlipControl              : std_logic_vector        (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
 
   --=====================================================================================--      

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
  
   --==================================== User Logic =====================================--

   gtxLatOpt_gen: for i in 1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS generate   
   
      --=============--
      -- Assignments --
      --=============--
   
      MGT_O.mgtLink(i).tx_resetDone                  <= txResetDone_r2_from_gtxTxRstDoneSync(i);
      MGT_O.mgtLink(i).rx_resetDone                  <= rxResetDone_r4_from_gtxRxRstDoneSync2(i);   
      MGT_O.mgtLink(i).ready                         <= txSyncDone_from_txSync(i) and rxSyncDone_from_rxSync(i);      
      GBT_RX_MGT_RDY_O(i)                            <= rxSyncDone_from_rxSync(i); 
      
      --====================--
      -- RX phase alignment --
      --====================--

      -- Comment: Note!! The standard version of the GTX does not align the phase of the  
      --                  RX_RECCLK (RX_WORDCLK) with respect to the TX_OUTCLK (TX_WORDCLK).

      rxPhaseAlign_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).RX_OPTIMIZATION = LATENCY_OPTIMIZED generate
      
         -- Bitslip control module:
         --------------------------
      
         bitSlipControl: entity work.mgt_latopt_bitslipctrl 
            port map (
               RX_RESET_I                            => MGT_I.mgtLink(i).rx_reset,
               RX_WORDCLK_I                          => MGT_CLKS_I.rx_wordclk(i),               
               NUMBITSLIPS_I                         => bitSlipNbr_to_bitSlipControl(i),
               ENABLE_I                              => bitSlipRun_to_bitSlipControl(i),
               BITSLIP_O                             => bitSlip_from_bitSlipControl(i),
               DONE_O                                => done_from_bitSlipControl(i)
            );       
         
         MGT_O.mgtLink(i).rxWordClkReady             <= done_from_bitSlipControl(i);
         GBT_RX_RXWORDCLK_READY_O(i)                 <= done_from_bitSlipControl(i);
         
         -- Manual or auto bitslip control selection logic:
         --------------------------------------------------
      
         -- Comment: * MGT_I(i).rx_slide_enable must be '1' to enable the GTX RX phase alignment.
         --
         --          * Manual control: MGT_I(i).rx_slide_ctrl = '1'
         --            Auto control  : MGT_I(i).rx_slide_ctrl = '0'
         --
         --          * In manual control, the user provides the number of bitslips (MGT_I(i).rx_slide_nbr)
         --            as well as triggers the GTX RX phase alignment (MGT_I(i).rx_slide_run).
      
         bitSlip_to_gtx(i)               <= bitSlip_from_bitSlipControl(i) when    MGT_I.mgtLink(i).rxSlide_enable   = '1'
                                            ------------------------------------------------------------------------------
                                            else '0'; 
                                      
         bitSlipRun_to_bitSlipControl(i) <= MGT_I.mgtLink(i).rxSlide_run  when      MGT_I.mgtLink(i).rxSlide_enable   = '1' 
                                                                                and MGT_I.mgtLink(i).rxSlide_ctrl     = '1'
                                            -------------------------------------------------------------------------------
                                            else GBT_RX_HEADER_LOCKED_I(i) when     MGT_I.mgtLink(i).rxSlide_enable   = '1'
                                                                                and MGT_I.mgtLink(i).rxSlide_ctrl     = '0'
                                            -------------------------------------------------------------------------------
                                            else '0';
                                      
         bitSlipNbr_to_bitSlipControl(i) <= MGT_I.mgtLink(i).rxSlide_nbr   when     MGT_I.mgtLink(i).rxSlide_enable   = '1'
                                                                                and MGT_I.mgtLink(i).rxSlide_ctrl     = '1'
                                            -------------------------------------------------------------------------------                                  
                                            else GBT_RX_BITSLIP_NBR_I(i)   when     MGT_I.mgtLink(i).rxSlide_enable   = '1'
                                                                                and MGT_I.mgtLink(i).rxSlide_ctrl     = '0'
                                            -------------------------------------------------------------------------------                                  
                                            else "00000"; 
 
      end generate;
      
      rxPhaseAlign_no_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).RX_OPTIMIZATION = STANDARD generate
      
         -- Bitslip control module:
         --------------------------
         
         MGT_O.mgtLink(i).rxWordClkReady             <= '0';
         GBT_RX_RXWORDCLK_READY_O(i)                 <= '0';
         
         -- Manual or auto bitslip control selection logic:
         --------------------------------------------------
      
         bitSlip_to_gtx(i)                           <= '0';
      
      end generate; 
      
      --================================================--
      -- Multi-Gigabit Transceivers (latency-optimized) --
      --================================================--  

      gtxLatOpt: entity work.xlx_v6_gtx_latopt
         generic map (
            GTX_SIM_GTXRESET_SPEEDUP                 => speedUp(GBT_BANKS_USER_SETUP(GBT_BANK_ID).SPEEDUP_FOR_SIMULATION))   
         port map (        
            -----------------------------------------
            LOOPBACK_IN                              => MGT_I.mgtLink(i).loopBack,
            -----------------------------------------
            RXSLIDE_IN                               => bitSlip_to_gtx(i),
            -----------------------------------------
            PRBSCNTRESET_IN                          => MGT_I.mgtLink(i).prbs_errCntRst, 
            RXENPRBSTST_IN                           => MGT_I.mgtLink(i).prbs_rxEn,  
            RXPRBSERR_OUT                            => MGT_O.mgtLink(i).prbs_rxErr,     
            -----------------------------------------
            RXDATA_OUT                               => GBT_RX_WORD_O(i),
            RXRECCLK_OUT                             => MGT_CLKS_O.rx_wordClk_noBuff(i),
            RXUSRCLK2_IN                             => MGT_CLKS_I.rx_wordClk(i),
            -----------------------------------------
            RXEQMIX_IN                               => MGT_I.mgtLink(i).conf_eqMix,
            RXN_IN                                   => MGT_I.mgtLink(i).rx_n,
            RXP_IN                                   => MGT_I.mgtLink(i).rx_p,
            -----------------------------------------
            RXDLYALIGNDISABLE_IN                     => rxDlyAlignDisable_from_rxSync(i),          
            RXDLYALIGNMONENB_IN                      => '0',                              
            RXDLYALIGNMONITOR_OUT                    => open,                             
            RXDLYALIGNOVERRIDE_IN                    => rxDlyAlignOverride_from_rxSync(i),      
            RXDLYALIGNRESET_IN                       => rxDlyAlignReset_from_rxSync(i),            
            RXENPMAPHASEALIGN_IN                     => rxEnPmaPhaseAlign_from_rxSync(i),    
            RXPMASETPHASE_IN                         => rxPmaSetPhase_from_rxSync(i),
            -----------------------------------------
            GTXRXRESET_IN                            => MGT_I.mgtLink(i).rx_reset,
            MGTREFCLKRX_IN                           => ('0' & MGT_CLKS_I.rx_refClk),
            PLLRXRESET_IN                            => '0',
            RXPLLLKDET_OUT                           => MGT_O.mgtLink(i).rx_pllLkDet,
            RXRESETDONE_OUT                          => rxResetDone_from_gtx(i),
            -----------------------------------------
            RXPOLARITY_IN                            => MGT_I.mgtLink(i).conf_rxPol,
            -----------------------------------------
            DADDR_IN                                 => MGT_I.mgtLink(i).drp_dAddr,   
            DCLK_IN                                  => MGT_I.mgtLink(i).drp_dClk,
            DEN_IN                                   => MGT_I.mgtLink(i).drp_dEn,    
            DI_IN                                    => MGT_I.mgtLink(i).drp_dI,     
            DRDY_OUT                                 => MGT_O.mgtLink(i).drp_dRdy,   
            DRPDO_OUT                                => MGT_O.mgtLink(i).drp_dRpDo,  
            DWE_IN                                   => MGT_I.mgtLink(i).drp_dWe,   
            -----------------------------------------
            TXDATA_IN                                => GBT_TX_WORD_I(i),
            TXOUTCLK_OUT                             => MGT_CLKS_O.tx_wordClk_noBuff(i),
            TXUSRCLK2_IN                             => MGT_CLKS_I.tx_wordClk(i),
            -----------------------------------------
            TXDIFFCTRL_IN                            => MGT_I.mgtLink(i).conf_diff,
            TXN_OUT                                  => MGT_O.mgtLink(i).tx_n,
            TXP_OUT                                  => MGT_O.mgtLink(i).tx_p,
            TXPOSTEMPHASIS_IN                        => MGT_I.mgtLink(i).conf_pstEmph,
            -----------------------------------------
            TXPREEMPHASIS_IN                         => MGT_I.mgtLink(i).conf_preEmph,
            -----------------------------------------
            TXDLYALIGNDISABLE_IN                     => txDlyAlignDisable_from_txSync(i),          
            TXDLYALIGNMONENB_IN                      => '0',                                
            TXDLYALIGNMONITOR_OUT                    => open,                                
            TXDLYALIGNRESET_IN                       => txDlyAlignReset_from_txSync(i),            
            TXENPMAPHASEALIGN_IN                     => txEnPmaPhaseAlign_from_txSync(i),          
            TXPMASETPHASE_IN                         => txPmaSetPhase_from_txSync(i),
            -----------------------------------------
            GTXTXRESET_IN                            => MGT_I.mgtLink(i).tx_reset,
            MGTREFCLKTX_IN                           => ('0' & MGT_CLKS_I.tx_refClk),
            PLLTXRESET_IN                            => '0',
            TXPLLLKDET_OUT                           => MGT_O.mgtLink(i).tx_pllLkDet,
            TXRESETDONE_OUT                          => txResetDone_from_gtx(i),
            -----------------------------------------
            TXENPRBSTST_IN                           => MGT_I.mgtLink(i).prbs_txEn,         
            TXPRBSFORCEERR_IN                        => MGT_I.mgtLink(i).prbs_forcErr,  
            -----------------------------------------
            TXPOLARITY_IN                            => MGT_I.mgtLink(i).conf_txPol     
         );
         
      --==============--
      -- Reset scheme --      
      --==============--    
      
      -- TX reset done synchronization registers:
      -------------------------------------------
      
      gtxTxRstDoneSync: process(txResetDone_from_gtx(i), MGT_CLKS_I.tx_wordClk(i))
      begin
         if txResetDone_from_gtx(i) = '0' then       
            txResetDone_r2_from_gtxTxRstDoneSync(i)  <= '0';         
            txResetDone_r(i)                         <= '0';
         elsif rising_edge(MGT_CLKS_I.tx_wordClk(i)) then   
            txResetDone_r2_from_gtxTxRstDoneSync(i)  <= txResetDone_r(i);
            txResetDone_r(i)                         <= txResetDone_from_gtx(i);         
         end if;
      end process;
      
      -- RX reset done synchronization registers:
      -------------------------------------------  
      
      gtxRxRstDoneSync1: process(MGT_CLKS_I.rx_wordClk(i))
      begin
         if rising_edge(MGT_CLKS_I.rx_wordClk(i)) then        
            rxResetDone_r_from_gtxRxRstDoneSync1(i)  <= rxResetDone_from_gtx(i);  
         end if; 
      end process;
      
      gtxRxRstDoneSync2: process(rxResetDone_r_from_gtxRxRstDoneSync1(i), MGT_CLKS_I.rx_wordClk(i))
      begin
         if rxResetDone_r_from_gtxRxRstDoneSync1(i) = '0' then      
            rxResetDone_r4_from_gtxRxRstDoneSync2(i) <= '0';
            rxResetDone_r3(i)                        <= '0';
            rxResetDone_r2(i)                        <= '0';
         elsif rising_edge(MGT_CLKS_I.rx_wordClk(i)) then       
            rxResetDone_r4_from_gtxRxRstDoneSync2(i) <= rxResetDone_r3(i);
            rxResetDone_r3(i)                        <= rxResetDone_r2(i);
            rxResetDone_r2(i)                        <= rxResetDone_r_from_gtxRxRstDoneSync1(i); 
         end if; 
      end process;
      
      --==============================--
      -- MGT internal phase alignment --
      --==============================--
      
      -- Comment: The internal clock domains of the GTX must be synchronized due to the elastic buffer bypassing. 
      
      -- TX synchronizer:
      -------------------
      
      reset_to_txSync(i)                             <= (not txResetDone_r2_from_gtxTxRstDoneSync(i)) or MGT_I.mgtLink(i).tx_syncReset;
      
      txSync: entity work.xlx_v6_gtx_latopt_tx_sync
         generic map (
            SIM_TXPMASETPHASE_SPEEDUP                => speedUp(GBT_BANKS_USER_SETUP(GBT_BANK_ID).SPEEDUP_FOR_SIMULATION))
         port map (         
            TXENPMAPHASEALIGN                        => txEnPmaPhaseAlign_from_txSync(i),
            TXPMASETPHASE                            => txPmaSetPhase_from_txSync(i),
            TXDLYALIGNDISABLE                        => txDlyAlignDisable_from_txSync(i),
            TXDLYALIGNRESET                          => txDlyAlignReset_from_txSync(i),
            SYNC_DONE                                => txSyncDone_from_txSync(i),
            USER_CLK                                 => MGT_CLKS_I.tx_wordClk(i),
            RESET                                    => reset_to_txSync(i)
         );

      -- RX synchronizer:
      -------------------
      
      reset_to_rxSync(i)                             <= (not rxResetDone_r4_from_gtxRxRstDoneSync2(i)) or MGT_I.mgtLink(i).rx_syncReset; 
      
      rxSync: entity work.xlx_v6_gtx_latopt_rx_sync
         port map (                                                                            
            RXENPMAPHASEALIGN                        => rxEnPmaPhaseAlign_from_rxSync(i),                        
            RXPMASETPHASE                            => rxPmaSetPhase_from_rxSync(i),                            
            RXDLYALIGNDISABLE                        => rxDlyAlignDisable_from_rxSync(i),                        
            RXDLYALIGNOVERRIDE                       => rxDlyAlignOverride_from_rxSync(i),                       
            RXDLYALIGNRESET                          => rxDlyAlignReset_from_rxSync(i),                          
            SYNC_DONE                                => rxSyncDone_from_rxSync(i),                               
            USER_CLK                                 => MGT_CLKS_I.rx_wordClk(i),
            RESET                                    => reset_to_rxSync(i)
         );         

   end generate;
   
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--