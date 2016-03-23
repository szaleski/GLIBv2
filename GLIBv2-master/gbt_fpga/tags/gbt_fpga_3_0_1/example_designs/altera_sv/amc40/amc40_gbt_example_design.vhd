--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           AMC40 - GBT Bank example design                                        
--                                                                                                 
-- Language:              VHDL'93                                                                  
--                                                                                                   
-- Target Device:         AMC40 (Altera Stratix V)                                                       
-- Tool version:          Quartus II 13.1                                                                
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        23/03/2013   3.0       M. Barros Marin   First .vhd module definition           
--
-- Additional Comments:   Note!! This example design instantiates two GBt Banks:
--
--                               - GBT Bank 1: One GBT Link (Standard GBT TX and Latency-Optimized GBT RX)
--
--                               - GBT Bank 2: Three GBT Links (Latency-Optimized GBT TX and Standard GBT RX)      
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

-- Custom libraries and packages:
use work.gbt_bank_package.all;
use work.vendor_specific_gbt_bank_package.all;

-- Libraries for direct instantiation:
library alt_sv_fabric_pll;
library alt_sv_issp_gbtBank1;
library alt_sv_issp_gbtBank2;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity amc40_gbt_example_design is   
   port (   
      
      --===============--
      -- General reset --
      --===============--      
     
      SYS_RESET_N                                             : in  std_logic; 
    
      --===============--
      -- Clocks scheme --
      --===============--

      -- Fabric clock:      
      ----------------

      REF_CLOCK_R5                                            : in  std_logic;   -- Comment:  (GBE REF CLOCK 125MHz)
      
      -- MGT(GT) clock:
      -----------------
      
      MAIN_CLOCK_SEL                                          : out std_logic;  
      --------------------------------------------------------
      REF_CLOCK_L4                                            : in  std_logic;   -- Comment:  (LHC PLL 120MHz)
      
      --==============--
      -- Serial lanes --
      --==============--
      
      -- GBT Bank 1:
      --------------
      
      GBTBANK1_TX_1                                           : out std_logic;
      GBTBANK1_RX_1                                           : in  std_logic;
      
      -- GBT Bank 2:
      --------------
      
      -- GBT Link 1:
      GBTBANK2_TX_1                                           : out std_logic;
      GBTBANK2_RX_1                                           : in  std_logic;
      
      -- GBT Link 2:
      GBTBANK2_TX_2                                           : out std_logic;
      GBTBANK2_RX_2                                           : in  std_logic;
      
      -- GBT Link 3:
      GBTBANK2_TX_3                                           : out std_logic;
      GBTBANK2_RX_3                                           : in  std_logic;
      
      --====================--
      -- Signals forwarding --
      --====================--
      
      -- Pattern match flags:
      -----------------------
      
      -- GBT Bank 1:
      RTM_TX_1_P                                              : out std_logic;
      RTM_TX_1_N                                              : out std_logic;
      
      -- GBT Bank 2:
      RTM_TX_2_P                                              : out std_logic;
      RTM_TX_2_N                                              : out std_logic;
      
      -- Clocks forwarding:
      ---------------------
      
      -- MGT REFCLK:
      AMC_CLOCK_OUT                                         : out std_logic;
      
      -- GBT Bank 1:
      RTM_TX_3_P                                              : out std_logic;
      RTM_TX_3_N                                              : out std_logic;
      --------------------------------------------------------
      THROTTLE_OUT_P                                          : out std_logic;
      THROTTLE_OUT_N                                          : out std_logic;
      
      -- GBT Bank 2:
      RTM_TX_0_P                                              : out std_logic;
      RTM_ORBIT_OUT_P                                         : out std_logic;
      RTM_ORBIT_OUT_N                                         : out std_logic;
      --------------------------------------------------------
      RTM_CLOCK_OUT_P                                         : out std_logic;
      RTM_CLOCK_OUT_N                                         : out std_logic 
      
   );
end amc40_gbt_example_design;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of amc40_gbt_example_design is
   
   --================================ Signal Declarations ================================--

   --==============--
   -- Fabric clock --
   --==============--
   
   signal locked_from_fabricPll                               : std_logic;
   signal fabricClk_from_fabricPll                            : std_logic;
   
   --=========--
   -- Control --
   --=========--
   
   -- GBT Bank 1:
   --------------
   
   signal generalReset_from_gbtBank1_user                     : std_logic;
   signal manualResetMgtTxPll_from_gbtBank1_user              : std_logic;   
   signal manualResetTx_from_gbtBank1_user                    : std_logic; 
   signal manualResetRx_from_gbtBank1_user                    : std_logic; 
   signal testPatterSel_from_gbtBank1_user                    : std_logic_vector(1 downto 0); 
   signal loopBack_from_gbtBank1_user                         : std_logic; 
   signal resetDataErrorSeenFlag_from_gbtBank1_user           : std_logic; 
   signal resetGbtRxReadyLostFlag_from_gbtBank1_user          : std_logic; 
   signal txIsDataSel_from_gbtBank1_user                      : std_logic;   
   --------------------------------------------------      
   signal gbtBank1_latOptGbtBankTx_from_gbtExmplDsgn          : std_logic;
   signal gbtBank1_latOptGbtBankRx_from_gbtExmplDsgn          : std_logic;
   signal gbtBank1_txFrameClkPllLocked_from_gbtExmplDsgn      : std_logic;
   signal gbtBank1_mgtTxPllLocked_from_gbtExmplDsgn           : std_logic;
   signal gbtBank1_mgtReady_from_gbtExmplDsgn                 : std_logic; 
   signal gbtBank1_rxBitSlipNbr_from_gbtExmplDsgn             : std_logic_vector(GBTRX_BITSLIP_NBR_MSB downto 0);
   signal gbtBank1_rxWordClkReady_from_gbtExmplDsgn           : std_logic; 
   signal gbtBank1_rxFrameClkReady_from_gbtExmplDsgn          : std_logic; 
   signal gbtBank1_gbtRxReady_from_gbtExmplDsgn               : std_logic;    
   signal gbtBank1_rxIsData_from_gbtExmplDsgn                 : std_logic;        
   signal gbtBank1_gbtRxReadyLostFlag_from_gbtExmplDsgn       : std_logic; 
   signal gbtBank1_rxDataErrorSeen_from_gbtExmplDsgn          : std_logic; 
   signal gbtBank1_rxExtrDataWidebusErSeen_from_gbtExmplDsgn  : std_logic; 
   signal gbtBank1_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn : std_logic;
   
   -- GBT Bank 2:
   --------------
   
   signal generalReset_from_gbtBank2_user                     : std_logic;      
   signal manualResetMgtTxPll_from_gbtBank2_user              : std_logic;
   signal manualResetTx_from_gbtBank2_user                    : std_logic_vector       (1 to 3); 
   signal manualResetRx_from_gbtBank2_user                    : std_logic_vector       (1 to 3); 
   signal testPatterSel_from_gbtBank2_user                    : std_logic_vector       (1 downto 0); 
   signal loopBack_from_gbtBank2_user                         : std_logic_vector       (1 to 3); 
   signal resetDataErrorSeenFlag_from_gbtBank2_user           : std_logic_vector       (1 to 3); 
   signal resetGbtRxReadyLostFlag_from_gbtBank2_user          : std_logic_vector       (1 to 3);
   signal txIsDataSel_from_gbtBank2_user                      : std_logic_vector       (1 to 3);   
   signal txFrmClkPhAlManualReset_from_gbtBank2_user          : std_logic;   
   signal txFrmClkPhAlStepsGbtLnk_from_gbtBank2_user          : frmClkPhAlSteps_nx6bit (1 to 3);
   signal txFrmClkPhAlStepsEnable_from_gbtBank2_user          : std_logic; 
   signal txFrmClkPhAlStepsTrigger_from_gbtBank2_user         : std_logic;  
   signal txWrdClkMonThresholdUp_from_gbtBank2_user           : std_logic_vector       (7 downto 0);  
   signal txWrdClkMonThresholdLow_from_gbtBank2_user          : std_logic_vector       (7 downto 0);  
   signal txWrdClkMonMgtTxResetEn_from_gbtBank2_user          : std_logic; 
   signal gbtBank2TxFlagSel_from_gbtBank2_user                : std_logic_vector       (1 downto 0); 
   signal gbtBank2RxFlagSel_from_gbtBank2_user                : std_logic_vector       (1 downto 0);
   signal gbtBank2TxWordClkSel_from_gbtBank2_user             : std_logic_vector       (1 downto 0);
   signal gbtBank2RxWordClkSel_from_gbtBank2_user             : std_logic_vector       (1 downto 0);
   signal gbtBank2TxFrameClkSel_from_gbtBank2_user            : std_logic_vector       (1 downto 0);
   --------------------------------------------------      
   signal gbtBank2_latOptGbtBankTx_from_gbtExmplDsgn          : std_logic; 
   signal gbtBank2_latOptGbtBankRx_from_gbtExmplDsgn          : std_logic;
   signal gbtBank2_txFrmClkPhAlLocked_from_gbtExmplDsgn       : std_logic;
   signal gbtBank2_txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn : std_logic;
   signal gbtBank2_mgtTxPllLocked_from_gbtExmplDsgn           : std_logic;
   signal gbtBank2_txWrdClkMonStats_from_gbtExmplDsgn         : std_logic_vector      (7 downto 0); 
   signal gbtBank2_txWrdClkMonPhaseOk_from_gbtExmplDsgn       : std_logic;    
   signal gbtBank2_mgtReady_from_gbtExmplDsgn                 : std_logic_vector      (1 to 3); 
   signal gbtBank2_rxBitSlipNbr_from_gbtExmplDsgn             : gbtRxSlideNbr_mxnbit_A(1 to 3);
   signal gbtBank2_rxWordClkReady_from_gbtExmplDsgn           : std_logic_vector      (1 to 3); 
   signal gbtBank2_rxFrameClkReady_from_gbtExmplDsgn          : std_logic; 
   signal gbtBank2_gbtRxReady_from_gbtExmplDsgn               : std_logic_vector      (1 to 3);    
   signal gbtBank2_rxIsData_from_gbtExmplDsgn                 : std_logic_vector      (1 to 3);        
   signal gbtBank2_gbtRxReadyLostFlag_from_gbtExmplDsgn       : std_logic_vector      (1 to 3); 
   signal gbtBank2_rxDataErrorSeen_from_gbtExmplDsgn          : std_logic_vector      (1 to 3); 
   signal gbtBank2_rxExtrDataWidebusErSeen_from_gbtExmplDsgn  : std_logic_vector      (1 to 3); 
   signal gbtBank2_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn : std_logic_vector      (1 to 3);
   
   --======--
   -- Data --
   --======--
   
   -- GBT Bank 1:
   --------------
   
   signal gbtBank1_txData_from_gbtExmplDsgn                   : std_logic_vector(83 downto 0);
   signal gbtBank1_rxData_from_gbtExmplDsgn                   : std_logic_vector(83 downto 0);
   --------------------------------------------------      
   signal gbtBank1_txExtraDataWidebus_from_gbtExmplDsgn       : std_logic_vector(31 downto 0);
   signal gbtBank1_rxExtraDataWidebus_from_gbtExmplDsgn       : std_logic_vector(31 downto 0);
   --------------------------------------------------
   signal gbtBank1_txExtraDataGbt8b10b_from_gbtExmplDsgn      : std_logic_vector( 3 downto 0);
   signal gbtBank1_rxExtraDataGbt8b10b_from_gbtExmplDsgn      : std_logic_vector( 3 downto 0);          

   -- GBT Bank 2:
   --------------
   
   signal gbtBank2_txData_from_gbtExmplDsgn                   : data_nx84bit_A  (1 to 3);
   signal gbtBank2_rxData_from_gbtExmplDsgn                   : data_nx84bit_A  (1 to 3);
   --------------------------------------------------      
   signal gbtBank2_txExtraDataWidebus_from_gbtExmplDsgn       : extraDataWidebus_nx32bit_A(1 to 3);
   signal gbtBank2_rxExtraDataWidebus_from_gbtExmplDsgn       : extraDataWidebus_nx32bit_A(1 to 3);
   --------------------------------------------------
   signal gbtBank2_txExtraDataGbt8b10b_from_gbtExmplDsgn      : extraDataGbt8b10b_nx4bit_A(1 to 3);
   signal gbtBank2_rxExtraDataGbt8b10b_from_gbtExmplDsgn      : extraDataGbt8b10b_nx4bit_A(1 to 3);
   
   --============================--
   -- In-System Probes & Sources --
   --============================--
   
   -- GBT Bank 1:
   --------------
   
   signal source_from_gbtBank1_inSysSrcAndPrb                 : std_logic_vector( 9 downto 0);  
   signal probe_to_gbtBank1_inSysSrcAndPrb                    : std_logic_vector(18 downto 0);
   
   -- GBT Bank 2:
   --------------
   
   signal source_from_gbtBank2_inSysSrcAndPrb                 : std_logic_vector(69 downto 0);  
   signal probe_to_gbtBank2_inSysSrcAndPrb                    : std_logic_vector(57 downto 0);   
   
   --=====================--
   -- Latency measurement --
   --=====================--

   -- GBT Bank 1:
   --------------
   
   signal gbtBank1_txFrameClk_from_gbtExmplDsgn               : std_logic;
   signal gbtBank1_txWordClk_from_gbtExmplDsgn                : std_logic;
   signal gbtBank1_rxFrameClk_from_gbtExmplDsgn               : std_logic;
   signal gbtBank1_rxWordClk_from_gbtExmplDsgn                : std_logic;
   --------------------------------------------------                                    
   signal gbtBank1_txMatchFlag_from_gbtExmplDsgn              : std_logic;
   signal gbtBank1_rxMatchFlag_from_gbtExmplDsgn              : std_logic;   
   
   -- GBT Bank 2:
   --------------
   
   signal gbtBank2_user_txFrameClk_from_gbtExmplDsgn          : std_logic;
   signal gbtBank2_gbtLink_txFrameClk_from_gbtExmplDsgn       : std_logic_vector(1 to 3);
   signal gbtBank2_txWordClk_from_gbtExmplDsgn                : std_logic_vector(1 to 3);
   signal gbtBank2_rxFrameClk_from_gbtExmplDsgn               : std_logic;
   signal gbtBank2_rxWordClk_from_gbtExmplDsgn                : std_logic_vector(1 to 3);
   --------------------------------------------------                                    
   signal gbtBank2_txMatchFlag_from_gbtExmplDsgn              : std_logic_vector(1 to 3);
   signal gbtBank2_rxMatchFlag_from_gbtExmplDsgn              : std_logic_vector(1 to 3);
   
   --=====================================================================================--   

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
   --==================================== User Logic =====================================--


       --#############################################################################--
     --#################################################################################--
   --#############################                           #############################--
   --#############################  GBT Bank example design  #############################--
   --#############################                           #############################--
     --#################################################################################--
       --#############################################################################--
       
       
   --==============--
   -- Fabric Clock --
   --==============--

   -- Fabric clock control:
   ------------------------
   
   MAIN_CLOCK_SEL                                             <= '0';
   
   -- Fabric clock PLL:
   --------------------
   
   -- Comment: * The Input of the fabric PLL is 125MHz. 
   --
   --          * The Input of the fabric PLL is 50MHz.
   
   fabricPll: alt_sv_fabric_pll.alt_sv_fabric_pll
      port map (
         REFCLK                                               => REF_CLOCK_R5,   -- Comment: 125MHz
         RST                                                  => '0',
         OUTCLK_0                                             => fabricClk_from_fabricPll,   -- Comment: 50MHz
         LOCKED                                               => locked_from_fabricPll
      );
      
   --=====================================--
   -- Stratix V - GBT Bank example design --
   --=====================================--
   
   gbtExmplDsgn: entity work.alt_sv_gbt_example_design
      port map (
         -- Resets scheme:
         GBTBANK1_GENERAL_RESET_I                             => generalReset_from_gbtBank1_user,                
         GBTBANK1_MANUAL_RESET_GT_TX_PLL_I                    => manualResetMgtTxPll_from_gbtBank1_user,
         GBTBANK1_MANUAL_RESET_TX_I                           => manualResetTx_from_gbtBank1_user,                
         GBTBANK1_MANUAL_RESET_RX_I                           => manualResetRx_from_gbtBank1_user,                
         -----------------------------------------------------
         GBTBANK2_GENERAL_RESET_I                             => generalReset_from_gbtBank2_user,                  
         GBTBANK2_MANUAL_RESET_GT_TX_PLL_I                    => manualResetMgtTxPll_from_gbtBank2_user,
         GBTBANK2_MANUAL_RESET_TX_I                           => manualResetTx_from_gbtBank2_user,                  
         GBTBANK2_MANUAL_RESET_RX_I                           => manualResetRx_from_gbtBank2_user,                  
         -- Clocks scheme:
         FABRIC_CLK_I                                         => fabricClk_from_fabricPll,                  
         -----------------------------------------------------
         GBTBANK1_MGT_REFCLK_I                                => REF_CLOCK_L4,                    
         GBTBANK2_MGT_REFCLK_I                                => REF_CLOCK_L4,                    
         -- Serial lanes:
         GBTBANK1_MGT_TX                                      => GBTBANK1_TX_1,                      
         GBTBANK1_MGT_RX                                      => GBTBANK1_RX_1,                      
         -----------------------------------------------------
         GBTBANK2_MGT_TX(1)                                   => GBTBANK2_TX_1,                      
         GBTBANK2_MGT_RX(1)                                   => GBTBANK2_RX_1, 
         GBTBANK2_MGT_TX(2)                                   => GBTBANK2_TX_2,                      
         GBTBANK2_MGT_RX(2)                                   => GBTBANK2_RX_2,          
         GBTBANK2_MGT_TX(3)                                   => GBTBANK2_TX_3,                      
         GBTBANK2_MGT_RX(3)                                   => GBTBANK2_RX_3,                   
         -- General control:
         GBTBANK1_LOOPBACK_I                                  => loopBack_from_gbtBank1_user,                 
         GBTBANK1_TX_ISDATA_SEL_I                             => txIsDataSel_from_gbtBank1_user,
         GBTBANK1_LATOPT_GBTBANK_TX_O                         => gbtBank1_latOptGbtBankTx_from_gbtExmplDsgn,           
         GBTBANK1_LATOPT_GBTBANK_RX_O                         => gbtBank1_latOptGbtBankRx_from_gbtExmplDsgn,          
         GBTBANK1_TX_FRAMECLK_PLL_LOCKED_O                    => gbtBank1_txFrameClkPllLocked_from_gbtExmplDsgn,
         GBTBANK1_MGT_TX_PLL_LOCKED_O                         => gbtBank1_mgtTxPllLocked_from_gbtExmplDsgn,
         GBTBANK1_MGT_READY_O                                 => gbtBank1_mgtReady_from_gbtExmplDsgn,         
         GBTBANK1_RX_BITSLIP_NUMBER_O                         => gbtBank1_rxBitSlipNbr_from_gbtExmplDsgn,                      
         GBTBANK1_RX_WORDCLK_READY_O                          => gbtBank1_rxWordClkReady_from_gbtExmplDsgn,         
         GBTBANK1_RX_FRAMECLK_READY_O                         => gbtBank1_rxFrameClkReady_from_gbtExmplDsgn,         
         GBTBANK1_GBT_RX_READY_O                              => gbtBank1_gbtRxReady_from_gbtExmplDsgn,                    
         GBTBANK1_RX_ISDATA_FLAG_O                            => gbtBank1_rxIsData_from_gbtExmplDsgn,          
         -----------------------------------------------------
         GBTBANK2_LOOPBACK_I                                  => loopBack_from_gbtBank2_user,   
         GBTBANK2_TX_ISDATA_SEL_I                             => txIsDataSel_from_gbtBank2_user,   
         GBTBANK2_LATOPT_GBTBANK_TX_O                         => gbtBank2_latOptGbtBankTx_from_gbtExmplDsgn,    
         GBTBANK2_LATOPT_GBTBANK_RX_O                         => gbtBank2_latOptGbtBankRx_from_gbtExmplDsgn, 
         GBTBANK2_TX_FRMCLKPHAL_MANUAL_RESET_I                => txFrmClkPhAlManualReset_from_gbtBank2_user,  
         GBTBANK2_TX_FRMCLKPHAL_STEPSGBTLNK_I                 => txFrmClkPhAlStepsGbtLnk_from_gbtBank2_user,     
         GBTBANK2_TX_FRMCLKPHAL_STEPSENABLE_I                 => txFrmClkPhAlStepsEnable_from_gbtBank2_user,        
         GBTBANK2_TX_FRMCLKPHAL_STEPSTRIGGER_I                => txFrmClkPhAlStepsTrigger_from_gbtBank2_user,        
         GBTBANK2_TX_FRMCLKPHAL_LOCKED_O                      => gbtBank2_txFrmClkPhAlLocked_from_gbtExmplDsgn,           
         GBTBANK2_TX_FRMCLKPHAL_PHSHIFTDONE_O                 => gbtBank2_txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn,               
         GBTBANK2_TX_WRDCLKMON_THRESHOLD_UP_I                 => txWrdClkMonThresholdUp_from_gbtBank2_user,      
         GBTBANK2_TX_WRDCLKMON_THRESHOLD_LOW_I                => txWrdClkMonThresholdLow_from_gbtBank2_user,          
         GBTBANK2_TX_WRDCLKMON_MGTTXRESET_EN_I                => txWrdClkMonMgtTxResetEn_from_gbtBank2_user,
         GBTBANK2_TX_WRDCLKMON_STATS_O                        => gbtBank2_txWrdClkMonStats_from_gbtExmplDsgn,            
         GBTBANK2_TX_WRDCLKMON_PHASEOK_O                      => gbtBank2_txWrdClkMonPhaseOk_from_gbtExmplDsgn,
         GBTBANK2_MGT_TX_PLL_LOCKED_O                         => gbtBank2_mgtTxPllLocked_from_gbtExmplDsgn,
         GBTBANK2_MGT_READY_O                                 => gbtBank2_mgtReady_from_gbtExmplDsgn,   
         GBTBANK2_RX_BITSLIP_NUMBER_O                         => gbtBank2_rxBitSlipNbr_from_gbtExmplDsgn,        
         GBTBANK2_RX_WORDCLK_READY_O                          => gbtBank2_rxWordClkReady_from_gbtExmplDsgn,        
         GBTBANK2_RX_FRAMECLK_READY_O                         => gbtBank2_rxFrameClkReady_from_gbtExmplDsgn,   
         GBTBANK2_GBT_RX_READY_O                              => gbtBank2_gbtRxReady_from_gbtExmplDsgn,           
         GBTBANK2_RX_ISDATA_FLAG_O                            => gbtBank2_rxIsData_from_gbtExmplDsgn,    
         -- GBT Bank data
         GBTBANK1_TX_DATA_O                                   => gbtBank1_txData_from_gbtExmplDsgn,  
         GBTBANK1_TX_EXTRA_DATA_WIDEBUS_O                     => gbtBank1_txExtraDataWidebus_from_gbtExmplDsgn,
         GBTBANK1_TX_EXTRA_DATA_GBT8B10B_O                    => gbtBank1_txExtraDataGbt8b10b_from_gbtExmplDsgn,          
         GBTBANK1_RX_DATA_O                                   => gbtBank1_rxData_from_gbtExmplDsgn,          
         GBTBANK1_RX_EXTRA_DATA_WIDEBUS_O                     => gbtBank1_rxExtraDataWidebus_from_gbtExmplDsgn,           
         GBTBANK1_RX_EXTRA_DATA_GBT8B10B_O                    => gbtBank1_rxExtraDataGbt8b10b_from_gbtExmplDsgn,          
         -----------------------------------------------------
         GBTBANK2_TX_DATA_O                                   => gbtBank2_txData_from_gbtExmplDsgn,         
         GBTBANK2_TX_EXTRA_DATA_WIDEBUS_O                     => gbtBank2_txExtraDataWidebus_from_gbtExmplDsgn,       
         GBTBANK2_TX_EXTRA_DATA_GBT8B10B_O                    => gbtBank2_txExtraDataGbt8b10b_from_gbtExmplDsgn,        
         GBTBANK2_RX_DATA_O                                   => gbtBank2_rxData_from_gbtExmplDsgn,                 
         GBTBANK2_RX_EXTRA_DATA_WIDEBUS_O                     => gbtBank2_rxExtraDataWidebus_from_gbtExmplDsgn,         
         GBTBANK2_RX_EXTRA_DATA_GBT8B10B_O                    => gbtBank2_rxExtraDataGbt8b10b_from_gbtExmplDsgn,        
         -- Test control:
         GBTBANK1_TEST_PATTERN_SEL_I                          => testPatterSel_from_gbtBank1_user,    
         GBTBANK1_RESET_DATA_ERR_FLAG_I                       => resetDataErrorSeenFlag_from_gbtBank1_user,    
         GBTBANK1_RESET_GBTRXREADY_LOST_FLAG_I                => resetGbtRxReadyLostFlag_from_gbtBank1_user,    
         GBTBANK1_GBTRXREADY_LOST_FLAG_O                      => gbtBank1_gbtRxReadyLostFlag_from_gbtExmplDsgn,    
         GBTBANK1_RXDATA_ERR_FLAG_O                           => gbtBank1_rxDataErrorSeen_from_gbtExmplDsgn,    
         GBTBANK1_RXEXTRADATA_WIDEBUS_ERR_FLAG_O              => gbtBank1_rxExtrDataWidebusErSeen_from_gbtExmplDsgn,     
         GBTBANK1_RXEXTRADATA_GBT8B10B_ERR_FLAG_O             => gbtBank1_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn,  
         -----------------------------------------------------
         GBTBANK2_TEST_PATTERN_SEL_I                          => testPatterSel_from_gbtBank2_user,   
         GBTBANK2_RESET_DATA_ERR_FLAG_I                       => resetDataErrorSeenFlag_from_gbtBank2_user,   
         GBTBANK2_RESET_GBTRXREADY_LOST_FLAG_I                => resetGbtRxReadyLostFlag_from_gbtBank2_user,   
         GBTBANK2_GBTRXREADY_LOST_FLAG_O                      => gbtBank2_gbtRxReadyLostFlag_from_gbtExmplDsgn,   
         GBTBANK2_RXDATA_ERR_FLAG_O                           => gbtBank2_rxDataErrorSeen_from_gbtExmplDsgn,   
         GBTBANK2_RXEXTRADATA_WIDEBUS_ERR_FLAG_O              => gbtBank2_rxExtrDataWidebusErSeen_from_gbtExmplDsgn,   
         GBTBANK2_RXEXTRADATA_GBT8B10B_ERR_FLAG_O             => gbtBank2_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn,   
         -- Latency measurement
         GBTBANK1_TX_FRAMECLK_O                               => gbtBank1_txFrameClk_from_gbtExmplDsgn,     
         GBTBANK1_TX_WORDCLK_O                                => gbtBank1_txWordClk_from_gbtExmplDsgn,      
         GBTBANK1_RX_FRAMECLK_O                               => gbtBank1_rxFrameClk_from_gbtExmplDsgn,     
         GBTBANK1_RX_WORDCLK_O                                => gbtBank1_rxWordClk_from_gbtExmplDsgn,      
         GBTBANK1_TX_MATCHFLAG_O                              => gbtBank1_txMatchFlag_from_gbtExmplDsgn,     
         GBTBANK1_RX_MATCHFLAG_O                              => gbtBank1_rxMatchFlag_from_gbtExmplDsgn,     
         -----------------------------------------------------
         GBTBANK2_USER_TX_FRAMECLK_O                          => gbtBank2_user_txFrameClk_from_gbtExmplDsgn,               
         GBTBANK2_GBTLINK_TX_FRAMECLK_O                       => gbtBank2_gbtLink_txFrameClk_from_gbtExmplDsgn,               
         GBTBANK2_TX_WORDCLK_O                                => gbtBank2_txWordClk_from_gbtExmplDsgn,                
         GBTBANK2_RX_FRAMECLK_O                               => gbtBank2_rxFrameClk_from_gbtExmplDsgn,               
         GBTBANK2_RX_WORDCLK_O                                => gbtBank2_rxWordClk_from_gbtExmplDsgn,                
         GBTBANK2_TX_MATCHFLAG_O                              => gbtBank2_txMatchFlag_from_gbtExmplDsgn,              
         GBTBANK2_RX_MATCHFLAG_O                              => gbtBank2_rxMatchFlag_from_gbtExmplDsgn  
      );
      
      
       --##############################################################################--
     --##################################################################################--
   --##########################                              #############################--
   --##########################  GBT Bank 1 (related logic)  #############################--
   --##########################                              #############################--
     --##################################################################################--
       --##############################################################################--

   
   --==============--   
   -- Test control --   
   --==============--
   
   -- Signals mapping:
   -------------------
   
   loopBack_from_gbtBank1_user                                <= source_from_gbtBank1_inSysSrcAndPrb(0);
   generalReset_from_gbtBank1_user                            <= source_from_gbtBank1_inSysSrcAndPrb(1);          
   manualResetMgtTxPll_from_gbtBank1_user                     <= source_from_gbtBank1_inSysSrcAndPrb(2);    
   manualResetTx_from_gbtBank1_user                           <= source_from_gbtBank1_inSysSrcAndPrb(3);
   manualResetRx_from_gbtBank1_user                           <= source_from_gbtBank1_inSysSrcAndPrb(4);
   testPatterSel_from_gbtBank1_user                           <= source_from_gbtBank1_inSysSrcAndPrb(6 downto  5); 
   txIsDataSel_from_gbtBank1_user                             <= source_from_gbtBank1_inSysSrcAndPrb(7);
   resetDataErrorSeenFlag_from_gbtBank1_user                  <= source_from_gbtBank1_inSysSrcAndPrb(8);
   resetGbtRxReadyLostFlag_from_gbtBank1_user                 <= source_from_gbtBank1_inSysSrcAndPrb(9);
   -----------------------------------------------------------
   probe_to_gbtBank1_inSysSrcAndPrb( 0)                       <= gbtBank1_latOptGbtBankTx_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb( 1)                       <= gbtBank1_latOptGbtBankRx_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb( 2)                       <= gbtBank1_txFrameClkPllLocked_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb( 3)                       <= gbtBank1_mgtTxPllLocked_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb( 4)                       <= gbtBank1_mgtReady_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb( 5)                       <= gbtBank1_rxWordClkReady_from_gbtExmplDsgn;    
   probe_to_gbtBank1_inSysSrcAndPrb( 6)                       <= gbtBank1_rxFrameClkReady_from_gbtExmplDsgn;    
   probe_to_gbtBank1_inSysSrcAndPrb( 7)                       <= gbtBank1_gbtRxReady_from_gbtExmplDsgn;   
   probe_to_gbtBank1_inSysSrcAndPrb(13 downto 8)              <= gbtBank1_rxBitSlipNbr_from_gbtExmplDsgn;       
   probe_to_gbtBank1_inSysSrcAndPrb(14)                       <= gbtBank1_rxIsData_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb(15)                       <= gbtBank1_gbtRxReadyLostFlag_from_gbtExmplDsgn;  
   probe_to_gbtBank1_inSysSrcAndPrb(16)                       <= gbtBank1_rxDataErrorSeen_from_gbtExmplDsgn;   
   probe_to_gbtBank1_inSysSrcAndPrb(17)                       <= gbtBank1_rxExtrDataWidebusErSeen_from_gbtExmplDsgn;
   probe_to_gbtBank1_inSysSrcAndPrb(18)                       <= gbtBank1_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn;
   
   -- In-system sources & probes:
   ------------------------------

   -- Comment: * In-System Sources and Probes is used to control the example design as well as for transmitted and received data analysis.
   --
   --          * After FPGA configuration using, open the project "amc40_gbt_example_design.spf" 
   --            that can be found in:
   --            "..\example_designs\altera_sv\amc40\issp_project\".  
   
   gbtBank1_inSysSrcAndPrb: entity alt_sv_issp_gbtBank1.alt_sv_issp_gbtBank1
      port map (
         PROBE                                                => probe_to_gbtBank1_inSysSrcAndPrb,
         SOURCE_CLK                                           => gbtBank1_txFrameClk_from_gbtExmplDsgn,
         SOURCE_ENA                                           => '1',
         SOURCE                                               => source_from_gbtBank1_inSysSrcAndPrb
      );
 
 
       --##############################################################################--
     --##################################################################################--
   --##########################                              #############################--
   --##########################  GBT Bank 2 (related logic)  #############################--
   --##########################                              #############################--
     --##################################################################################--
       --##############################################################################--


   --==============--   
   -- Test control --   
   --==============--
   
   -- Signals mapping:
   -------------------
   
   loopBack_from_gbtBank2_user(1)                             <= source_from_gbtBank2_inSysSrcAndPrb( 0);
   loopBack_from_gbtBank2_user(2)                             <= source_from_gbtBank2_inSysSrcAndPrb( 1);
   loopBack_from_gbtBank2_user(3)                             <= source_from_gbtBank2_inSysSrcAndPrb( 2);
   generalReset_from_gbtBank2_user                            <= source_from_gbtBank2_inSysSrcAndPrb( 3);         
   manualResetMgtTxPll_from_gbtBank2_user                     <= source_from_gbtBank2_inSysSrcAndPrb( 4);    
   manualResetTx_from_gbtBank2_user(1)                        <= source_from_gbtBank2_inSysSrcAndPrb( 5);
   manualResetTx_from_gbtBank2_user(2)                        <= source_from_gbtBank2_inSysSrcAndPrb( 6);
   manualResetTx_from_gbtBank2_user(3)                        <= source_from_gbtBank2_inSysSrcAndPrb( 7);
   manualResetRx_from_gbtBank2_user(1)                        <= source_from_gbtBank2_inSysSrcAndPrb( 8);
   manualResetRx_from_gbtBank2_user(2)                        <= source_from_gbtBank2_inSysSrcAndPrb( 9);
   manualResetRx_from_gbtBank2_user(3)                        <= source_from_gbtBank2_inSysSrcAndPrb(10);
   txWrdClkMonThresholdUp_from_gbtBank2_user                  <= source_from_gbtBank2_inSysSrcAndPrb(18 downto 11);
   txWrdClkMonThresholdLow_from_gbtBank2_user                 <= source_from_gbtBank2_inSysSrcAndPrb(26 downto 19);
   txWrdClkMonMgtTxResetEn_from_gbtBank2_user                 <= source_from_gbtBank2_inSysSrcAndPrb(27);
   testPatterSel_from_gbtBank2_user                           <= source_from_gbtBank2_inSysSrcAndPrb(29 downto 28); 
   txIsDataSel_from_gbtBank2_user(1)                          <= source_from_gbtBank2_inSysSrcAndPrb(30);
   txIsDataSel_from_gbtBank2_user(2)                          <= source_from_gbtBank2_inSysSrcAndPrb(31);
   txIsDataSel_from_gbtBank2_user(3)                          <= source_from_gbtBank2_inSysSrcAndPrb(32);
   resetDataErrorSeenFlag_from_gbtBank2_user (1)              <= source_from_gbtBank2_inSysSrcAndPrb(33);
   resetDataErrorSeenFlag_from_gbtBank2_user (2)              <= source_from_gbtBank2_inSysSrcAndPrb(34);
   resetDataErrorSeenFlag_from_gbtBank2_user (3)              <= source_from_gbtBank2_inSysSrcAndPrb(35);
   resetGbtRxReadyLostFlag_from_gbtBank2_user(1)              <= source_from_gbtBank2_inSysSrcAndPrb(36);
   resetGbtRxReadyLostFlag_from_gbtBank2_user(2)              <= source_from_gbtBank2_inSysSrcAndPrb(37);
   resetGbtRxReadyLostFlag_from_gbtBank2_user(3)              <= source_from_gbtBank2_inSysSrcAndPrb(38);
   txFrmClkPhAlManualReset_from_gbtBank2_user                 <= source_from_gbtBank2_inSysSrcAndPrb(39);
   txFrmClkPhAlStepsGbtLnk_from_gbtBank2_user(1)              <= source_from_gbtBank2_inSysSrcAndPrb(45 downto 40);     
   txFrmClkPhAlStepsGbtLnk_from_gbtBank2_user(2)              <= source_from_gbtBank2_inSysSrcAndPrb(51 downto 46);     
   txFrmClkPhAlStepsGbtLnk_from_gbtBank2_user(3)              <= source_from_gbtBank2_inSysSrcAndPrb(57 downto 52);     
   txFrmClkPhAlStepsEnable_from_gbtBank2_user                 <= source_from_gbtBank2_inSysSrcAndPrb(58);   
   txFrmClkPhAlStepsTrigger_from_gbtBank2_user                <= source_from_gbtBank2_inSysSrcAndPrb(59);
   gbtBank2TxFlagSel_from_gbtBank2_user                       <= source_from_gbtBank2_inSysSrcAndPrb(61 downto 60);
   gbtBank2RxFlagSel_from_gbtBank2_user                       <= source_from_gbtBank2_inSysSrcAndPrb(63 downto 62);
   gbtBank2TxWordClkSel_from_gbtBank2_user                    <= source_from_gbtBank2_inSysSrcAndPrb(65 downto 64);
   gbtBank2RxWordClkSel_from_gbtBank2_user                    <= source_from_gbtBank2_inSysSrcAndPrb(67 downto 66);
   gbtBank2TxFrameClkSel_from_gbtBank2_user                   <= source_from_gbtBank2_inSysSrcAndPrb(69 downto 68);   
   -----------------------------------------------------------
   probe_to_gbtBank2_inSysSrcAndPrb( 0)                       <= gbtBank2_latOptGbtBankTx_from_gbtExmplDsgn;
   probe_to_gbtBank2_inSysSrcAndPrb( 1)                       <= gbtBank2_latOptGbtBankRx_from_gbtExmplDsgn;
   probe_to_gbtBank2_inSysSrcAndPrb( 2)                       <= locked_from_fabricPll;
   probe_to_gbtBank2_inSysSrcAndPrb( 3)                       <= gbtBank2_mgtTxPllLocked_from_gbtExmplDsgn;
   probe_to_gbtBank2_inSysSrcAndPrb(11 downto  4)             <= gbtBank2_txWrdClkMonStats_from_gbtExmplDsgn;
   probe_to_gbtBank2_inSysSrcAndPrb(12)                       <= gbtBank2_txWrdClkMonPhaseOk_from_gbtExmplDsgn;
   probe_to_gbtBank2_inSysSrcAndPrb(13)                       <= gbtBank2_mgtReady_from_gbtExmplDsgn(1);
   probe_to_gbtBank2_inSysSrcAndPrb(14)                       <= gbtBank2_mgtReady_from_gbtExmplDsgn(2);
   probe_to_gbtBank2_inSysSrcAndPrb(15)                       <= gbtBank2_mgtReady_from_gbtExmplDsgn(3);
   probe_to_gbtBank2_inSysSrcAndPrb(16)                       <= gbtBank2_rxWordClkReady_from_gbtExmplDsgn(1);    
   probe_to_gbtBank2_inSysSrcAndPrb(17)                       <= gbtBank2_rxWordClkReady_from_gbtExmplDsgn(2);    
   probe_to_gbtBank2_inSysSrcAndPrb(18)                       <= gbtBank2_rxWordClkReady_from_gbtExmplDsgn(3);    
   probe_to_gbtBank2_inSysSrcAndPrb(19)                       <= gbtBank2_rxFrameClkReady_from_gbtExmplDsgn;    
   probe_to_gbtBank2_inSysSrcAndPrb(20)                       <= gbtBank2_gbtRxReady_from_gbtExmplDsgn(1);   
   probe_to_gbtBank2_inSysSrcAndPrb(21)                       <= gbtBank2_gbtRxReady_from_gbtExmplDsgn(2);   
   probe_to_gbtBank2_inSysSrcAndPrb(22)                       <= gbtBank2_gbtRxReady_from_gbtExmplDsgn(3);   
   probe_to_gbtBank2_inSysSrcAndPrb(28 downto 23)             <= gbtBank2_rxBitSlipNbr_from_gbtExmplDsgn(1);          
   probe_to_gbtBank2_inSysSrcAndPrb(34 downto 29)             <= gbtBank2_rxBitSlipNbr_from_gbtExmplDsgn(2);          
   probe_to_gbtBank2_inSysSrcAndPrb(40 downto 35)             <= gbtBank2_rxBitSlipNbr_from_gbtExmplDsgn(3);          
   probe_to_gbtBank2_inSysSrcAndPrb(41)                       <= gbtBank2_rxIsData_from_gbtExmplDsgn(1);
   probe_to_gbtBank2_inSysSrcAndPrb(42)                       <= gbtBank2_rxIsData_from_gbtExmplDsgn(2);
   probe_to_gbtBank2_inSysSrcAndPrb(43)                       <= gbtBank2_rxIsData_from_gbtExmplDsgn(3);
   probe_to_gbtBank2_inSysSrcAndPrb(44)                       <= gbtBank2_gbtRxReadyLostFlag_from_gbtExmplDsgn(1);  
   probe_to_gbtBank2_inSysSrcAndPrb(45)                       <= gbtBank2_gbtRxReadyLostFlag_from_gbtExmplDsgn(2);  
   probe_to_gbtBank2_inSysSrcAndPrb(46)                       <= gbtBank2_gbtRxReadyLostFlag_from_gbtExmplDsgn(3);  
   probe_to_gbtBank2_inSysSrcAndPrb(47)                       <= gbtBank2_rxDataErrorSeen_from_gbtExmplDsgn(1);   
   probe_to_gbtBank2_inSysSrcAndPrb(48)                       <= gbtBank2_rxDataErrorSeen_from_gbtExmplDsgn(2);   
   probe_to_gbtBank2_inSysSrcAndPrb(49)                       <= gbtBank2_rxDataErrorSeen_from_gbtExmplDsgn(3);   
   probe_to_gbtBank2_inSysSrcAndPrb(50)                       <= gbtBank2_rxExtrDataWidebusErSeen_from_gbtExmplDsgn(1);
   probe_to_gbtBank2_inSysSrcAndPrb(51)                       <= gbtBank2_rxExtrDataWidebusErSeen_from_gbtExmplDsgn(2);
   probe_to_gbtBank2_inSysSrcAndPrb(52)                       <= gbtBank2_rxExtrDataWidebusErSeen_from_gbtExmplDsgn(3);
   probe_to_gbtBank2_inSysSrcAndPrb(53)                       <= gbtBank2_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn(1);
   probe_to_gbtBank2_inSysSrcAndPrb(54)                       <= gbtBank2_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn(2);
   probe_to_gbtBank2_inSysSrcAndPrb(55)                       <= gbtBank2_rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn(3);
   probe_to_gbtBank2_inSysSrcAndPrb(56)                       <= gbtBank2_txFrmClkPhAlLocked_from_gbtExmplDsgn;       
   probe_to_gbtBank2_inSysSrcAndPrb(57)                       <= gbtBank2_txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn; 
   
   -- In-system sources & probes:
   ------------------------------

   -- Comment: * In-System Sources and Probes is used to control the example design as well as for transmitted and received data analysis.
   --
   --          * After FPGA configuration using, open the project "amc40_gbt_example_design.spf" 
   --            that can be found in:
   --            "..\example_designs\altera_sv\amc40\issp_project\".  
   
   gbtBank2_inSysSrcAndPrb: entity alt_sv_issp_gbtBank2.alt_sv_issp_gbtBank2
      port map (
         PROBE                                                => probe_to_gbtBank2_inSysSrcAndPrb,
         SOURCE_CLK                                           => gbtBank1_txFrameClk_from_gbtExmplDsgn,   -- Comment: Note!! TX_FRAMECLK from GBT Bank 1 is used   
         SOURCE_ENA                                           => '1',                                     --          because the PLL that generates this clock
         SOURCE                                               => source_from_gbtBank2_inSysSrcAndPrb      --          ("gbtBank1_txFrmClkStdPll")is reset automatically
      );                                                                                                  --          only once after power up.

      
       --#############################################################################--
     --#################################################################################--
   --#############################                       ################################--
   --#############################  Latency measurement  ################################--
   --#############################                       ################################--
     --#################################################################################--
       --#############################################################################--

       
   --=====================--
   -- Pattern match flags --
   --=====================--
   
   -- Comment: * The latency of the link can be measured using an oscilloscope by comparing 
   --            the TX FLAG with the RX FLAG.
   --
   --          * The COUNTER TEST PATTERN must be used for this test. 
   
   -- GBT Bank 1:
   --------------
   
   RTM_TX_1_P                                                 <= gbtBank1_txMatchFlag_from_gbtExmplDsgn;   
   -----------------------------------------------------------
   RTM_TX_1_N                                                 <= gbtBank1_rxMatchFlag_from_gbtExmplDsgn;
   
   -- GBT Bank 2:
   --------------

   THROTTLE_OUT_P                                             <= gbtBank2_txMatchFlag_from_gbtExmplDsgn(3) when gbtBank2TxFlagSel_from_gbtBank2_user = "11" else
                                                                 gbtBank2_txMatchFlag_from_gbtExmplDsgn(2) when gbtBank2TxFlagSel_from_gbtBank2_user = "10" else
                                                                 gbtBank2_txMatchFlag_from_gbtExmplDsgn(1) when gbtBank2TxFlagSel_from_gbtBank2_user = "01" else
                                                                 '0';
   -----------------------------------------------------------
   THROTTLE_OUT_N                                             <= gbtBank2_rxMatchFlag_from_gbtExmplDsgn(3) when gbtBank2RxFlagSel_from_gbtBank2_user = "11" else
                                                                 gbtBank2_rxMatchFlag_from_gbtExmplDsgn(2) when gbtBank2RxFlagSel_from_gbtBank2_user = "10" else
                                                                 gbtBank2_rxMatchFlag_from_gbtExmplDsgn(1) when gbtBank2RxFlagSel_from_gbtBank2_user = "01" else
                                                                 '0';

   --===================--
   -- Clocks forwarding --
   --===================--

   -- Comment: * The forwarding of the clocks allows to check the phase alignment of the different
   --            clocks using an oscilloscope.
   --
   --          * Note!! If RX DATA comes from another board with a different reference clock, 
   --                   then the TX_FRAMECLK/TX_WORDCLK domains are asynchronous with respect to the
   --                   TX_FRAMECLK/TX_WORDCLK domains.
   
   -- MGT REFCLK:
   --------------

   AMC_CLOCK_OUT                                              <= REF_CLOCK_L4;
   
   -- GBT Bank 1:
   --------------
   
   RTM_TX_2_P                                                 <= gbtBank1_txFrameClk_from_gbtExmplDsgn;
   
   RTM_TX_2_N                                                 <= gbtBank1_txWordClk_from_gbtExmplDsgn;
   
   RTM_TX_3_P                                                 <= gbtBank1_rxFrameClk_from_gbtExmplDsgn;  
   
   RTM_TX_3_N                                                 <= gbtBank1_rxWordClk_from_gbtExmplDsgn;
   
   -- GBT Bank 2:
   
   RTM_TX_0_P                                                 <= gbtBank2_user_txFrameClk_from_gbtExmplDsgn;
   -----------------------------------------------------------
   RTM_ORBIT_OUT_P                                            <= gbtBank2_gbtLink_txFrameClk_from_gbtExmplDsgn(3) when gbtBank2TxFrameClkSel_from_gbtBank2_user = "11" else 
                                                                 gbtBank2_gbtLink_txFrameClk_from_gbtExmplDsgn(2) when gbtBank2TxFrameClkSel_from_gbtBank2_user = "10" else
                                                                 gbtBank2_gbtLink_txFrameClk_from_gbtExmplDsgn(1) when gbtBank2TxFrameClkSel_from_gbtBank2_user = "01" else                                                                  
                                                                 '0';
   
   RTM_CLOCK_OUT_P                                            <= gbtBank2_txWordClk_from_gbtExmplDsgn(3) when gbtBank2TxWordClkSel_from_gbtBank2_user = "11" else 
                                                                 gbtBank2_txWordClk_from_gbtExmplDsgn(2) when gbtBank2TxWordClkSel_from_gbtBank2_user = "10" else 
                                                                 gbtBank2_txWordClk_from_gbtExmplDsgn(1) when gbtBank2TxWordClkSel_from_gbtBank2_user = "01" else 
                                                                 '0';
   
   RTM_ORBIT_OUT_N                                            <= gbtBank2_rxFrameClk_from_gbtExmplDsgn;
   
   RTM_CLOCK_OUT_N                                            <= gbtBank2_rxWordClk_from_gbtExmplDsgn(3) when gbtBank2RxWordClkSel_from_gbtBank2_user = "11" else 
                                                                 gbtBank2_rxWordClk_from_gbtExmplDsgn(2) when gbtBank2RxWordClkSel_from_gbtBank2_user = "10" else 
                                                                 gbtBank2_rxWordClk_from_gbtExmplDsgn(1) when gbtBank2RxWordClkSel_from_gbtBank2_user = "01" else
                                                                 '0';
                                                                 
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--