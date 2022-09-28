#include <iostream>
#include "ap_int.h"
using namespace std;
#include<math.h>
#include <hls_math.h>

ap_uint<64> squareXilinxDP(ap_uint<64> src){
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<53> Mant;
	ap_uint<24> MantB;
	ap_uint<11> exp, expN, expT;	// ampliar a 12 para 65 bits
	ap_uint<13> expExt;

	ap_uint<17> a, b;
	ap_uint<19> c;
	ap_uint<34> aa, b2;
	ap_uint<36> ab;
	ap_uint<43> ac;
	ap_uint<11> bc;

	ap_uint<61> man;
	ap_uint<64> RES;


	Mant.bit(52) = 1; 	Mant(51, 0) = src(51, 0);	// [  0, -52]
	c = src(18, 0);									// [-34, -52]
	b = src(35, 19);								// [-17, -33]
	a = Mant(52, 36);								// [  0, -16]
	exp = src(62, 52);

    expExt = (((ap_uint<13>)exp)<<1) + 0x1c01; 		// exponent is doubled
    expT = expExt(10, 0);

	aa = a * a;										// [  1, -32]
	ab = a * Mant(35, 17);							// [-16, -51]
	b2 = b * b;										// [-33, -66]
	ac = Mant(16, 0) * Mant(52, 27);				// [-35, -77]
	bc = Mant(35, 27) * Mant(18, 17);				// [-50, -60]		this product uses LUTs because Mant(18, 17) is only 2-bit long

	man = (((ap_uint<61>)aa) << 27) + (((ap_uint<61>)ab) << 9) + (((ap_uint<61>)b2) >> 7) + (((ap_uint<61>)ac) >> 17) + bc; // correct alignment is crucial


	if(expT>0x554){		
		expN = 0x7ff;
		RES(51, 0) = 0;
	}else if(expT<0x2aa){	
		expN = 0;
		RES(51, 0) = 0;
	}else if(man.bit(60)){
		expN = expT + 1;
		RES(51, 0) = man(59, 8);
	}else{
		expN = expT;
		RES(51, 0) = man(58, 7);
	}

	RES.bit(63) = 0;	// sign
	RES(62, 52) = expN;

	return RES;

}



ap_uint<42> prodCuboXilinxDP(ap_uint<17> a, ap_uint<24> b, ap_uint<25> c){
#pragma HLS INTERFACE ap_ctrl_none port=a
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_ctrl_none port=b
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_ctrl_none port=c
#pragma HLS INTERFACE ap_none port=c


	return (a * (b+c));

}

ap_uint<42> prodCuboXilinxDP_postAdder(ap_uint<17> a, ap_uint<24> b, ap_uint<25> c, ap_uint<24>d){
#pragma HLS INTERFACE ap_ctrl_none port=a
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_ctrl_none port=b
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_ctrl_none port=c
#pragma HLS INTERFACE ap_none port=c
#pragma HLS INTERFACE ap_ctrl_none port=d
#pragma HLS INTERFACE ap_none port=d

	return ((a * (b+c)) + d);

}



ap_uint<64> cubeXilinx_HLS_mantisa_DP(ap_uint<64> src){		// HLS will "cube" the mantissa
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<53> Mant;
	ap_uint<106> cuad;
	ap_uint<107> cube;

	ap_uint<11> exp, expN, expT;	// ampliar a 12 para 65 bits
	ap_uint<13> expExt;

	ap_uint<64> RES;


	Mant.bit(52) = 1; 	Mant(51, 0) = src(51, 0);
	cuad = Mant * Mant;
	cube = Mant * cuad(105, 52);

	exp = src(62, 52);
    expExt = (((ap_uint<13>)exp)<<1) + ((ap_uint<13>)exp) + 0x1802;
    expT = expExt(10, 0);

	if(expT>0x554){ 		
		expN = 0x7ff;
		RES(51, 0) = 0;
	}else if(expT<0x2aa){
		expN = 0;
		RES(51, 0) = 0;
	}else if(cube.bit(106)){
		expN = expT + 2;
		RES(51, 0) = cube(105, 54);
	}else if(cube.bit(105)){
		expN = expT + 1;
		RES(51, 0) = cube(104, 53);
	}else{
		expN = expT;
		RES(51, 0) = cube(103, 52);
	}

	RES.bit(63) = src.bit(63);	// signo
	RES(62, 52) = expN;

	return RES;

}




ap_uint<64> cuboXilinxDP(ap_uint<64> src){
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<53> Mant;
	ap_uint<24> MantB;
	ap_uint<11> exp, expN, expT;	
	ap_uint<13> expExt;

	ap_uint<17> a, b, aa1, aa0, ab1, ab0;
	ap_uint<19> c;
	ap_uint<34> aa, ab;
	ap_uint<26> mant1, mant0;
	ap_uint<43> prod00, prod01, prod10, prod11;
	ap_uint<8> a2c;
	const ap_uint<8> a2cTab[128] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66, 69, 72, 75, 78, 81, 84, 87, 90, 93, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155};

	ap_uint<15> b3, b3_x3;
	const ap_uint<15> b3_x3Tab[64] ={0, 0, 1, 3, 8, 15, 27, 42, 64, 91, 125, 166, 216, 274, 343, 421, 512, 614, 729, 857, 1000, 1157, 1331, 1520, 1728, 1953, 2197, 2460, 2744, 3048, 3375, 3723, 4096, 4492, 4913, 5359, 5832, 6331, 6859, 7414, 8000, 8615, 9261, 9938, 10648, 11390, 12167, 12977, 13824, 14706, 15625, 16581, 17576, 18609, 19683, 20796, 21952, 23149, 24389, 25672, 27000, 28372, 29791, 31255};
	ap_uint<11> b3_3y2z;
	const ap_uint<11> b3_3y2zTab[64] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 12, 15, 18, 21, 0, 12, 24, 36, 48, 60, 72, 84, 0, 27, 54, 81, 108, 135, 162, 189, 0, 48, 96, 144, 192, 240, 288, 336, 0, 75, 150, 225, 300, 375, 450, 525, 0, 108, 216, 324, 432, 540, 648, 756, 0, 147, 294, 441, 588, 735, 882, 1029};

#pragma HLS RESOURCE variable=a2cTab latency=0
#pragma HLS RESOURCE variable=b3_x3Tab latency=0
#pragma HLS RESOURCE variable=b3_3y2zTab latency=0
#pragma HLS RESOURCE variable=a2c latency=0
#pragma HLS RESOURCE variable=b3_x3 latency=0
#pragma HLS RESOURCE variable=b3_3y2z latency=0


	ap_uint<7> a2cKey;
	ap_uint<6> b3_x3Key, b3_3y2zKey;

	ap_uint<60> suma1, man, sumando1, sumando2, tripleLarge;
	ap_uint<8> tt0[8], tt1[8], tt2[8];

	ap_uint<36> b2c;
	ap_uint<43> prodb1, prodb0;
	ap_uint<44> tripleSmall;

	ap_uint<64> RES;


	Mant.bit(52) = 1; 	Mant(51, 0) = src(51, 0);
	c = src(18, 0);												// [-34, -52]
	b = src(35, 19);											// [-17, -33]
	a = Mant(52, 36);											// [  0, -16]
	exp = src(62, 52);

    expExt = (((ap_uint<13>)exp)<<1) + ((ap_uint<13>)exp) + 0x1802;
    expT = expExt(10, 0);



	aa = a * a;													// [  1, -32]
	aa1 = aa(33, 17);											// [  1, -15]
	aa0 = aa(16, 0);											// [-16, -32]

	mant1 = src(51, 27) + src(35, 26);
	mant0 = src(26, 2) + src(25, 1);
	prod00 = aa0 * mant0;												// [-40, -82]
	prod01 = aa0 * mant1;												// [-15, -57]
	prod10 = aa1 * mant0;												// [-23, -65]
	prod11 = aa1 * mant1;												// [  2, -40]

	a2cKey(6, 5) = c(1, 0); 	a2cKey(4, 0) = aa(33, 29);
	a2c = a2cTab[a2cKey];

	/// b3
	b3_x3Key = b(16, 11);
	b3_x3 = b3_x3Tab[b3_x3Key];

	b3_3y2zKey(5, 3) = b(16, 14); 		b3_3y2zKey(2, 0) = b(10, 8);
	b3_3y2z = b3_3y2zTab[b3_3y2zKey];
	b3 = b3_x3 + b3_3y2z;


	// 3ab2 y 6abc
	ab = a * b;									// [-16, -49]
	ab1 = ab(33, 17);							// [-16, -32]
	ab0 = ab(16, 0);							// [-33, -49]
	b2c(17, 0) = c(17, 0);
	b2c(35, 18) = b + c.bit(18);				// [-16, -51]	b2c = ((b << 19) + (c << 1)) >> 1


	prodb0 = prodCuboXilinxDP(ab0, b2c(35, 12), b2c(35, 11)); // mucho cuidado, creo que fallará
	prodb1 = prodCuboXilinxDP_postAdder(ab1, b2c(35, 12), b2c(35, 11), a2c << 16); // mucho cuidado, creo que fallará
	sumando1 = (((ap_uint<60>)prod11)<<17) | prodb0(42, 31);
	sumando2 = (((ap_uint<60>)aa)<<25) | prod00(42, 25);

	tripleLarge = sumando1 + sumando2 + b3(14, 6);
	tripleSmall = prod01 + prod10(42, 8) + prodb1(42, 14);
	man = tripleLarge + tripleSmall;


	if(expT>0x554){
		expN = 0x7ff;
		RES(51, 0) = 0;
	}else if(expT<0x299){
		expN = 0;
		RES(51, 0) = 0;
	}else if(man.bit(59)){
		expN = expT + 2;
		RES(51, 0) = man(58, 7);
	}else if(man.bit(58)){
		expN = expT + 1;
		RES(51, 0) = man(57, 6);
	}else{
		expN = expT;
		RES(51, 0) = man(56, 5);
	}


	RES.bit(63) = src.bit(63);	// signo
	RES(62, 52) = expN;

	return RES;

}

