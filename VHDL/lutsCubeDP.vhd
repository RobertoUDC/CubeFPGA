library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity lutsCubeDP is
  Port ( 
	a2cKey : in unsigned(6 downto 0);
	b3_x3Key, b3_3y2zKey : in unsigned(5 downto 0);

    a2c : out unsigned(7 downto 0);
    b3_x3 : out unsigned(14 downto 0);
    b3_3y2z : out unsigned(10 downto 0));
end lutsCubeDP;


architecture rlt of lutsCubeDP is

begin

with b3_3y2zKey select b3_3y2z <=
"00000000000" when "000000",
"00000000000" when "000001",
"00000000000" when "000010",
"00000000000" when "000011",
"00000000000" when "000100",
"00000000000" when "000101",
"00000000000" when "000110",
"00000000000" when "000111",
"00000000000" when "001000",
"00000000011" when "001001",
"00000000110" when "001010",
"00000001001" when "001011",
"00000001100" when "001100",
"00000001111" when "001101",
"00000010010" when "001110",
"00000010101" when "001111",
"00000000000" when "010000",
"00000001100" when "010001",
"00000011000" when "010010",
"00000100100" when "010011",
"00000110000" when "010100",
"00000111100" when "010101",
"00001001000" when "010110",
"00001010100" when "010111",
"00000000000" when "011000",
"00000011011" when "011001",
"00000110110" when "011010",
"00001010001" when "011011",
"00001101100" when "011100",
"00010000111" when "011101",
"00010100010" when "011110",
"00010111101" when "011111",
"00000000000" when "100000",
"00000110000" when "100001",
"00001100000" when "100010",
"00010010000" when "100011",
"00011000000" when "100100",
"00011110000" when "100101",
"00100100000" when "100110",
"00101010000" when "100111",
"00000000000" when "101000",
"00001001011" when "101001",
"00010010110" when "101010",
"00011100001" when "101011",
"00100101100" when "101100",
"00101110111" when "101101",
"00111000010" when "101110",
"01000001101" when "101111",
"00000000000" when "110000",
"00001101100" when "110001",
"00011011000" when "110010",
"00101000100" when "110011",
"00110110000" when "110100",
"01000011100" when "110101",
"01010001000" when "110110",
"01011110100" when "110111",
"00000000000" when "111000",
"00010010011" when "111001",
"00100100110" when "111010",
"00110111001" when "111011",
"01001001100" when "111100",
"01011011111" when "111101",
"01101110010" when "111110",
"10000000101" when others;



with b3_x3Key select b3_x3 <= 
"000000000000000" when "000000",
"000000000000000" when "000001",
"000000000000001" when "000010",
"000000000000011" when "000011",
"000000000001000" when "000100",
"000000000001111" when "000101",
"000000000011011" when "000110",
"000000000101010" when "000111",
"000000001000000" when "001000",
"000000001011011" when "001001",
"000000001111101" when "001010",
"000000010100110" when "001011",
"000000011011000" when "001100",
"000000100010010" when "001101",
"000000101010111" when "001110",
"000000110100101" when "001111",
"000001000000000" when "010000",
"000001001100110" when "010001",
"000001011011001" when "010010",
"000001101011001" when "010011",
"000001111101000" when "010100",
"000010010000101" when "010101",
"000010100110011" when "010110",
"000010111110000" when "010111",
"000011011000000" when "011000",
"000011110100001" when "011001",
"000100010010101" when "011010",
"000100110011100" when "011011",
"000101010111000" when "011100",
"000101111101000" when "011101",
"000110100101111" when "011110",
"000111010001011" when "011111",
"001000000000000" when "100000",
"001000110001100" when "100001",
"001001100110001" when "100010",
"001010011101111" when "100011",
"001011011001000" when "100100",
"001100010111011" when "100101",
"001101011001011" when "100110",
"001110011110110" when "100111",
"001111101000000" when "101000",
"010000110100111" when "101001",
"010010000101101" when "101010",
"010011011010010" when "101011",
"010100110011000" when "101100",
"010110001111110" when "101101",
"010111110000111" when "101110",
"011001010110001" when "101111",
"011011000000000" when "110000",
"011100101110010" when "110001",
"011110100001001" when "110010",
"100000011000101" when "110011",
"100010010101000" when "110100",
"100100010110001" when "110101",
"100110011100011" when "110110",
"101000100111100" when "110111",
"101010111000000" when "111000",
"101101001101101" when "111001",
"101111101000101" when "111010",
"110010001001000" when "111011",
"110100101111000" when "111100",
"110111011010100" when "111101",
"111010001011111" when "111110",
"111101000010111" when others;




 with a2cKey select a2c <= 
"00000000" when "0000000",
"00000000" when "0000001",
"00000000" when "0000010",
"00000000" when "0000011",
"00000000" when "0000100",
"00000000" when "0000101",
"00000000" when "0000110",
"00000000" when "0000111",
"00000000" when "0001000",
"00000000" when "0001001",
"00000000" when "0001010",
"00000000" when "0001011",
"00000000" when "0001100",
"00000000" when "0001101",
"00000000" when "0001110",
"00000000" when "0001111",
"00000000" when "0010000",
"00000000" when "0010001",
"00000000" when "0010010",
"00000000" when "0010011",
"00000000" when "0010100",
"00000000" when "0010101",
"00000000" when "0010110",
"00000000" when "0010111",
"00000000" when "0011000",
"00000000" when "0011001",
"00000000" when "0011010",
"00000000" when "0011011",
"00000000" when "0011100",
"00000000" when "0011101",
"00000000" when "0011110",
"00000000" when "0011111",
"00000000" when "0100000",
"00000011" when "0100001",
"00000110" when "0100010",
"00001001" when "0100011",
"00001100" when "0100100",
"00001111" when "0100101",
"00010010" when "0100110",
"00010101" when "0100111",
"00011000" when "0101000",
"00011011" when "0101001",
"00011110" when "0101010",
"00100001" when "0101011",
"00100100" when "0101100",
"00100111" when "0101101",
"00101010" when "0101110",
"00101101" when "0101111",
"00110000" when "0110000",
"00110011" when "0110001",
"00110110" when "0110010",
"00111001" when "0110011",
"00111100" when "0110100",
"00111111" when "0110101",
"01000010" when "0110110",
"01000101" when "0110111",
"01001000" when "0111000",
"01001011" when "0111001",
"01001110" when "0111010",
"01010001" when "0111011",
"01010100" when "0111100",
"01010111" when "0111101",
"01011010" when "0111110",
"01011101" when "0111111",
"00000000" when "1000000",
"00000010" when "1000001",
"00000100" when "1000010",
"00000110" when "1000011",
"00001000" when "1000100",
"00001010" when "1000101",
"00001100" when "1000110",
"00001110" when "1000111",
"00010000" when "1001000",
"00010010" when "1001001",
"00010100" when "1001010",
"00010110" when "1001011",
"00011000" when "1001100",
"00011010" when "1001101",
"00011100" when "1001110",
"00011110" when "1001111",
"00100000" when "1010000",
"00100010" when "1010001",
"00100100" when "1010010",
"00100110" when "1010011",
"00101000" when "1010100",
"00101010" when "1010101",
"00101100" when "1010110",
"00101110" when "1010111",
"00110000" when "1011000",
"00110010" when "1011001",
"00110100" when "1011010",
"00110110" when "1011011",
"00111000" when "1011100",
"00111010" when "1011101",
"00111100" when "1011110",
"00111110" when "1011111",
"00000000" when "1100000",
"00000101" when "1100001",
"00001010" when "1100010",
"00001111" when "1100011",
"00010100" when "1100100",
"00011001" when "1100101",
"00011110" when "1100110",
"00100011" when "1100111",
"00101000" when "1101000",
"00101101" when "1101001",
"00110010" when "1101010",
"00110111" when "1101011",
"00111100" when "1101100",
"01000001" when "1101101",
"01000110" when "1101110",
"01001011" when "1101111",
"01010000" when "1110000",
"01010101" when "1110001",
"01011010" when "1110010",
"01011111" when "1110011",
"01100100" when "1110100",
"01101001" when "1110101",
"01101110" when "1110110",
"01110011" when "1110111",
"01111000" when "1111000",
"01111101" when "1111001",
"10000010" when "1111010",
"10000111" when "1111011",
"10001100" when "1111100",
"10010001" when "1111101",
"10010110" when "1111110",
"10011011" when others;    
    

end rlt;