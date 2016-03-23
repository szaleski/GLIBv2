--=================================================================================================--
--##################################   Package Information   ######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Altera Cyclone V - Multi Gigabit Transceivers standard
--                                                                                                 
-- Language:              VHDL'93                                                                 
--                                                                                                   
-- Target Device:         Altera Cyclone V                                                      
-- Tool version:          Quartus II 13.1                                                              
--                                                                                                   
-- Revision:              3.0                                                                      
--
-- Description:           
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        06/04/2014   3.0       M. Barros Marin   First .vhd module definition           
--
-- Additional Comments:                                                                               
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
library alt_cv_gt_std_x1;
library alt_cv_gt_std_x2;
library alt_cv_gt_std_x3;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity mgt_std is
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

      GBT_RX_MGT_RDY_O                          : out std_logic_vector (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      GBT_RX_RXWORDCLK_READY_O                  : out std_logic_vector (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
      
      --=======-- 
      -- Words -- 
      --=======-- 
 
      GBT_TX_WORD_I                             : in  word_mxnbit_A    (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);     
      GBT_RX_WORD_O                             : out word_mxnbit_A    (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS) 
   
   );
end mgt_std;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of mgt_std is 

   --================================ Signal Declarations ================================--
   
   --===================--
   -- Reset controllers --
   --===================--  
   
   signal txAnalogReset_from_gtRstCtrl          : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txDigitalReset_from_gtRstCtrl         : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal txReady_from_gtRstCtrl                : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   ---------------------------------------------
   signal rxAnalogreset_from_gtRstCtrl          : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxDigitalreset_from_gtRstCtrl         : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxReady_from_gtRstCtrl                : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);

   --=======================================--
   -- Multi-Gigabit Transceivers (standard) --
   --=======================================--      

   signal rxIsLockedToData_from_gtStd           : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);   
   signal txCalBusy_from_gtStd                  : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS);
   signal rxCalBusy_from_gtStd                  : std_logic_vector     (1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS); 
   
   --=====================================================================================--   
   
--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
   --==================================== User Logic =====================================-- 
   
   --=============--
   -- Assignments --
   --=============--
   
   commonAssign_gen: for i in 1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS generate

      MGT_O.mgtLink(i).rxWordClkReady           <= rxReady_from_gtRstCtrl(i);
      GBT_RX_RXWORDCLK_READY_O(i)               <= rxReady_from_gtRstCtrl(i);      
      MGT_O.mgtLink(i).txCal_busy               <= txCalBusy_from_gtStd(i);
      MGT_O.mgtLink(i).rxCal_busy               <= rxCalBusy_from_gtStd(i);
      MGT_O.mgtLink(i).ready                    <= txReady_from_gtRstCtrl(i) and rxReady_from_gtRstCtrl(i);
      MGT_O.mgtLink(i).tx_ready                 <= txReady_from_gtRstCtrl(i);
      MGT_O.mgtLink(i).rx_ready                 <= rxReady_from_gtRstCtrl(i);
      GBT_RX_MGT_RDY_O(i)                       <= rxReady_from_gtRstCtrl(i);         
      MGT_O.mgtLink(i).rxIsLocked_toData        <= rxIsLockedToData_from_gtStd(i);
      
   end generate;
   
   --=====================--
   -- GT reset controllers --
   --=====================--      
   
   gtRstCtrl_gen: for i in 1 to GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS generate
   
      gtRstCtrl: entity work.alt_cv_mgt_resetctrl       
         port map (
            CLK_I                               => MGT_CLKS_I.mgtTxRefClk,                            
            ------------------------------------         
            TX_RESET_I                          => MGT_I.mgtLink(i).tx_reset,    
            RX_RESET_I                          => MGT_I.mgtLink(i).rx_reset,    
            ------------------------------------          
            TX_ANALOGRESET_O                    => txAnalogReset_from_gtRstCtrl(i),
            TX_DIGITALRESET_O                   => txDigitalReset_from_gtRstCtrl(i),                  
            TX_READY_O                          => txReady_from_gtRstCtrl(i),                         
            PLL_LOCKED_I                        => MGT_I.mgtCommon.extGtTxPll_locked,                       
            TX_CAL_BUSY_I                       => txCalBusy_from_gtStd(i),                          
            ------------------------------------          
            RX_ANALOGRESET_O                    => rxAnalogreset_from_gtRstCtrl(i),
            RX_DIGITALRESET_O                   => rxDigitalreset_from_gtRstCtrl(i),                          
            RX_READY_O                          => rxReady_from_gtRstCtrl(i),                         
            RX_IS_LOCKEDTODATA_I                => rxIsLockedToData_from_gtStd(i),                     
            RX_CAL_BUSY_I                       => rxCalBusy_from_gtStd(i)                                 
         );
   
   end generate;
   
   --=======================================--
   -- Multi-Gigabit Transceivers (standard) --
   --=======================================--
   
   -- MGT standard x1:
   -------------------
   
   gtStd_x1_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS = 1 generate
      
      gtStd_x1: entity alt_cv_gt_std_x1.alt_cv_gt_std_x1
         port map (
            PLL_POWERDOWN(0)                    => MGT_I.mgtCommon.extGtTxPll_powerDown,    
            TX_ANALOGRESET(0)                   => txAnalogReset_from_gtRstCtrl(1), 
            TX_DIGITALRESET(0)                  => txDigitalReset_from_gtRstCtrl(1), 
            TX_SERIAL_DATA(0)                   => MGT_O.mgtLink(1).txSerialData,                      
            EXT_PLL_CLK(0)                      => MGT_CLKS_I.extGtTxPll_clk,                 
            RX_ANALOGRESET(0)                   => rxAnalogReset_from_gtRstCtrl(1),
            RX_DIGITALRESET(0)                  => rxDigitalReset_from_gtRstCtrl(1),
            RX_CDR_REFCLK(0)                    => MGT_CLKS_I.mgtRxRefClk,        
            RX_SERIAL_DATA(0)                   => MGT_I.mgtLink(1).rxSerialData,       
            RX_IS_LOCKEDTOREF(0)                => MGT_O.mgtLink(1).rxIsLocked_toRef,    
            RX_IS_LOCKEDTODATA (0)              => rxIsLockedToData_from_gtStd(1),
            RX_SERIALLPBKEN(0)                  => MGT_I.mgtLink(1).loopBack, 
            TX_STD_CORECLKIN(0)                 => MGT_CLKS_I.tx_wordClk(1),      
            RX_STD_CORECLKIN(0)                 => MGT_CLKS_I.rx_wordClk(1),      
            TX_STD_CLKOUT(0)                    => MGT_CLKS_O.tx_wordClk(1),         
            RX_STD_CLKOUT(0)                    => MGT_CLKS_O.rx_wordClk(1),         
            TX_STD_POLINV(0)                    => MGT_I.mgtLink(1).tx_polarity,      
            RX_STD_POLINV(0)                    => MGT_I.mgtLink(1).rx_polarity,      
            TX_CAL_BUSY(0)                      => txCalBusy_from_gtStd(1),         
            RX_CAL_BUSY(0)                      => rxCalBusy_from_gtStd(1),         
            RECONFIG_TO_XCVR                    => MGT_I.mgtCommon.reconfigXcvr((1*70)-1 downto 0),  
            RECONFIG_FROM_XCVR                  => MGT_O.mgtCommon.reconfigXcvr((1*46)-1 downto 0),
            TX_PARALLEL_DATA                    => GBT_TX_WORD_I(1),
            RX_PARALLEL_DATA                    => GBT_RX_WORD_O(1)    
         );
      
   end generate;
   
   -- MGT standard x2:
   -------------------
   
   gtStd_x2_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS = 2 generate
      
      gtStd_x2: entity alt_cv_gt_std_x2.alt_cv_gt_std_x2
         port map (
            PLL_POWERDOWN(0)                    => MGT_I.mgtCommon.extGtTxPll_powerDown,     
            TX_ANALOGRESET(0)                   => txAnalogReset_from_gtRstCtrl(1), 
            TX_ANALOGRESET(1)                   => txAnalogReset_from_gtRstCtrl(2), 
            TX_DIGITALRESET(0)                  => txDigitalReset_from_gtRstCtrl(1), 
            TX_DIGITALRESET(1)                  => txDigitalReset_from_gtRstCtrl(2), 
            TX_SERIAL_DATA(0)                   => MGT_O.mgtLink(1).txSerialData,                      
            TX_SERIAL_DATA(1)                   => MGT_O.mgtLink(2).txSerialData,                      
            EXT_PLL_CLK(0)                      => MGT_CLKS_I.extGtTxPll_clk,                
            RX_ANALOGRESET(0)                   => rxAnalogReset_from_gtRstCtrl(1),
            RX_ANALOGRESET(1)                   => rxAnalogReset_from_gtRstCtrl(2),
            RX_DIGITALRESET(0)                  => rxDigitalReset_from_gtRstCtrl(1),
            RX_DIGITALRESET(1)                  => rxDigitalReset_from_gtRstCtrl(2),
            RX_CDR_REFCLK(0)                    => MGT_CLKS_I.mgtRxRefClk,     
            RX_SERIAL_DATA(0)                   => MGT_I.mgtLink(1).rxSerialData,       
            RX_SERIAL_DATA(1)                   => MGT_I.mgtLink(2).rxSerialData,       
            RX_IS_LOCKEDTOREF(0)                => MGT_O.mgtLink(1).rxIsLocked_toRef,    
            RX_IS_LOCKEDTOREF(1)                => MGT_O.mgtLink(2).rxIsLocked_toRef,    
            RX_IS_LOCKEDTODATA(0)               => rxIsLockedToData_from_gtStd(1),
            RX_IS_LOCKEDTODATA(1)               => rxIsLockedToData_from_gtStd(2),
            RX_SERIALLPBKEN(0)                  => MGT_I.mgtLink(1).loopBack, 
            RX_SERIALLPBKEN(1)                  => MGT_I.mgtLink(2).loopBack, 
            TX_STD_CORECLKIN(0)                 => MGT_CLKS_I.tx_wordClk(1),      
            TX_STD_CORECLKIN(1)                 => MGT_CLKS_I.tx_wordClk(2),      
            RX_STD_CORECLKIN(0)                 => MGT_CLKS_I.rx_wordClk(1),      
            RX_STD_CORECLKIN(1)                 => MGT_CLKS_I.rx_wordClk(2),      
            TX_STD_CLKOUT(0)                    => MGT_CLKS_O.tx_wordClk(1),         
            TX_STD_CLKOUT(1)                    => MGT_CLKS_O.tx_wordClk(2),         
            RX_STD_CLKOUT(0)                    => MGT_CLKS_O.rx_wordClk(1),         
            RX_STD_CLKOUT(1)                    => MGT_CLKS_O.rx_wordClk(2),         
            TX_STD_POLINV(0)                    => MGT_I.mgtLink(1).tx_polarity,      
            TX_STD_POLINV(1)                    => MGT_I.mgtLink(2).tx_polarity,      
            RX_STD_POLINV(0)                    => MGT_I.mgtLink(1).rx_polarity,      
            RX_STD_POLINV(1)                    => MGT_I.mgtLink(2).rx_polarity,      
            TX_CAL_BUSY(0)                      => txCalBusy_from_gtStd(1),         
            TX_CAL_BUSY(1)                      => txCalBusy_from_gtStd(2),         
            RX_CAL_BUSY(0)                      => rxCalBusy_from_gtStd(1),         
            RX_CAL_BUSY(1)                      => rxCalBusy_from_gtStd(2),         
            RECONFIG_TO_XCVR                    => MGT_I.mgtCommon.reconfigXcvr((2*70)-1 downto 0), 
            RECONFIG_FROM_XCVR                  => MGT_O.mgtCommon.reconfigXcvr((2*46)-1 downto 0),
            TX_PARALLEL_DATA(39 downto  0)      => GBT_TX_WORD_I(1),
            TX_PARALLEL_DATA(79 downto 40)      => GBT_TX_WORD_I(2),
            RX_PARALLEL_DATA(39 downto  0)      => GBT_RX_WORD_O(1),
            RX_PARALLEL_DATA(79 downto 40)      => GBT_RX_WORD_O(2)
         );
      
   end generate;
   
   -- MGT standard x3:
   -------------------
   
   gtStd_x3_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).NUM_LINKS = 3 generate
      
      gtStd_x3: entity alt_cv_gt_std_x3.alt_cv_gt_std_x3
         port map (
            PLL_POWERDOWN(0)                    => MGT_I.mgtCommon.extGtTxPll_powerDown,    
            TX_ANALOGRESET(0)                   => txAnalogReset_from_gtRstCtrl(1), 
            TX_ANALOGRESET(1)                   => txAnalogReset_from_gtRstCtrl(2), 
            TX_ANALOGRESET(2)                   => txAnalogReset_from_gtRstCtrl(3), 
            TX_DIGITALRESET(0)                  => txDigitalReset_from_gtRstCtrl(1), 
            TX_DIGITALRESET(1)                  => txDigitalReset_from_gtRstCtrl(2), 
            TX_DIGITALRESET(2)                  => txDigitalReset_from_gtRstCtrl(3), 
            TX_SERIAL_DATA(0)                   => MGT_O.mgtLink(1).txSerialData,                      
            TX_SERIAL_DATA(1)                   => MGT_O.mgtLink(2).txSerialData,                      
            TX_SERIAL_DATA(2)                   => MGT_O.mgtLink(3).txSerialData,                      
            EXT_PLL_CLK(0)                      => MGT_CLKS_I.extGtTxPll_clk,                
            RX_ANALOGRESET(0)                   => rxAnalogReset_from_gtRstCtrl(1),
            RX_ANALOGRESET(1)                   => rxAnalogReset_from_gtRstCtrl(2),
            RX_ANALOGRESET(2)                   => rxAnalogReset_from_gtRstCtrl(3),
            RX_DIGITALRESET(0)                  => rxDigitalReset_from_gtRstCtrl(1),
            RX_DIGITALRESET(1)                  => rxDigitalReset_from_gtRstCtrl(2),
            RX_DIGITALRESET(2)                  => rxDigitalReset_from_gtRstCtrl(3),
            RX_CDR_REFCLK(0)                    => MGT_CLKS_I.mgtRxRefClk,      
            RX_SERIAL_DATA(0)                   => MGT_I.mgtLink(1).rxSerialData,       
            RX_SERIAL_DATA(1)                   => MGT_I.mgtLink(2).rxSerialData,       
            RX_SERIAL_DATA(2)                   => MGT_I.mgtLink(3).rxSerialData,       
            RX_IS_LOCKEDTOREF(0)                => MGT_O.mgtLink(1).rxIsLocked_toRef,    
            RX_IS_LOCKEDTOREF(1)                => MGT_O.mgtLink(2).rxIsLocked_toRef,    
            RX_IS_LOCKEDTOREF(2)                => MGT_O.mgtLink(3).rxIsLocked_toRef,    
            RX_IS_LOCKEDTODATA(0)               => rxIsLockedToData_from_gtStd(1),
            RX_IS_LOCKEDTODATA(1)               => rxIsLockedToData_from_gtStd(2),
            RX_IS_LOCKEDTODATA(2)               => rxIsLockedToData_from_gtStd(3),
            RX_SERIALLPBKEN(0)                  => MGT_I.mgtLink(1).loopBack, 
            RX_SERIALLPBKEN(1)                  => MGT_I.mgtLink(2).loopBack, 
            RX_SERIALLPBKEN(2)                  => MGT_I.mgtLink(3).loopBack, 
            TX_STD_CORECLKIN(0)                 => MGT_CLKS_I.tx_wordClk(1),      
            TX_STD_CORECLKIN(1)                 => MGT_CLKS_I.tx_wordClk(2),      
            TX_STD_CORECLKIN(2)                 => MGT_CLKS_I.tx_wordClk(3),      
            RX_STD_CORECLKIN(0)                 => MGT_CLKS_I.rx_wordClk(1),      
            RX_STD_CORECLKIN(1)                 => MGT_CLKS_I.rx_wordClk(2),      
            RX_STD_CORECLKIN(2)                 => MGT_CLKS_I.rx_wordClk(3),      
            TX_STD_CLKOUT(0)                    => MGT_CLKS_O.tx_wordClk(1),         
            TX_STD_CLKOUT(1)                    => MGT_CLKS_O.tx_wordClk(2),         
            TX_STD_CLKOUT(2)                    => MGT_CLKS_O.tx_wordClk(3),         
            RX_STD_CLKOUT(0)                    => MGT_CLKS_O.rx_wordClk(1),         
            RX_STD_CLKOUT(1)                    => MGT_CLKS_O.rx_wordClk(2),         
            RX_STD_CLKOUT(2)                    => MGT_CLKS_O.rx_wordClk(3),         
            TX_STD_POLINV(0)                    => MGT_I.mgtLink(1).tx_polarity,      
            TX_STD_POLINV(1)                    => MGT_I.mgtLink(2).tx_polarity,      
            TX_STD_POLINV(2)                    => MGT_I.mgtLink(3).tx_polarity,      
            RX_STD_POLINV(0)                    => MGT_I.mgtLink(1).rx_polarity,      
            RX_STD_POLINV(1)                    => MGT_I.mgtLink(2).rx_polarity,      
            RX_STD_POLINV(2)                    => MGT_I.mgtLink(3).rx_polarity,      
            TX_CAL_BUSY(0)                      => txCalBusy_from_gtStd(1),         
            TX_CAL_BUSY(1)                      => txCalBusy_from_gtStd(2),         
            TX_CAL_BUSY(2)                      => txCalBusy_from_gtStd(3),         
            RX_CAL_BUSY(0)                      => rxCalBusy_from_gtStd(1),         
            RX_CAL_BUSY(1)                      => rxCalBusy_from_gtStd(2),         
            RX_CAL_BUSY(2)                      => rxCalBusy_from_gtStd(3),         
            RECONFIG_TO_XCVR                    => MGT_I.mgtCommon.reconfigXcvr((3*70)-1 downto 0), 
            RECONFIG_FROM_XCVR                  => MGT_O.mgtCommon.reconfigXcvr((3*46)-1 downto 0), 
            TX_PARALLEL_DATA( 39 downto  0)     => GBT_TX_WORD_I(1),
            TX_PARALLEL_DATA( 79 downto 40)     => GBT_TX_WORD_I(2),
            TX_PARALLEL_DATA(119 downto 80)     => GBT_TX_WORD_I(3),
            RX_PARALLEL_DATA( 39 downto  0)     => GBT_RX_WORD_O(1),
            RX_PARALLEL_DATA( 79 downto 40)     => GBT_RX_WORD_O(2),
            RX_PARALLEL_DATA(119 downto 80)     => GBT_RX_WORD_O(3)
         );
      
   end generate;
      
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--