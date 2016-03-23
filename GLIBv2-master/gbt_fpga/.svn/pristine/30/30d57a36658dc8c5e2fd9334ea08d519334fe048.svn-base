--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                        (Original design by P. Vichoudis (CERN) & M. Barros Marin)                          
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Altera Stratix V - MGT latency-optimized TX_WORDCLK monitoring
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
--                        18/03/2014   3.0       M. Barros Marin   First .vhd module definition.
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

-- Custom libraries and packages:
use work.vendor_specific_gbt_bank_package.all;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity alt_sv_mgt_latopt_txwordclkmon is
   port ( 

      --=======--
      -- Reset --
      --=======--
      
      TX_RESET_I                                : in  std_logic; 
      
      --===============--
      -- Clocks scheme --
      --===============--   
      
      -- Sampled clock:
      -----------------
      
      SAMPLED_CLK_I                             : in  std_logic;      
      
      -- MGT_REFCLK:                     
      --------------            
      
      MGT_TX_PLL_LOCKED_I                       : in  std_logic;
      ------------------------------------------
      MGT_REFCLK_I                              : in  std_logic;
      
      -- TX_WORCLK:
      -------------
      
      TX_READY_I                                : in std_logic;
      ------------------------------------------
      TX_WORDCLK_I                              : in  std_logic;
      
      --=================--
      -- Monitor control --
      --=================--
      
      THRESHOLD_UP_I                            : in  std_logic_vector(7 downto 0);
      STATS_O                                   : out std_logic_vector(7 downto 0);
      THRESHOLD_LOW_I                           : in  std_logic_vector(7 downto 0);
      PHASE_OK_O                                : out std_logic;
      DONE_O                                    : out std_logic;
      
      --==============--
      -- MGT TX reset --
      --==============--

      MGT_TX_RESET_EN_I                         : in  std_logic;
      MGT_TX_RESET_O                            : out std_logic
      
   );
end alt_sv_mgt_latopt_txwordclkmon;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of alt_sv_mgt_latopt_txwordclkmon is
   
   --================================ Signal Declarations ================================--
   
   --==================--
   -- Comparison stage --
   --==================--
   
   signal sampledClk_from_mgtRefClkReg          : std_logic;
   signal sampledClk_from_txWordClkReg          : std_logic;
   
   signal test_from_xorGate                     : std_logic;
   
   --==================--
   -- Processing stage --
   --==================--
   
   signal test_from_xorGate_r                   : std_logic;
   
   signal mgtTxResetEn_r2                       : std_logic;
   signal mgtTxResetEn_r                        : std_logic;
   
   signal state                                 : txWordClkMonitoringFsmLatOpt_T;
   signal timer                                 : unsigned(31 downto 0);
   signal monitoringStats                       : unsigned(15 downto 0);  
   
   --=====================================================================================--   
   
--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--

   --==================================== User Logic =====================================--

   --==================--
   -- Comparison stage --
   --==================--
   
   -- MGT_REFCLK register:
   -----------------------
   
   mgtRefClkReg: process(TX_RESET_I, MGT_REFCLK_I)
   begin
      if TX_RESET_I = '1' then
         sampledClk_from_mgtRefClkReg           <= '0';
      elsif rising_edge(MGT_REFCLK_I) then
         sampledClk_from_mgtRefClkReg           <= SAMPLED_CLK_I;
      end if;
   end process;

   -- TX_WORCLK register:
   ----------------------
   
   txWordClkReg: process(TX_RESET_I, TX_WORDCLK_I)
   begin
      if TX_RESET_I = '1' then
         sampledClk_from_txWordClkReg           <= '0';
      elsif rising_edge(TX_WORDCLK_I) then
         sampledClk_from_txWordClkReg           <= SAMPLED_CLK_I;
      end if;
   end process;  
   
   -- XOR gate:
   ------------
   
   test_from_xorGate                            <= sampledClk_from_mgtRefClkReg xor sampledClk_from_txWordClkReg;

   --==================--
   -- Processing stage --
   --==================--

   main: process(TX_RESET_I, MGT_REFCLK_I)
   begin
      if TX_RESET_I = '1' then
         test_from_xorGate_r                    <= '0';
         mgtTxResetEn_r2                        <= '0'; 
         mgtTxResetEn_r                         <= '0';
         state                                  <= s0_idle;
         timer                                  <= (others => '0');
         monitoringStats                        <= (others => '0');
         STATS_O                                <= (others => '0');
         MGT_TX_RESET_O                         <= '0';
         PHASE_OK_O                             <= '0';
         DONE_O                                 <= '0';
      elsif rising_edge(MGT_REFCLK_I) then      
         
         -- XOR gate registered output:
         ------------------------------
         
         test_from_xorGate_r                    <= test_from_xorGate;         
         
         -- Synchronizers:
         -----------------
         
         mgtTxResetEn_r2                        <= mgtTxResetEn_r;
         mgtTxResetEn_r                         <= MGT_TX_RESET_EN_I;
         
         -- Finite Sate Machine (FSM):
         -----------------------------
         
         case state is
            when s0_idle =>
               if (MGT_TX_PLL_LOCKED_I = '1') and (TX_READY_I = '1') then
                  state                         <= s1_stats;
               end if;
            when s1_stats =>         
               if timer = TXWORDCLK_MON_TIMEOUT-1 then
                  if mgtTxResetEn_r2 = '1' then 
                     state                      <= s2_thresholds;
                     timer                      <= (others => '0');
                     DONE_O                     <= '0'; 
                  else
                     DONE_O                     <= '1';                     
                  end if;
               else
                  if test_from_xorGate_r = '1' then
                     monitoringStats            <= monitoringStats + 1; 
                  end if;   
                  timer                         <= timer + 1;
               end if;
            when s2_thresholds =>
               if     (monitoringStats(15 downto 8) < unsigned(THRESHOLD_UP_I))            
                  and (monitoringStats(15 downto 8) > unsigned(THRESHOLD_LOW_I))
               then
                  state                         <= s4_phaseOk;
               else
                  state                         <= s3_resetMgtTx;
               end if;               
            when s3_resetMgtTx =>
               MGT_TX_RESET_O                   <= '1';
            when s4_phaseOk => 
               PHASE_OK_O                       <= '1';
               DONE_O                           <= '1';               
         end case;
         
         -- Stats out:
         -------------
         
         STATS_O                                <= std_logic_vector(monitoringStats(15 downto 8));
         
      end if;
   end process;
   
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--