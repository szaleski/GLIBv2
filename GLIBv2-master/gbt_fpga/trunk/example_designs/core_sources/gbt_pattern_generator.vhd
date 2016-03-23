--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                        (Original design by F. Marin (CPPM) & S.Baron (CERN))   
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           GBT pattern generator                                        
--                                                                                                 
-- Language:              VHDL'93                                                                  
--                                                                                                   
-- Target Device:         Device agnostic                                                         
-- Tool version:                                                                            
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:             
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--                                                                 
--                        10/05/2009   0.1       F. Marin          First .bdf entity definition.           
--                                                                 
--                        07/07/2009   0.2       S. Baron          Translate from .bdf to .vhd.
--                                                                 
--                        15/01/2010   0.3       S. Baron          Use of agnostic counters.
--                                                                 
--                        23/06/2013   1.0       M. Barros Marin   - Cosmetic modifications
--                                                                 - Added pattern selector multiplexor
--                                                                 - Added static pattern
--                                                                 - Merged with "agnostic_21bits_counter"
--                                                                 
--                        06/08/2013   1.1       M. Barros Marin   - Added Wide-Bus extra data generation
--                                                                 
--                        12/02/2014   3.0       M. Barros Marin   - Added GBT-8b10b extra data generation
--                                                                 - Removed dynamic encoding selection
--                                                                 
-- Additional Comments:                                                                               
--                                                                                                   
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--

-- IEEE VHDL standard library:
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity gbt_pattern_generator is  
   port (   

      --===============--
      -- Reset & Clock --
      --===============-- 
      
      -- Reset:
      ---------   
      
      RESET_I                                   : in  std_logic; 
      
      -- Clock:                                 
      ---------                           
      
      TX_FRAMECLK_I                             : in  std_logic; 
   
      --========--                              
      -- Inputs --                              
      --========--
      
      -- Encoding:
      ------------
      
      TX_ENCODING_SEL_I                         : in  std_logic_vector( 1 downto 0);
      
      -- Test pattern:
      ----------------
      
      TEST_PATTERN_SEL_I                        : in  std_logic_vector( 1 downto 0);
      ------------------------------------------
      STATIC_PATTERN_DATA_I                     : in  std_logic_vector(83 downto 0);
      STATIC_PATTERN_EXTRADATA_WIDEBUS_I        : in  std_logic_vector(31 downto 0);
      STATIC_PATTERN_EXTRADATA_GBT8B10B_I       : in  std_logic_vector( 3 downto 0);

      --=========--                             
      -- Outputs --                             
      --=========--                       
      
      -- Common data:           
      ---------------           
      
      TX_DATA_O                                 : out std_logic_vector(83 downto 0);      
       
      -- Wide-Bus extra data:          
      ----------------------- 
      
      TX_EXTRA_DATA_WIDEBUS_O                   : out std_logic_vector(31 downto 0);
      
      -- GBT-8b10b extra data:          
      ------------------------       
      
      TX_EXTRA_DATA_GBT8B10B_O                  : out std_logic_vector(3 downto 0)
      
   );
end gbt_pattern_generator;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture behavioral of gbt_pattern_generator is

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--

   --==================================== User Logic =====================================--
   
   main: process(RESET_I, TX_FRAMECLK_I) 
      constant COMMONWORDCOUNTER_OVERFLOW       : integer := 2**21;
      constant WIDEBUSWORDCOUNTER_OVERFLOW      : integer := 2**16;
      constant GBT8B10BWORDCOUNTER_OVERFLOW     : integer := 2**4;
      variable commonWordCounter                : unsigned(20 downto 0);
      variable widebusWordCounter               : unsigned(15 downto 0);
      variable gbt8b10bWordCounter              : unsigned( 3 downto 0);
   begin                                      
      if RESET_I = '1' then                     
         commonWordCounter                      := (others => '0');      
         widebusWordCounter                     := (others => '0');      
         gbt8b10bWordCounter                    := (others => '0');      
         TX_DATA_O                              <= (others => '0');
         TX_EXTRA_DATA_WIDEBUS_O                <= (others => '0');
         TX_EXTRA_DATA_GBT8B10B_O               <= (others => '0');      
      elsif rising_edge(TX_FRAMECLK_I) then 
      
         case TEST_PATTERN_SEL_I is 
            when "01" => 
             
               --=========--
               -- Counter --
               --=========--
            
               -- Common counter pattern generation:
               -------------------------------------
               for i in 0 to 3 loop
                  TX_DATA_O((21*i)+20 downto (21*i)) <= std_logic_vector(commonWordCounter);   
               end loop;              
               if commonWordCounter = COMMONWORDCOUNTER_OVERFLOW-1 then 
                  commonWordCounter             := (others => '0');
               else                             
                  commonWordCounter             := commonWordCounter + 1;
               end if;                              
               -- Wide-Bus extra data counter pattern error generation:
               --------------------------------------------------------
               if TX_ENCODING_SEL_I = "01" then
                  for i in 0 to 1 loop
                     TX_EXTRA_DATA_WIDEBUS_O((16*i)+15 downto (16*i)) <= std_logic_vector(widebusWordCounter);   
                  end loop;              
                  if widebusWordCounter = WIDEBUSWORDCOUNTER_OVERFLOW-1 then 
                     widebusWordCounter         := (others => '0');
                  else                          
                     widebusWordCounter         := widebusWordCounter + 1;
                  end if; 
               else
                  TX_EXTRA_DATA_WIDEBUS_O       <= (others => '0');
               end if;
               -- GBT-8b10b extra data counter pattern error generation:
               ---------------------------------------------------------
               if TX_ENCODING_SEL_I = "10" then
                  TX_EXTRA_DATA_GBT8B10B_O      <= std_logic_vector(gbt8b10bWordCounter);   
                  if gbt8b10bWordCounter = GBT8B10BWORDCOUNTER_OVERFLOW-1 then 
                     gbt8b10bWordCounter        := (others => '0');
                  else                          
                     gbt8b10bWordCounter        := gbt8b10bWordCounter + 1;
                  end if; 
               else
                  TX_EXTRA_DATA_GBT8B10B_O      <= (others => '0');
               end if;
               
            when "10" =>
            
               --========--
               -- Static --
               --========--
               
               -- Common static pattern generation:
               ------------------------------------               
               TX_DATA_O                        <= STATIC_PATTERN_DATA_I;               
               -- Wide-Bus extra data counter pattern error generation:
               --------------------------------------------------------               
               if TX_ENCODING_SEL_I = "01" then
                  TX_EXTRA_DATA_WIDEBUS_O       <= STATIC_PATTERN_EXTRADATA_WIDEBUS_I;  
                else
                  TX_EXTRA_DATA_WIDEBUS_O       <= (others => '0');
               end if;
               -- GBT-8b10b extra data counter pattern error generation:
               ---------------------------------------------------------                      
               if TX_ENCODING_SEL_I = "10" then
                  TX_EXTRA_DATA_GBT8B10B_O      <= STATIC_PATTERN_EXTRADATA_GBT8B10B_I;
               else
                  TX_EXTRA_DATA_GBT8B10B_O      <= (others => '0');
               end if;
            
            when others => 

               --==========--
               -- Disabled --
               --==========--
               
               TX_DATA_O                        <= (others => '0');               
               TX_EXTRA_DATA_WIDEBUS_O          <= (others => '0'); 
               TX_EXTRA_DATA_GBT8B10B_O         <= (others => '0');
               
         end case;
         
      end if;
   end process;

--=====================================================================================--
end behavioral;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--