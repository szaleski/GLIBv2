--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Cyclone V GT FPGA Development Board - GBT Bank example design                                        
--                                                                                                 
-- Language:              VHDL'93                                                                  
--                                                                                                   
-- Target Device:         Cyclone V GT FPGA Development Board (Altera Cyclone V GT)                                                       
-- Tool version:          Quartus II 13.1                                                                
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        20/08/2013   3.0       M. Barros Marin   First .vhd module definition           
--
-- Additional Comments:   Note!! Only ONE GBT Bank with ONE link can be used in this example design.     
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
use work.vendor_specific_gbt_bank_package.all;

-- Libraries for direct instantiation:
library alt_cv_gbt_tx_frameclk_stdpll;
library alt_cv_issp;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity cvGtFpgaDevkit_gbt_example_design is   
   port (   
      
      --===============--
      -- General reset --
      --===============--      
     
      USER_PB0                                       : in  std_logic; 
    
      --===============--
      -- Clocks scheme --
      --===============--

      -- Fabric clock:      
      ----------------

      CLKIN_50                                       : in  std_logic;  
      
      -- MGT(GT) clock:
      -----------------
      
      REFCLK_QL3                                     : in  std_logic;
   
      --==============--
      -- Serial lanes --
      --==============--
      
      HSMB_TX_2                                      : out std_logic;
      HSMB_RX_2                                      : in  std_logic;
      
      --====================--
      -- On-board user LEDs --    
      --====================--    

      USER_LED                                       : out std_logic_vector(7 downto 0);

      --====================--
      -- Signals forwarding --
      --====================--      

      -- SMA output:
      --------------
      
      -- Comment: SMA_CLKOUT is connected to a multiplexor that switches between TX_FRAMECLK and TX_WORDCLK.

      SMA_CLKOUT                                     : out std_logic;

      -- Pattern match flags:
      -----------------------
      
      HSMA_TX_D_P0                                   : out std_logic;       
      HSMA_TX_D_P1                                   : out std_logic;
      
      -- Clocks forwarding:
      ---------------------  
      
      -- Comment: * HSMA_TX_D_P2 and HSMA_TX_D_P3 are used for forwarding TX_FRAMECLK and TX_WORDCLK respectively.
      --      
      --          * HSMA_TX_D_P4 and HSMA_TX_D_P5 are used for forwarding RX_FRAMECLK and RX_WORDCLK respectively.
      
      HSMA_TX_D_P2                                   : out std_logic; 
      HSMA_TX_D_P3                                   : out std_logic; 
      -----------------------------------------------
      HSMA_TX_D_P4                                   : out std_logic; 
      HSMA_TX_D_P5                                   : out std_logic
      
   );
end cvGtFpgaDevkit_gbt_example_design;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of cvGtFpgaDevkit_gbt_example_design is
   
   --================================ Signal Declarations ================================--
   
   --===============--     
   -- General reset --     
   --===============--     

   signal reset_from_orGate                          : std_logic;
   
   --=========================--
   -- GBT Bank example design --
   --=========================--
   
   -- Control:
   -----------
   
   signal generalReset_from_user                     : std_logic;      
   signal manualResetMgtTxPll_from_user              : std_logic;    
   signal manualResetTx_from_user                    : std_logic; 
   signal manualResetRx_from_user                    : std_logic; 
   signal testPatterSel_from_user                    : std_logic_vector(1 downto 0); 
   signal loopBack_from_user                         : std_logic; 
   signal resetDataErrorSeenFlag_from_user           : std_logic; 
   signal resetGbtRxReadyLostFlag_from_user          : std_logic; 
   signal txIsDataSel_from_user                      : std_logic;   
   signal txFrmClkPhAlManualReset_from_user          : std_logic; 
   signal txFrmClkPhAlStepsNbr_from_user             : std_logic_vector(5 downto 0);
   signal txFrmClkPhAlStepsEnable_from_user          : std_logic; 
   signal txFrmClkPhAlStepsTrigger_from_user         : std_logic; 
   signal txWrdClkMonThresholdUp_from_user           : std_logic_vector(7 downto 0); 
   signal txWrdClkMonThresholdLow_from_user          : std_logic_vector(7 downto 0); 
   signal txWrdClkMonMgtTxResetEn_from_user          : std_logic;   
   --------------------------------------------------      
   signal latOptGbtBankTx_from_gbtExmplDsgn          : std_logic;
   signal latOptGbtBankRx_from_gbtExmplDsgn          : std_logic;
   signal mgtTxPllLocked_from_gbtExmplDsgn           : std_logic;
   signal txFrmClkPhAlLocked_from_gbtExmplDsgn       : std_logic;
   signal txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn : std_logic;
   signal txWrdClkMonStats_from_gbtExmplDsgn         : std_logic_vector(7 downto 0);
   signal txWrdClkMonPhaseOk_from_gbtExmplDsgn       : std_logic;
   signal mgtReady_from_gbtExmplDsgn                 : std_logic; 
   signal rxBitSlipNbr_from_gbtExmplDsgn             : std_logic_vector(GBTRX_BITSLIP_NBR_MSB downto 0);
   signal rxWordClkReady_from_gbtExmplDsgn           : std_logic; 
   signal rxFrameClkReady_from_gbtExmplDsgn          : std_logic; 
   signal gbtRxReady_from_gbtExmplDsgn               : std_logic;    
   signal rxIsData_from_gbtExmplDsgn                 : std_logic;        
   signal gbtRxReadyLostFlag_from_gbtExmplDsgn       : std_logic; 
   signal rxDataErrorSeen_from_gbtExmplDsgn          : std_logic; 
   signal rxExtrDataWidebusErSeen_from_gbtExmplDsgn  : std_logic; 
   signal rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn : std_logic;
   
   -- Data:
   --------
   
   signal txData_from_gbtExmplDsgn                   : std_logic_vector(83 downto 0);
   signal rxData_from_gbtExmplDsgn                   : std_logic_vector(83 downto 0);
   --------------------------------------------------      
   signal txExtraDataWidebus_from_gbtExmplDsgn       : std_logic_vector(31 downto 0);
   signal rxExtraDataWidebus_from_gbtExmplDsgn       : std_logic_vector(31 downto 0);
   --------------------------------------------------
   signal txExtraDataGbt8b10b_from_gbtExmplDsgn      : std_logic_vector( 3 downto 0);
   signal rxExtraDataGbt8b10b_from_gbtExmplDsgn      : std_logic_vector( 3 downto 0);          

   --============================--
   -- In-System Probes & Sources --
   --============================--
   
   signal clk40mhz_from_isspPll                      : std_logic; 
   signal locked_from_isspPll                        : std_logic; 
   
   signal source_from_inSysSrcAndPrb                 : std_logic_vector(35 downto 0);  
   signal probe_to_inSysSrcAndPrb                    : std_logic_vector(29 downto 0);
   
   --=====================--
   -- Latency measurement --
   --=====================--
   
   signal txFrameClk_from_gbtExmplDsgn               : std_logic;
   signal txWordClk_from_gbtExmplDsgn                : std_logic;
   signal rxFrameClk_from_gbtExmplDsgn               : std_logic;
   signal rxWordClk_from_gbtExmplDsgn                : std_logic;
   --------------------------------------------------                                    
   signal txMatchFlag_from_gbtExmplDsgn              : std_logic;
   signal rxMatchFlag_from_gbtExmplDsgn              : std_logic;   
   
   --=====================================================================================--   

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
   --==================================== User Logic =====================================--
   
   --===============--
   -- General reset --
   --===============--
   
   reset_from_orGate                                 <= (not USER_PB0) or generalReset_from_user;
      
   --=========================--
   -- GBT Bank example design --
   --=========================--
   
   gbtExmplDsgn: entity work.alt_cv_gbt_example_design
      port map (
         -- Resets scheme:      
         GBTBANK1_GENERAL_RESET_I                    => reset_from_orGate,                   
         GBTBANK1_MANUAL_RESET_GT_TX_PLL_I           => manualResetMgtTxPll_from_user,
         GBTBANK1_MANUAL_RESET_TX_I                  => manualResetTx_from_user,
         GBTBANK1_MANUAL_RESET_RX_I                  => manualResetRx_from_user,
         -- Clocks scheme:                           
         GBTBANK1_FABRIC_CLK_I                       => CLKIN_50,
         GBTBANK1_MGT_REFCLK_I                       => REFCLK_QL3,             
         -- Serial lanes:                            
         GBTBANK1_MGT_TX                             => HSMB_TX_2,                
         GBTBANK1_MGT_RX                             => HSMB_RX_2,                 
         -- General control:                       
         GBTBANK1_LOOPBACK_I                         => loopBack_from_user,  
         GBTBANK1_TX_ISDATA_SEL_I                    => txIsDataSel_from_user,                 
         GBTBANK1_LATOPT_GBTBANK_TX_O                => latOptGbtBankTx_from_gbtExmplDsgn,             
         GBTBANK1_LATOPT_GBTBANK_RX_O                => latOptGbtBankRx_from_gbtExmplDsgn,       
         GBTBANK1_TX_FRMCLKPHAL_MANUAL_RESET_I       => txFrmClkPhAlManualReset_from_user, 
         GBTBANK1_TX_FRMCLKPHAL_STEPSNBR_I           => txFrmClkPhAlStepsNbr_from_user,  
         GBTBANK1_TX_FRMCLKPHAL_STEPSENABLE_I        => txFrmClkPhAlStepsEnable_from_user,  
         GBTBANK1_TX_FRMCLKPHAL_STEPSTRIGGER_I       => txFrmClkPhAlStepsTrigger_from_user, 
         GBTBANK1_TX_FRMCLKPHAL_LOCKED_O             => txFrmClkPhAlLocked_from_gbtExmplDsgn,       
         GBTBANK1_TX_FRMCLKPHAL_PHSHIFTDONE_O        => txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn,
         GBTBANK1_TX_WRDCLKMON_THRESHOLD_UP_I        => txWrdClkMonThresholdUp_from_user,         
         GBTBANK1_TX_WRDCLKMON_THRESHOLD_LOW_I       => txWrdClkMonThresholdLow_from_user,        
         GBTBANK1_TX_WRDCLKMON_MGTTXRESET_EN_I       => txWrdClkMonMgtTxResetEn_from_user,       
         GBTBANK1_TX_WRDCLKMON_STATS_O               => txWrdClkMonStats_from_gbtExmplDsgn,          
         GBTBANK1_TX_WRDCLKMON_PHASEOK_O             => txWrdClkMonPhaseOk_from_gbtExmplDsgn, 
         GBTBANK1_MGT_TX_PLL_LOCKED_O                => mgtTxPllLocked_from_gbtExmplDsgn,
         GBTBANK1_MGT_READY_O                        => mgtReady_from_gbtExmplDsgn,             
         GBTBANK1_RX_BITSLIP_NUMBER_O                => rxBitSlipNbr_from_gbtExmplDsgn,            
         GBTBANK1_RX_WORDCLK_READY_O                 => rxWordClkReady_from_gbtExmplDsgn,           
         GBTBANK1_RX_FRAMECLK_READY_O                => rxFrameClkReady_from_gbtExmplDsgn,            
         GBTBANK1_GBT_RX_READY_O                     => gbtRxReady_from_gbtExmplDsgn,
         GBTBANK1_RX_ISDATA_FLAG_O                   => rxIsData_from_gbtExmplDsgn,            
         -- GBT Bank data:                           
         GBTBANK1_TX_DATA_O                          => txData_from_gbtExmplDsgn,            
         GBTBANK1_TX_EXTRA_DATA_WIDEBUS_O            => txExtraDataWidebus_from_gbtExmplDsgn,
         GBTBANK1_TX_EXTRA_DATA_GBT8B10B_O           => txExtraDataGbt8b10b_from_gbtExmplDsgn,
         --------------------------------------------   
         GBTBANK1_RX_DATA_O                          => rxData_from_gbtExmplDsgn,           
         GBTBANK1_RX_EXTRA_DATA_WIDEBUS_O            => rxExtraDataWidebus_from_gbtExmplDsgn,
         GBTBANK1_RX_EXTRA_DATA_GBT8B10B_O           => rxExtraDataGbt8b10b_from_gbtExmplDsgn,
         -- Test control:                    
         GBTBANK1_TEST_PATTERN_SEL_I                 => testPatterSel_from_user,        
         GBTBANK1_RESET_DATA_ERR_FLAG_I              => resetGbtRxReadyLostFlag_from_user,     
         GBTBANK1_RESET_GBTRXREADY_LOST_FLAG_I       => resetDataErrorSeenFlag_from_user,     
         GBTBANK1_GBTRXREADY_LOST_FLAG_O             => gbtRxReadyLostFlag_from_gbtExmplDsgn,       
         GBTBANK1_RXDATA_ERR_FLAG_O                  => rxDataErrorSeen_from_gbtExmplDsgn,      
         GBTBANK1_RXEXTRADATA_WIDEBUS_ERR_FLAG_O     => rxExtrDataWidebusErSeen_from_gbtExmplDsgn,
         GBTBANK1_RXEXTRADATA_GBT8B10B_ERR_FLAG_O    => rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn,
         -- Latency measurement:                     
         GBTBANK1_TX_FRAMECLK_O                      => txFrameClk_from_gbtExmplDsgn,        
         GBTBANK1_TX_WORDCLK_O                       => txWordClk_from_gbtExmplDsgn,          
         GBTBANK1_RX_FRAMECLK_O                      => rxFrameClk_from_gbtExmplDsgn,         
         GBTBANK1_RX_WORDCLK_O                       => rxWordClk_from_gbtExmplDsgn,          
         --------------------------------------------            
         GBTBANK1_TX_MATCHFLAG_O                     => txMatchFlag_from_gbtExmplDsgn,          
         GBTBANK1_RX_MATCHFLAG_O                     => rxMatchFlag_from_gbtExmplDsgn          
      ); 
      
   --==============--   
   -- Test control --   
   --==============--
   
   -- Signals mapping:
   -------------------
   
   loopBack_from_user                                <= source_from_inSysSrcAndPrb( 0);
   generalReset_from_user                            <= source_from_inSysSrcAndPrb( 1);          
   manualResetMgtTxPll_from_user                     <= source_from_inSysSrcAndPrb( 2);    
   manualResetTx_from_user                           <= source_from_inSysSrcAndPrb( 3);
   manualResetRx_from_user                           <= source_from_inSysSrcAndPrb( 4);
   testPatterSel_from_user                           <= source_from_inSysSrcAndPrb( 6 downto  5); 
   txIsDataSel_from_user                             <= source_from_inSysSrcAndPrb( 7);
   resetDataErrorSeenFlag_from_user                  <= source_from_inSysSrcAndPrb( 8);
   resetGbtRxReadyLostFlag_from_user                 <= source_from_inSysSrcAndPrb( 9);
   txFrmClkPhAlManualReset_from_user                 <= source_from_inSysSrcAndPrb(10);
   txFrmClkPhAlStepsNbr_from_user                    <= source_from_inSysSrcAndPrb(16 downto 11);     
   txFrmClkPhAlStepsEnable_from_user                 <= source_from_inSysSrcAndPrb(17);   
   txFrmClkPhAlStepsTrigger_from_user                <= source_from_inSysSrcAndPrb(18);
   txWrdClkMonThresholdUp_from_user                  <= source_from_inSysSrcAndPrb(26 downto 19);
   txWrdClkMonThresholdLow_from_user                 <= source_from_inSysSrcAndPrb(34 downto 27);   
   txWrdClkMonMgtTxResetEn_from_user                 <= source_from_inSysSrcAndPrb(35);
   --------------------------------------------------
   probe_to_inSysSrcAndPrb( 0)                       <= latOptGbtBankTx_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb( 1)                       <= latOptGbtBankRx_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb( 2)                       <= mgtTxPllLocked_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb( 3)                       <= txFrmClkPhAlLocked_from_gbtExmplDsgn;       
   probe_to_inSysSrcAndPrb( 4)                       <= txFrmClkPhAl_phshiftdone_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb(12 downto 5)              <= txWrdClkMonStats_from_gbtExmplDsgn;    
   probe_to_inSysSrcAndPrb(13)                       <= txWrdClkMonPhaseOk_from_gbtExmplDsgn; 
   probe_to_inSysSrcAndPrb(14)                       <= mgtReady_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb(15)                       <= rxWordClkReady_from_gbtExmplDsgn;    
   probe_to_inSysSrcAndPrb(16)                       <= rxFrameClkReady_from_gbtExmplDsgn;    
   probe_to_inSysSrcAndPrb(17)                       <= gbtRxReady_from_gbtExmplDsgn;   
   probe_to_inSysSrcAndPrb(23 downto 18)             <= rxBitSlipNbr_from_gbtExmplDsgn;       
   probe_to_inSysSrcAndPrb(24)                       <= rxIsData_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb(25)                       <= gbtRxReadyLostFlag_from_gbtExmplDsgn;  
   probe_to_inSysSrcAndPrb(26)                       <= rxDataErrorSeen_from_gbtExmplDsgn;   
   probe_to_inSysSrcAndPrb(27)                       <= rxExtrDataWidebusErSeen_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb(28)                       <= rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn;
   probe_to_inSysSrcAndPrb(29)                       <= locked_from_isspPll;
   
   -- In-system sources & probes:
   ------------------------------
   
   -- Comment: * The "isspPll" is used in order to provide a 40Mhz clock to "inSysSrcAndPrb".
   --
   --          * Note!! - TX_FRAMECLK from the GBT example design is not used because the TX_FRAMECLK aligner is reset by "inSysSrcAndPrb".
   --
   --          * If TX_FRAMECLK was provided by a source independent not controlled by "inSysSrcAndPrb", then TX_FRAMECLK would be the best option
   --            for clocking "inSysSrcAndPrb".
   
   isspPll: entity alt_cv_gbt_tx_frameclk_stdpll.alt_cv_gbt_tx_frameclk_stdpll
      port map (
         REFCLK                                      => REFCLK_QL3,          
         RST                                         => '0',          
         OUTCLK_0                                    => clk40mhz_from_isspPll,   -- Comment: 40MHz
         LOCKED                                      => locked_from_isspPll
      );
   
   -- Comment: * In-System Sources and Probes is used to control the example design as well as for transmitted and received data analysis.
   --
   --          * After FPGA configuration, open the project "cvGtFpgaDevkit_gbt_example_design.spf" 
   --            that can be found in:
   --            "..\example_designs\altera_cv\cv_gt_fpga_devkit\issp_project\".  
   
   inSysSrcAndPrb: entity alt_cv_issp.alt_cv_issp
      port map (
         PROBE                                       => probe_to_inSysSrcAndPrb,
         SOURCE_CLK                                  => clk40mhz_from_isspPll,
         SOURCE_ENA                                  => '1',
         SOURCE                                      => source_from_inSysSrcAndPrb
      );  
   
   -- On-board user LEDs:    
   ----------------------
   
   USER_LED(0)                                       <= not mgtTxPllLocked_from_gbtExmplDsgn;
   USER_LED(1)                                       <= not txFrmClkPhAlLocked_from_gbtExmplDsgn;  
   USER_LED(2)                                       <= not mgtReady_from_gbtExmplDsgn; 
   USER_LED(3)                                       <= not gbtRxReady_from_gbtExmplDsgn; 
   USER_LED(4)                                       <= not gbtRxReadyLostFlag_from_gbtExmplDsgn;
   USER_LED(5)                                       <= not rxDataErrorSeen_from_gbtExmplDsgn;   
   USER_LED(6)                                       <= not rxExtrDataWidebusErSeen_from_gbtExmplDsgn; 
   USER_LED(7)                                       <= not locked_from_isspPll;--rxExtrDataGbt8b10bErSeen_from_gbtExmplDsgn;
   
   --=====================--
   -- Latency measurement --
   --=====================--
   
   -- Clock forwarding:
   --------------------
   
   -- Comment: * The forwarding of the clocks allows to check the phase alignment of the different
   --            clocks using an oscilloscope.
   --
   --          * Note!! If RX DATA comes from another board with a different reference clock, 
   --                   then the TX_FRAMECLK/TX_WORDCLK domains are asynchronous with respect to the
   --                   TX_FRAMECLK/TX_WORDCLK domains.

   SMA_CLKOUT                                        <= REFCLK_QL3;
   
   HSMA_TX_D_P2                                      <= txFrameClk_from_gbtExmplDsgn;
   HSMA_TX_D_P3                                      <= txWordClk_from_gbtExmplDsgn;
   --------------------------------------------------   
   HSMA_TX_D_P4                                      <= rxFrameClk_from_gbtExmplDsgn;  
   HSMA_TX_D_P5                                      <= rxWordClk_from_gbtExmplDsgn;

   -- Pattern match flags:
   -----------------------
   
   -- Comment: * The latency of the link can be measured using an oscilloscope by comparing 
   --            the TX FLAG with the RX FLAG.
   --
   --          * The COUNTER TEST PATTERN must be used for this test. 
   
   HSMA_TX_D_P0                                      <= txMatchFlag_from_gbtExmplDsgn;
   HSMA_TX_D_P1                                      <= rxMatchFlag_from_gbtExmplDsgn;
   
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--