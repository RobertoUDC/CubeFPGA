library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity intelCubeDP is
  Port ( src : in unsigned (63 downto 0);
        res : out unsigned(63 downto 0));
end intelCubeDP;

architecture RTL of intelCubeDP is

    signal a, a21, a20 : unsigned(26 downto 0);
    signal b : unsigned(25 downto 0);    
    signal a2 : unsigned(53 downto 0);
    signal a30, a31 : unsigned(53 downto 0);
    signal a2b1 :  unsigned(52 downto 0);
    signal a2b0_3 :  unsigned(7 downto 0);		-- computed with LUTs, product by 3 included
    signal b2 :  unsigned(5 downto 0);
    signal ab2 :  unsigned(11 downto 0);

    signal suma2 :  unsigned(34 downto 0);   -- truncated @ -57
    signal prod :  unsigned(54 downto 0);	 -- truncated @ -57
    
    signal exp, expT, expN : unsigned(10 downto 0);	
    signal expExt : unsigned(12 downto 0);


component lutsIntelDP
  Port ( 
    a2 : in unsigned(2 downto 0);
	b : in unsigned(5 downto 0);
	a2b0 : out unsigned(7 downto 0);
	b2 : out unsigned(5 downto 0));
end component;


 begin

	exp <= src(62 downto 52);
    expExt <= ("0" & exp & "0") + ("00" & exp) + "1100000000010"; --  "1802"; 
    expT <= expExt(10 downto 0); 

    a <= "1" & src(51 downto 26);           -- [  0, -26]
    b <= src(25 downto 0);                  -- [-27, -52]
    a2 <= a * a;                            -- [  1, -52]
    a21 <= a2(53 downto 27);                -- [  1, -25]
    a20 <= a2(26 downto 0);                 -- [-26, -52]
    a31 <= a * a21;                         -- [  2, -51]
    
    a30 <= (a * a20) + (a2b0_3 & x"00000" & "0");  -- [-25, -78]
    a2b1 <= (b * a21) + (ab2 & x"000" & "00");                -- [-25, -77]  
    
    
    
    instTables : lutsIntelDP port map(a2 => a2(53 downto 51), b=> b(25 downto 20), a2b0 => a2b0_3, b2 => b2 );		
    
    -- b2	[-53, -58]
	-- a2b0	[-52, -57]  * 3 [-50, -57]

    ab2 <= b2 * a(26 downto 21);                    -- [-52, -69] 
    

    suma2 <= a30(53 downto 21) + ("0" & a2b1(52 downto 19)) + a2b1(52 downto 20);  -- a30 + 3 * a2b1     -- truncated @ -57 
--             [-25, -57]          *2 [-24, -57]        *1 [-25, -57]

	prod <= (a31 & "0") + suma2(34 downto 5); 
	
	
	
	
	
aligning:process(prod, exp, expT)
constant UNO : std_ulogic  := '1';
begin

    if exp > x"554" then     -- fast, estrict, way to detect infinite
        expN <= (others=>'1');
        res(51 downto 0) <= (others=>'0');
    elsif exp < x"2aa" then    -- fast, estrict, way to detect zero
        expN <= (others=>'0');
        res(51 downto 0) <= (others=>'0');
    elsif prod(54) = UNO then          
		expN <= expT + "10";
		res(51 downto 0) <= prod(53 downto 2);        
	elsif prod(53) = '1' then          
		expN <= expT + "1";
		res(51 downto 0) <= prod(52 downto 1);        
	else
		expN <= expT;
		res(51 downto 0) <= prod(51 downto 0);        
	end if;
end process; 

	res(63) <= src(63);	           -- sign
	res(62 downto 52) <= expN;


end RTL;



