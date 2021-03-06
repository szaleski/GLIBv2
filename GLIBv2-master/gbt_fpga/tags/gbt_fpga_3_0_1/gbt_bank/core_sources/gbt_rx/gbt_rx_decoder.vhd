--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           GBT RX decoder
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
-- Versions history:      DATE         VERSION   AUTHOR                               DESCRIPTION
--    
--                        10/05/2009   0.1       S. Muschter (Stockholm University)   First .bdf entity definition.           
--                                                                   
--                        08/07/2009   0.2       S. Baron (CERN)                      Translate from .bdf to .vhd.
--
--                        04/07/2013   3.0       M. Barros Marin                      - Cosmetic and minor modifications.   
--                                                                                    - Add Wide-Bus encoding. 
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
use work.gbt_bank_package.all;
use work.vendor_specific_gbt_bank_package.all;
use work.gbt_banks_user_setup.all;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--

entity gbt_rx_decoder is
   generic (   
      GBT_BANK_ID                               : integer := 1   
   );
   port (
      
      --===============--
      -- Reset & Clock --
      --===============--    
      
      -- Reset:
      ---------
      
      RX_RESET_I                                : in  std_logic;
      
      -- Clock:
      ---------
      
      RX_FRAMECLK_I                             : in  std_logic;
      
      --=========--
      -- Control --
      --=========--
      
      -- Ready flags:
      ---------------
      
      RX_GEARBOX_READY_I                        : in  std_logic;
      READY_O                                   : out std_logic;
      
      -- RX is data flag:
      -------------------
      
      RX_ISDATA_FLAG_ENABLE_I                   : in  std_logic;  
      RX_ISDATA_FLAG_O                          : out std_logic;  
      
      --=======--
      -- Frame --
      --=======--
      
      -- Frame:
      ---------      
      
      RX_FRAME_I                                : in  std_logic_vector(119 downto 0);      
      
      -- Common:
      ----------
      
      RX_COMMON_FRAME_O                         : out std_logic_vector( 83 downto 0);
      
      -- Wide-Bus:
      ------------
      
      RX_EXTRA_FRAME_WIDEBUS_O                  : out std_logic_vector( 31 downto 0);
      
      -- GBT-8b10b:
      -------------
      
      RX_EXTRA_FRAME_GBT8B10B_O                 : out std_logic_vector(  3 downto 0)
      
   );
end gbt_rx_decoder;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of gbt_rx_decoder is 

   --================================ Signal Declarations ================================--

   signal rxFrame_from_deinterleaver            : std_logic_vector(119 downto 0);
   signal rxCommonFrame_from_reedSolomonDecoder : std_logic_vector( 87 downto 0); 

   --=====================================================================================--

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--  

   --==================================== User Logic =====================================--   

   --==============--
   -- Frame header --
   --==============--
   
   RX_ISDATA_FLAG_O  <= '1' when (RX_FRAME_I(119 downto 116) = DATA_HEADER_PATTERN) and (RX_ISDATA_FLAG_ENABLE_I = '1') else '0'; 
   
   --=========--
   -- Decoder --
   --=========--   
   
   -- GBT-Frame:
   -------------  
   
   gbtFrame_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).TX_ENCODING = GBT_FRAME generate
   
      deinterleaver: entity work.gbt_rx_decoder_gbtframe_deintlver
         port map (        
            RX_FRAME_I                          => RX_FRAME_I,
            RX_FRAME_O                          => rxFrame_from_deinterleaver
         );   
      
      reedSolomonDecoder60to119: entity work.gbt_rx_decoder_gbtframe_rsdec
         port map (
            RX_COMMON_FRAME_ENCODED_I           => rxFrame_from_deinterleaver(119 downto 60),
            RX_COMMON_FRAME_O                   => rxCommonFrame_from_reedSolomonDecoder(87 downto 44),
            ERROR_DETECT_O                      => open   -- Comment: Port added for debugging.
         );   

      reedSolomonDecoder0to50: entity work.gbt_rx_decoder_gbtframe_rsdec
         port map(
            RX_COMMON_FRAME_ENCODED_I           => rxFrame_from_deinterleaver(59 downto 0),
            RX_COMMON_FRAME_O                   => rxCommonFrame_from_reedSolomonDecoder(43 downto 0),
            ERROR_DETECT_O                      => open   -- Comment: Port added for debugging.
         );    
      
      RX_COMMON_FRAME_O                         <= rxCommonFrame_from_reedSolomonDecoder(83 downto 0);
      
   end generate;
   
   -- Wide-Bus: 
   ------------
   
   wideBus_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).TX_ENCODING = WIDE_BUS generate
      
      RX_COMMON_FRAME_O                         <= RX_FRAME_I(115 downto 32);
      RX_EXTRA_FRAME_WIDEBUS_O                  <= RX_FRAME_I( 31 downto  0);                                

   end generate;

   widebus_no_gen: if GBT_BANKS_USER_SETUP(GBT_BANK_ID).TX_ENCODING /= WIDE_BUS generate
   
      RX_EXTRA_FRAME_WIDEBUS_O                  <= (others => '0');
   
   end generate;
   
   -- GBT-8b10b:
   -------------
   
   -- Comment: Not implemented yet
   
   RX_EXTRA_FRAME_GBT8B10B_O                    <= (others => '0');
   
   --============--
   -- Ready flag --
   --============--
   
   READY_O                                      <= RX_GEARBOX_READY_I;             

   --=====================================================================================--
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--