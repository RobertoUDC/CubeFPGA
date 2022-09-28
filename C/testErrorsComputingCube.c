#include<stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned long long cubeTotal(unsigned long long p1, unsigned long long p2, unsigned long long p3);




unsigned long long cubeTotalAltera(unsigned long long p1, unsigned long long p2) {

	// only mantissa, split in 2 parts: p1 and p2
	// completely accurate, used for comparison

	int i;
	unsigned long long V, a, b;
	unsigned long long X[4];
	unsigned long long q00, q01, q11, M = 0xffffffff, carry;
	unsigned long long c[4], d[4], e[6];

	carry = M + 1;

	V = (p1 << 26) | p2;

	a = V >> 32;
	b = V & M;

	q00 = b * b;
	q01 = a * b;
	q11 = a * a;

	X[0] = q00 & M;
	X[1] = (q00 >> 32) + ((q01 & M) << 1);
	X[2] = (q11 & M) + ((q01 >> 32) << 1);
	X[3] = q11 >> 32;

	if (X[1] & carry) {
		X[1] &= M;
		++X[2];
	}

	if (X[2] & carry) {
		X[2] &= M;
		++X[3];
	}

	// so far, square has been computed

	for (i = 0; i < 4; ++i) {
		d[i] = X[i] * a;
		c[i] = X[i] * b;
	}

	e[0] = c[0] & M;
	e[1] = (c[0] >> 32) + (c[1] & M) + (d[0] & M);
	e[2] = (c[1] >> 32) + (c[2] & M) + (d[0] >> 32) + (d[1] & M);
	e[3] = (c[2] >> 32) + (c[3] & M) + (d[1] >> 32) + (d[2] & M);
	e[4] = (c[3] >> 32) + (d[2] >> 32) + (d[3] & M);
	e[5] = d[3] >> 32;

	for (i = 0; i < 5; ++i) {
		if (e[i] > M) {
			e[i + 1] += e[i] >> 32;	// a veces el acarreo es de 2, no de 1
			e[i] &= M;
		}
	}

	if (!e[5]) {
		for (i = 5; i > 0; --i)
			e[i] = e[i - 1];
	}

	while (e[5] < 0x10000000000000) {	// normalizing
		e[5] <<= 1;
		e[4] <<= 1;
		e[3] <<= 1; 
	}


	e[4] += 0x80000000;	// rounding

	e[5] += e[4] >> 32;		

	if (e[5] >= 0x20000000000000)
		printf("ERROR\n");

	return e[5];

}




void TEST_cube_DP_Altera() {

	double num, * ptN, cubeThis, cube, * ptM;
	int i, j, k;
	unsigned long long N, a, b, c, exp, M, Mbueno;
	unsigned long long Mant, man, UNO = 1;
	unsigned long long aa, a21, a20, a31, a30, b2, ab2, a2b1, a2b0;

	unsigned long long EXACTO, dif1, dif2, step = 0x4000000000000;
	unsigned long long cont1, contX, cont2;

	time_t t;

	ptN = (double*)(&N);
	ptM = (double*)(&M);

	num = 1.25;
	cont1 = contX = cont2 = 0;


	scanf_s("%d", &i);	// improves seed randomness

	srand((unsigned)time(&t));

	for (i = 0; i < 262144; ++i) {	// 

		a = 0x4000000 | ((i & 0x3fe00) << 8) | ((rand() & 0xff) << 9) | (i & 0x1ff);

		for (j = 0; j < 262144; ++j) {

			b = ((j & 0x3fe00) << 8) | ((rand() & 0xff) << 9) | (j & 0x1ff);

			Mant = (a << 26) | b;

			*ptN = num;
			N = 0x3ff0000000000000 | (Mant & 0xfffffffffffff);
			num = *ptN;
			cube = num * num * num;
			*ptM = cube;
			exp = 0x3ff;

			Mbueno = M;

			aa = a * a;															// [  1, -52]
			a21 = aa >> 27;														// [  1, -25]
			a20 = aa & 0x7ffffff;												// [-26, -52]
			a31 = a * a21;														// [  2, -51]
			a30 = a * a20;														// [-25, -78]

			a2b1 = a21 * b;														// [-25, -77]
			a2b0 = (a20 >> 24) * (b >> 23);
	
			b2 = b >> 20;			// 6 MSB									// [-27, -32]
			b2 *= b2;				// b square									// [-53, -64]
			b2 >>= 6;															// [-53, -58]
			ab2 = b2 * (a >> 21);												// [-52, -63]

			man = (a31 << 6) + (a30 >> 21) + ((3 * ab2) >> 6) + ((3 * a2b1) >> 20) + (3 * a2b0);	

			if (man & (UNO << 59)) {
				exp += 2;
				man >>= 7;
			}
			else if (man & (UNO << 58)) {
				exp++;
				man >>= 6;
			}
			else
				man >>= 5;

			M = (exp << 52) | (man & 0xfffffffffffff);
			cubeThis = *ptM;


			EXACTO = cubeTotalAltera(a, b) & 0xfffffffffffff;
			Mbueno &= 0xfffffffffffff;
			man &= 0xfffffffffffff;

			if (Mbueno > EXACTO)	dif1 = Mbueno - EXACTO;
			else					dif1 = EXACTO - Mbueno;

			if (EXACTO > man)	dif2 = EXACTO - man;
			else				dif2 = man - EXACTO;

			if (dif2 > 2)					// detects 2 ulp errors
				printf("%lld  ", dif2);

			cont1 += dif1;
			cont2 += dif2;


		}
	}

	printf("%lld  %lld  %lld\n", cont1, contX, cont2);	// reports accumulated errors

}



unsigned long long  longMultiplication(unsigned long long x, unsigned long long y) {

	unsigned long long a, b, p00, p01, p10, p11, H1, H0;

	a = x & 0x7fffffff;
	b = y & 0x7fffffff;
	p00 = a * b;

	b = (y >> 31) & 0x7fffffff;
	p01 = a * b;

	a = (x >> 31) & 0x7fffffff;
	p11 = a * b;

	b = y & 0x7fffffff;
	p10 = a * b;

	H0 = p00 + ((p01 << 31) & 0x3fffffffffffffff) + ((p10 << 31) & 0x3fffffffffffffff);
	H1 = p11 + (p01 >> 31) + (p10 >> 31);
	H1 += H0 >> 62;
	H0 &= 0x3fffffffffffffff;
	H0 |= H1 << 62;
	H1 >>= 2;
//	printf("%016llx %016llx\n", H1, H0);

	H1 <<= 14; // pasamos a una única palabra de hasta 56 bits
	H1 |= H0 >> 50;
	return H1;

}


void TEST_cube_DP_Xilinx() {

	double num, * ptN, cubeThis, cube, * ptM;
	int i, j, k; 
	unsigned long long N, a, b, c, exp, M, Mbueno;
	unsigned long long aa, aa1, aa0, a2b, Mant, res1, res2, man, UNO = 1;
	unsigned long long mant1, mant0, prod00, prod01, prod10, prod11, a2c, suma1, suma2, suma, b3;
	unsigned char a2cTab[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75};
	unsigned char C3[4] = { 0, 3, 2, 5 };

	unsigned int stmp, b3_x3, b3_3y2z;
	unsigned long long ab, ab1, ab0, b2c, b2c3, prodb1, prodb0;
	unsigned long long EXACTO, dif1, dif2, step = 0x4000000000000;
	unsigned long long cont1, contX, cont2;

	time_t t; 

	ptN = (double*)(&N);
	ptM = (double*)(&M);

	num = 1.25;
	cont1 = contX = cont2 = 0;

	scanf_s("%d", &i);	// improves seed randomness
	srand((unsigned)time(&t));


	for (i = 0; i < 4096; ++i) {

		a = 0x10000 | ((i & 0xfc0) << 4) | ((rand() & 0xf) << 6) | (i & 0x3f);	// solo 16 bits porque después viene el 1

		for (j = 0; j < 4096; ++j) {

			b = ((j & 0xfc0) << 5) | ((rand() & 0x1f) << 6) | (j & 0x3f);

			for (k = 0; k < 4096; ++k) {

				c = ((k & 0xfc0) << 7) | ((rand() & 0x7f) << 6) | (k & 0x3f);


				Mant = (a << 36) | (b << 19) | c;

				*ptN = num;
				N = 0x3ff0000000000000 | (Mant & 0xfffffffffffff);
				num = *ptN;
				cube = num * num * num;
				*ptM = cube;
				exp = 0x3ff;

				Mbueno = M; 

				aa = a * a;															// [  1, -32]
				aa1 = aa >> 17;														// [  1, -15]
				aa0 = aa & 0x1ffff;													// [-16, -32]

				mant1 = ((Mant >> 27) & 0x1ffffff) + ((Mant >> 26) & 0x3ff);		// [  0, -25]
				mant0 = ((Mant >> 2) & 0x1ffffff) + ((Mant >> 1) & 0x1ffffff);		// [-25, -50]
				prod00 = aa0 * mant0;												// [-40, -82]
				prod01 = aa0 * mant1;												// [-15, -57]
				prod10 = aa1 * mant0;												// [-23, -65]
				prod11 = aa1 * mant1;												// [  2, -40]

				a2c = a2cTab[((c & 3) << 4) | (aa >> 30)];		// [-48, -54]
				suma1 = (prod00 >> 25) + prod01 + (prod10 >> 8) + (prod11 << 17) + (aa << 25) + (a2c << 3);	//  [ 2, -57]	

				/// b3
				stmp = b >> 11;								// [-17, -22]  (x)
				b3_x3 = stmp * stmp * stmp;					// [-49, -66]	LUTs
				b3_3y2z = stmp >> 3;						// [-17, -19]  (y)
				stmp = (b >> 8) & 0x7;						// [-23, -25]
				b3_3y2z = 3 * b3_3y2z * b3_3y2z * stmp;		// [-53, -63]

				b3 = (unsigned long long)((b3_x3 >> 3) + b3_3y2z);				// [-49, -63]

				// 3ab2 y 6abc
				ab = a * b;									// [-16, -49]
				ab1 = ab >> 17;								// [-16, -32]
				ab0 = ab & 0x1ffff;							// [-33, -49] 
				b2c = (b << 19) + (c << 1); 				// [-16, -51]	b + 2c 

				b2c3 = (b2c >> 12) + (b2c >> 11);			// [-14, -39]	
				prodb1 = ab1 * b2c3;						// [-29, -71]	error < 2^-54
				prodb0 = ab0 * b2c3;						// [-46, -88]	

				suma2 = prodb1 + (prodb0 >> 17);			// [-28, -71]	
				man = suma1 + (suma2 >> 15) + (b3 >> 6);	// [ 2, -57]

				if (man & (UNO << 59)) {
					exp += 2;
					man >>= 7;
				}
				else if (man & (UNO << 58)) {
					exp++;
					man >>= 6;
				} 
				else
					man >>= 5;

				M = (exp << 52) | (man & 0xfffffffffffff);
				cubeThis = *ptM;

				EXACTO = cubeTotal(a, b, c) & 0xfffffffffffff;
				Mbueno &= 0xfffffffffffff;
				man &= 0xfffffffffffff;

				if (Mbueno > EXACTO)	dif1 = Mbueno - EXACTO;
				else					dif1 = EXACTO - Mbueno;

				if (EXACTO > man)	dif2 = EXACTO - man;
				else				dif2 = man - EXACTO; 

				if (dif2 > 2)
					printf("%lld  ", dif2);		// pinpoints large errors 

				cont1 += dif1;
				cont2 += dif2; 

			}
		}

	}

	printf("%lld  %lld  %lld\n", cont1, contX, cont2);	// reports accumulated errors

}














unsigned long long cubeTotal(unsigned long long p1, unsigned long long p2, unsigned long long p3) {
	//Xilinx, ACCURATE

	int i; 
	unsigned long long V, a, b;
	unsigned long long X[4];
	unsigned long long q00, q01, q11, M = 0xffffffff, carry;
	unsigned long long c[4], d[4], e[6]; 

	carry = M + 1; 

	V = (p1 << 36) | (p2 << 19) | p3;

	a = V >> 32; 
	b = V & M;
	
	q00 = b * b; 
	q01 = a * b; 
	q11 = a * a; 
	
	X[0] = q00 & M; 
	X[1] = (q00 >> 32) + ((q01 & M) << 1);
	X[2] = (q11 & M) + ((q01 >> 32) << 1); 
	X[3] = q11 >> 32; 

	if (X[1] & carry) {
		X[1] &= M;
		++X[2];
	}

	if (X[2] & carry) {
		X[2] &= M;
		++X[3];
	}

	// hasta aquí el square

	for (i = 0; i < 4; ++i) {
		d[i] = X[i] * a;
		c[i] = X[i] * b;
	}
	
	e[0] = c[0] & M; 
	e[1] = (c[0] >> 32) + (c[1] & M)				 + (d[0] & M);
	e[2] = (c[1] >> 32) + (c[2] & M) +	(d[0] >> 32) + (d[1] & M);
	e[3] = (c[2] >> 32) + (c[3] & M) +	(d[1] >> 32) + (d[2] & M);
	e[4] = (c[3] >> 32) +				(d[2] >> 32) + (d[3] & M);
	e[5] = d[3] >> 32;

	for (i = 0; i < 5; ++i) {
		if (e[i] > M) {
			e[i + 1] += e[i]>>32;	// a veces el acarreo es de 2, no de 1
			e[i] &= M;
		}
	}

	if (!e[5]) {
		for (i = 5; i >0; --i)
			e[i] = e[i - 1]; 
	}

	while (e[5] < 0x10000000000000) {
		e[5] <<= 1; 
		e[4] <<= 1;
		e[3] <<= 1; 
	}

	e[4] >>= 32;

	e[5] += e[4];

	if (e[5] >= 0x20000000000000)
		printf("ERROR\n");

	return e[5]; 

}




void test_cube_SP_Xilinx() {

	float num, * ptN, cubeThis, cube, * ptM;
	unsigned int N, i, j, a, b, exp, M;
	unsigned long long mant, mant2, aa, aa1, aa0, a31, a30, res, UNO = 1, tmp;
	unsigned long long cuad, cuad1, cuad0, q0, q1;
	unsigned long long errThis, errIntel, cont, max; 
	long long diff; 

	ptN = (float*)(&N);
	ptM = (float*)(&M);
		
	errThis = errIntel = cont = 0;
	max = 0x0;

	for (i = 0x3f800000; i <= 0x3fffffff; ++i) { // selected range

		N = i; 
		num = *ptN;	
		cube = num * num * num;
		*ptM = cube;

		mant = (N & 0x7fffff) | 0x800000;
		b = mant & 0x7f;
		a = mant >> 7;
		exp = N >> 23;

		aa = (unsigned long long)a;
		aa *= aa;					
		aa1 = aa >> 17;	
		aa0 = aa & 0x1ffff; 
		mant2 = mant + (b << 1); 
		a31 = aa1 * mant2;
		a30 = aa0 * mant2;
		
		res = a31 + (a30 >> 17);


		cuad = mant * mant; 
		cuad1 = cuad >> 32;		cuad0 = cuad & 0xffffffff; 
		q1 = cuad1 * mant;		q0 = cuad0 * mant; 		
		q0 >>= 16;
		q0 += (q1 << 16);
		q0 >>= 15; 

		M &= 0x7fffff;

		if (M) {
			M |= 0x800000;
			M = M;
			j = -24; tmp = res;
			while (tmp) {
				++j;
				tmp >>= 1;
			};
			tmp = M;
			q0 >>= j;
			res >>= j; 

			diff = res - q0;
			if (diff > 20)		// just a "large" number - avoids triggering alarms in the case 0xfffff.... versus 0x8000...
				continue; 
			if (diff < 0)	diff = -diff;	
			if (diff > max)
				max = diff;
			errThis += diff;


			diff = tmp - q0;
			if (diff < 0)	diff = -diff;			
			errIntel += diff;
			++cont; 
		}

	}

	printf("%lld     %lld\n", errThis, errIntel);
	printf("%llf     %llf\n", ((double)errThis)/((double)cont), ((double)errIntel) / ((double)cont));		// reports errors

}


void test_cube_SP_Altera() {

	float num, * ptN, cubeThis, cube, * ptM;
	unsigned int N, i, j, a, b, exp, M;
	unsigned long long mant, mant2, aa, aa1, aa0, a31, a30, res, UNO = 1, tmp;
	unsigned long long cuad, cuad1, cuad0, q0, q1;
	unsigned long long errThis, errIntel, cont, max;
	long long diff;

	ptN = (float*)(&N);
	ptM = (float*)(&M);

	errThis = errIntel = cont = 0;
	max = 0x0;

	for (i = 0x3f800000; i <= 0x3fffffff; ++i) { // selected range

		N = i;
		num = *ptN;
		cube = num * num * num;
		*ptM = cube;

		mant = (N & 0x7fffff) | 0x800000;
		b = mant & 0x7f;
		a = mant >> 7;
		exp = N >> 23;

		aa = (unsigned long long)a;
		aa *= aa;						//   [1, -32]
		mant2 = mant + (b << 1);		//   [1, -23]	A + 3B taking 25 bits at most

		res = mant2 * (aa >> 7);			// 34 bits - 7 = 27 bits 
										// [1, -23] * [1, -25] = [3, -48]

		cuad = mant * mant;
		cuad1 = cuad >> 32;		cuad0 = cuad & 0xffffffff;
		q1 = cuad1 * mant;		q0 = cuad0 * mant;
		q0 >>= 16;
		q0 += (q1 << 16);
		q0 >>= 5;

		M &= 0x7fffff;

		if (M) {
			M |= 0x800000;
			M = M;
			j = -24; tmp = res;
			while (tmp) {
				++j;
				tmp >>= 1;
			};
			tmp = M;

			q0 += 1 << (j - 1); 
			q0 >>= j;
			res >>= j;

			diff = res - q0;
			if (diff > 20)		// just a "large" number - avoids triggering alarms in the case 0xfffff.... versus 0x8000...
				continue;
			if (diff < 0)	diff = -diff;
			if (diff > max)
				max = diff;
			errThis += diff;


			diff = tmp - q0;
			if (diff < 0)	diff = -diff;
			//			if (diff > max)				max = diff;
			errIntel += diff;
			++cont;
		}
	}

	printf("%lld     %lld\n", errThis, errIntel);
	printf("%llf     %llf\n", ((double)errThis) / ((double)cont), ((double)errIntel) / ((double)cont));		// reports error

}



void main() {

	//test_cube_SP_Altera();
	//test_cube_SP_Xilinx(); 

	
	//TEST_cube_DP_Xilinx();
	//TEST_cube_DP_Altera();


}








