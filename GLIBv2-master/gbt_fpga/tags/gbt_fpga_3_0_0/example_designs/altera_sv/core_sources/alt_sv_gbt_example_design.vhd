--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Altera Stratix V - GBT Bank example design                                         
--                                                                                                 
-- Language:              VHDL'93                                                                  
--                                                                                                   
-- Target Device:         Altera Stratix V                                                        
-- Tool version:          Quartus II 13.1                                                               
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        09/08/2013   3.0       M. Barros Marin   - First .vhd module definition           
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

-- Altera devices library:
library altera; 
library altera_mf;
library lpm;
use altera.altera_primitives_components.all;   
use altera_mf.altera_mf_components.all;
use lpm.lpm_components.all;

-- Custom libraries and packages:
use work.gbt_bank_package.all;
use work.vendor_specific_gbt_bank_package.all;
use work.gbt_banks_user_setup.all;

-- Libraries for direct instantiation:
library alt_sv_gbt_tx_frameclk_stdpll;
library alt_sv_gbt_rx_frameclk_stdpll;
library alt_sv_gt_reconfctrl_x1;
library alt_sv_gt_reconfctrl_x3;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity alt_sv_gbt_example_design is 
   port ( 

      --===============--
      -- Resets scheme --
      --===============-- 
      
      -- GBT Bank 1:
      --------------
      
      GBTBANK1_GENERAL_RESET_I                             : in  std_logic;
      -----------------------------------------------------
      GBTBANK1_MANUAL_RESET_GT_TX_PLL_I                    : in  std_logic;
      GBTBANK1_MANUAL_RESET_TX_I                           : in  std_logic;
      GBTBANK1_MANUAL_RESET_RX_I                           : in  std_logic;

      -- GBT Bank 2:
      --------------
      
      GBTBANK2_GENERAL_RESET_I                             : in  std_logic;
      -----------------------------------------------------
      GBTBANK2_MANUAL_RESET_GT_TX_PLL_I                    : in  std_logic;
      GBTBANK2_MANUAL_RESET_TX_I                           : in  std_logic_vector(1 to 3);
      GBTBANK2_MANUAL_RESET_RX_I                           : in  std_logic_vector(1 to 3);     
      
      --===============--
      -- Clocks scheme --
      --===============--
      
      -- Fabric clock:      
      ----------------
      
      FABRIC_CLK_I                                         : in  std_logic; 
      
      -- MGT (GTX) reference clock:
      -----------------------------

      -- GBT Bank 1:      
      GBTBANK1_MGT_REFCLK_I                                : in  std_logic;
      
      -- GBT Bank 2:
      GBTBANK2_MGT_REFCLK_I                                : in  std_logic;

      --==============--
      -- Serial lanes --
      --==============--
      
      -- GBT Bank 1:
      --------------
      
      GBTBANK1_MGT_TX                                      : out std_logic; 
      GBTBANK1_MGT_RX                                      : in  std_logic; 
      
      -- GBT Bank 2:
      --------------      
      
      GBTBANK2_MGT_TX                                      : out std_logic_vector(1 to 3);
      GBTBANK2_MGT_RX                                      : in  std_logic_vector(1 to 3); 
     
      --=================--
      -- General control --
      --=================--

      -- GBT Bank 1:
      --------------
      
      GBTBANK1_LOOPBACK_I                                  : in  std_logic;  
      GBTBANK1_TX_ISDATA_SEL_I                             : in  std_logic;
      GBTBANK1_LATOPT_GBTBANK_TX_O                         : out std_logic;
      GBTBANK1_LATOPT_GBTBANK_RX_O                         : out std_logic;
      -----------------------------------------------------
      GBTBANK1_TX_FRAMECLK_PLL_LOCKED_O                    : out std_logic;
      GBTBANK1_MGT_TX_PLL_LOCKED_O                         : out std_logic;
      GBTBANK1_MGT_READY_O                                 : out std_logic;
      GBTBANK1_RX_BITSLIP_NUMBER_O                         : out std_logic_vector(GBTRX_BITSLIP_NBR_MSB downto 0);      
      GBTBANK1_RX_WORDCLK_READY_O                          : out std_logic;
      GBTBANK1_RX_FRAMECLK_READY_O                         : out std_logic;
      GBTBANK1_GBT_RX_READY_O                              : out std_logic;        
      GBTBANK1_RX_ISDATA_FLAG_O                            : out std_logic;

      -- GBT Bank 2:
      --------------      
      
      GBTBANK2_LOOPBACK_I                                  : in  std_logic_vector       (1 to 3);  
      GBTBANK2_TX_ISDATA_SEL_I                             : in  std_logic_vector       (1 to 3);
      GBTBANK2_LATOPT_GBTBANK_TX_O                         : out std_logic;
      GBTBANK2_LATOPT_GBTBANK_RX_O                         : out std_logic;
      -----------------------------------------------------
      GBTBANK2_TX_FRMCLKPHAL_MANUAL_RESET_I                : in  std_logic;
      GBTBANK2_TX_FRMCLKPHAL_STEPSGBTLNK_I                 : in  frmClkPhAlSteps_nx6bit (1 to 3);
      GBTBANK2_TX_FRMCLKPHAL_STEPSENABLE_I                 : in  std_logic;
      GBTBANK2_TX_FRMCLKPHAL_STEPSTRIGGER_I                : in  std_logic;
      GBTBANK2_TX_FRMCLKPHAL_LOCKED_O                      : out std_logic;
      GBTBANK2_TX_FRMCLKPHAL_PHSHIFTDONE_O                 : out std_logic;
      -----------------------------------------------------
      GBTBANK2_TX_WRDCLKMON_THRESHOLD_UP_I                 : in  std_logic_vector       (7 downto 0);
      GBTBANK2_TX_WRDCLKMON_THRESHOLD_LOW_I                : in  std_logic_vector       (7 downto 0);
      GBTBANK2_TX_WRDCLKMON_MGTTXRESET_EN_I                : in  std_logic;
      GBTBANK2_TX_WRDCLKMON_STATS_O                        : out std_logic_vector       (7 downto 0); 
      GBTBANK2_TX_WRDCLKMON_PHASEOK_O                      : out std_logic;    
      -----------------------------------------------------
      GBTBANK2_MGT_TX_PLL_LOCKED_O                         : out std_logic;
      GBTBANK2_MGT_READY_O                                 : out std_logic_vector       (1 to 3);
      GBTBANK2_RX_BITSLIP_NUMBER_O                         : out gbtRxSlideNbr_mxnbit_A (1 to 3);     
      GBTBANK2_RX_WORDCLK_READY_O                          : out std_logic_vector       (1 to 3);     
      GBTBANK2_RX_FRAMECLK_READY_O                         : out std_logic;
      GBTBANK2_GBT_RX_READY_O                              : out std_logic_vector       (1 to 3);        
      GBTBANK2_RX_ISDATA_FLAG_O                            : out std_logic_vector       (1 to 3); 
      
      --===============--
      -- GBT Bank data -- 
      --===============--
      
      -- GBT Bank 1:
      --------------

      -- TX data:          
      GBTBANK1_TX_DATA_O                                   : out std_logic_vector(83 downto 0); 
      GBTBANK1_TX_EXTRA_DATA_WIDEBUS_O                     : out std_logic_vector(31 downto 0);     
      GBTBANK1_TX_EXTRA_DATA_GBT8B10B_O                    : out std_logic_vector( 3 downto 0);
                  
      -- RX data:          
      GBTBANK1_RX_DATA_O                                   : out std_logic_vector(83 downto 0);
      GBTBANK1_RX_EXTRA_DATA_WIDEBUS_O                     : out std_logic_vector(31 downto 0);
      GBTBANK1_RX_EXTRA_DATA_GBT8B10B_O                    : out std_logic_vector( 3 downto 0);
            
      -- GBT Bank 2:
      --------------

      -- TX data:
      GBTBANK2_TX_DATA_O                                   : out data_nx84bit_A            (1 to 3); 
      GBTBANK2_TX_EXTRA_DATA_WIDEBUS_O                     : out extraDataWidebus_nx32bit_A(1 to 3);     
      GBTBANK2_TX_EXTRA_DATA_GBT8B10B_O                    : out extraDataGbt8b10b_nx4bit_A(1 to 3);
      
      -- RX data:
      GBTBANK2_RX_DATA_O                                   : out data_nx84bit_A            (1 to 3);
      GBTBANK2_RX_EXTRA_DATA_WIDEBUS_O                     : out extraDataWidebus_nx32bit_A(1 to 3);
      GBTBANK2_RX_EXTRA_DATA_GBT8B10B_O                    : out extraDataGbt8b10b_nx4bit_A(1 to 3);
      
      --==============--
      -- Test control --
      --==============--

      -- GBT Bank 1:
      --------------
      
      -- Test pattern:
      GBTBANK1_TEST_PATTERN_SEL_I                          : in  std_logic_vector(1 downto 0);      
      
      -- Error detection:
      GBTBANK1_RESET_DATA_ERR_FLAG_I                       : in  std_logic;
      GBTBANK1_RESET_GBTRXREADY_LOST_FLAG_I                : in  std_logic;
      -----------------------------------------------------
      GBTBANK1_GBTRXREADY_LOST_FLAG_O                      : out std_logic;
      GBTBANK1_RXDATA_ERR_FLAG_O                           : out std_logic;
      GBTBANK1_RXEXTRADATA_WIDEBUS_ERR_FLAG_O              : out std_logic;
      GBTBANK1_RXEXTRADATA_GBT8B10B_ERR_FLAG_O             : out std_logic;

      -- GBT Bank 2:
      --------------
      
      -- Test pattern:
      GBTBANK2_TEST_PATTERN_SEL_I                          : in  std_logic_vector(1 downto 0);      
      
      -- Error detection:
      GBTBANK2_RESET_DATA_ERR_FLAG_I                       : in  std_logic_vector(1 to 3);
      GBTBANK2_RESET_GBTRXREADY_LOST_FLAG_I                : in  std_logic_vector(1 to 3);
      -----------------------------------------------------
      GBTBANK2_GBTRXREADY_LOST_FLAG_O                      : out std_logic_vector(1 to 3);
      GBTBANK2_RXDATA_ERR_FLAG_O                           : out std_logic_vector(1 to 3);
      GBTBANK2_RXEXTRADATA_WIDEBUS_ERR_FLAG_O              : out std_logic_vector(1 to 3);
      GBTBANK2_RXEXTRADATA_GBT8B10B_ERR_FLAG_O             : out std_logic_vector(1 to 3);
      
      --=====================--
      -- Latency measurement --
      --=====================--
      
      -- GBT Bank 1:
      --------------

      -- Pattern match flags:
      GBTBANK1_TX_MATCHFLAG_O                              : out std_logic;
      GBTBANK1_RX_MATCHFLAG_O                              : out std_logic;      
      
      -- Clocks forwarding:
      GBTBANK1_TX_FRAMECLK_O                               : out std_logic;
      GBTBANK1_TX_WORDCLK_O                                : out std_logic;
      GBTBANK1_RX_FRAMECLK_O                               : out std_logic;
      GBTBANK1_RX_WORDCLK_O                                : out std_logic;
      
 
      -- GBT Bank 2:
      --------------

      -- Pattern match flags:
      GBTBANK2_TX_MATCHFLAG_O                              : out std_logic_vector(1 to 3);
      GBTBANK2_RX_MATCHFLAG_O                              : out std_logic_vector(1 to 3);      
      
      -- Clocks forwarding:
      GBTBANK2_USER_TX_FRAMECLK_O                          : out std_logic;
      GBTBANK2_GBTLINK_TX_FRAMECLK_O                       : out std_logic_vector(1 to 3);
      GBTBANK2_TX_WORDCLK_O                                : out std_logic_vector(1 to 3);
      GBTBANK2_RX_FRAMECLK_O                               : out std_logic;
      GBTBANK2_RX_WORDCLK_O                                : out std_logic_vector(1 to 3)
     
   );
end alt_sv_gbt_example_design;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of alt_sv_gbt_example_design is 
   
   --================================ Signal Declarations ================================--   
  
   --=========--
   -- TX data --
   --=========--   

   -- GBT Bank 1:
   --------------

   signal gbtBank1_txEncodingSel                           : std_logic_vector( 1 downto 0);
   signal txData_from_gbtBank1_pattGen                     : std_logic_vector(83 downto 0);   
   signal txExtraDataWidebus_from_gbtBank1_pattGen         : std_logic_vector(31 downto 0);   
   signal txExtraDataGbt8b10b_from_gbtBank1_pattGen        : std_logic_vector( 3 downto 0);

   -- GBT Bank 2:                                                                  
   --------------                                                                  

   signal gbtBank2_txEncodingSel                           : std_logic_vector          (1 downto 0);
   signal txData_from_gbtBank2_pattGen                     : data_nx84bit_A            (1 to 3);   
   signal txExtraDataWidebus_from_gbtBank2_pattGen         : extraDataWidebus_nx32bit_A(1 to 3);   
   signal txExtraDataGbt8b10b_from_gbtBank2_pattGen        : extraDataGbt8b10b_nx4bit_A(1 to 3);
   
   --=========================--  
   -- GBT Banks resets scheme --
   --=========================--
   
   -- GBT Bank 1:
   --------------
   
   signal reset_from_gbtBank1_genRst                       : std_logic;
   --------------------------------------------------------
   signal reset_to_gbtBank1_mgtTxPll                       : std_logic; 
   --------------------------------------------------------
   signal mgtTxReset_from_gbtBank1_gbtBankRst              : std_logic; 
   signal mgtRxReset_from_gbtBank1_gbtBankRst              : std_logic; 
   signal gbtTxReset_from_gbtBank1_gbtBankRst              : std_logic;
   signal gbtRxReset_from_gbtBank1_gbtBankRst              : std_logic; 

   -- GBT Bank 2:
   --------------
   
   signal reset_from_gbtBank2_genRst                       : std_logic;
   --------------------------------------------------------
   signal reset_to_gbtBank2_mgtTxPll                       : std_logic; 
   signal reset_to_gbtBank2_txFrmClkPhAlgnr                : std_logic; 
   --------------------------------------------------------
   signal mgtTxReset_from_gbtBank2_gbtBankRst              : std_logic_vector(1 to 3); 
   signal mgtRxReset_from_gbtBank2_gbtBankRst              : std_logic_vector(1 to 3); 
   signal gbtTxReset_from_gbtBank2_gbtBankRst              : std_logic_vector(1 to 3);
   signal gbtRxReset_from_gbtBank2_gbtBankRst              : std_logic_vector(1 to 3);
   
   --=========================--
   -- GBT Banks clocks scheme -- 
   --=========================--   

   -- GBT Bank 1:
   --------------
   
   signal locked_from_gbtBank1_mgtTxPll                    : std_logic;
   signal txFrameClk_from_gbtBank1_txFrmClkStdPll          : std_logic;
   -------------------------------------------------------- 
   signal phaseAlignDone_from_gbtBank1_rxFrmClkPhAlgnr     : std_logic;   
   signal rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr         : std_logic;
   
   -- GBT Bank 2:
   --------------
   
   signal txFrameClk_from_gbtBank2_txFrmClkPhAlgnr         : std_logic;   
   signal txFrameClk_to_gbtBank2_gbtLink                   : std_logic_vector(1 to 3);
   --------------------------------------------------------
   signal locked_from_gbtBank2_mgtTxPll                    : std_logic;   
   --------------------------------------------------------
   signal locked_from_gbtBank2_rxFrmClkStdPll              : std_logic;
   signal rxFrameClk_from_gbtBank2_rxFrmClkStdPll          : std_logic;

   --===========--
   -- GBT Banks --
   --===========--   
   
-- Comment: Note!! This example design instantiates two GBt Banks:
--
--          - GBT Bank 1: One GBT Link (Standard GBT TX and Latency-Optimized GBT RX).
--
--          - GBT Bank 2: Three GBT Links (Latency-Optimized GBT TX and Standard GBT RX).                     
   
   -- GBT Bank 1:
   --------------
   
   signal to_gbtBank1_clks                                 : gbtBankClks_i_R;                          
   signal from_gbtBank1_clks                               : gbtBankClks_o_R;
   --------------------------------------------------------        
   signal to_gbtBank1_gbtTx                                : gbtTx_i_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   signal from_gbtBank1_gbtTx                              : gbtTx_o_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   --------------------------------------------------------        
   signal to_gbtBank1_mgt                                  : mgt_i_R;
   signal from_gbtBank1_mgt                                : mgt_o_R; 
   --------------------------------------------------------        
   signal to_gbtBank1_gbtRx                                : gbtRx_i_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   signal from_gbtBank1_gbtRx                              : gbtRx_o_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS);
                                                           
   signal reconfig_from_gbtBank1_mgtTxPll                  : std_logic_vector(45 downto 0);
   signal mgtTxPllReconfig_from_gbtBank1_reconfCtrl_x1     : std_logic_vector(69 downto 0);
   --------------------------------------------------------
   signal mgmtReset_to_gbtBank1_reconfCtrl_x1              : std_logic;   
   signal mgmtAddress_to_gbtBank1_reconfCtrl_x1            : std_logic_vector( 6 downto 0);  
   signal mgmtRead_to_gbtBank1_reconfCtrl_x1               : std_logic;   
   signal mgmtReadData_from_gbtBank1_reconfCtrl_x1         : std_logic_vector(31 downto 0);  
   signal mgmtWaitRequest_from_gbtBank1_reconfCtrl_x1      : std_logic;   
   signal mgmtWrite_to_gbtBank1_reconfCtrl_x1              : std_logic;   
   signal mgmtWriteData_to_gbtBank1_reconfCtrl_x1          : std_logic_vector(31 downto 0);
   
   -- GBT Bank 2:
   --------------
   
   signal to_gbtBank2_clks                                 : gbtBankClks_i_R;                          
   signal from_gbtBank2_clks                               : gbtBankClks_o_R;
   --------------------------------------------------------        
   signal to_gbtBank2_gbtTx                                : gbtTx_i_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
   signal from_gbtBank2_gbtTx                              : gbtTx_o_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
   --------------------------------------------------------        
   signal to_gbtBank2_mgt                                  : mgt_i_R;
   signal from_gbtBank2_mgt                                : mgt_o_R; 
   --------------------------------------------------------        
   signal to_gbtBank2_gbtRx                                : gbtRx_i_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
   signal from_gbtBank2_gbtRx                              : gbtRx_o_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS);

   signal reconfig_from_gbtBank2_mgtTxPll                  : std_logic_vector(45 downto 0);
   signal mgtTxPllReconfig_from_gbtBank2_reconfCtrl_x3     : std_logic_vector(69 downto 0);
   --------------------------------------------------------
   signal mgmtReset_to_gbtBank2_reconfCtrl_x3              : std_logic;   
   signal mgmtAddress_to_gbtBank2_reconfCtrl_x3            : std_logic_vector( 6 downto 0);  
   signal mgmtRead_to_gbtBank2_reconfCtrl_x3               : std_logic;   
   signal mgmtReadData_from_gbtBank2_reconfCtrl_x3         : std_logic_vector(31 downto 0);  
   signal mgmtWaitRequest_from_gbtBank2_reconfCtrl_x3      : std_logic;   
   signal mgmtWrite_to_gbtBank2_reconfCtrl_x3              : std_logic;   
   signal mgmtWriteData_to_gbtBank2_reconfCtrl_x3          : std_logic_vector(31 downto 0);
   
   -- GBT Bank 3:
   --------------
   
-- signal to_gbtBank3_clks                                 : gbtBankClks_i_R;                          
-- signal from_gbtBank3_clks                               : gbtBankClks_o_R;
-- --------------------------------------------------------        
-- signal to_gbtBank3_gbtTx                                : gbtTx_i_R_A(1 to GBT_BANKS_USER_SETUP(3).NUM_LINKS); 
-- signal from_gbtBank3_gbtTx                              : gbtTx_o_R_A(1 to GBT_BANKS_USER_SETUP(3).NUM_LINKS); 
-- --------------------------------------------------------        
-- signal to_gbtBank3_mgt                                  : mgt_i_R;
-- signal from_gbtBank3_mgt                                : mgt_o_R; 
-- --------------------------------------------------------         
-- signal to_gbtBank3_gbtRx                                : gbtRx_i_R_A(1 to GBT_BANKS_USER_SETUP(3).NUM_LINKS); 
-- signal from_gbtBank3_gbtRx                              : gbtRx_o_R_A(1 to GBT_BANKS_USER_SETUP(3).NUM_LINKS);

-- signal reconfig_from_gbtBank3_mgtTxPll                  : std_logic_vector(45 downto 0);
-- signal mgtTxPllReconfig_from_gbtBank3_reconfCtrl_xn     : std_logic_vector(69 downto 0);
   --------------------------------------------------------
-- signal mgmtReset_to_gbtBank3_reconfCtrl_xn              : std_logic;   
-- signal mgmtAddress_to_gbtBank3_reconfCtrl_xn            : std_logic_vector(6 downto 0);  
-- signal mgmtRead_to_gbtBank3_reconfCtrl_xn               : std_logic;   
-- signal mgmtReadData_from_gbtBank3_reconfCtrl_xn         : std_logic_vector(31 downto 0);  
-- signal mgmtWaitRequest_from_gbtBank3_reconfCtrl_xn      : std_logic;   
-- signal mgmtWrite_to_gbtBank3_reconfCtrl_xn              : std_logic;   
-- signal mgmtWriteData_to_gbtBank3_reconfCtrl_xn          : std_logic_vector(31 downto 0);   
   
   --=========--
   -- RX data --
   --=========--   

   -- GBT Bank 1:
   --------------   

   signal gbtBank1_rxEncodingSel                           : std_logic_vector(1 downto 0);
   
   -- GBT Bank 2:
   --------------   

   signal gbtBank2_rxEncodingSel                           : std_logic_vector(1 downto 0);   
   
   --=====================================================================================--    

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
   --==================================== User Logic =====================================--

      --##############################################################################--
    --##################################################################################--
   --##################################              #####################################--
   --##################################  GBT Bank 1  #####################################--
   --##################################              #####################################--
    --##################################################################################--
      --##############################################################################--  

      
   --=========--
   -- TX data --
   --=========--
   
   -- TX encoding selector:
   ------------------------
   
   gbtBank1_txEncodingSel                                  <= "10" when GBT_BANKS_USER_SETUP(1).TX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
                                                              "01" when GBT_BANKS_USER_SETUP(1).TX_ENCODING = WIDE_BUS  else 
                                                              "00";                                                            -- Comment: GBT_FRAME

   -- Pattern generator:
   ---------------------
   
   -- Comment: * The pattern generated can be either:
   --
   --            - Counter pattern
   --            - Static pattern          
   --
   --          * The TEST_PATTERN_SEL_I port of both, the "pattern generator" as well as the "error detector" are controlled by the same port.
   
   gbtBank1_pattGen: entity work.gbt_pattern_generator
      port map (                                                        
         RESET_I                                           => gbtTxReset_from_gbtBank1_gbtBankRst,   
         TX_FRAMECLK_I                                     => txFrameClk_from_gbtBank1_txFrmClkStdPll,
         --------------------------------------------------     
         TX_ENCODING_SEL_I                                 => gbtBank1_txEncodingSel,
         TEST_PATTERN_SEL_I                                => GBTBANK1_TEST_PATTERN_SEL_I,
         STATIC_PATTERN_DATA_I                             => x"0000BABEAC1DACDCFFFFF",
         STATIC_PATTERN_EXTRADATA_WIDEBUS_I                => x"BEEFCAFE",
         STATIC_PATTERN_EXTRADATA_GBT8B10B_I               => x"C",
         --------------------------------------------------
         TX_DATA_O                                         => txData_from_gbtBank1_pattGen,
         TX_EXTRA_DATA_WIDEBUS_O                           => txExtraDataWidebus_from_gbtBank1_pattGen,
         TX_EXTRA_DATA_GBT8B10B_O                          => txExtraDataGbt8b10b_from_gbtBank1_pattGen
      );       
         
   GBTBANK1_TX_DATA_O                                      <= txData_from_gbtBank1_pattGen;
   GBTBANK1_TX_EXTRA_DATA_WIDEBUS_O                        <= txExtraDataWidebus_from_gbtBank1_pattGen;
   GBTBANK1_TX_EXTRA_DATA_GBT8B10B_O                       <= txExtraDataGbt8b10b_from_gbtBank1_pattGen;   -- Comment: Note!! Not implemented yet.
 
   --==========================--
   -- GBT Bank 1 resets scheme --
   --==========================--
   
   -- General reset:
   -----------------
   
   gbtBank1_genRst: entity work.alt_sv_reset
      generic map (
         CLK_FREQ                                          => 120e6)
      port map (                                           
         CLK_I                                             => GBTBANK1_MGT_REFCLK_I,
         RESET1_B_I                                        => not GBTBANK1_GENERAL_RESET_I,
         RESET2_B_I                                        => '1',                              
         RESET_O                                           => reset_from_gbtBank1_genRst                
      );

   -- GBT Bank reset:
   ------------------
   
   -- Comment:  * GENERAL_RESET_I is used to reset the GBT Bank sequentially.
   --             (see the timing diagram as a comment in the "gbtBankRst" module)  
   --
   --           * Manual resets are used to reset:
   --
   --             - "MANUAL_RESET_GT_TX_PLL_I" resets the MGT TX PLL.
   --
   --             - "MANUAL_RESET_TX_I" resets GBT_TX and MGT_TX.
   --
   --             - "MANUAL_RESET_RX_I" resets GBT_RX and MGT_RX.
   --
   --           * Note!! "gbtBankRst" must be synchronous with MGT_REFCLK when LATENCY-OPTIMIZED GBT Bank TX is used
   --                    thus the MGT clock dividers provide a TX_WORDCLK that is phase aligned with MGT_REFCLK.

   gbtBank1_gbtBankRst: entity work.gbt_bank_reset    
      generic map (
         RX_INIT_FIRST                                     => false,
         INITIAL_DELAY                                     => 1 * 40e6,   -- Comment: * 1s  
         TIME_N                                            => 1 * 40e6,   --          * 1s
         GAP_DELAY                                         => 1 * 40e6)   --          * 1s
      port map (     
         CLK_I                                             => txFrameClk_from_gbtBank1_txFrmClkStdPll,                                               
         --------------------------------------------------
         GENERAL_RESET_I                                   => reset_from_gbtBank1_genRst,                                                                 
         MANUAL_RESET_TX_I                                 => GBTBANK1_MANUAL_RESET_TX_I,
         MANUAL_RESET_RX_I                                 => GBTBANK1_MANUAL_RESET_RX_I,
         --------------------------------------------------         
         MGT_TX_RESET_O                                    => mgtTxReset_from_gbtBank1_gbtBankRst,                              
         MGT_RX_RESET_O                                    => mgtRxReset_from_gbtBank1_gbtBankRst,                             
         GBT_TX_RESET_O                                    => gbtTxReset_from_gbtBank1_gbtBankRst,                                      
         GBT_RX_RESET_O                                    => gbtRxReset_from_gbtBank1_gbtBankRst,                              
         --------------------------------------------------          
         BUSY_O                                            => open,                                                                         
         DONE_O                                            => open                                                                          
      );          
   
   to_gbtBank1_gbtTx(1).reset                              <= gbtTxReset_from_gbtBank1_gbtBankRst; 
   to_gbtBank1_mgt.mgtLink(1).tx_reset                     <= mgtTxReset_from_gbtBank1_gbtBankRst;
   to_gbtBank1_mgt.mgtLink(1).rx_reset                     <= mgtRxReset_from_gbtBank1_gbtBankRst;   
   to_gbtBank1_gbtRx(1).reset                              <= gbtRxReset_from_gbtBank1_gbtBankRst;
   
   reset_to_gbtBank1_mgtTxPll                              <= mgtTxReset_from_gbtBank1_gbtBankRst or GBTBANK1_MANUAL_RESET_GT_TX_PLL_I;   
   
   --==========================--
   -- GBT Bank 1 clocks scheme --
   --==========================-- 
   
   -- MGT Reference clock:
   -----------------------
   
   to_gbtBank1_clks.mgt_clks.mgtTxRefClk                   <= GBTBANK1_MGT_REFCLK_I;
   
   -- MGT TX PLL: 
   --------------
   
   -- Comment: Note!!  In this example design, the MGT TX PLL of GBT Bank 1 does not use "FEEDBACK_CLK" because
   --                  the optimization of the TX is standard so it is not necessary to have a known relationship
   --                  between "MGT_REFCLK" and "TX_WORDCLK".
   
   gbtBank_1_mgtTxPll: entity work.alt_sv_mgt_txpll
      port map (      
         RESET_I                                           => reset_to_gbtBank1_mgtTxPll,                           
         --------------------------------------------------    
         MGT_REFCLK_I                                      => GBTBANK1_MGT_REFCLK_I,                     
         FEEDBACK_CLK_I                                    => '0',                                                         
         EXTGTTXPLL_CLK_O                                  => to_gbtBank1_clks.mgt_clks.extGtTxPll_clk,                      
         --------------------------------------------------    
         POWER_DOWN_O                                      => to_gbtBank1_mgt.mgtCommon.extGtTxPll_powerDown,  
         LOCKED_O                                          => locked_from_gbtBank1_mgtTxPll,                              
         --------------------------------------------------    
         RECONFIG_I                                        => mgtTxPllReconfig_from_gbtBank1_reconfCtrl_x1,                         
         RECONFIG_O                                        => reconfig_from_gbtBank1_mgtTxPll                         
      );
      
   to_gbtBank1_mgt.mgtCommon.extGtTxPll_locked             <= locked_from_gbtBank1_mgtTxPll;
   GBTBANK1_MGT_TX_PLL_LOCKED_O                            <= locked_from_gbtBank1_mgtTxPll;
   
   -- MGT RX Reference clock:
   --------------------------
   
   to_gbtBank1_clks.mgt_clks.mgtRxRefClk                   <= GBTBANK1_MGT_REFCLK_I; 
   
   -- TX_WORDCLK & TX_FRAMECLK:
   ----------------------------
    
   -- Comment: The clock "txWrdClkMon_sampledClk" is used for monitoring the phase of TX_WORDCLK when LATENCY-OPTIMIZED GBT Bank TX is used. 
   --          (In this reference design, the TX of the GBT Link 1 of the GBT Bank is "standard" therefore it does not implement the TX_WORDCLK monitor)
   
   to_gbtBank1_clks.mgt_clks.txWrdClkMon_sampledClk        <= '0';
   
   -- Comment: Note!! In order to save clocking resources, it is strongly recommended that all GBT Links of the GBT Bank share the same "tx_wordClk"
   --                 (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "from_gbtBank2_clks.mgt_clks.tx_wordClk(1)" to feed
   --                   "to_gbtBank2_clks.mgt_clks.tx_wordClk(1)",  "to_gbtBank2_clks.mgt_clks.tx_wordClk(2)" and  "to_gbtBank2_clks.mgt_clks.tx_wordClk(3)").      
  
   to_gbtBank1_clks.mgt_clks.tx_wordClk(1)                 <= from_gbtBank1_clks.mgt_clks.tx_wordClk(1);
   
   -- Comment: * TX_FRAMECLK frequency: 40MHz
   --
   --          * "gbtBank1_txFrmClkStdPll" is automatically reset if lock is lost.
 
   gbtBank1_txFrmClkStdPll: entity alt_sv_gbt_tx_frameclk_stdpll.alt_sv_gbt_tx_frameclk_stdpll
      port map (
         REFCLK                                            => GBTBANK1_MGT_REFCLK_I,          
         RST                                               => '0',          
         OUTCLK_0                                          => txFrameClk_from_gbtBank1_txFrmClkStdPll,   -- Comment: This clock should be used for clocking user logic (40MHz@0deg).
         LOCKED                                            => GBTBANK1_TX_FRAMECLK_PLL_LOCKED_O
      );
   
   -- Comment: * When using "standard" GBT Bank, It is strongly recommended that all GBT Links of the GBT Bank share the same "tx_frameClk" in order to save clocking resources 
   --           (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "txFrameClk_from_txPll" to feed "to_gbtBank1_clks.tx_frameClk(1)",
   --            "to_gbtBank1_clks.tx_frameClk(2)" and  "to_gbtBank1_clks.tx_frameClk(3)").   
   --
   --          * When using the "latency-optimized" GBT Bank, each GBT Link of the GBT Bank use its own "tx_frameClk" that may be shifted independently in order to find the 
   --            correct sampling point between the TX_FRAMECLK/TX_WORDCLK clock domains in the GBT TX GEARBOX of each GBT Link. 

   to_gbtBank1_clks.tx_frameClk(1)                         <= txFrameClk_from_gbtBank1_txFrmClkStdPll;  
   
   -- RX_WORDCLK & RX_FRAMECLK:
   ----------------------------
   
   -- Comment: * Due to the Clock & Data Recovery (CDR), it is recommended that the "rx_wordClk" of each GBT Link of the GBT Bank should be clocked by its own "rx_wordClk".
   --
   --          * Each latency-optimized GBT Link of the GBT Bank should have its own RX_FRAMECLK aligner.    
   
   to_gbtBank1_clks.mgt_clks.rx_wordClk(1)                 <= from_gbtBank1_clks.mgt_clks.rx_wordClk(1); 
   
   -- Comment: * This phase aligner uses the header of the incoming data stream as a reference point for matching the rising
   --            edge of the recovered RX_FRAMECLK with the rising edge of the TX_FRAMECLK of the TRANSMITTER BOARD.
   -- 
   --          * Note!! The phase alignment is only triggered when LATENCY-OPTIMIZED GBT Bank RX is used.  
   --
   --          * RX_FRAMECLK frequency: 40MHz   
   
   gbtBank1_rxFrmClkPhAlgnr: entity work.gbt_rx_frameclk_phalgnr
      port map (
         RESET_I                                           => gbtRxReset_from_gbtBank1_gbtBankRst,
         --------------------------------------------------          
         RX_WORDCLK_I                                      => from_gbtBank1_clks.mgt_clks.rx_wordClk(1),
         RX_FRAMECLK_O                                     => rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr,         
         --------------------------------------------------           
         SYNC_ENABLE_I                                     => from_gbtBank1_gbtRx(1).latOptGbtBank_rx and from_gbtBank1_mgt.mgtLink(1).rxWordClkReady,
         SYNC_I                                            => from_gbtBank1_gbtRx(1).header_flag,
         --------------------------------------------------     
         DONE_O                                            => phaseAlignDone_from_gbtBank1_rxFrmClkPhAlgnr
      );                      
   
   GBTBANK1_RX_FRAMECLK_READY_O                            <= phaseAlignDone_from_gbtBank1_rxFrmClkPhAlgnr;
   
   to_gbtBank1_gbtRx(1).rxFrameClkReady                    <= phaseAlignDone_from_gbtBank1_rxFrmClkPhAlgnr;   
   to_gbtBank1_clks.rx_frameClk(1)                         <= rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr;   
   
   --============--
   -- GBT Bank 1 --
   --============--
   
   -- Comment: Note!! This example design instantiates two GBT Banks:
   --
   --          - GBT Bank 1: One GBT Link (Standard GBT TX and Latency-Optimized GBT RX).
   --
   --          - GBT Bank 2: Three GBT Links (Latency-Optimized GBT TX and Standard GBT RX).  
   
   gbtBank_1: entity work.gbt_bank
      generic map (
         GBT_BANK_ID                                       => 1)
      port map (                       
         CLKS_I                                            => to_gbtBank1_clks,                                  
         CLKS_O                                            => from_gbtBank1_clks,               
         --------------------------------------------------               
         GBT_TX_I                                          => to_gbtBank1_gbtTx,             
         GBT_TX_O                                          => from_gbtBank1_gbtTx,         
         --------------------------------------------------               
         MGT_I                                             => to_gbtBank1_mgt,              
         MGT_O                                             => from_gbtBank1_mgt,              
         --------------------------------------------------               
         GBT_RX_I                                          => to_gbtBank1_gbtRx,              
         GBT_RX_O                                          => from_gbtBank1_gbtRx         
      ); 
   
   -- MGT reconfiguration controller:
   ----------------------------------
   
   gbtBank_1_reconfCtrl_x1: entity alt_sv_gt_reconfctrl_x1.alt_sv_gt_reconfctrl_x1
      port map (
         RECONFIG_BUSY                                     => open,     
         MGMT_CLK_CLK                                      => GBTBANK1_MGT_REFCLK_I,             
         MGMT_RST_RESET                                    => mgmtReset_to_gbtBank1_reconfCtrl_x1,      
         RECONFIG_MGMT_ADDRESS                             => mgmtAddress_to_gbtBank1_reconfCtrl_x1,    
         RECONFIG_MGMT_READ                                => mgmtRead_to_gbtBank1_reconfCtrl_x1,         
         RECONFIG_MGMT_READDATA                            => mgmtReadData_from_gbtBank1_reconfCtrl_x1,      -- Comment: Note!! Left floating.   
         RECONFIG_MGMT_WAITREQUEST                         => mgmtWaitRequest_from_gbtBank1_reconfCtrl_x1,   -- Comment: Note!! Left floating.  
         RECONFIG_MGMT_WRITE                               => mgmtWrite_to_gbtBank1_reconfCtrl_x1,      
         RECONFIG_MGMT_WRITEDATA                           => mgmtWriteData_to_gbtBank1_reconfCtrl_x1,  
         CH0_0_TO_XCVR                                     => to_gbtBank1_mgt.mgtCommon.reconfigXcvr  ((GBT_BANKS_USER_SETUP(1).NUM_LINKS*70)-1 downto 0), 
         CH0_0_FROM_XCVR                                   => from_gbtBank1_mgt.mgtCommon.reconfigXcvr((GBT_BANKS_USER_SETUP(1).NUM_LINKS*46)-1 downto 0),
         CH1_1_TO_XCVR                                     => mgtTxPllReconfig_from_gbtBank1_reconfCtrl_x1,
         CH1_1_FROM_XCVR                                   => reconfig_from_gbtBank1_mgtTxPll 
      );
   
   -- Comment: The manual setup of the "MGT reconfiguration controller" is not used in this reference design.
   
   mgmtReset_to_gbtBank1_reconfCtrl_x1                     <= '0';         
   mgmtAddress_to_gbtBank1_reconfCtrl_x1                   <= "0000000";  
   mgmtRead_to_gbtBank1_reconfCtrl_x1                      <= '0';         
   mgmtWrite_to_gbtBank1_reconfCtrl_x1                     <= '0';         
   mgmtWriteData_to_gbtBank1_reconfCtrl_x1                 <= x"00000000"; 
   
   -- Serial lanes assignments:
   ----------------------------
   
   to_gbtBank1_mgt.mgtLink(1).rxSerialData                 <= GBTBANK1_MGT_RX;   
   GBTBANK1_MGT_TX                                         <= from_gbtBank1_mgt.mgtLink(1).txSerialData;    
   
   -- Data assignments:
   --------------------   

   to_gbtBank1_gbtTx(1).isDataSel                          <= GBTBANK1_TX_ISDATA_SEL_I;  
   GBTBANK1_RX_ISDATA_FLAG_O                               <= from_gbtBank1_gbtRx(1).isDataFlag;
   
   to_gbtBank1_gbtTx(1).data                               <= txData_from_gbtBank1_pattGen;   
   to_gbtBank1_gbtTx(1).extraData_wideBus                  <= txExtraDataWidebus_from_gbtBank1_pattGen;
   to_gbtBank1_gbtTx(1).extraData_gbt8b10b                 <= txExtraDataGbt8b10b_from_gbtBank1_pattGen;   -- Comment: Note!! Not implemented yet.
   
   -- Control assignments:
   -----------------------     
   
   to_gbtBank1_mgt.mgtLink(1).loopBack                     <= GBTBANK1_LOOPBACK_I;   

   -- Comment: * The manual RX_WORDCLK phase alignment control of the MGT(GT) transceivers is not used in this 
   --            reference design (auto RX_WORDCLK phase alignment is used instead).
   --
   --          * Note!! The manual RX_WORDCLK phase alignment control of the MGT(GT) MUST be synchronous with RX_WORDCLK
   --                   (in this example design this clock is "to_gbtBank1_clks.mgt_clks.rx_wordClk(1)").
   
   to_gbtBank1_mgt.mgtLink(1).rxSlide_enable               <= '1'; 
   to_gbtBank1_mgt.mgtLink(1).rxSlide_ctrl                 <= '0'; 
   to_gbtBank1_mgt.mgtLink(1).rxSlide_nbr                  <= "000000";
   to_gbtBank1_mgt.mgtLink(1).rxSlide_run                  <= '0';

   -- Comment: When using the latency-optimized GBT TX, the user MUST set the TX_WORDCLK monitoring in order to ensure a deterministic phase.
   
   to_gbtBank1_mgt.mgtCommon.txWrdClkMon_thresholdUp       <= x"00";
   to_gbtBank1_mgt.mgtCommon.txWrdClkMon_thresholdLow      <= x"00";
   to_gbtBank1_mgt.mgtCommon.txWrdClkMon_mgtTxRstEn        <= '0';
   
   GBTBANK1_LATOPT_GBTBANK_TX_O                            <= from_gbtBank1_gbtTx(1).latOptGbtBank_tx;
   GBTBANK1_LATOPT_GBTBANK_RX_O                            <= from_gbtBank1_gbtRx(1).latOptGbtBank_rx;
   GBTBANK1_MGT_READY_O                                    <= from_gbtBank1_mgt.mgtLink(1).ready;
   GBTBANK1_RX_WORDCLK_READY_O                             <= from_gbtBank1_mgt.mgtLink(1).rxWordClkReady;   
   GBTBANK1_RX_BITSLIP_NUMBER_O                            <= from_gbtBank1_gbtRx(1).bitSlipNbr;
   GBTBANK1_GBT_RX_READY_O                                 <= from_gbtBank1_gbtRx(1).ready;
   
   --=========--
   -- RX data --
   --=========--  
   
   -- RX encoding selector:
   ------------------------
   
   gbtBank1_rxEncodingSel                                  <= "10" when GBT_BANKS_USER_SETUP(1).RX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
                                                              "01" when GBT_BANKS_USER_SETUP(1).RX_ENCODING = WIDE_BUS  else 
                                                              "00";                                                            -- Comment: GBT_FRAME

   -- Pattern Checker:
   -------------------
   
   -- Comment: * This module checks:
   --
   --            - Errors in the received data. 
   --
   --            - GBT_RX_READY port status during data transmission.   
   --
   --          * If GBT_RX_READY goes low during data transmission, GBTRXREADY_LOST_FLAG_O is asserted until assertion of:
   --
   --            - RESET_GBTRXREADY_LOST_FLAG_I
   --            - GENERAL_RESET_I           
   --            - MANUAL_RESET_RX_I
   --
   --          * If GBT_RX_READY = '1':   
   --
   --            - "RXDATA_ERRORSEEN_FLAG_O" is asserted when an error is seen in "COMMON_DATA_I".         
   --            - "RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O" is asserted when an error is seen in "RX_EXTRA_DATA_WIDEBUS_I".
   --            - "RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O" is asserted when an error is seen in "RX_EXTRA_DATA_GBT8B10B_I".
   --
   --            - These flags are asserted until:
   --
   --              ~ RESET_GBTRXREADY_LOST_FLAG_I
   --              ~ GENERAL_RESET_I           
   --              ~ MANUAL_RESET_RX_I
   --
   --          * Note!! "RXDATA_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.
   --          * Note!! "RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.   
   --          * Note!! "RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.
   --
   --          * The TEST_PATTERN_SEL_I port of both, the "pattern generator" as well as the "patter checker" are controlled by the same port.

   gbtBank1_pattCheck: entity work.gbt_pattern_checker
      port map (
         RESET_I                                           => gbtRxReset_from_gbtBank1_gbtBankRst,         
         RX_FRAMECLK_I                                     => rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr, 
         --------------------------------------------------           
         RX_DATA_I                                         => from_gbtBank1_gbtRx(1).data,        
         RX_EXTRA_DATA_WIDEBUS_I                           => from_gbtBank1_gbtRx(1).extraData_widebus,
         RX_EXTRA_DATA_GBT8B10B_I                          => from_gbtBank1_gbtRx(1).extraData_gbt8b10b,
         --------------------------------------------------           
         GBT_RX_READY_I                                    => from_gbtBank1_gbtRx(1).ready,
         RX_ENCODING_SEL_I                                 => gbtBank1_rxEncodingSel,
         TEST_PATTERN_SEL_I                                => GBTBANK1_TEST_PATTERN_SEL_I,        
         STATIC_PATTERN_DATA_I                             => x"0000BABEAC1DACDCFFFFF",        
         STATIC_PATTERN_EXTRADATA_WIDEBUS_I                => x"BEEFCAFE",  
         STATIC_PATTERN_EXTRADATA_GBT8B10B_I               => x"C",
         RESET_GBTRXREADY_LOST_FLAG_I                      => GBTBANK1_RESET_GBTRXREADY_LOST_FLAG_I,               
         RESET_DATA_ERRORSEEN_FLAG_I                       => GBTBANK1_RESET_DATA_ERR_FLAG_I,  
         --------------------------------------------------           
         GBTRXREADY_LOST_FLAG_O                            => GBTBANK1_GBTRXREADY_LOST_FLAG_O, 
         RXDATA_ERRORSEEN_FLAG_O                           => GBTBANK1_RXDATA_ERR_FLAG_O,
         RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O              => GBTBANK1_RXEXTRADATA_WIDEBUS_ERR_FLAG_O,
         RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O             => GBTBANK1_RXEXTRADATA_GBT8B10B_ERR_FLAG_O
      ); 
      
   GBTBANK1_RX_DATA_O                                      <= from_gbtBank1_gbtRx(1).data;
   GBTBANK1_RX_EXTRA_DATA_WIDEBUS_O                        <= from_gbtBank1_gbtRx(1).extraData_widebus;
   GBTBANK1_RX_EXTRA_DATA_GBT8B10B_O                       <= from_gbtBank1_gbtRx(1).extraData_gbt8b10b;
   
   --=====================--
   -- Latency measurement --
   --=====================--   
   
   -- Clocks forwarding:
   ---------------------
   
   -- Comment: * The forwarding of these clocks allows to check the phase alignment of the different
   --            clocks using an oscilloscope.
   --
   --          * Note!! If RX DATA comes from another board with a different reference clock, 
   --                   then the TX_FRAMECLK/TX_WORDCLK domains are asynchronous with respect to the
   --                   RX_FRAMECLK/RX_WORDCLK domains.
   
   GBTBANK1_TX_FRAMECLK_O                                  <= txFrameClk_from_gbtBank1_txFrmClkStdPll;
   GBTBANK1_TX_WORDCLK_O                                   <= to_gbtBank1_clks.mgt_clks.tx_wordClk(1); 
   --------------------------------------------------------    
   GBTBANK1_RX_FRAMECLK_O                                  <= rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr; 
   GBTBANK1_RX_WORDCLK_O                                   <= to_gbtBank1_clks.mgt_clks.rx_wordClk(1);    
   
   -- Pattern match flags:
   -----------------------      
   
   -- Comment: * The latency of the link can be measured using an oscilloscope by comparing 
   --            the TX FLAG with the RX FLAG.
   --
   --          * Note!! The COUNTER TEST PATTERN must be used for this test.
   
   gbtBank1_txFlag: entity work.gbt_pattern_matchflag
      PORT MAP (
         RESET_I                                           => gbtTxReset_from_gbtBank1_gbtBankRst,
         CLK_I                                             => txFrameClk_from_gbtBank1_txFrmClkStdPll,
         DATA_I                                            => txData_from_gbtBank1_pattGen,
         MATCHFLAG_O                                       => GBTBANK1_TX_MATCHFLAG_O
      );
   
   gbtBank1_rxFlag: entity work.gbt_pattern_matchflag
      PORT MAP (
         RESET_I                                           => gbtRxReset_from_gbtBank1_gbtBankRst,
         CLK_I                                             => rxFrameClk_from_gbtBank1_rxFrmClkPhAlgnr,
         DATA_I                                            => from_gbtBank1_gbtRx(1).data,
         MATCHFLAG_O                                       => GBTBANK1_RX_MATCHFLAG_O
      );

      
       --#############################################################################--
    --##################################################################################--
   --##################################              #####################################--
   --##################################  GBT Bank 2  #####################################--
   --##################################              #####################################--
    --##################################################################################--
      --##############################################################################--     
       
      
   --=========--
   -- TX data --
   --=========--
   
   -- TX encoding selector:
   ------------------------
   
   gbtBank2_txEncodingSel                                  <= "10" when GBT_BANKS_USER_SETUP(2).TX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
                                                              "01" when GBT_BANKS_USER_SETUP(2).TX_ENCODING = WIDE_BUS  else 
                                                              "00";                                                            -- Comment: GBT_FRAME

   -- Pattern generators:
   ----------------------
   
   gbtBank2_pattGen_gen: for i in 1 to 3 generate
   
      -- Comment: * The pattern generated can be either:
      --
      --            - Counter pattern
      --            - Static pattern          
      --
      --          * The TEST_PATTERN_SEL_I port of both, the "pattern generator" as well as the "error detector" are controlled by the same port.
      
      gbtBank2_pattGen: entity work.gbt_pattern_generator
         port map (                                                        
            RESET_I                                        => gbtTxReset_from_gbtBank2_gbtBankRst(i),   
            TX_FRAMECLK_I                                  => txFrameClk_from_gbtBank2_txFrmClkPhAlgnr,
            -----------------------------------------------         
            TX_ENCODING_SEL_I                              => gbtBank2_txEncodingSel,
            TEST_PATTERN_SEL_I                             => GBTBANK2_TEST_PATTERN_SEL_I,
            STATIC_PATTERN_DATA_I                          => x"0000BABEAC1DACDCFFFFF",
            STATIC_PATTERN_EXTRADATA_WIDEBUS_I             => x"BEEFCAFE",
            STATIC_PATTERN_EXTRADATA_GBT8B10B_I            => x"C",
            -----------------------------------------------    
            TX_DATA_O                                      => txData_from_gbtBank2_pattGen(i),
            TX_EXTRA_DATA_WIDEBUS_O                        => txExtraDataWidebus_from_gbtBank2_pattGen(i),
            TX_EXTRA_DATA_GBT8B10B_O                       => txExtraDataGbt8b10b_from_gbtBank2_pattGen(i)
         );                                                
                                                           
      GBTBANK2_TX_DATA_O(i)                                <= txData_from_gbtBank2_pattGen(i);
      GBTBANK2_TX_EXTRA_DATA_WIDEBUS_O(i)                  <= txExtraDataWidebus_from_gbtBank2_pattGen(i);
      GBTBANK2_TX_EXTRA_DATA_GBT8B10B_O(i)                 <= txExtraDataGbt8b10b_from_gbtBank2_pattGen(i);   -- Comment: Note!! Not implemented yet.
 
   end generate;
 
   --==========================--
   -- GBT Bank 2 resets scheme --
   --==========================--
   
   -- General reset:
   -----------------
   
   gbtBank2_genRst: entity work.alt_sv_reset
      generic map (
         CLK_FREQ                                          => 120e6)
      port map (                                           
         CLK_I                                             => GBTBANK2_MGT_REFCLK_I,
         RESET1_B_I                                        => not GBTBANK2_GENERAL_RESET_I,
         RESET2_B_I                                        => not from_gbtBank2_mgt.mgtCommon.txWrdClkMon_reset,                              
         RESET_O                                           => reset_from_gbtBank2_genRst                
      );   
   
   -- TX_FRAMECLK phase aligner reset:
   -----------------------------------
   
   reset_to_gbtBank2_txFrmClkPhAlgnr                       <=    reset_from_gbtBank2_genRst
                                                              or GBTBANK2_TX_FRMCLKPHAL_MANUAL_RESET_I;  
 
   -- GBT Bank reset:
   ------------------
   
   -- Comment:  * GENERAL_RESET_I is used to reset the GBT Bank sequentially.
   --             (see the timing diagram as a comment in the "gbtBankRst" module)  
   --
   --           * Manual resets are used to reset:
   --
   --             - "MANUAL_RESET_GT_TX_PLL_I" resets the MGT TX PLL.
   --
   --             - "GBTBANK2_TX_FRMCLKPHAL_MANUAL_RESET_I" resets the TX_WORDCLK phase aligner
   --
   --             - "MANUAL_RESET_TX_I(i)" resets its corresponding GBT_TX and MGT_TX.
   --
   --             - "MANUAL_RESET_RX_I(i)" resets its corresponding GBT_RX and MGT_RX.
   --
   --           * Note!! "gbtBankRst" must be synchronous with "TX_FRAMECLK" when LATENCY-OPTIMIZED GBT Bank TX is used
   --                    thus the MGT clock dividers provide a TX_WORDCLK that is phase aligned with MGT_REFCLK and the 
   --                    header of the TX data frame..
   
   gbtBank2_gbtBankRst_gen: for i in 1 to 3 generate

      gbtBank2_gbtBankRst: entity work.gbt_bank_reset    
         generic map (
            RX_INIT_FIRST                                  => false,
            INITIAL_DELAY                                  => 1 * 40e6,   -- Comment: * 1s  
            TIME_N                                         => 1 * 40e6,   --          * 1s
            GAP_DELAY                                      => 1 * 40e6)   --          * 1s
         port map (     
            CLK_I                                          => txFrameClk_from_gbtBank2_txFrmClkPhAlgnr,                                               
            -----------------------------------------------    
            GENERAL_RESET_I                                => reset_from_gbtBank2_genRst,                                                               
            MANUAL_RESET_TX_I                              => GBTBANK2_MANUAL_RESET_TX_I(i),
            MANUAL_RESET_RX_I                              => GBTBANK2_MANUAL_RESET_RX_I(i),
            -----------------------------------------------         
            MGT_TX_RESET_O                                 => mgtTxReset_from_gbtBank2_gbtBankRst(i),                              
            MGT_RX_RESET_O                                 => mgtRxReset_from_gbtBank2_gbtBankRst(i),                             
            GBT_TX_RESET_O                                 => gbtTxReset_from_gbtBank2_gbtBankRst(i),                                      
            GBT_RX_RESET_O                                 => gbtRxReset_from_gbtBank2_gbtBankRst(i),                              
            -----------------------------------------------          
            BUSY_O                                         => open,                                                                         
            DONE_O                                         => open                                                                          
         );          
      
      to_gbtBank2_gbtTx(i).reset                           <= gbtTxReset_from_gbtBank2_gbtBankRst(i);
      to_gbtBank2_mgt.mgtLink(i).tx_reset                  <= mgtTxReset_from_gbtBank2_gbtBankRst(i);
      to_gbtBank2_mgt.mgtLink(i).rx_reset                  <= mgtRxReset_from_gbtBank2_gbtBankRst(i);   
      to_gbtBank2_gbtRx(i).reset                           <= gbtRxReset_from_gbtBank2_gbtBankRst(i);

   end generate;
   
   reset_to_gbtBank2_mgtTxPll                              <=    mgtTxReset_from_gbtBank2_gbtBankRst(1)
                                                              or GBTBANK2_MANUAL_RESET_GT_TX_PLL_I;
   
   --==========================--
   -- GBT Bank 2 clocks scheme --
   --==========================-- 

   -- MGT TX Reference clock:
   --------------------------
   
   to_gbtBank2_clks.mgt_clks.mgtTxRefClk                   <= GBTBANK2_MGT_REFCLK_I;
   
   -- MGT TX PLL: 
   --------------

   -- Comment: Note!!  In this example design, the MGT TX PLL of GBT Bank 2 use "TX_WORDCLK" as "FEEDBACK_CLK" because
   --                  the optimization of the TX is "latency-optimized" so it is necessary to have a known relationship
   --                  between "MGT_REFCLK" and "TX_WORDCLK".
   
   gbtBank_2_mgtTxPll: entity work.alt_sv_mgt_txpll
      port map (      
         RESET_I                                           => reset_to_gbtBank2_mgtTxPll,                           
         --------------------------------------------------    
         MGT_REFCLK_I                                      => GBTBANK2_MGT_REFCLK_I,                     
         FEEDBACK_CLK_I                                    => from_gbtBank2_clks.mgt_clks.tx_wordClk(1),                     
         EXTGTTXPLL_CLK_O                                  => to_gbtBank2_clks.mgt_clks.extGtTxPll_clk,                      
         --------------------------------------------------    
         POWER_DOWN_O                                      => to_gbtBank2_mgt.mgtCommon.extGtTxPll_powerDown,  
         LOCKED_O                                          => locked_from_gbtBank2_mgtTxPll,                              
         --------------------------------------------------    
         RECONFIG_I                                        => mgtTxPllReconfig_from_gbtBank2_reconfCtrl_x3,                         
         RECONFIG_O                                        => reconfig_from_gbtBank2_mgtTxPll                         
      );

   to_gbtBank2_mgt.mgtCommon.extGtTxPll_locked             <= locked_from_gbtBank2_mgtTxPll;
   GBTBANK2_MGT_TX_PLL_LOCKED_O                            <= locked_from_gbtBank2_mgtTxPll;
      
   -- MGT RX Reference clock:
   --------------------------
   
   to_gbtBank2_clks.mgt_clks.mgtRxRefClk                   <= GBTBANK2_MGT_REFCLK_I; 
   
   -- TX_WORDCLK & TX_FRAMECLK:
   ----------------------------
    
   -- Comment: * The clock "txWrdClkMon_sampledClk" is used for monitoring the phase of TX_WORDCLK when LATENCY-OPTIMIZED GBT Bank TX is used. 
   --
   --          * Note!!  "txWrdClkMon_sampledClk" MUST be slower than "MGT_REFCLK"
   
   to_gbtBank2_clks.mgt_clks.txWrdClkMon_sampledClk        <= FABRIC_CLK_I;   -- Comment: 50MHz
   
   -- Comment: Note!! In order to save clocking resources, it is strongly recommended that all GBT Links of the GBT Bank share the same "tx_wordClk"
   --                 (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "from_gbtBank2_clks.mgt_clks.tx_wordClk(1)" to feed
   --                  "to_gbtBank2_clks.mgt_clks.tx_wordClk(1)",  "to_gbtBank2_clks.mgt_clks.tx_wordClk(2)" and  "to_gbtBank2_clks.mgt_clks.tx_wordClk(3)").   
   
   to_gbtBank2_clks.mgt_clks.tx_wordClk(1)                 <= from_gbtBank2_clks.mgt_clks.tx_wordClk(1);
   to_gbtBank2_clks.mgt_clks.tx_wordClk(2)                 <= from_gbtBank2_clks.mgt_clks.tx_wordClk(2);
   to_gbtBank2_clks.mgt_clks.tx_wordClk(3)                 <= from_gbtBank2_clks.mgt_clks.tx_wordClk(3);
   
   -- Comment: * TX_FRAMECLK frequency: 40MHz  
   --
   --          * When using the "latency-optimized" GBT Bank, the outputs [1:3] may be used for clocking the "tx_frameClk" of the GBT Links of the GBT Bank. This 
   --            outputs may be shifted independently in order to find the correct sampling point between the TX_FRAMECLK/TX_WORDCLK clock domains in the GBT TX GEARBOX
   --            of each GBT Link. 
   
   gbtBank2_txFrmClkPhAlgnr: entity work.alt_sv_gbt_tx_frameclk_phalgnr
      port map (
         RESET_I                                           => reset_to_gbtBank2_txFrmClkPhAlgnr,          
         --------------------------------------------------
         TTC_CLK_I                                         => GBTBANK2_MGT_REFCLK_I,                   
         TX_FRAMECLK_USER_O                                => txFrameClk_from_gbtBank2_txFrmClkPhAlgnr,   -- Comment: This clock should be used for clocking user logic (40MHz@0deg).
         TX_FRAMECLK_GBTLINK1_O                            => txFrameClk_to_gbtBank2_gbtLink(1),          -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 1 when using "latency-optimized" GBT-Bank (40MHz@90deg).
         TX_FRAMECLK_GBTLINK2_O                            => txFrameClk_to_gbtBank2_gbtLink(2),          -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 2 when using "latency-optimized" GBT-Bank (40MHz@180deg).
         TX_FRAMECLK_GBTLINK3_O                            => txFrameClk_to_gbtBank2_gbtLink(3),          -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 3 when using "latency-optimized" GBT-Bank (40MHz@270deg).
         --------------------------------------------------
         GBTLINK1_PHASE_CNTSEL_I                           => "00001",   -- Comment: Note!! Do not modify (default value: 00001) (for more information see Altera AN-661-3.0, page 9)
         GBTLINK2_PHASE_CNTSEL_I                           => "00010",   -- Comment: Note!! Do not modify (default value: 00010) (for more information see Altera AN-661-3.0, page 9)
         GBTLINK3_PHASE_CNTSEL_I                           => "00011",   -- Comment: Note!! Do not modify (default value: 00011) (for more information see Altera AN-661-3.0, page 9)
         GBTLINK1_PHASE_STEPS_I                            => GBTBANK2_TX_FRMCLKPHAL_STEPSGBTLNK_I(1),  
         GBTLINK2_PHASE_STEPS_I                            => GBTBANK2_TX_FRMCLKPHAL_STEPSGBTLNK_I(2),   
         GBTLINK3_PHASE_STEPS_I                            => GBTBANK2_TX_FRMCLKPHAL_STEPSGBTLNK_I(3),   
         PHASE_STEPS_ENABLE_I                              => GBTBANK2_TX_FRMCLKPHAL_STEPSENABLE_I,
         PHASE_STEPS_TRIGGER_I                             => GBTBANK2_TX_FRMCLKPHAL_STEPSTRIGGER_I,   
         PLL_LOCKED_O                                      => GBTBANK2_TX_FRMCLKPHAL_LOCKED_O,     
         PHASE_SHIFT_DONE_O                                => GBTBANK2_TX_FRMCLKPHAL_PHSHIFTDONE_O     
      );
      
   -- Comment: * When using "standard" GBT Bank, It is strongly recommended that all GBT Links of the GBT Bank share the same "tx_frameClk" in order to save clocking resources 
   --           (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "txFrameClk_from_txPll" to feed "to_gbtBank1_clks.tx_frameClk(1)",
   --            "to_gbtBank1_clks.tx_frameClk(2)" and  "to_gbtBank1_clks.tx_frameClk(3)").   
   --
   --          * When using the "latency-optimized" GBT Bank, each GBT Link of the GBT Bank use its own "tx_frameClk" that may be shifted independently in order to find the 
   --            correct sampling point between the TX_FRAMECLK/TX_WORDCLK clock domains in the GBT TX GEARBOX of each GBT Link. 
   
   to_gbtBank2_clks.tx_frameClk(1)                         <= txFrameClk_to_gbtBank2_gbtLink(1);    
   to_gbtBank2_clks.tx_frameClk(2)                         <= txFrameClk_to_gbtBank2_gbtLink(2);    
   to_gbtBank2_clks.tx_frameClk(3)                         <= txFrameClk_to_gbtBank2_gbtLink(3);    
   
   -- RX_WORDCLK & RX_FRAMECLK:
   ----------------------------
   
   -- Comment: * Due to the Clock & Data Recovery (CDR), it is recommended that the "rx_wordClk" of each GBT Link of the GBT Bank should be clocked by its own "rx_wordClk".
   --
   --          * Each latency-optimized GBT Link of the GBT Bank should have its own RX_FRAMECLK aligner.    
   
   to_gbtBank2_clks.mgt_clks.rx_wordClk(1)                 <= from_gbtBank2_clks.mgt_clks.rx_wordClk(1);
   to_gbtBank2_clks.mgt_clks.rx_wordClk(2)                 <= from_gbtBank2_clks.mgt_clks.rx_wordClk(2);
   to_gbtBank2_clks.mgt_clks.rx_wordClk(3)                 <= from_gbtBank2_clks.mgt_clks.rx_wordClk(3);
   
   -- Comment: * In order to save clocking resources, when using Standard GBT Bank, only one PLL ("gbtBank2_rxPll") clocked by "to_gbtBank2_clks.mgt_clks.rx_wordClk(1)" is used
   --            to clock: "to_gbtBank2_clks.rx_frameClk(1)", "to_gbtBank2_clks.rx_frameClk(2)", "to_gbtBank2_clks.rx_frameClk(3)".  
   --
   --          * RX_FRAMECLK frequency: 40MHz   
 
   gbtBank2_rxFrmClkStdPll: entity alt_sv_gbt_rx_frameclk_stdpll.alt_sv_gbt_rx_frameclk_stdpll
      port map (
         REFCLK                                            => from_gbtBank2_clks.mgt_clks.rx_wordClk(1),       
         RST                                               => gbtRxReset_from_gbtBank2_gbtBankRst(1),          
         OUTCLK_0                                          => rxFrameClk_from_gbtBank2_rxFrmClkStdPll,   
         LOCKED                                            => locked_from_gbtBank2_rxFrmClkStdPll        
      );
   
   to_gbtBank2_gbtRx(1).rxFrameClkReady                    <= locked_from_gbtBank2_rxFrmClkStdPll;
   to_gbtBank2_gbtRx(2).rxFrameClkReady                    <= locked_from_gbtBank2_rxFrmClkStdPll;
   to_gbtBank2_gbtRx(3).rxFrameClkReady                    <= locked_from_gbtBank2_rxFrmClkStdPll;
   --------------------------------------------------------
   GBTBANK2_RX_FRAMECLK_READY_O                            <= locked_from_gbtBank2_rxFrmClkStdPll;
      
   to_gbtBank2_clks.rx_frameClk(1)                         <= rxFrameClk_from_gbtBank2_rxFrmClkStdPll;  
   to_gbtBank2_clks.rx_frameClk(2)                         <= rxFrameClk_from_gbtBank2_rxFrmClkStdPll;  
   to_gbtBank2_clks.rx_frameClk(3)                         <= rxFrameClk_from_gbtBank2_rxFrmClkStdPll;  
   
   --============--
   -- GBT Bank 2 --
   --============--
   
   -- Note!! This example design instantiates two GBT Banks:
   --
   --        - GBT Bank 1: One GBT Link (Standard GBT TX and Latency-Optimized GBT RX).
   --
   --        - GBT Bank 2: Three GBT Links (Latency-Optimized GBT TX and Standard GBT RX).     
   
   gbtBank_2: entity work.gbt_bank
      generic map (
         GBT_BANK_ID                                       => 2)
      port map (                       
         CLKS_I                                            => to_gbtBank2_clks,                                  
         CLKS_O                                            => from_gbtBank2_clks,               
         --------------------------------------------------               
         GBT_TX_I                                          => to_gbtBank2_gbtTx,             
         GBT_TX_O                                          => from_gbtBank2_gbtTx,         
         --------------------------------------------------               
         MGT_I                                             => to_gbtBank2_mgt,              
         MGT_O                                             => from_gbtBank2_mgt,              
         --------------------------------------------------               
         GBT_RX_I                                          => to_gbtBank2_gbtRx,              
         GBT_RX_O                                          => from_gbtBank2_gbtRx         
      ); 
      
   -- MGT reconfiguration controller:
   ----------------------------------
   
   gbtBank_2_reconfCtrl_x3: entity alt_sv_gt_reconfctrl_x3.alt_sv_gt_reconfctrl_x3
      port map (
         RECONFIG_BUSY                                     => open,     
         MGMT_CLK_CLK                                      => GBTBANK2_MGT_REFCLK_I,             
         MGMT_RST_RESET                                    => mgmtReset_to_gbtBank2_reconfCtrl_x3,      
         RECONFIG_MGMT_ADDRESS                             => mgmtAddress_to_gbtBank2_reconfCtrl_x3,    
         RECONFIG_MGMT_READ                                => mgmtRead_to_gbtBank2_reconfCtrl_x3,       
         RECONFIG_MGMT_READDATA                            => mgmtReadData_from_gbtBank2_reconfCtrl_x3,      -- Comment: Note!! Left floating.   
         RECONFIG_MGMT_WAITREQUEST                         => mgmtWaitRequest_from_gbtBank2_reconfCtrl_x3,   -- Comment: Note!! Left floating.
         RECONFIG_MGMT_WRITE                               => mgmtWrite_to_gbtBank2_reconfCtrl_x3,      
         RECONFIG_MGMT_WRITEDATA                           => mgmtWriteData_to_gbtBank2_reconfCtrl_x3,  
         CH0_2_TO_XCVR                                     => to_gbtBank2_mgt.mgtCommon.reconfigXcvr  ((GBT_BANKS_USER_SETUP(2).NUM_LINKS*70)-1 downto 0),  
         CH0_2_FROM_XCVR                                   => from_gbtBank2_mgt.mgtCommon.reconfigXcvr((GBT_BANKS_USER_SETUP(2).NUM_LINKS*46)-1 downto 0),
         CH3_3_TO_XCVR                                     => mgtTxPllReconfig_from_gbtBank2_reconfCtrl_x3,
         CH3_3_FROM_XCVR                                   => reconfig_from_gbtBank2_mgtTxPll 
      );
   
   -- Comment: The manual setup of the "MGT reconfiguration controller" is not used in this reference design.
   
   mgmtReset_to_gbtBank2_reconfCtrl_x3                     <= '0';         
   mgmtAddress_to_gbtBank2_reconfCtrl_x3                   <= "0000000";  
   mgmtRead_to_gbtBank2_reconfCtrl_x3                      <= '0';         
   mgmtWrite_to_gbtBank2_reconfCtrl_x3                     <= '0';         
   mgmtWriteData_to_gbtBank2_reconfCtrl_x3                 <= x"00000000"; 
   
   gbtBank2_assignments_gen: for i in 1 to 3 generate

      -- Serial lanes assignments:
      ----------------------------
      
      to_gbtBank2_mgt.mgtLink(i).rxSerialData              <= GBTBANK2_MGT_RX(i);   
      GBTBANK2_MGT_TX(i)                                   <= from_gbtBank2_mgt.mgtLink(i).txSerialData;    
      
      -- Data assignments:
      --------------------   

      to_gbtBank2_gbtTx(i).isDataSel                       <= GBTBANK2_TX_ISDATA_SEL_I(i);  
      GBTBANK2_RX_ISDATA_FLAG_O(i)                         <= from_gbtBank2_gbtRx(i).isDataFlag;
      
      to_gbtBank2_gbtTx(i).data                            <= txData_from_gbtBank2_pattGen(i);   
      to_gbtBank2_gbtTx(i).extraData_wideBus               <= txExtraDataWidebus_from_gbtBank2_pattGen(i);
      to_gbtBank2_gbtTx(i).extraData_gbt8b10b              <= txExtraDataGbt8b10b_from_gbtBank2_pattGen(i);   -- Comment: Note!! Not implemented yet.
      
      -- Control assignments:
      -----------------------     
      
      to_gbtBank2_mgt.mgtLink(i).loopBack                  <= GBTBANK2_LOOPBACK_I(i);
      
      -- Comment: * The manual RX_WORDCLK phase alignment control of the MGT(GT) transceivers is not used in this 
      --            reference design (auto RX_WORDCLK phase alignment is used instead).
      --
      --          * Note!! The manual RX_WORDCLK phase alignment control of the MGT(GT) MUST be synchronous with RX_WORDCLK
      --                   (in this example design this clock are: "to_gbtBank2_clks.mgt_clks.rx_wordClk(1)", "to_gbtBank2_clks.mgt_clks.rx_wordClk(1)")
      --                    and "to_gbtBank2_clks.mgt_clks.rx_wordClk(1)".
      
      to_gbtBank2_mgt.mgtLink(i).rxSlide_enable            <= '1'; 
      to_gbtBank2_mgt.mgtLink(i).rxSlide_ctrl              <= '0'; 
      to_gbtBank2_mgt.mgtLink(i).rxSlide_nbr               <= "000000";
      to_gbtBank2_mgt.mgtLink(i).rxSlide_run               <= '0';

      GBTBANK2_MGT_READY_O(i)                              <= from_gbtBank2_mgt.mgtLink(i).ready;
      GBTBANK2_RX_WORDCLK_READY_O(i)                       <= from_gbtBank2_mgt.mgtLink(i).rxWordClkReady;   
      GBTBANK2_RX_BITSLIP_NUMBER_O(i)                      <= from_gbtBank2_gbtRx(i).bitSlipNbr;
      GBTBANK2_GBT_RX_READY_O(i)                           <= from_gbtBank2_gbtRx(i).ready;
   
   end generate;
   
   -- Comment: * When using the latency-optimized GBT TX, the user MUST set the TX_WORDCLK monitoring in order to ensure a deterministic phase.
   --
   --          * The TX_WORDCLK monitoring is shared by all GBT Links of the GBT Bank.
   
   to_gbtBank2_mgt.mgtCommon.txWrdClkMon_thresholdUp       <= GBTBANK2_TX_WRDCLKMON_THRESHOLD_UP_I;  
   to_gbtBank2_mgt.mgtCommon.txWrdClkMon_thresholdLow      <= GBTBANK2_TX_WRDCLKMON_THRESHOLD_LOW_I; 
   to_gbtBank2_mgt.mgtCommon.txWrdClkMon_mgtTxRstEn        <= GBTBANK2_TX_WRDCLKMON_MGTTXRESET_EN_I;
   
   GBTBANK2_TX_WRDCLKMON_STATS_O                           <= from_gbtBank2_mgt.mgtCommon.txWrdClkMon_stats;   
   GBTBANK2_TX_WRDCLKMON_PHASEOK_O                         <= from_gbtBank2_mgt.mgtCommon.txWrdClkMon_phaseOk;
   
   -- Comment: All GBT Links of the same GBT Bank have the same optimization (either "standard" of "latency-optimized").
   
   GBTBANK2_LATOPT_GBTBANK_TX_O                            <= from_gbtBank2_gbtTx(1).latOptGbtBank_tx;
   GBTBANK2_LATOPT_GBTBANK_RX_O                            <= from_gbtBank2_gbtRx(1).latOptGbtBank_rx;
   
   --=========--
   -- RX data --
   --=========--  
   
   -- RX encoding selector:
   ------------------------
   
   gbtBank2_rxEncodingSel                                  <= "10" when GBT_BANKS_USER_SETUP(2).RX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
                                                              "01" when GBT_BANKS_USER_SETUP(2).RX_ENCODING = WIDE_BUS  else 
                                                              "00";                                                            -- Comment: GBT_FRAME

   -- Pattern Checker:
   -------------------
   
   -- Comment: * This module checks:
   --
   --            - Errors in the received data. 
   --
   --            - GBT_RX_READY port status during data transmission.   
   --
   --          * If GBT_RX_READY goes low during data transmission, GBTRXREADY_LOST_FLAG_O is asserted until assertion of:
   --
   --            - RESET_GBTRXREADY_LOST_FLAG_I
   --            - GENERAL_RESET_I           
   --            - MANUAL_RESET_RX_I
   --
   --          * If GBT_RX_READY = '1':   
   --
   --            - "RXDATA_ERRORSEEN_FLAG_O" is asserted when an error is seen in "COMMON_DATA_I".         
   --            - "RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O" is asserted when an error is seen in "RX_EXTRA_DATA_WIDEBUS_I".
   --            - "RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O" is asserted when an error is seen in "RX_EXTRA_DATA_GBT8B10B_I".
   --
   --            - These flags are asserted until:
   --
   --              ~ RESET_GBTRXREADY_LOST_FLAG_I
   --              ~ GENERAL_RESET_I           
   --              ~ MANUAL_RESET_RX_I
   --
   --          * Note!! "RXDATA_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.
   --          * Note!! "RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.   
   --          * Note!! "RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O" must be reset after selecting a new test pattern.
   --
   --          * The TEST_PATTERN_SEL_I port of both, the "pattern generator" as well as the "patter checker" are controlled by the same port.

   gbtBank2_pattCheck_gen: for i in 1 to 3 generate
   
      gbtBank2_pattCheck: entity work.gbt_pattern_checker
         port map (
            RESET_I                                        => gbtRxReset_from_gbtBank2_gbtBankRst(i),         
            RX_FRAMECLK_I                                  => rxFrameClk_from_gbtBank2_rxFrmClkStdPll, 
            -----------------------------------------------          
            RX_DATA_I                                      => from_gbtBank2_gbtRx(i).data,        
            RX_EXTRA_DATA_WIDEBUS_I                        => from_gbtBank2_gbtRx(i).extraData_widebus,
            RX_EXTRA_DATA_GBT8B10B_I                       => from_gbtBank2_gbtRx(i).extraData_gbt8b10b,
            -----------------------------------------------           
            GBT_RX_READY_I                                 => from_gbtBank2_gbtRx(i).ready,
            RX_ENCODING_SEL_I                              => gbtBank2_rxEncodingSel,
            TEST_PATTERN_SEL_I                             => GBTBANK2_TEST_PATTERN_SEL_I,        
            STATIC_PATTERN_DATA_I                          => x"0000BABEAC1DACDCFFFFF",        
            STATIC_PATTERN_EXTRADATA_WIDEBUS_I             => x"BEEFCAFE",  
            STATIC_PATTERN_EXTRADATA_GBT8B10B_I            => x"C",
            RESET_GBTRXREADY_LOST_FLAG_I                   => GBTBANK2_RESET_GBTRXREADY_LOST_FLAG_I(i),               
            RESET_DATA_ERRORSEEN_FLAG_I                    => GBTBANK2_RESET_DATA_ERR_FLAG_I(i),  
            -----------------------------------------------           
            GBTRXREADY_LOST_FLAG_O                         => GBTBANK2_GBTRXREADY_LOST_FLAG_O(i), 
            RXDATA_ERRORSEEN_FLAG_O                        => GBTBANK2_RXDATA_ERR_FLAG_O(i),
            RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O           => GBTBANK2_RXEXTRADATA_WIDEBUS_ERR_FLAG_O(i),
            RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O          => GBTBANK2_RXEXTRADATA_GBT8B10B_ERR_FLAG_O(i)
         ); 
         
      GBTBANK2_RX_DATA_O(i)                                <= from_gbtBank2_gbtRx(i).data;
      GBTBANK2_RX_EXTRA_DATA_WIDEBUS_O(i)                  <= from_gbtBank2_gbtRx(i).extraData_widebus;
      GBTBANK2_RX_EXTRA_DATA_GBT8B10B_O(i)                 <= from_gbtBank2_gbtRx(i).extraData_gbt8b10b;
   
   end generate;
   
   --=====================--
   -- Latency measurement --
   --=====================--   
   
   -- Clocks forwarding:
   ---------------------
   
   -- Comment: * The forwarding of these clocks allows to check the phase alignment of the different
   --            clocks using an oscilloscope.
   --
   --          * Note!! If RX DATA comes from another board with a different reference clock, 
   --                   then the TX_FRAMECLK/TX_WORDCLK domains are asynchronous with respect to the
   --                   RX_FRAMECLK/RX_WORDCLK domains.
   
   GBTBANK2_USER_TX_FRAMECLK_O                             <= txFrameClk_from_gbtBank2_txFrmClkPhAlgnr;
   GBTBANK2_GBTLINK_TX_FRAMECLK_O                          <= txFrameClk_to_gbtBank2_gbtLink;            -- Comment: One "GBTLINK_TX_FRAMECLK" per GBT Link of GBT Bank 2.
   --------------------------------------------------------
   GBTBANK2_RX_FRAMECLK_O                                  <= rxFrameClk_from_gbtBank2_rxFrmClkStdPll; 

   GBTBANK2_TX_WORDCLK_O                                   <= to_gbtBank2_clks.mgt_clks.tx_wordClk;      -- Comment: One "TX_WORDCLK" per GBT Link of GBT Bank 2.
   GBTBANK2_RX_WORDCLK_O                                   <= to_gbtBank2_clks.mgt_clks.rx_wordClk;      -- Comment: One "RX_WORDCLK" per GBT Link of GBT Bank 2.   
   
   -- Pattern match flags:
   -----------------------      
   
   gbtBank2_pattMatchFlags_gen: for i in 1 to 3 generate   
   
      -- Comment: * The latency of the link can be measured using an oscilloscope by comparing 
      --            the TX FLAG with the RX FLAG.
      --
      --          * Note!! The COUNTER TEST PATTERN must be used for this test.
      
      gbtBank2_txFlag: entity work.gbt_pattern_matchflag
         PORT MAP (
            RESET_I                                        => gbtTxReset_from_gbtBank2_gbtBankRst(i),
            CLK_I                                          => txFrameClk_from_gbtBank2_txFrmClkPhAlgnr,
            DATA_I                                         => txData_from_gbtBank2_pattGen(i),
            MATCHFLAG_O                                    => GBTBANK2_TX_MATCHFLAG_O(i)
         );
      
      gbtBank2_rxFlag: entity work.gbt_pattern_matchflag
         PORT MAP (
            RESET_I                                        => gbtRxReset_from_gbtBank2_gbtBankRst(i),
            CLK_I                                          => rxFrameClk_from_gbtBank2_rxFrmClkStdPll,
            DATA_I                                         => from_gbtBank2_gbtRx(i).data,
            MATCHFLAG_O                                    => GBTBANK2_RX_MATCHFLAG_O(i)
         );     

   end generate;
   
      
      --##############################################################################--
    --##################################################################################--
   --##################################              #####################################--
   --##################################  GBT Bank 3  #####################################--
   --##################################              #####################################--
    --##################################################################################--
      --##############################################################################--  


   --============--
   -- GBT Bank 3 --
   --============--
      
-- gbtBank_3: entity work.gbt_bank
--    generic map (
--      GBT_BANK_ID                                        => 3)
--    port map (                       
--       CLKS_I                                            => to_gbtBank_3_clks,                                  
--       CLKS_O                                            => from_gbtBank_3_clks,               
--       --------------------------------------------------               
--       GBT_TX_I                                          => to_gbtBank_3_gbtTx,             
--       GBT_TX_O                                          => from_gbtBank_3_gbtTx,         
--       --------------------------------------------------                   
--       MGT_I                                             => to_gbtBank_3_mgt,              
--       MGT_O                                             => from_gbtBank_3_mgt,              
--       --------------------------------------------------               
--       GBT_RX_I                                          => to_gbtBank_3_gbtRx,              
--       GBT_RX_O                                          => from_gbtBank_3_gbtRx         
--    ); 

   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--