library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity xilinxCubeSP is
  Port ( clk : in std_logic;  
        src : in unsigned (63 downto 0);
        res : out unsigned(63 downto 0));
        attribute use_dsp48 : string;
end xilinxCubeSP;

architecture RTL of xilinxCubeSP is

    signal mant : unsigned(23 downto 0);
    signal a : unsigned(16 downto 0);
    signal b2 : unsigned(7 downto 0);    
    signal aa : unsigned(33 downto 0);
    signal prod0 : unsigned(41 downto 0);
    signal prod : unsigned(51 downto 0);
    
    signal exp, expT, expN : unsigned(7 downto 0);	
    signal expExt : unsigned(9 downto 0);

component prodCubeXilinxSP
  Port ( 
	aa : in unsigned(16 downto 0);
	Mant : in unsigned(23 downto 0);
    b2 : in unsigned(7 downto 0);
    res : out unsigned(41 downto 0));
end component;    
    

 begin

	exp <= src(30 downto 23);
    expExt <= ("0" & exp & "0") + ("00" & exp) + "1100000010"; --  "302"; 
    expT <= expExt(7 downto 0); 

	Mant(23) <= '1'; 	Mant(22 downto 0) <= src(22 downto 0);

    a <= mant(23 downto 7); 
    b2(7 downto 1) <= src(6 downto 0); 
    b2(0) <= '0'; 
    aa <= a * a; 

    instProd: prodCubeXilinxSP port map(aa => aa(16 downto 0), Mant => Mant, b2 => b2, res => prod0);
    
    prod <=  (aa(33 downto 17) * (Mant + b2)) + prod0(41 downto 17);   
    

aligning:process(prod, exp, expT)
constant UNO : std_ulogic  := '1';
begin

    if exp > x"a9" then     -- fast, estrict, way to detect infinite
        expN <= (others=>'1');
        res(22 downto 0) <= (others=>'0');
    elsif exp < x"55" then    -- fast, estrict, way to detect zero
        expN <= (others=>'0');
        res(22 downto 0) <= (others=>'0');
    elsif prod(50) = UNO then
		expN <= expT + "10";
		res(22 downto 0) <= prod(49 downto 27);
	elsif prod(49) = '1' then
		expN <= expT + "1";
		res(22 downto 0) <= prod(48 downto 26);
	else
		expN <= expT;
		res(22 downto 0) <= prod(47 downto 25);
	end if;
	
end process; 

	res(31) <= src(31);	           -- sign
	res(30 downto 23) <= expN;



end RTL;



