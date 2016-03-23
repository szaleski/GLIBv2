--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           GBT RX gearbox          
--                                                                                                 
-- Language:              VHDL'93                                                              
--                                                                                                   
-- Target Device:         Vendor agnostic                                                      
-- Tool version:                                                                             
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        01/10/2009   0.1       F. Marin (CPPM)   First .vhd module definition. 
--                                                                  
--                        12/11/2013   3.0       M. Barros Marin   Cosmetic and minor modifications. 
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

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity gbt_rx_gearbox_std_rdctrl is
   port (
      
      --================--
      -- Reset & Clocks --
      --================--    
      
      -- Reset:
      ---------
      
      RX_RESET_I                                : in std_logic;
      
      -- Clocks:
      ----------
      
      RX_WORDCLK_I                              : in std_logic;
      RX_FRAMECLK_I                             : in std_logic;
      
      --=========--
      -- Control --
      --=========--
      
      RX_HEADER_LOCKED_I                        : in std_logic;
      READ_ADDRESS_O                            : out std_logic_vector(2 downto 0);
      READY_O                                   : out std_logic
   
   ); 
end gbt_rx_gearbox_std_rdctrl;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture behavioral of gbt_rx_gearbox_std_rdctrl is

   --================================ Signal Declarations ================================--   

   signal ready_r                               : std_logic_vector(2 downto 0);
   signal readEnable                            : std_logic;
   
   --=====================================================================================--            
  
--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--  
  
   --==================================== User Logic =====================================-- 
  
   -- Comment: DPRAM read delay after header locked.
   
   readDly: process(RX_RESET_I, RX_WORDCLK_I)
      variable counter                          : integer range 0 to 10;
   begin    
      if RX_RESET_I = '1' then
         counter                                :=  0 ;
         readEnable                             <= '0';
      elsif rising_edge(RX_WORDCLK_I) then
         if RX_HEADER_LOCKED_I = '1' then
            if counter < 10 then          
               counter                          := counter + 1;
            else
               readEnable                       <= '1';
            end if;
         else
            counter                             :=  0 ;
            readEnable                          <= '0';
         end if;
      end if;
   end process;

   -- Comment: DPRAM read address generation.
   
   readAddr: process(RX_RESET_I, RX_FRAMECLK_I)
      variable resetReadAddress                 : std_logic;
      variable readAddress                      : unsigned(2 downto 0);
   begin    
      if RX_RESET_I = '1' then
         ready_r                                <= (others => '0');
         READY_O                                <= '0';
         resetReadAddress                       := '0';
         readAddress                            := (others => '0');
      elsif rising_edge(RX_FRAMECLK_I) then
         -- Comment: Registers to compensate the 2 cycles of delay of the DPRAM.
         READY_O                                <= ready_r(2);
         ready_r(2)                             <= ready_r(1);
         ready_r(1)                             <= ready_r(0);             
         if readEnable = '0' then
            resetReadAddress                    := '1';
         end if;
         if (readEnable = '1') and (resetReadAddress = '1') then
            ready_r(0)                          <= '1';
            readAddress                         := (others => '0');
            resetReadAddress                    := '0';
         elsif readEnable = '1' then
            if readAddress = "111" then 
               readAddress                      := (others => '0');
            else
               readAddress                      := readAddress + 1;
            end if;
         else                       
            ready_r(0)                          <= '0';
         end if;
         READ_ADDRESS_O                         <= std_logic_vector(readAddress);         
      end if;
   end process;
   
   --=====================================================================================--
end behavioral;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--