--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Altera Cyclone V - GBT TX_FRAMECLK phase aligner    
--                                                                                                 
-- Language:              VHDL'93                                                              
--                                                                                                   
-- Target Device:         Altera Cyclone V                                                   
-- Tool version:          Quartus II 13.1                                                                  
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--                                                                  
--                        07/04/2014   3.0       M. Barros Marin   First .vhd module definition.
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

-- Libraries for direct instantiation:
library alt_cv_gbt_tx_frameclk_phalgnr_pll;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity alt_cv_gbt_tx_frameclk_phalgnr is   
   port (  
   
      --=======--
      -- Reset --
      --=======--     
   
      RESET_I                                   : in  std_logic;
      
      --===============--
      -- Clocks scheme --
      --===============--                
      
      TTC_CLK_I                                 : in  std_logic;         
      
      TX_FRAMECLK_USER_O                        : out std_logic;   -- Comment: This clock should be used for clocking user logic (40MHz@0deg).
      ------------------------------------------
      TX_FRAMECLK_GBTLINK1_O                    : out std_logic;   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 1 when using "latency-optimized" GBT-Bank.
      TX_FRAMECLK_GBTLINK2_O                    : out std_logic;   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 2 when using "latency-optimized" GBT-Bank.
      TX_FRAMECLK_GBTLINK3_O                    : out std_logic;   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 3 when using "latency-optimized" GBT-Bank.
      
      --=========--
      -- Control --
      --=========--                        

      GBTLINK1_PHASE_CNTSEL_I                   : in  std_logic_vector(4 downto 0);      
      GBTLINK2_PHASE_CNTSEL_I                   : in  std_logic_vector(4 downto 0);      
      GBTLINK3_PHASE_CNTSEL_I                   : in  std_logic_vector(4 downto 0);      
      -----------------------------------------------
      GBTLINK1_PHASE_STEPS_I                    : in  std_logic_vector(5 downto 0);
      GBTLINK2_PHASE_STEPS_I                    : in  std_logic_vector(5 downto 0);
      GBTLINK3_PHASE_STEPS_I                    : in  std_logic_vector(5 downto 0);
      ------------------------------------------
      PHASE_STEPS_ENABLE_I                      : in  std_logic; 
      PHASE_STEPS_TRIGGER_I                     : in  std_logic; 
      
      PLL_LOCKED_O                              : out std_logic;
      PHASE_SHIFT_DONE_O                        : out std_logic 
      
   );
end alt_cv_gbt_tx_frameclk_phalgnr;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of alt_cv_gbt_tx_frameclk_phalgnr is 
  
   --================================ Signal Declarations ================================--
   
   signal phaseStepsTrigger_r2                  : std_logic;      
   signal phaseStepsTrigger_r                   : std_logic;      
   ---------------------------------------------
   signal oneStepDone_from_pll_r2               : std_logic;      
   signal oneStepDone_from_pll_r                : std_logic;      

   signal pllLocked_from_pll                    : std_logic;     
   signal phaseEn_from_fsm                      : std_logic;     
   signal cntSel_from_fsm                       : std_logic_vector(4 downto 0); 
   signal oneStepDone_from_pll                  : std_logic; 
 
   --=====================================================================================--   

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--

   --==================================== User Logic =====================================--

   --=========--
   -- Control --
   --=========--

   main: process(RESET_I, pllLocked_from_pll, TTC_CLK_I)   
      -- constant GBTLINK1                         : std_logic_vector(4 downto 0) := "00001";
      -- constant GBTLINK2                         : std_logic_vector(4 downto 0) := "00010";
      -- constant GBTLINK3                         : std_logic_vector(4 downto 0) := "00011";     
      -- Comment: Each 0.520ns step needs 3 shifts (tVCO/8=1.388ns/8=173.611ps per shift). 
      constant STEP_520PS                       : integer := 3;
      ------------------------------------------
      type stateT is (e0_idle, e1_waitTrigger,
                      e2_gbtLink1_shift,  e3_gbtLink1_assertPhaseEn,  e4_gbtLink1_waitOneStepDone,  e5_gbtLink1_waitDoneHigh,  
                      e6_gbtLink2_shift,  e7_gbtLink2_assertPhaseEn,  e8_gbtLink2_waitOneStepDone,  e9_gbtLink2_waitDoneHigh,  
                      e10_gbtLink3_shift, e11_gbtLink3_assertPhaseEn, e12_gbtLink3_waitOneStepDone, e13_gbtLink3_waitDoneHigh,  
                      e14_phaseShiftDone);
      variable state                            : stateT;                
      -- Comment: One clock cycle of TX_FRAMECLK (25 ns) is 48 steps of 520 ps.
      variable counter1                         : integer range 0 to 63*STEP_520PS;  
      variable counter2                         : integer range 0 to 63*STEP_520PS;
      variable counter3                         : integer range 0 to 63*STEP_520PS;
   begin                                                                  
      if (RESET_I = '1') or (pllLocked_from_pll = '0') then
         PLL_LOCKED_O                           <= '0';
         PHASE_SHIFT_DONE_O                     <= '0';
         phaseStepsTrigger_r2                   <= '0';
         phaseStepsTrigger_r                    <= '0';
         oneStepDone_from_pll_r2                <= '1';   
         oneStepDone_from_pll_r                 <= '1';   
         ---------------------------------------
         state                                  := e0_idle;
         counter1                               := 0;
         counter2                               := 0;
         counter3                               := 0;
         cntSel_from_fsm                        <= GBTLINK1_PHASE_CNTSEL_I;--GBTLINK1;   
         phaseEn_from_fsm                       <= '0';
      elsif rising_edge(TTC_CLK_I) then
         
         -- Register:
         ------------
         
         PLL_LOCKED_O                           <= pllLocked_from_pll;   
         
         -- Edge detectors:
         ------------------
         
         phaseStepsTrigger_r2    <= phaseStepsTrigger_r;    phaseStepsTrigger_r <= PHASE_STEPS_TRIGGER_I;
         
         oneStepDone_from_pll_r2 <= oneStepDone_from_pll_r; oneStepDone_from_pll_r <= oneStepDone_from_pll;

         -- Finite State Machine (FSM):
         ------------------------------
         
         case state is 
            when e0_idle =>               
               if PHASE_STEPS_ENABLE_I = '1' then
                  state                         := e1_waitTrigger; 
               end if;               
            when e1_waitTrigger =>
               if phaseStepsTrigger_r2 = '1' then
                  state                         := e2_gbtLink1_shift;
                  PHASE_SHIFT_DONE_O            <= '0';
                  counter1                      := to_integer(unsigned(GBTLINK1_PHASE_STEPS_I)) * 3; 
                  counter2                      := to_integer(unsigned(GBTLINK2_PHASE_STEPS_I)) * 3;                        
                  counter3                      := to_integer(unsigned(GBTLINK3_PHASE_STEPS_I)) * 3;    
                  cntSel_from_fsm               <= GBTLINK1_PHASE_CNTSEL_I;--GBTLINK1;
               end if;             
            -- GBT Link 1:            
            when e2_gbtLink1_shift =>                 
               if counter1 = 0 then
                  state                         := e6_gbtLink2_shift;   
                  cntSel_from_fsm               <= GBTLINK2_PHASE_CNTSEL_I;--GBTLINK2;   
               else       
                  counter1                      := counter1 - 1;
                  state                         := e3_gbtLink1_assertPhaseEn;                
               end if;
            when e3_gbtLink1_assertPhaseEn =>   
                  phaseEn_from_fsm              <= '1';
                  state                         := e4_gbtLink1_waitOneStepDone;               
            when e4_gbtLink1_waitOneStepDone =>   
               if (oneStepDone_from_pll_r2 = '1') and (oneStepDone_from_pll_r = '0') then
                  phaseEn_from_fsm              <= '0';
                  state                         := e5_gbtLink1_waitDoneHigh; 
               end if;
            when e5_gbtLink1_waitDoneHigh =>
               if (oneStepDone_from_pll_r2 = '0') and (oneStepDone_from_pll_r = '1') then
                  state                         := e2_gbtLink1_shift;
               end if;            
            -- GBT Link 2:            
            when e6_gbtLink2_shift =>                 
               if counter2 = 0 then
                  state                         := e10_gbtLink3_shift;   
                  cntSel_from_fsm               <= GBTLINK3_PHASE_CNTSEL_I;--GBTLINK3;
               else       
                  counter2                      := counter2 - 1;
                  state                         := e7_gbtLink2_assertPhaseEn;                
               end if;
            when e7_gbtLink2_assertPhaseEn =>   
                  phaseEn_from_fsm              <= '1';
                  state                         := e8_gbtLink2_waitOneStepDone;               
            when e8_gbtLink2_waitOneStepDone =>   
               if (oneStepDone_from_pll_r2 = '1') and (oneStepDone_from_pll_r = '0') then
                  phaseEn_from_fsm              <= '0';
                  state                         := e9_gbtLink2_waitDoneHigh; 
               end if;
            when e9_gbtLink2_waitDoneHigh =>
               if (oneStepDone_from_pll_r2 = '0') and (oneStepDone_from_pll_r = '1') then
                  state                         := e6_gbtLink2_shift;
               end if;      
            -- GBT Link 3:            
            when e10_gbtLink3_shift =>                 
               if counter3 = 0 then
                  state                         := e14_phaseShiftDone;   
               else       
                  counter3                      := counter3 - 1;
                  state                         := e11_gbtLink3_assertPhaseEn;                
               end if;
            when e11_gbtLink3_assertPhaseEn =>   
                  phaseEn_from_fsm              <= '1';
                  state                         := e12_gbtLink3_waitOneStepDone;               
            when e12_gbtLink3_waitOneStepDone =>   
               if (oneStepDone_from_pll_r2 = '1') and (oneStepDone_from_pll_r = '0') then
                  phaseEn_from_fsm              <= '0';
                  state                         := e13_gbtLink3_waitDoneHigh; 
               end if;
            when e13_gbtLink3_waitDoneHigh =>
               if (oneStepDone_from_pll_r2 = '0') and (oneStepDone_from_pll_r = '1') then
                  state                         := e10_gbtLink3_shift;
               end if;      
            -- Done:
            when e14_phaseShiftDone =>
               PHASE_SHIFT_DONE_O               <= '1';         
               if (phaseStepsTrigger_r2 = '1') and (phaseStepsTrigger_r = '0') then
                  state                         := e0_idle;   
               end if;
         end case;                                            
                                                                             -- Comment: -- PLL Configuration: -- 
         end if;                                                             --          -- N = 1              --        
   end process;                                                              --          -- M = 6              --  
                                                                             --          -- C = 18             -- 
   --=====--                                                                 --          -- VCO = 720MHz       -- 
   -- PLL --                                                                 --          -- Shift = 173.6ps    --         
   --=====--                                                                 --          ------------------------
   
   pll: entity alt_cv_gbt_tx_frameclk_phalgnr_pll.alt_cv_gbt_tx_frameclk_phalgnr_pll                                               
      port map (                                                                                      
         REFCLK                                 => TTC_CLK_I,                                         
         RST                                    => RESET_I,                                           
         OUTCLK_0                               => TX_FRAMECLK_USER_O,       -- Comment: This clock should be used for clocking user logic (40MHz@0deg).    
         OUTCLK_1                               => TX_FRAMECLK_GBTLINK1_O,   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 1 when using "latency-optimized" GBT-Bank.                                            
         OUTCLK_2                               => TX_FRAMECLK_GBTLINK2_O,   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 2 when using "latency-optimized" GBT-Bank.                                        
         OUTCLK_3                               => TX_FRAMECLK_GBTLINK3_O,   -- Comment: This clock is reserved for clocking the "tx_frameClk" of GBT Link 3 when using "latency-optimized" GBT-Bank.                                         
         LOCKED                                 => pllLocked_from_pll,                                  
         PHASE_EN                               => phaseEn_from_fsm,                                  
         SCANCLK                                => TTC_CLK_I,                                         
         UPDN                                   => '1',
         CNTSEL                                 => cntSel_from_fsm,
         PHASE_DONE                             => oneStepDone_from_pll
      );
      
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--