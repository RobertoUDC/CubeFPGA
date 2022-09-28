#include <iostream>
#include "ap_int.h"
using namespace std;




ap_uint<32> squareXilinxSP(ap_uint<32> src){
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<24> Mant;
	ap_uint<25> Mant2;
	ap_uint<8> exp, expN, expT;
	ap_uint<9> expExt;

	ap_uint<17> a;
	ap_uint<7> b;
	ap_uint<42> res0;
	ap_uint<32> RES;

	Mant.bit(23) = 1; 	Mant(22, 0) = src(22, 0);
	exp = src(30, 23);

    expExt = (((ap_uint<10>)exp)<<1) + 0x181; 
    expT = expExt(7, 0);

	a = Mant(23, 7);
	b = src(6, 0);
	Mant2 = Mant + b;
	res0 = Mant2 * a;

	if(exp >= 0xbf){ 	// (40 + 7f = bf)
		expN = 0xff;
		RES(22, 0) = 0;
	}else if(exp<0x40){	// check again
		expN = 0;
		RES(22, 0) = 0;
	}else if(res0.bit(41)){
		expN = expT + 1;
		RES(22, 0) = res0(40, 18);
	}else{
		expN = expT;
		RES(22, 0) = res0(39, 17);
	}

	RES(30, 23) = expN;
	RES.bit(31) = 0;


	return RES;

}



ap_uint<42> prodCubeXilinxSP(ap_uint<17> a, ap_uint<25> b, ap_uint<8> c){
#pragma HLS INTERFACE ap_ctrl_none port=a
#pragma HLS INTERFACE ap_none port=a
#pragma HLS INTERFACE ap_ctrl_none port=b
#pragma HLS INTERFACE ap_none port=b
#pragma HLS INTERFACE ap_ctrl_none port=c
#pragma HLS INTERFACE ap_none port=c


	return (a * (b+c));

}



ap_uint<32> cubeXilinxSP(ap_uint<32> src){
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<24> Mant;
	ap_uint<24> MantB;
	ap_uint<8> exp, expN, expT;
	ap_uint<10> expExt;

	ap_uint<17> a;
	ap_uint<8> b2;
	ap_uint<34> aa;
	ap_uint<42> res0, res1, res2;
	ap_uint<32> RES;

	Mant.bit(23) = 1; 	Mant(22, 0) = src(22, 0);
	exp = src(30, 23);

    expExt = (((ap_uint<10>)exp)<<1) + ((ap_uint<10>)exp) + 0x302;
    expT = expExt(7, 0);

	a = Mant(23, 7);
	b2(7, 1) = src(6, 0);	b2.bit(0) = 0; 	// el doble de 2
	aa = a * a;			//   [1, -32]

	//res2 = aa(16, 0) * (Mant + b2);
	res2 = prodCuboXilinxSP(aa(16, 0), Mant, b2);
	res1 = (aa(33, 17) * (Mant + b2)) + res2(41, 17);



	if(exp > 0xa9){ 	// gt 42 (0x2a) (2a + 7f = a9)
		expN = 0xff;
		RES(22, 0) = 0;
	}else if(exp<0x55){		// lt (127 - 42)
		expN = 0;
		RES(22, 0) = 0;
	}else if(res1.bit(40)){
		expN = expT + 2;
		RES(22, 0) = res1(39, 17);
	}else if(res1.bit(39)){
		expN = expT + 1;
		RES(22, 0) = res1(38, 16);
	}else{
		expN = expT;
		RES(22, 0) = res1(37, 15);
	}

	RES(30, 23) = expN;
	RES.bit(31) = src.bit(31);


	return RES;

}



ap_uint<32> cubeXilinx_HLS_mantisa_SP(ap_uint<32> src){		// HLS will "cube" the mantissa
#pragma HLS INTERFACE ap_ctrl_none port=src
#pragma HLS INTERFACE ap_none port=src
#pragma HLS pipeline

	ap_uint<24> Mant;
	ap_uint<48> cuad;
	ap_uint<49> cube;

	ap_uint<8> exp, expN, expT;
	ap_uint<9> expExt;


	ap_uint<32> RES;


	Mant.bit(23) = 1; 	Mant(22, 0) = src(22, 0);
	cuad = Mant * Mant;
	cube = Mant * cuad(47, 23);

	exp = src(30, 23);
    expExt = (((ap_uint<10>)exp)<<1) + ((ap_uint<10>)exp) + 0x302;
    expT = expExt(7, 0);


	if(exp >= 0xbf){ 	// (40 + 7f = bf)
		expN = 0xff;
		RES(22, 0) = 0;
	}else if(exp<0x40){	// check again 
		expN = 0;
		RES(22, 0) = 0;
	}else if(cube.bit(48)){
		expN = expT + 2;
		RES(22, 0) = cube(47, 25);
	}else if(cube.bit(47)){
			expN = expT + 1;
			RES(22, 0) = cube(46, 24);
	}else{
			expN = expT;
			RES(22, 0) = cube(45, 23);
	}


	RES.bit(31) = src.bit(31);	// sign
	RES(30, 23) = expN;

	return RES;

}



