--=================================================================================================--
--##################################   Package Information   ######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Xilinx Kintex 7 & Virtex 7 - Multi Gigabit Transceivers latency-optimized
--                                                                                                 
-- Language:              VHDL'93                                                                 
--                                                                                                   
-- Target Device:         Xilinx Kintex 7 & Virtex 7                                                         
-- Tool version:          ISE 14.5                                                               
--                                                                                                   
-- Revision:              3.0                                                                      
--
-- Description:           
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        08/01/14     3.0       M. Barros Marin   First .vhd module definition           
--
-- Additional Comments: 
--
-- * Note!! The GTX PLL reference clocks frequency is 120 MHz
--
-- * Note!! The Elastic buffers are bypassed in this latency-optimized GTX (reduces the latency as well
--          as ensures deterministic latency within the GTX)
--
-- * Note!! The phase of the recovered clock is shifted during bitslip. This is done to achieve
--          deterministic phase when crossing from serial clock (2.4Ghz DDR) to RX_RECCLK (120MHz SDR)                                                                                   
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
      GBT_BANK_ID                                  : integer := 1
   ); 
   port (      
      
      --===============--  
      -- Clocks scheme --  
      --===============--  
   
      MGT_CLKS_I                                   : in  gbtBankMgtClks_i_R;
      MGT_CLKS_O                                   : out gbtBankMgtClks_o_R;        
         
      --=========--  
      -- MGT I/O --  
      --=========--  
   
      MGT_I                                        : in  mgt_i_R;
      MGT_O                                        : out mgt_o_R;
   
      --=============-- 
      -- GBT Control -- 
      --=============-- 
      
      GBT_RX_MGT_RDY_O                             : out std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      GBT_RX_RXWORDCLK_READY_O                     : out std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);      
      ---------------------------------------------
      GBT_RX_HEADER_LOCKED_I                       : in  std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      GBT_RX_BITSLIP_NBR_I                         : in  gbtRxSlideNbr_mxnbit_A(1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      
      --=======-- 
      -- Words -- 
      --=======-- 
   
      GBT_TX_WORD_I                                : in  word_mxnbit_A         (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);     
      GBT_RX_WORD_O                                : out word_mxnbit_A         (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS) 
   
   );
end mgt_latopt;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of mgt_latopt is

   --==================================== Attributes =====================================--   
   
   attribute CORE_GENERATION_INFO                  : string;
   attribute CORE_GENERATION_INFO of structural    : architecture is "xlx_k7v7_gtx_latopt,gtwizard_v2_5,{protocol_file=Start_from_scratch}";

   --=====================================================================================--
  
   --=============================== Constant Declarations ===============================--

   constant GT_SIM_GTRESET_SPEEDUP                 : string := speedUp(GBT_BANKS_USER_SETUP(GBT_BANK_ID).SIM_GTRESET_SPEEDUP);  
      
   constant TX_GTX_BUFFBYPASS_MANUAL_MULTILINK     : boolean := txGtxBuffBypassManual(GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   constant RX_GTX_BUFFBYPASS_MANUAL_MULTILINK     : boolean := rxGtxBuffBypassManual(GBT_BANKS_USER_SETUP(GBT_BANK_ID).RX_GTX_BUFFBYPASS_MANUAL,
                                                                                      GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);

   constant RX_CDRLOCK_TIME                        : integer := getCdrLockTime(GBT_BANKS_USER_SETUP(GBT_BANK_ID).SIMULATION);   -- Comment: * 200us
   constant WAIT_TIME_CDRLOCK                      : integer := RX_CDRLOCK_TIME / STABLE_CLOCK_PERIOD;    
  
   constant QPLL_FBDIV_IN                          : bit_vector(9 downto 0) := convQpllFbDivTop(QPLL_FBDIV_TOP);
   constant QPLL_FBDIV_RATIO                       : bit := convQpllFbDivRatio(QPLL_FBDIV_TOP);   
   
   --=====================================================================================--
    
   --================================ Signal Declarations ================================--

   --====================--
   -- RX phase alignment --
   --====================--
   
   signal bitSlipNbr_to_bitSlipControl             : gbtRxSlideNbr_mxnbit_A(1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlipRun_to_bitSlipControl             : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlip_from_bitSlipControl              : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal bitSlip_to_gtx                           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);   
   signal done_from_bitSlipControl                 : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   --================================================--
   -- Multi-Gigabit Transceivers (latency-optimized) --
   --================================================--  

   signal cpllLock_from_gtx                        : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal cpllRefClkLost_from_gtx                  : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txResetDone_from_gtx                     : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxResetDone_from_gtx                     : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txDlySResetDone_from_gtx                 : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txPhAlignDone_from_gtx                   : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txPhInitDone_from_gtx                    : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDlySResetDone_from_gtx                 : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxPhAlignDone_from_gtx                   : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   --===============--
   -- Resets scheme --      
   --===============--
   
   signal gtTxReset_from_txResetFsm                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal cpllReset_from_txResetFsm                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txUserRdy_from_txResetFsm                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txFsmResetDone_from_txResetFsm           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);   
   signal runTxPhalignment_from_txResetFsm         : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal resetTxPhalignment_from_txResetFsm       : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   ------------------------------------------------
   signal gtRxReset_from_rxResetFsm                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDfeAgcHold_from_rxResetFsm             : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxUserRdy_from_rxResetFsm                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxFsmResetDone_from_rxResetFsm           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal runRxPhAlignment_from_rxResetFsm         : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal resetRxPhalignment_from_rxResetFsm       : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   signal timer                                    : integer               range 0 to WAIT_TIME_CDRLOCK;
   signal rxCdrLocked_from_cdrLockTimeout          : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   signal rxResetDone_r3                           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txResetDone_r2                           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxResetDone_r2                           : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txResetDone_r                            : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);   
   signal rxResetDone_r                            : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   
   --=====================--
   -- GTX synchronization -- 
   --=====================--
   
   signal txPhAlignmentDone_from_txPhaseAlign      : std_logic;
   signal txDlySReset_from_txPhaseAlign            : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txPhInit_from_txPhaseAlign               : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txPhAlign_from_txPhaseAlign              : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS); 
   signal txDlyEn_from_txPhaseAlign                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS); 
   signal resetTxPhalignment_from_oRgate           : std_logic;
   signal runTxPhalignment_from_oRgate             : std_logic;
   ------------------------------------------------
   signal rxPhAlignmentDone_from_autoRxPhaseAlign  : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxPhAlignmentDone_from_manualRxPhaseAlign: std_logic;
   signal rxDlySReset_from_rxPhaseAlign            : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxPhAlign_from_rxPhaseAlign              : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDlyEn_from_rxPhaseAlign                : std_logic_vector      (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal resetRxPhalignment_from_oRgate           : std_logic;
   signal runRxPhalignment_from_oRgate             : std_logic;
   
   --=============-- 
   -- Common core --
   --=============-- 
   
   signal qpllOutClk_from_gtxCommon                : std_logic;
   signal qpllOutRefClk_from_gtxCommon             : std_logic;  
   
   --=====================================================================================--
   
--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--

   --==================================== User Logic =====================================--
   
   gtxLatOpt_gen: for i in 1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS generate
      
      --=============--
      -- Assignments --
      --=============--
      
      MGT_O.mgtLink(i).cpllLock                    <= cpllLock_from_gtx(i);
      MGT_O.mgtLink(i).tx_resetDone                <= txResetDone_r2(i);
      MGT_O.mgtLink(i).rx_resetDone                <= rxResetDone_r3(i);
      MGT_O.mgtLink(i).tx_fsmResetDoneOut          <= txFsmResetDone_from_txResetFsm(i);
      MGT_O.mgtLink(i).rx_fsmResetDoneOut          <= rxFsmResetDone_from_rxResetFsm(i);
      MGT_O.mgtLink(i).ready                       <=     txFsmResetDone_from_txResetFsm(i)
                                                      and rxFsmResetDone_from_rxResetFsm(i)
                                                      and txResetDone_r2(i)
                                                      and rxResetDone_r3(i);
      GBT_RX_MGT_RDY_O(i)                          <=     rxFsmResetDone_from_rxResetFsm(i)
                                                      and rxResetDone_r3(i); 
      
      --====================--
      -- RX phase alignment --
      --====================--
      
      -- Comment: Note!! The standard version of the GTX does not align the phase of the  
      --                 RX_RECCLK (RX_WORDCLK) with respect to the TX_OUTCLK (TX_WORDCLK).
      
      rxPhaseAlign_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).RX_OPTIMIZATION = LATENCY_OPTIMIZED generate
      
         -- Bitslip control module:
         --------------------------
         
         bitSlipControl: entity work.mgt_latopt_bitslipctrl 
            port map (
               RX_RESET_I                          => gtRxReset_from_rxResetFsm(i),
               RX_WORDCLK_I                        => MGT_CLKS_I.rx_wordClk(i),
               NUMBITSLIPS_I                       => bitSlipNbr_to_bitSlipControl(i),
               ENABLE_I                            => bitSlipRun_to_bitSlipControl(i),
               BITSLIP_O                           => bitSlip_from_bitSlipControl(i),
               DONE_O                              => done_from_bitSlipControl(i)
            );   
         
         MGT_O.mgtLink(i).rxWordClkReady           <= done_from_bitSlipControl(i);
         GBT_RX_RXWORDCLK_READY_O(i)               <= done_from_bitSlipControl(i);
            
         -- Manual or auto bitslip control selection logic:
         --------------------------------------------------
         
         -- Comment: * MGT_I(i).rxSlide_enable must be '1' to enable the GTX RX phase alignment.
         --
         --          * Manual control: MGT_I(i).rxSlide_ctrl = '1'
         --            Auto control  : MGT_I(i).rxSlide_ctrl = '0'
         --
         --          * In manual control, the user provides the number of bitslips (MGT_I(i).rxSlide_nbr)
         --            as well as triggers the GTX RX phase alignment (MGT_I(i).rxSlide_run).
         
         bitSlip_to_gtx(i)                         <= bitSlip_from_bitSlipControl(i) when     MGT_I.mgtLink(i).rxSlide_enable = '1'
                                                      -----------------------------------------------------------------------------
                                                      else '0'; 
                                                   
         bitSlipRun_to_bitSlipControl(i)           <= MGT_I.mgtLink(i).rxSlide_run   when     MGT_I.mgtLink(i).rxSlide_enable = '1' 
                                                                                          and MGT_I.mgtLink(i).rxSlide_ctrl   = '1'
                                                      -----------------------------------------------------------------------------
                                                      else GBT_RX_HEADER_LOCKED_I(i) when     MGT_I.mgtLink(i).rxSlide_enable = '1'
                                                                                          and MGT_I.mgtLink(i).rxSlide_ctrl   = '0'
                                                      -----------------------------------------------------------------------------
                                                      else '0';
                           
         bitSlipNbr_to_bitSlipControl(i)           <= MGT_I.mgtLink(i).rxSlide_nbr   when     MGT_I.mgtLink(i).rxSlide_enable = '1'
                                                                                          and MGT_I.mgtLink(i).rxSlide_ctrl   = '1'
                                                      -----------------------------------------------------------------------------                                
                                                      else GBT_RX_BITSLIP_NBR_I(i)   when     MGT_I.mgtLink(i).rxSlide_enable = '1'
                                                                                          and MGT_I.mgtLink(i).rxSlide_ctrl   = '0'
                                                      -----------------------------------------------------------------------------                                
                                                      else (others => '0');   
      
      end generate;
      
      rxPhaseAlign_no_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).RX_OPTIMIZATION = STANDARD generate
      
         -- Bitslip control module:
         --------------------------
         
         MGT_O.mgtLink(i).rxWordClkReady           <= '0';
         GBT_RX_RXWORDCLK_READY_O(i)               <= '0';
         
         -- Manual or auto bitslip control selection logic:
         --------------------------------------------------
      
         bitSlip_to_gtx(i)                         <= '0';
      
      end generate;
      
      --================================================--
      -- Multi-Gigabit Transceivers (latency-optimized) --
      --================================================--   

      gtxLatOpt: entity work.xlx_k7v7_gtx_latopt
         generic map (
            GT_SIM_GTRESET_SPEEDUP                 => GT_SIM_GTRESET_SPEEDUP,
            RX_DFE_KL_CFG2_IN                      => x"3010D90C",
            PMA_RSV_IN                             => x"00018480",
            PCS_RSVD_ATTR_IN                       => x"000000000002")
         port map (     
            CPLLFBCLKLOST_OUT                      => open,
            CPLLLOCK_OUT                           => cpllLock_from_gtx(i),
            CPLLLOCKDETCLK_IN                      => MGT_CLKS_I.cpllLockDetClk,
            CPLLREFCLKLOST_OUT                     => cpllRefClkLost_from_gtx(i),        
            CPLLRESET_IN                           => cpllReset_from_txResetFsm(i),           
            ---------------------------------------   
            GTREFCLK0_IN                           => MGT_CLKS_I.mgtRefClk,
            ---------------------------------------   
            DRPADDR_IN                             => MGT_I.mgtLink(i).drp_addr,
            DRPCLK_IN                              => MGT_CLKS_I.drpClk,
            DRPDI_IN                               => MGT_I.mgtLink(i).drp_di,
            DRPDO_OUT                              => MGT_O.mgtLink(i).drp_do,
            DRPEN_IN                               => MGT_I.mgtLink(i).drp_en,
            DRPRDY_OUT                             => MGT_O.mgtLink(i).drp_rdy,
            DRPWE_IN                               => MGT_I.mgtLink(i).drp_we,
            ---------------------------------------   
            QPLLCLK_IN                             => qpllOutClk_from_gtxCommon,
            QPLLREFCLK_IN                          => qpllOutRefClk_from_gtxCommon,
            ---------------------------------------   
            LOOPBACK_IN                            => MGT_I.mgtLink(i).loopBack,
            ---------------------------------------   
            RXUSERRDY_IN                           => rxUserRdy_from_rxResetFsm(i),
            EYESCANDATAERROR_OUT                   => MGT_O.mgtLink(i).eyeScanDataError,
            RXCDRLOCK_OUT                          => MGT_O.mgtLink(i).rxCdrLock,
            RXUSRCLK_IN                            => MGT_CLKS_I.rx_wordClk(i),
            RXUSRCLK2_IN                           => MGT_CLKS_I.rx_wordClk(i),
            RXDATA_OUT                             => GBT_RX_WORD_O(i),
            RXPRBSERR_OUT                          => MGT_O.mgtLink(i).prbs_rxErr,
            RXPRBSSEL_IN                           => MGT_I.mgtLink(i).prbs_rxSel,
            RXPRBSCNTRESET_IN                      => MGT_I.mgtLink(i).prbs_rxCntReset,
            GTXRXP_IN                              => MGT_I.mgtLink(i).rx_p,
            GTXRXN_IN                              => MGT_I.mgtLink(i).rx_n,
            RXDLYEN_IN                             => rxDlyEn_from_rxPhaseAlign(i),              
            RXDLYSRESET_IN                         => rxDlySReset_from_rxPhaseAlign(i),              
            RXDLYSRESETDONE_OUT                    => rxDlySResetDone_from_gtx(i),              
            RXPHALIGN_IN                           => rxPhAlign_from_rxPhaseAlign(i),              
            RXPHALIGNDONE_OUT                      => rxPhAlignDone_from_gtx(i),              
            RXPHALIGNEN_IN                         => '1',              
            RXPHDLYRESET_IN                        => '0',              
            RXPHMONITOR_OUT                        => MGT_O.mgtLink(i).rxMonitor_ph,              
            RXPHSLIPMONITOR_OUT                    => MGT_O.mgtLink(i).rxMonitor_phSlip,                        
            RXDFEAGCHOLD_IN                        => rxDfeAgcHold_from_rxResetFsm(i),
            RXOUTCLK_OUT                           => MGT_CLKS_O.rx_wordClk_noBuff(i),
            GTRXRESET_IN                           => gtRxReset_from_rxResetFsm(i),
            RXPMARESET_IN                          => '0', 
            RXPOLARITY_IN                          => MGT_I.mgtLink(i).conf_rxPol,
            RXSLIDE_IN                             => bitSlip_to_gtx(i),
            RXRESETDONE_OUT                        => rxResetDone_from_gtx(i),
            ---------------------------------------   
            TXPOSTCURSOR_IN                        => MGT_I.mgtLink(i).conf_postCursor,
            TXPRECURSOR_IN                         => MGT_I.mgtLink(i).conf_preCursor, 
            GTTXRESET_IN                           => gtTxReset_from_txResetFsm(i),
            TXUSERRDY_IN                           => txUserRdy_from_txResetFsm(i),
            TXUSRCLK_IN                            => MGT_CLKS_I.tx_wordClk(i),
            TXUSRCLK2_IN                           => MGT_CLKS_I.tx_wordClk(i),
            TXPRBSFORCEERR_IN                      => MGT_I.mgtLink(i).prbs_txForceErr,
            TXDLYEN_IN                             => txDlyEn_from_txPhaseAlign(i),   
            TXDLYSRESET_IN                         => txDlySReset_from_txPhaseAlign(i),   
            TXDLYSRESETDONE_OUT                    => txDlySResetDone_from_gtx(i),   
            TXPHALIGN_IN                           => txPhAlign_from_txPhaseAlign(i),   
            TXPHALIGNDONE_OUT                      => txPhAlignDone_from_gtx(i),   
            TXPHALIGNEN_IN                         => '1',   
            TXPHDLYRESET_IN                        => '0',   
            TXPHINIT_IN                            => txPhInit_from_txPhaseAlign(i),   
            TXPHINITDONE_OUT                       => txPhInitDone_from_gtx(i),
            TXDIFFCTRL_IN                          => MGT_I.mgtLink(i).conf_diffCtrl,
            TXDATA_IN                              => GBT_TX_WORD_I(i), 
            GTXTXN_OUT                             => MGT_O.mgtLink(i).tx_n,
            GTXTXP_OUT                             => MGT_O.mgtLink(i).tx_p,
            TXOUTCLK_OUT                           => MGT_CLKS_O.tx_wordClk_noBuff(i),
            TXOUTCLKFABRIC_OUT                     => open,
            TXOUTCLKPCS_OUT                        => open,
            TXRESETDONE_OUT                        => txResetDone_from_gtx(i),
            TXPOLARITY_IN                          => MGT_I.mgtLink(i).conf_txPol,
            TXPRBSSEL_IN                           => MGT_I.mgtLink(i).prbs_txSel            
         ); 
   
      --===============--
      -- Resets scheme --
      --===============--
      
      -- Comment: * The values of "TX_QPLL_USED" and "RX_QPLL_USED" must be the same.
      --       
      --          * Decision if a manual phase-alignment is necessary or the automatic 
      --            is enough. For single-lane applications the automatic alignment is 
      --            sufficient
      
      txResetFsm: entity work.xlx_k7v7_gtx_TX_STARTUP_FSM
         generic map (
            GT_TYPE                                => "GTX",
            STABLE_CLOCK_PERIOD                    => STABLE_CLOCK_PERIOD,      
            RETRY_COUNTER_BITWIDTH                 => 8, 
            TX_QPLL_USED                           => false,                   
            RX_QPLL_USED                           => false,                    
            PHASE_ALIGNMENT_MANUAL                 => TX_GTX_BUFFBYPASS_MANUAL_MULTILINK)     
         port map (  
            STABLE_CLOCK                           => MGT_CLKS_I.sysClk,
            TXUSERCLK                              => MGT_CLKS_I.tx_wordClk(i),
            SOFT_RESET                             => MGT_I.mgtLink(i).tx_reset,
            QPLLREFCLKLOST                         => '0',
            CPLLREFCLKLOST                         => cpllRefClkLost_from_gtx(i),
            QPLLLOCK                               => '1',
            CPLLLOCK                               => rxCdrLocked_from_cdrLockTimeout(i),--cpllLock_from_gtx(i),
            TXRESETDONE                            => txResetDone_from_gtx(i),
            MMCM_LOCK                              => '1',
            GTTXRESET                              => gtTxReset_from_txResetFsm(i),
            MMCM_RESET                             => open,
            QPLL_RESET                             => open,
            CPLL_RESET                             => cpllReset_from_txResetFsm(i),
            TX_FSM_RESET_DONE                      => txFsmResetDone_from_txResetFsm(i),
            TXUSERRDY                              => txUserRdy_from_txResetFsm(i),
            RUN_PHALIGNMENT                        => runTxPhalignment_from_txResetFsm(i),
            RESET_PHALIGNMENT                      => resetTxPhalignment_from_txResetFsm(i),
            PHALIGNMENT_DONE                       => txPhAlignmentDone_from_txPhaseAlign,
            RETRY_COUNTER                          => open
         );      
      
      rxResetFsm: entity work.xlx_k7v7_gtx_RX_STARTUP_FSM
         generic map (
            EXAMPLE_SIMULATION                     => GBT_BANKS_USER_SETUP(GBT_BANK_ID).SIMULATION,
            GT_TYPE                                => "GTX",                 
            EQ_MODE                                => "DFE",                 
            STABLE_CLOCK_PERIOD                    => STABLE_CLOCK_PERIOD,   
            RETRY_COUNTER_BITWIDTH                 => 8, 
            TX_QPLL_USED                           => false,                
            RX_QPLL_USED                           => false,                 
            PHASE_ALIGNMENT_MANUAL                 => RX_GTX_BUFFBYPASS_MANUAL_MULTILINK)
         port map ( 
            STABLE_CLOCK                           => MGT_CLKS_I.sysClk,
            RXUSERCLK                              => MGT_CLKS_I.rx_wordClk(i),
            SOFT_RESET                             => MGT_I.mgtLink(i).rx_reset,
            DONT_RESET_ON_DATA_ERROR               => '1',
            QPLLREFCLKLOST                         => '0',
            CPLLREFCLKLOST                         => cpllRefClkLost_from_gtx(i),
            QPLLLOCK                               => '1',
            CPLLLOCK                               => rxCdrLocked_from_cdrLockTimeout(i),--cpllLock_from_gtx(i),
            RXRESETDONE                            => txResetDone_from_gtx(i),
            MMCM_LOCK                              => '1',
            RECCLK_STABLE                          => rxCdrLocked_from_cdrLockTimeout(i),
            RECCLK_MONITOR_RESTART                 => '0',
            DATA_VALID                             => '1',
            TXUSERRDY                              => '1',
            GTRXRESET                              => gtRxReset_from_rxResetFsm(i),
            MMCM_RESET                             => open,
            QPLL_RESET                             => open,
            CPLL_RESET                             => open,
            RX_FSM_RESET_DONE                      => rxFsmResetDone_from_rxResetFsm(i),
            RXUSERRDY                              => rxUserRdy_from_rxResetFsm(i),
            RUN_PHALIGNMENT                        => runRxPhAlignment_from_rxResetFsm(i),
            RESET_PHALIGNMENT                      => resetRxPhalignment_from_rxResetFsm(i),
            PHALIGNMENT_DONE                       => rxPhAlignmentDone_from_autoRxPhaseAlign(i) or rxPhAlignmentDone_from_manualRxPhaseAlign,
            RXDFEAGCHOLD                           => rxDfeAgcHold_from_rxResetFsm(i),
            RXDFELFHOLD                            => open,
            RXLPMLFHOLD                            => open,
            RXLPMHFHOLD                            => open,
            RETRY_COUNTER                          => open
         ); 
  
      --------------------------------------------

      cdrLockTimeout: process(MGT_CLKS_I.sysClk)
      begin
         if rising_edge(MGT_CLKS_I.sysClk) then
            if gtRxReset_from_rxResetFsm(i) = '1' then
               timer                               <=  0 ;
               rxCdrLocked_from_cdrLockTimeout(i)  <= '0';
            elsif timer = WAIT_TIME_CDRLOCK then              
               rxCdrLocked_from_cdrLockTimeout(i)  <= '1';                   
            else                                                             
               timer                               <= timer + 1   after DLY;
            end if;                                                          
         end if;
      end process;
   
      ---------------------------------------------
   
      txResetSync: process(txResetDone_from_gtx(i), MGT_CLKS_I.tx_wordClk(i))
      begin
         if txResetDone_from_gtx(i) = '0' then
            txResetDone_r2(i)                      <= '0';
            txResetDone_r (i)                      <= '0';
         elsif rising_edge(MGT_CLKS_I.tx_wordClk(i)) then
            txResetDone_r2(i)                      <= txResetDone_r(i);
            txResetDone_r (i)                      <= txResetDone_from_gtx(i);
         end if;
      end process;      
         
      rxResetSync: process(rxResetDone_from_gtx(i), MGT_CLKS_I.rx_wordClk(i))
      begin
         if rxResetDone_from_gtx(i) = '0' then
            rxResetDone_r3(i)                      <= '0';
            rxResetDone_r2(i)                      <= '0';
            rxResetDone_r (i)                      <= '0';
         elsif rising_edge(MGT_CLKS_I.rx_wordClk(i)) then
            rxResetDone_r3(i)                      <= rxResetDone_r2(i);
            rxResetDone_r2(i)                      <= rxResetDone_r(i);
            rxResetDone_r (i)                      <= rxResetDone_from_gtx(i);
         end if;
      end process;      
      
      --==============================--
      -- MGT internal phase alignment --
      --==============================--
      
      autoRxPhaseAlign_gen: if RX_GTX_BUFFBYPASS_MANUAL_MULTILINK = false generate
                               
         autoRxPhaseAlign: entity work.xlx_k7v7_gtx_AUTO_PHASE_ALIGN    
            generic map (
               GT_TYPE                             => "GTX")
            port map ( 
               STABLE_CLOCK                        => MGT_CLKS_I.sysClk,
               RUN_PHALIGNMENT                     => runRxPhAlignment_from_rxResetFsm(i),
               PHASE_ALIGNMENT_DONE                => rxPhAlignmentDone_from_autoRxPhaseAlign(i),
               PHALIGNDONE                         => rxPhAlignDone_from_gtx(i),
               DLYSRESET                           => rxDlySReset_from_rxPhaseAlign(i),
               DLYSRESETDONE                       => rxDlySResetDone_from_gtx(i),
               RECCLKSTABLE                        => rxCdrLocked_from_cdrLockTimeout(i)
            );
      
      end generate;

      rxPhAlignmentDone_from_manualRxPhaseAlign    <= '0';
      
   end generate; 
   
   manualRxPhaseAlign_gen: if RX_GTX_BUFFBYPASS_MANUAL_MULTILINK = true generate
 
      manualRxPhaseAlign: entity work.xlx_k7v7_gtx_RX_MANUAL_PHASE_ALIGN
         generic map (
            NUMBER_OF_LANES                        => GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS,
            MASTER_LANE_ID                         => 0)
         port map (  
            STABLE_CLOCK                           => MGT_CLKS_I.sysClk,
            RESET_PHALIGNMENT                      => resetRxPhalignment_from_oRgate,   
            RUN_PHALIGNMENT                        => runRxPhalignment_from_oRgate,      
            PHASE_ALIGNMENT_DONE                   => rxPhAlignmentDone_from_manualRxPhaseAlign,
            RXDLYSRESET                            => rxDlySReset_from_rxPhaseAlign,
            RXDLYSRESETDONE                        => rxDlySResetDone_from_gtx,
            RXPHALIGN                              => rxPhAlign_from_rxPhaseAlign,
            RXPHALIGNDONE                          => rxPhAlignDone_from_gtx,
            RXDLYEN                                => rxDlyEn_from_rxPhaseAlign
         );        
         
      resetRxPhalignment_from_oRgate               <= orGate(resetRxPhalignment_from_rxResetFsm);
      runRxPhalignment_from_oRgate                 <= orGate(runRxPhalignment_from_rxResetFsm);                
   
      rxPhAlignmentDone_from_autoRxPhaseAlign      <= (others => '0');
   
   end generate;
      
   manualTxPhaseAlign: entity work.xlx_k7v7_gtx_TX_MANUAL_PHASE_ALIGN
      generic map (
         NUMBER_OF_LANES                           => GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS,
         MASTER_LANE_ID                            => 0)
      port map (     
         STABLE_CLOCK                              => MGT_CLKS_I.sysClk,
         RESET_PHALIGNMENT                         => resetTxPhalignment_from_oRgate,    
         RUN_PHALIGNMENT                           => runTxPhalignment_from_oRgate,    
         PHASE_ALIGNMENT_DONE                      => txPhAlignmentDone_from_txPhaseAlign,
         TXDLYSRESET                               => txDlySReset_from_txPhaseAlign,
         TXDLYSRESETDONE                           => txDlySResetDone_from_gtx,
         TXPHINIT                                  => txPhInit_from_txPhaseAlign,
         TXPHINITDONE                              => txPhInitDone_from_gtx,
         TXPHALIGN                                 => txPhAlign_from_txPhaseAlign,
         TXPHALIGNDONE                             => txPhAlignDone_from_gtx,
         TXDLYEN                                   => txDlyEn_from_txPhaseAlign
      );
   
   resetTxPhalignment_from_oRgate                  <= orGate(resetTxPhalignment_from_txResetFsm);
   runTxPhalignment_from_oRgate                    <= orGate(runTxPhalignment_from_txResetFsm);
   
   --=============-- 
   -- Common core --
   --=============--   
   
   gtxCommon: GTXE2_COMMON
      generic map (
         SIM_RESET_SPEEDUP                         => GT_SIM_GTRESET_SPEEDUP,
         SIM_QPLLREFCLK_SEL                        => ("001"),
         SIM_VERSION                               => "4.0",
         ------------------------------------------   
         BIAS_CFG                                  => (x"0000040000001000"),
         COMMON_CFG                                => (x"00000000"),
         QPLL_CFG                                  => (x"06801C1"),
         QPLL_CLKOUT_CFG                           => ("0000"),
         QPLL_COARSE_FREQ_OVRD                     => ("010000"),
         QPLL_COARSE_FREQ_OVRD_EN                  => ('0'),
         QPLL_CP                                   => ("0000011111"),
         QPLL_CP_MONITOR_EN                        => ('0'),
         QPLL_DMONITOR_SEL                         => ('0'),
         QPLL_FBDIV                                => (QPLL_FBDIV_IN),
         QPLL_FBDIV_MONITOR_EN                     => ('0'),
         QPLL_FBDIV_RATIO                          => (QPLL_FBDIV_RATIO),
         QPLL_INIT_CFG                             => (x"000006"),
         QPLL_LOCK_CFG                             => (x"21E8"),
         QPLL_LPF                                  => ("1111"),
         QPLL_REFCLK_DIV                           => (1))
      port map (  
         DRPADDR                                   => x"00",
         DRPCLK                                    => '0',
         DRPDI                                     => x"0000",
         DRPDO                                     => open,
         DRPEN                                     => '0',
         DRPRDY                                    => open,
         DRPWE                                     => '0',
         ------------------------------------------   
         GTGREFCLK                                 => '0',
         GTNORTHREFCLK0                            => '0',
         GTNORTHREFCLK1                            => '0',
         GTREFCLK0                                 => '0',
         GTREFCLK1                                 => '0',
         GTSOUTHREFCLK0                            => '0',
         GTSOUTHREFCLK1                            => '0',
         ------------------------------------------   
         QPLLDMONITOR                              => open,
         ------------------------------------------   
         QPLLOUTCLK                                => qpllOutClk_from_gtxCommon,
         QPLLOUTREFCLK                             => qpllOutRefClk_from_gtxCommon,
         REFCLKOUTMONITOR                          => open,
         ------------------------------------------   
         QPLLFBCLKLOST                             => open,
         QPLLLOCK                                  => open,
         QPLLLOCKDETCLK                            => '0',
         QPLLLOCKEN                                => '1',
         QPLLOUTRESET                              => '0',
         QPLLPD                                    => '0',
         QPLLREFCLKLOST                            => open,
         QPLLREFCLKSEL                             => "001",
         QPLLRESET                                 => '0',
         QPLLRSVD1                                 => x"0000",
         QPLLRSVD2                                 => "11111",
         ------------------------------------------   
         BGBYPASSB                                 => '1',
         BGMONITORENB                              => '1',
         BGPDB                                     => '1',
         BGRCALOVRD                                => "00000",
         PMARSVD                                   => x"00",
         RCALENB                                   => '1'
      );   
      
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--