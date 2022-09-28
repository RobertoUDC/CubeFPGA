library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity intelCubeSP is
  Port ( src : in unsigned (31 downto 0);
        res : out unsigned(31 downto 0));
end intelCubeSP;

architecture RTL of intelCubeSP is

    signal mant : unsigned(24 downto 0);
    signal a : unsigned(16 downto 0);
    signal b : unsigned(6 downto 0);    
    signal a2 : unsigned(33 downto 0);
    signal prod : unsigned(51 downto 0);
    
    signal exp, expT, expN : unsigned(7 downto 0);	
    signal expExt : unsigned(9 downto 0);

 begin

	exp <= src(30 downto 23);
    expExt <= ("0" & exp & "0") + ("00" & exp) + "1100000010"; --  "302"; 
    expT <= expExt(7 downto 0); 

	mant(24 downto 23) <= "01"; 	mant(22 downto 0) <= src(22 downto 0);

    a <= mant(23 downto 7); 
    b <= src(6 downto 0);     
    a2 <= a * a; 

    prod <= a2(33 downto 7) * (mant + (b & "0")); 
	
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



