#ifndef _BCHDECODER_H_
#define _BCHDECODER_H_

#include<iostream>
#include<iomanip>
#include <stdio.h> 
#include <stdlib.h> 
#include<vector.h>
#include<matrix.h>   


using namespace std;
using namespace splab;

namespace splab {
	class BchDecoder {
	public:
		int p[21];
		int alpha_to[1048576], index_of[1048576], g[548576];

		int Count = 0;
		int syn_error = 0;
		int elp[1026][1024], l[1], s[1025];
		int root[200], loc[200], err[1024], reg[201];
		int qt[513], r[129][513];
		int b[12][513];
		int degr[129], degb[129];
		int temp, aux[513];

		void get_px(int m);

		void generate_gf(int n, int m);

		int generate_gx(int n, int m, int t);

		Vector<int>bch_decode(Vector<int>recd, int n, int k, int t);

		
	};
}

#endif