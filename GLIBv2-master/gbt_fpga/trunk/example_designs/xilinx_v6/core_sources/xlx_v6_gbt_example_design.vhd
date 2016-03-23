--=================================================================================================--
--##################################   Module Information   #######################################--
--=================================================================================================--
--                                                                                         
-- Company:               CERN (PH-ESE-BE)                                                         
-- Engineer:              Manoel Barros Marin (manoel.barros.marin@cern.ch) (m.barros.marin@ieee.org)
--                                                                                                 
-- Project Name:          GBT-FPGA                                                                
-- Module Name:           Xilinx Virtex 6 - GBT Bank example design                                         
--                                                                                                 
-- Language:              VHDL'93                                                                  
--                                                                                                   
-- Target Device:         Xilinx Virtex 6                                                         
-- Tool version:          ISE 14.5                                                               
--                                                                                                   
-- Version:               3.0                                                                      
--
-- Description:            
--
-- Versions history:      DATE         VERSION   AUTHOR            DESCRIPTION
--
--                        15/11/2013   3.0       M. Barros Marin   First .vhd module definition           
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

entity xlx_v6_gbt_example_design is   
   generic (
      GBTBANK_RESET_CLK_FREQ                    : integer := 125e6 
   );
   port (   
      
      --===============--
      -- Resets scheme --
      --===============-- 
      
      -- General reset:
      -----------------
      
      GENERAL_RESET_I                           : in  std_logic;
      
      -- Manual resets:
      -----------------
      
      MANUAL_RESET_TX_I                         : in  std_logic;
      MANUAL_RESET_RX_I                         : in  std_logic;
      
      --===============--
      -- Clocks scheme --
      --===============--
      
      -- Fabric clock:      
      ----------------
      
      FABRIC_CLK_I                              : in  std_logic; 
      
      -- MGT (GTX) reference clock:
      -----------------------------
      
      MGT_REFCLK_I                              : in  std_logic;
      
      --==============--
      -- Serial lanes --
      --==============--
      
      MGT_TX_P                                  : out std_logic; 
      MGT_TX_N                                  : out std_logic; 
      MGT_RX_P                                  : in  std_logic;
      MGT_RX_N                                  : in  std_logic;      
      
      --=================--
      -- General control --
      --=================--
      
      LOOPBACK_I                                : in  std_logic_vector(2 downto 0);  
      TX_ISDATA_SEL_I                           : in  std_logic;
      ------------------------------------------    
      LATOPT_GBTBANK_TX_O                       : out std_logic;
      LATOPT_GBTBANK_RX_O                       : out std_logic;
      TX_FRAMECLK_PLL_LOCKED_O                  : out std_logic;
      MGT_READY_O                               : out std_logic;
      RX_BITSLIP_NUMBER_O                       : out std_logic_vector(GBTRX_BITSLIP_NBR_MSB downto 0);      
      RX_WORDCLK_READY_O                        : out std_logic;
      RX_FRAMECLK_READY_O                       : out std_logic;
      GBT_RX_READY_O                            : out std_logic;        
      RX_ISDATA_FLAG_O                          : out std_logic;
      
      --===============--
      -- GBT Bank data -- 
      --===============--
      
      -- TX data:
      -----------
      
      TX_DATA_O                                 : out std_logic_vector(83 downto 0); 
      TX_EXTRA_DATA_WIDEBUS_O                   : out std_logic_vector(31 downto 0);     
      TX_EXTRA_DATA_GBT8B10B_O                  : out std_logic_vector( 3 downto 0);
      
      -- RX data:
      -----------
      
      RX_DATA_O                                 : out std_logic_vector(83 downto 0);
      RX_EXTRA_DATA_WIDEBUS_O                   : out std_logic_vector(31 downto 0);
      RX_EXTRA_DATA_GBT8B10B_O                  : out std_logic_vector( 3 downto 0);
      
      --==============--
      -- Test control --
      --==============--
      
      -- Test pattern:
      ----------------
      
      TEST_PATTERN_SEL_I                        : in  std_logic_vector(1 downto 0);      
      
      -- Error detection:
      -------------------      
      
      RESET_DATA_ERRORSEEN_FLAG_I               : in  std_logic;
      RESET_GBTRXREADY_LOST_FLAG_I              : in  std_logic;
      ------------------------------------------
      GBTRXREADY_LOST_FLAG_O                    : out std_logic;
      RXDATA_ERRORSEEN_FLAG_O                   : out std_logic;
      RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O      : out std_logic;
      RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O     : out std_logic;
      
      --=====================--
      -- Latency measurement --
      --=====================--
      
      -- Clocks forwarding:
      ---------------------
      
      TX_FRAMECLK_O                             : out std_logic;
      TX_WORDCLK_O                              : out std_logic;
      RX_FRAMECLK_O                             : out std_logic;
      RX_WORDCLK_O                              : out std_logic;
      
      -- Pattern match flags:
      -----------------------
      
      TX_MATCHFLAG_O                            : out std_logic;
      RX_MATCHFLAG_O                            : out std_logic       
     
   );
end xlx_v6_gbt_example_design;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture structural of xlx_v6_gbt_example_design is  
   
   --================================ Signal Declarations ================================--   
  
   --=========--
   -- TX data --
   --=========--   
   
   signal txEncodingSel                         : std_logic_vector( 1 downto 0);
   signal txData_from_pattGen                   : std_logic_vector(83 downto 0);   
   signal txExtraDataWidebus_from_pattGen       : std_logic_vector(31 downto 0);   
   signal txExtraDataGbt8b10b_from_pattGen      : std_logic_vector( 3 downto 0);
   
   --========================--  
   -- GBT Bank resets scheme --
   --========================--
   
   signal mgtTxReset_from_gbtBankRst            : std_logic; 
   signal mgtRxReset_from_gbtBankRst            : std_logic; 
   signal gbtTxReset_from_gbtBankRst            : std_logic;
   signal gbtRxReset_from_gbtBankRst            : std_logic; 
   
   --========================--
   -- GBT Bank clocks scheme -- 
   --========================--   
   
   signal txPllRefClk_from_mgtRefClkBufg        : std_logic;
   ---------------------------------------------
   signal txFrameClk_from_txPll                 : std_logic;   
   ---------------------------------------------  
   signal pllLocked_from_rxFrmClkPhAlgnr        : std_logic; 
   signal phaseAlignDone_from_rxFrmClkPhAlgnr   : std_logic;   
   signal rxFrameClkReady_from_orGate           : std_logic; 
   signal rxFrameClk_from_rxFrmClkPhAlgnr       : std_logic;

   --==========--
   -- GBT Bank --
   --==========--   
   
   -- Comment: Note!! Only ONE GBT Bank with ONE link can be used in this example design.
   
   -- GBT Bank 1:
   --------------
   
   signal to_gbtBank_1_clks                     : gbtBankClks_i_R;                          
   signal from_gbtBank_1_clks                   : gbtBankClks_o_R;
   ---------------------------------------------        
   signal to_gbtBank_1_gbtTx                    : gbtTx_i_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   signal from_gbtBank_1_gbtTx                  : gbtTx_o_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   ---------------------------------------------        
   signal to_gbtBank_1_mgt                      : mgt_i_R;
   signal from_gbtBank_1_mgt                    : mgt_o_R; 
   ---------------------------------------------        
   signal to_gbtBank_1_gbtRx                    : gbtRx_i_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS); 
   signal from_gbtBank_1_gbtRx                  : gbtRx_o_R_A(1 to GBT_BANKS_USER_SETUP(1).NUM_LINKS);
   
   -- GBT Bank 2:
   --------------
   
-- signal to_gbtBank_2_clks                     : gbtBankClks_i_R;                          
-- signal from_gbtBank_2_clks                   : gbtBankClks_o_R;
-- ---------------------------------------------        
-- signal to_gbtBank_2_gbtTx                    : gbtTx_i_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
-- signal from_gbtBank_2_gbtTx                  : gbtTx_o_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
-- ---------------------------------------------        
-- signal to_gbtBank_2_mgt                      : mgt_i_R;
-- signal from_gbtBank_2_mgt                    : mgt_o_R; 
-- ---------------------------------------------         
-- signal to_gbtBank_2_gbtRx                    : gbtRx_i_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS); 
-- signal from_gbtBank_2_gbtRx                  : gbtRx_o_R_A(1 to GBT_BANKS_USER_SETUP(2).NUM_LINKS);
   
   --=========--
   -- RX data --
   --=========--   
   
   signal rxEncodingSel                         : std_logic_vector( 1 downto 0);
   
   --=====================================================================================--   

--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
   --==================================== User Logic =====================================--
  
   --=========--
   -- TX data --
   --=========--
   
   -- TX encoding selector:
   ------------------------
   
   txEncodingSel                                <= "10" when GBT_BANKS_USER_SETUP(1).TX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
                                                   "01" when GBT_BANKS_USER_SETUP(1).TX_ENCODING = WIDE_BUS  else 
                                                   "00";                                                            -- Comment: GBT_FRAME

   -- Pattern generator:
   ---------------------
   
   -- Comment: * The pattern generated can be either:
   --
   --            - Counter pattern
   --            - Static pattern          
   --
   --          * The TEST_PATTERN_SEL_I port of both, the pattern generator as well as the error detector are controlled by the same port.
   
   pattGen: entity work.gbt_pattern_generator
      port map (                                                        
         RESET_I                                => gbtTxReset_from_gbtBankRst,   
         TX_FRAMECLK_I                          => txFrameClk_from_txPll,
         ---------------------------------------     
         TX_ENCODING_SEL_I                      => txEncodingSel,
         TEST_PATTERN_SEL_I                     => TEST_PATTERN_SEL_I,
         STATIC_PATTERN_DATA_I                  => x"0000BABEAC1DACDCFFFFF",
         STATIC_PATTERN_EXTRADATA_WIDEBUS_I     => x"BEEFCAFE",
         STATIC_PATTERN_EXTRADATA_GBT8B10B_I    => x"C",
         ---------------------------------------
         TX_DATA_O                              => txData_from_pattGen,
         TX_EXTRA_DATA_WIDEBUS_O                => txExtraDataWidebus_from_pattGen,
         TX_EXTRA_DATA_GBT8B10B_O               => txExtraDataGbt8b10b_from_pattGen
      );       
         
   TX_DATA_O                                    <= txData_from_pattGen;
   TX_EXTRA_DATA_WIDEBUS_O                      <= txExtraDataWidebus_from_pattGen;
   TX_EXTRA_DATA_GBT8B10B_O                     <= txExtraDataGbt8b10b_from_pattGen;   -- Comment: Note!! Not implemented yet.
   
   --========================--
   -- GBT Bank resets scheme --
   --========================--
   
   -- Comment:  * GENERAL_RESET_I is used to reset the GBT Bank sequentially.
   --             (see the timing diagram as a comment in the "gbtBankRst" module)  
   --
   --           * Manual reset is used to reset the TX and the RX independently:
   --
   --             - MANUAL_RESET_TX_I resets GBT_TX and MGT_TX.
   --
   --             - MANUAL_RESET_RX_I resets GBT_RX and MGT_RX.
   
   gbtBankRst: entity work.gbt_bank_reset    
      generic map (
         RX_INIT_FIRST                          => false,
         INITIAL_DELAY                          => 1 * GBTBANK_RESET_CLK_FREQ,  -- Comment: * 1s  
         TIME_N                                 => 1 * GBTBANK_RESET_CLK_FREQ,  --          * 1s
         GAP_DELAY                              => 1 * GBTBANK_RESET_CLK_FREQ)  --          * 1s
      port map (     
         CLK_I                                  => FABRIC_CLK_I,                                               
         ---------------------------------------
         GENERAL_RESET_I                        => GENERAL_RESET_I,                                                                 
         MANUAL_RESET_TX_I                      => MANUAL_RESET_TX_I,
         MANUAL_RESET_RX_I                      => MANUAL_RESET_RX_I,
         ---------------------------------------     
         MGT_TX_RESET_O                         => mgtTxReset_from_gbtBankRst,                              
         MGT_RX_RESET_O                         => mgtRxReset_from_gbtBankRst,                             
         GBT_TX_RESET_O                         => gbtTxReset_from_gbtBankRst,                                      
         GBT_RX_RESET_O                         => gbtRxReset_from_gbtBankRst,                              
         ---------------------------------------      
         BUSY_O                                 => open,                                                                         
         DONE_O                                 => open                                                                          
      );          
   
   to_gbtBank_1_gbtTx(1).reset                  <= gbtTxReset_from_gbtBankRst;   
   to_gbtBank_1_mgt.mgtLink(1).tx_reset         <= mgtTxReset_from_gbtBankRst;
   to_gbtBank_1_mgt.mgtLink(1).rx_reset         <= mgtRxReset_from_gbtBankRst;   
   to_gbtBank_1_gbtRx(1).reset                  <= gbtRxReset_from_gbtBankRst;  
   
   --========================--
   -- GBT Bank clocks scheme --
   --========================-- 
   
   -- MGT reference clock: 
   -----------------------
   
   to_gbtBank_1_clks.mgt_clks.tx_refClk         <= MGT_REFCLK_I;
   to_gbtBank_1_clks.mgt_clks.rx_refClk         <= MGT_REFCLK_I;
   
   -- Comment: In this example design, MGT_REFCLK_I is used to generate the reference clock of the txPll. If available, the TTC CLK could be used instead.    
   
   mgtRefClkBufg: bufg
      port map (
         O                                      => txPllRefClk_from_mgtRefClkBufg, 
         I                                      => MGT_REFCLK_I
      );    

   -- TX_WORDCLK & TX_FRAMECLK:
   ----------------------------
    
   -- Comment: Note!! In order to save clocking resources, it is strongly recommended that all GBT Links of the GBT Bank share the same "tx_wordClk_noBuff"
   --                 (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "tx_wordClk_noBuff(1)", using just one clock buffer to feed
   --                  "tx_wordClk(1)", "tx_wordClk(2)" and "tx_wordClk(3)").

   txWordClkBufg: bufg
      port map (
         O                                      => to_gbtBank_1_clks.mgt_clks.tx_wordClk(1), 
         I                                      => from_gbtBank_1_clks.mgt_clks.tx_wordClk_noBuff(1)
      ); 
   
   -- Comment: * The txPll (MMCM) does not have input buffer.
   --      
   --          * TX_FRAMECLK frequency: 40MHz
   --
   --          * Note!! The 40MHz output of the "txPll" is shifter 90deg in order to sample correctly when using the latency-optimized GBT Bank.
   
   txPll: entity work.xlx_v6_tx_mmcm
      port map (
         -- Clock in ports:
         CLK_IN1                                => txPllRefClk_from_mgtRefClkBufg,
         -- Clock out ports:               
         CLK_OUT1                               => txFrameClk_from_txPll,   -- Comment: Note!! 40MHz output shifted 90deg.
         -- Status and control signals:
         RESET                                  => '0',
         LOCKED                                 => TX_FRAMECLK_PLL_LOCKED_O
      );    
   
   -- Comment: Note!! In order to save clocking resources, it is strongly recommended that all GBT Links of the GBT Bank share the same "tx_frameClk"
   --                 (e.g. In a GBT Bank composed by GBT Links 1,2 and 3, all GBT Links use "txFrameClk_from_txPll" to feed "to_gbtBank_1_clks.tx_frameClk(1)",
   --                   "to_gbtBank_1_clks.tx_frameClk(2)" and  "to_gbtBank_1_clks.tx_frameClk(3)").   
   
   to_gbtBank_1_clks.tx_frameClk(1)             <= txFrameClk_from_txPll;     
   
   -- RX_WORDCLK & RX_FRAMECLK:
   ----------------------------
   
   -- Comment: * Due to the Clock & Data Recovery (CDR), the "rx_wordClk" of each GBT Link of the GBT Bank should clocked by its own  
   --            "rx_wordClk_noBuff" using a dedicated clock buffer. 
   --
   --          * Each latency-optimized GBT Link of the GBT Bank should have its own RX_FRAMECLK aligner. 
   
   rxWordClkBufg: bufg
      port map (
         O                                      => to_gbtBank_1_clks.mgt_clks.rx_wordClk(1), 
         I                                      => from_gbtBank_1_clks.mgt_clks.rx_wordClk_noBuff(1) 
      );   
     
   -- Comment: * This phase aligner uses the header of the incoming data stream as a reference point for matching the rising
   --            edge of the recovered RX_FRAMECLK with the rising edge of the TX_FRAMECLK of the TRANSMITTER BOARD.
   -- 
   --          * Note!! The phase alignment is only triggered when LATENCY-OPTIMIZED GBT Bank RX is used.  
   --
   --          * The PLL (MMCM) does not have input buffer.
   --      
   --          * RX_FRAMECLK frequency: 40MHz   
   
   rxFrmClkPhAlgnr: entity work.gbt_rx_frameclk_phalgnr
      port map (
         RESET_I                                => gbtRxReset_from_gbtBankRst,
         ---------------------------------------      
         RX_WORDCLK_I                           => to_gbtBank_1_clks.mgt_clks.rx_wordClk(1),
         RX_FRAMECLK_O                          => rxFrameClk_from_rxFrmClkPhAlgnr,         
         ---------------------------------------      
         SYNC_ENABLE_I                          => from_gbtBank_1_gbtRx(1).latOptGbtBank_rx and from_gbtBank_1_mgt.mgtLink(1).rxWordClkReady,
         SYNC_I                                 => from_gbtBank_1_gbtRx(1).header_flag,
         ---------------------------------------
         PLL_LOCKED_O                           => pllLocked_from_rxFrmClkPhAlgnr,
         DONE_O                                 => phaseAlignDone_from_rxFrmClkPhAlgnr
      );                      
   
   rxFrameClkReady_from_orGate                  <=    (pllLocked_from_rxFrmClkPhAlgnr      and (not from_gbtBank_1_gbtRx(1).latOptGbtBank_rx))
                                                   or (phaseAlignDone_from_rxFrmClkPhAlgnr and      from_gbtBank_1_gbtRx(1).latOptGbtBank_rx);
                                                   
   to_gbtBank_1_gbtRx(1).rxFrameClkReady        <= rxFrameClkReady_from_orGate;
   RX_FRAMECLK_READY_O                          <= rxFrameClkReady_from_orGate;     
   
   to_gbtBank_1_clks.rx_frameClk(1)             <= rxFrameClk_from_rxFrmClkPhAlgnr;
   
   --==========--
   -- GBT Bank --
   --==========--
   
   -- Comment: Note!! Only ONE GBT Bank with ONE link can be used in this example design.    
   
   gbtBank_1: entity work.gbt_bank
      generic map (
         GBT_BANK_ID                            => 1)
      port map (                       
         CLKS_I                                 => to_gbtBank_1_clks,                                  
         CLKS_O                                 => from_gbtBank_1_clks,               
         ---------------------------------------          
         GBT_TX_I                               => to_gbtBank_1_gbtTx,             
         GBT_TX_O                               => from_gbtBank_1_gbtTx,         
         ---------------------------------------          
         MGT_I                                  => to_gbtBank_1_mgt,              
         MGT_O                                  => from_gbtBank_1_mgt,              
         ---------------------------------------          
         GBT_RX_I                               => to_gbtBank_1_gbtRx,              
         GBT_RX_O                               => from_gbtBank_1_gbtRx         
      ); 

-- gbtBank_2: entity work.gbt_bank
--    generic map (
--      GBT_BANK_ID                             => 2)
--    port map (                       
--       CLKS_I                                 => to_gbtBank_2_clks,                                  
--       CLKS_O                                 => from_gbtBank_2_clks,               
--       ---------------------------------------          
--       GBT_TX_I                               => to_gbtBank_2_gbtTx,             
--       GBT_TX_O                               => from_gbtBank_2_gbtTx,         
--       ---------------------------------------          
--       MGT_I                                  => to_gbtBank_2_mgt,              
--       MGT_O                                  => from_gbtBank_2_mgt,              
--       ---------------------------------------          
--       GBT_RX_I                               => to_gbtBank_2_gbtRx,              
--       GBT_RX_O                               => from_gbtBank_2_gbtRx         
--    ); 

   -- Serial lanes assignments:
   ----------------------------
   
   to_gbtBank_1_mgt.mgtLink(1).rx_p             <= MGT_RX_P;   
   to_gbtBank_1_mgt.mgtLink(1).rx_n             <= MGT_RX_N;
   MGT_TX_P                                     <= from_gbtBank_1_mgt.mgtLink(1).tx_p;
   MGT_TX_N                                     <= from_gbtBank_1_mgt.mgtLink(1).tx_n;     
   
   -- Data assignments:
   --------------------   

   to_gbtBank_1_gbtTx(1).isDataSel              <= TX_ISDATA_SEL_I;  
   RX_ISDATA_FLAG_O                             <= from_gbtBank_1_gbtRx(1).isDataFlag;
   
   to_gbtBank_1_gbtTx(1).data                   <= txData_from_pattGen;   
   to_gbtBank_1_gbtTx(1).extraData_wideBus      <= txExtraDataWidebus_from_pattGen;
   to_gbtBank_1_gbtTx(1).extraData_gbt8b10b     <= txExtraDataGbt8b10b_from_pattGen;   -- Comment: Note!! Not implemented yet.
   
   -- Control assignments:
   -----------------------  
   
   to_gbtBank_1_mgt.mgtLink(1).loopBack         <= LOOPBACK_I;

   to_gbtBank_1_mgt.mgtLink(1).tx_syncReset     <= '0';
   to_gbtBank_1_mgt.mgtLink(1).rx_syncReset     <= '0';

   -- Comment: The built-in PRBS generator/checker of the MGT(GTX) transceiver is not used in this example design.

   to_gbtBank_1_mgt.mgtLink(1).prbs_txEn        <= "000";
   to_gbtBank_1_mgt.mgtLink(1).prbs_rxEn        <= "000";
   to_gbtBank_1_mgt.mgtLink(1).prbs_forcErr     <= '0';
   to_gbtBank_1_mgt.mgtLink(1).prbs_errCntRst   <= '0';             
   
   to_gbtBank_1_mgt.mgtLink(1).conf_diff        <= "1000";    -- Comment: 810 mVppd
   to_gbtBank_1_mgt.mgtLink(1).conf_pstEmph     <= "00000";   -- Comment: 0.18 dB (default)
   to_gbtBank_1_mgt.mgtLink(1).conf_preEmph     <= "0000";    -- Comment: 0.15 dB (default)
   to_gbtBank_1_mgt.mgtLink(1).conf_eqMix       <= "000";     -- Comment: 12 dB (default)
   to_gbtBank_1_mgt.mgtLink(1).conf_txPol       <= '0';       -- Comment: Not inverted
   to_gbtBank_1_mgt.mgtLink(1).conf_rxPol       <= '0';       -- Comment: Not inverted 
   
   -- Comment: The DRP port of the MGT(GTX) transceiver is not used in this example design.   
   
   to_gbtBank_1_mgt.mgtLink(1).drp_dClk         <= '0';
   to_gbtBank_1_mgt.mgtLink(1).drp_dAddr        <= x"00";
   to_gbtBank_1_mgt.mgtLink(1).drp_dEn          <= '0';
   to_gbtBank_1_mgt.mgtLink(1).drp_dI           <= x"0000";
   to_gbtBank_1_mgt.mgtLink(1).drp_dWe          <= '0';

   -- Comment: * The manual RX_WORDCLK phase alignment control of the MGT(GTX) transceivers is not used in this 
   --            reference design (auto RX_WORDCLK phase alignment is used instead).
   --
   --          * Note!! The manual RX_WORDCLK phase alignment control of the MGT(GTX) MUST be synchronous with RX_WORDCLK
   --                   (in this example design this clock is "to_gbtBank_1_clks.mgt_clks.rx_wordClk(1)").
   
   to_gbtBank_1_mgt.mgtLink(1).rxSlide_enable   <= '1'; 
   to_gbtBank_1_mgt.mgtLink(1).rxSlide_ctrl     <= '0'; 
   to_gbtBank_1_mgt.mgtLink(1).rxSlide_nbr      <= "00000";
   to_gbtBank_1_mgt.mgtLink(1).rxSlide_run      <= '0';
   
   LATOPT_GBTBANK_TX_O                          <= from_gbtBank_1_gbtTx(1).latOptGbtBank_tx;
   LATOPT_GBTBANK_RX_O                          <= from_gbtBank_1_gbtRx(1).latOptGbtBank_rx;
   MGT_READY_O                                  <= from_gbtBank_1_mgt.mgtLink(1).ready;
   RX_WORDCLK_READY_O                           <= from_gbtBank_1_mgt.mgtLink(1).rxWordClkReady;   
   RX_BITSLIP_NUMBER_O                          <= from_gbtBank_1_gbtRx(1).bitSlipNbr;
   GBT_RX_READY_O                               <= from_gbtBank_1_gbtRx(1).ready;
   
  --=========--
   -- RX data --
   --=========--  
   
   -- RX encoding selector:
   ------------------------
   
   rxEncodingSel                                <= "10" when GBT_BANKS_USER_SETUP(1).RX_ENCODING = GBT_8B10B else   -- Comment: Note!! Not implemented yet.
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
   --          * The TEST_PATTERN_SEL_I port of both, the pattern generator as well as the error detector are controlled by the same port.

   pattCheck: entity work.gbt_pattern_checker
      port map (
         RESET_I                                => gbtRxReset_from_gbtBankRst,         
         RX_FRAMECLK_I                          => rxFrameClk_from_rxFrmClkPhAlgnr, 
         ---------------------------------------      
         RX_DATA_I                              => from_gbtBank_1_gbtRx(1).data,        
         RX_EXTRA_DATA_WIDEBUS_I                => from_gbtBank_1_gbtRx(1).extraData_widebus,
         RX_EXTRA_DATA_GBT8B10B_I               => from_gbtBank_1_gbtRx(1).extraData_gbt8b10b,
         ---------------------------------------      
         GBT_RX_READY_I                         => from_gbtBank_1_gbtRx(1).ready,
         RX_ENCODING_SEL_I                      => rxEncodingSel,
         TEST_PATTERN_SEL_I                     => TEST_PATTERN_SEL_I,        
         STATIC_PATTERN_DATA_I                  => x"0000BABEAC1DACDCFFFFF",        
         STATIC_PATTERN_EXTRADATA_WIDEBUS_I     => x"BEEFCAFE",  
         STATIC_PATTERN_EXTRADATA_GBT8B10B_I    => x"C",
         RESET_GBTRXREADY_LOST_FLAG_I           => RESET_GBTRXREADY_LOST_FLAG_I,               
         RESET_DATA_ERRORSEEN_FLAG_I            => RESET_DATA_ERRORSEEN_FLAG_I,  
         ---------------------------------------      
         GBTRXREADY_LOST_FLAG_O                 => GBTRXREADY_LOST_FLAG_O, 
         RXDATA_ERRORSEEN_FLAG_O                => RXDATA_ERRORSEEN_FLAG_O,
         RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O   => RXEXTRADATA_WIDEBUS_ERRORSEEN_FLAG_O,
         RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O  => RXEXTRADATA_GBT8B10B_ERRORSEEN_FLAG_O
      ); 
      
   RX_DATA_O                                    <= from_gbtBank_1_gbtRx(1).data;
   RX_EXTRA_DATA_WIDEBUS_O                      <= from_gbtBank_1_gbtRx(1).extraData_widebus;
   RX_EXTRA_DATA_GBT8B10B_O                     <= from_gbtBank_1_gbtRx(1).extraData_gbt8b10b;
   
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
   
   TX_FRAMECLK_O                                <= txFrameClk_from_txPll;
   TX_WORDCLK_O                                 <= to_gbtBank_1_clks.mgt_clks.tx_wordClk(1); 
   ---------------------------------------------
   RX_FRAMECLK_O                                <= rxFrameClk_from_rxFrmClkPhAlgnr; 
   RX_WORDCLK_O                                 <= to_gbtBank_1_clks.mgt_clks.rx_wordClk(1);    
   
   -- Pattern match flags:
   -----------------------      
   
   -- Comment: * The latency of the link can be measured using an oscilloscope by comparing 
   --            the TX FLAG with the RX FLAG.
   --
   --          * Note!! The COUNTER TEST PATTERN must be used for this test.
   
   txFlag: entity work.gbt_pattern_matchflag
      PORT MAP (
         RESET_I                                => gbtTxReset_from_gbtBankRst,
         CLK_I                                  => txFrameClk_from_txPll,
         DATA_I                                 => txData_from_pattGen,
         MATCHFLAG_O                            => TX_MATCHFLAG_O
      );
   
   rxFlag: entity work.gbt_pattern_matchflag
      PORT MAP (
         RESET_I                                => gbtRxReset_from_gbtBankRst,
         CLK_I                                  => rxFrameClk_from_rxFrmClkPhAlgnr,
         DATA_I                                 => from_gbtBank_1_gbtRx(1).data,
         MATCHFLAG_O                            => RX_MATCHFLAG_O
      );
      
   --=====================================================================================--   
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--