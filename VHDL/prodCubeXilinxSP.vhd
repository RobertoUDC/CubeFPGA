library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity prodCubeXilinxSP is
  Port (    aa : in unsigned(16 downto 0);
            Mant : in unsigned(23 downto 0);
            b2 : in unsigned(7 downto 0);
            res : out unsigned(41 downto 0));
        attribute use_dsp48 : string;
end prodCubeXilinxSP;

architecture rtl of prodCubeXilinxSP is
 attribute use_dsp48 of res : signal is "yes";
begin

    res <= aa * (Mant+b2); 
end rtl;

