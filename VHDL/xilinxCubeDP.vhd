library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity xilinxCubeDP is
  Port ( clk : in std_logic;  
        src : in unsigned (63 downto 0);
        res : out unsigned(63 downto 0));
        attribute use_dsp48 : string;
end xilinxCubeDP;

architecture RTL of xilinxCubeDP is

    signal Mant : unsigned(52 downto 0);
    signal exp, expT, expN : unsigned(10 downto 0);	-- ampliar a 12 para 65 bits
    signal expExt : unsigned(12 downto 0);
	signal a, b, aa1, aa0, ab1, ab0 : unsigned(16 downto 0);
	signal c : unsigned(18 downto 0);
	signal aa, ab : unsigned(33 downto 0);
	signal mant1, mant0 : unsigned(25 downto 0);
	signal prod00, prod01, prod10, prod11 : unsigned(42 downto 0);
	signal a2c: unsigned(7 downto 0); 
	signal a2c8: unsigned(15 downto 0);

	signal b3, b3_x3: unsigned(14 downto 0); 
	signal b3_3y2z: unsigned(10 downto 0); 


	signal a2cKey : unsigned(6 downto 0);
	signal b3_x3Key, b3_3y2zKey : unsigned(5 downto 0);

	signal suma1, man, sumando1, sumando2, tripleLarge : unsigned(59 downto 0);
	signal g0, g1, g2, g3, g4, g5, g6, g7, t0, t1, t2 : unsigned(59 downto 0);	

	signal b2c : unsigned(35 downto 0);
	signal prodb1, prodb0 : unsigned(41 downto 0);
	signal tripleSmall : unsigned(43 downto 0);

    attribute use_dsp48 of prodb0, prodb1 : signal is "yes";

component lutsCubeDP
  Port ( 
	a2cKey : in unsigned(6 downto 0);
	b3_x3Key, b3_3y2zKey : in unsigned(5 downto 0);

    a2c : out unsigned(7 downto 0);
    b3_x3 : out unsigned(14 downto 0);
    b3_3y2z : out unsigned(10 downto 0));
end component;

component dspPreAdder
  Port (    a : in unsigned (16 downto 0);
            b : in unsigned (23 downto 0);
            c : in unsigned (24 downto 0);
            res : out unsigned(41 downto 0));
end component;

component dspPreAdderPostAdder
  Port (    a : in unsigned (16 downto 0);
            b : in unsigned (23 downto 0);
            c : in unsigned (24 downto 0);
            d : in unsigned (15 downto 0);
            res : out unsigned(41 downto 0));
end component;



 begin
 
    expExt <= ("0" & exp & "0") + ("00" & exp) + "1100000000010"; --  "1802"; 
    expT <= expExt(10 downto 0); 

	Mant(52) <= '1'; 	Mant(51 downto 0) <= src(51 downto 0);
	c <= src(18 downto 0);											-- [-34, -52]
	b <= src(35 downto 19);											-- [-17, -33]
	a <= Mant(52 downto 36);											-- [  0, -16]
	exp <= src(62 downto 52);

	aa <= a * a;													-- [  1, -32]
	aa1 <= aa(33 downto 17);											-- [  1, -15]
	aa0 <= aa(16 downto 0);											-- [-16, -32]

	mant1 <= ("0" & src(51 downto 27)) + src(35 downto 26);
	mant0 <= ("0" & src(26 downto 2)) + src(25 downto 1);
	prod00 <= aa0 * mant0;										-- [-40, -82]
	prod01 <= aa0 * mant1;										-- [-15, -57]
	prod10 <= aa1 * mant0;										-- [-23, -65]
	prod11 <= aa1 * mant1;										-- [  2, -40]

	a2cKey(6 downto 5) <= c(1 downto 0); 	a2cKey(4 downto 0) <= aa(33 downto 29);
    b3_x3Key <= b(16 downto 11);
    b3_3y2zKey(5 downto 3) <= b(16 downto 14); 		b3_3y2zKey(2 downto 0) <= b(10 downto 8);
    
    instTablas : lutsCuboDP port map(a2cKey, b3_x3Key, b3_3y2zKey, a2c, b3_x3, b3_3y2z);

	b3 <= b3_x3 + b3_3y2z;

	-- 3ab2 y 6abc
	ab <= a * b;								-- [-16, -49]
	ab1 <= ab(33 downto 17);					-- [-16, -32]
	ab0 <= ab(16 downto 0);						-- [-33, -49]
	b2c(17 downto 0) <= c(17 downto 0);
	b2c(35 downto 18) <= ("0" & b) + ("0" & c(18));		-- [-16, -51]	b2c = ((b << 19) + (c << 1)) >> 1

    instB0: dspPreAdder port map(a=> ab0, b=> b2c(35 downto 12), c=> b2c(35 downto 11), res=> prodb0); 
     
    a2c8 <= a2c & x"00";
    instB1: dspPreAdderPostAdder port map(a=> ab1, b=> b2c(35 downto 12), c=> b2c(35 downto 11), d=> a2c8, res=> prodb1);

	g0 <= prod11 & "000000" & prodb0(41 downto 31);     -- order is important
	g1 <= "0" & aa & "0000000" & prod00(42 downto 25); -- order is important
	g2 <= x"0000" & "0" & prod01;
	g3 <= x"000000" & "0" & prod10(42 downto 8);
	g4 <= x"00000000" & prodb1(41 downto 14);
	g5 <= x"000000000000" & "000" & b3(14 downto 6);
	g6 <= x"000000000000" & "00" & a2c & "00";	
	
	sumando1 <= g0 + g1 + g2; 
	sumando2 <= g3 + g4 + g5; 
	man <= sumando1 + sumando2;   
	
	
	
aligning:process(man, exp)
constant UNO : std_ulogic  := '1';
begin

    if exp > x"554" then			-- fast, estrict, way to detect infinite
        expN <= (others=>'1');
        res(51 downto 0) <= (others=>'0');
    elsif exp < x"2aa" then    -- fast, estrict, way to detect zero
        expN <= (others=>'0');
        res(51 downto 0) <= (others=>'0');
    elsif man(59) = UNO then
		expN <= expT + "10";
		res(51 downto 0) <= man(58 downto 7);
	elsif man(58) = '1' then
		expN <= expT + "1";
		res(51 downto 0) <= man(57 downto 6);
	else
		expN <= expT;
		res(51 downto 0) <= man(56 downto 5);
	end if;
end process; 

	res(63) <= src(63);	           -- sign
	res(62 downto 52) <= expN;


end RTL;




library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity dspPreAdder is
  Port (    a : in unsigned (16 downto 0);
            b : in unsigned (23 downto 0);
            c : in unsigned (24 downto 0);
            res : out unsigned(41 downto 0));
        attribute use_dsp48 : string;
end dspPreAdder;

architecture rtl of dspPreAdder is
 attribute use_dsp48 of res : signal is "yes";
begin

    res <= a * (b+c); 

end rtl;





library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity dspPreAdderPostAdder is
  Port (    a : in unsigned (16 downto 0);
            b : in unsigned (23 downto 0);
            c : in unsigned (24 downto 0);
            d : in unsigned (15 downto 0);
            res : out unsigned(41 downto 0));
        attribute use_dsp48 : string;
end dspPreAdderPostAdder;

architecture rtl of dspPreAdderPostAdder is
 attribute use_dsp48 of res : signal is "yes";
begin

    res <= (a * (b+c)) + d;  

end rtl;


