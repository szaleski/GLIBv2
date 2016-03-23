----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:22:31 02/24/2016 
-- Design Name: 
-- Module Name:    gbt - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

 
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

library work;
use work.ipbus.all;
use work.system_package.all;
use work.user_package.all;

entity gbt is

port(
   reset_i                     : in std_logic;
	ipb_clk_i                   : in std_logic;
	ipb_mosi_i                  : in ipb_wbus;
	ipb_miso_o                  : out ipb_rbus;
	led1_o							 : out std_logic;
	led2_o							 : out std_logic
	
);

end gbt;

architecture Behavioral of gbt is

type ipb_state_t is (IDLE, RSPD, RST);
    signal ipb_state                : ipb_state_t := IDLE;    
    signal ipb_reg_sel              : integer range 0 to 31; -- 16 regs for AMC evt builder and 16 regs for each chamber evt builder   
    signal ipb_read_reg_data        : std32_array_t(0 to 31); -- 16 regs for AMC evt builder and 16 regs for each chamber evt builder
    signal ipb_write_reg_data       : std32_array_t(0 to 31); -- 16 regs for AMC evt builder and 16 regs for each chamber evt builder
    signal led_state 					: std_logic_vector(31 downto 0);
begin

ipb_read_reg_data(0)<=led_state;
led_state <= ipb_write_reg_data(0);
led1_o <= led_state(0);
led2_o <= led_state(1);
   --================================--
    -- IPbus
    --================================--

    process(ipb_clk_i)       
    begin    
        if (rising_edge(ipb_clk_i)) then      
            if (reset_i = '1') then    
                ipb_miso_o <= (ipb_ack => '0', ipb_err => '0', ipb_rdata => (others => '0'));    
                ipb_state <= IDLE;
                ipb_reg_sel <= 0;
                
                ipb_write_reg_data <= (others => (others => '0'));
                ipb_write_reg_data(0) <= x"00000001"; -- enable the first input by default
                
                
            else         
                case ipb_state is
                    when IDLE =>                    
                        ipb_reg_sel <= to_integer(unsigned(ipb_mosi_i.ipb_addr(8 downto 0)));
                        if (ipb_mosi_i.ipb_strobe = '1') then
                            ipb_state <= RSPD;
                        end if;
                    when RSPD =>
                        ipb_miso_o <= (ipb_ack => '1', ipb_err => '0', ipb_rdata => ipb_read_reg_data(ipb_reg_sel));
                        if (ipb_mosi_i.ipb_write = '1') then
                            ipb_write_reg_data(ipb_reg_sel) <= ipb_mosi_i.ipb_wdata;
                        end if;
                        ipb_state <= RST;
                    when RST =>
                        ipb_miso_o.ipb_ack <= '0';
                        ipb_state <= IDLE;
                    when others => 
                        ipb_miso_o <= (ipb_ack => '0', ipb_err => '0', ipb_rdata => (others => '0'));    
                        ipb_state <= IDLE;
                        ipb_reg_sel <= 0;
                    end case;
            end if;        
        end if;        
    end process;
    




end Behavioral;

