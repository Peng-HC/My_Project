
#ifndef _BCHENCODER_H_
#define _BCHENCODER_H_

#include<iostream>
#include<iomanip>
#include <stdio.h> 
#include <stdlib.h> 
#include<vector.h>
#include<matrix.h>   

using namespace std;
using namespace splab;

namespace splab {
	class BchEncoder {
	public:
		int p[21];
		int alpha_to[1048576], index_of[1048576], g[548576];
		int  recd[1048576], bb[548576];

		void get_px(int m);

		void generate_gf(int n, int m);

		int generate_gx(int n, int m, int t);


		Vector<int> add_0bits(Vector<int>a, int& blocknum, int k);


		Vector<int> bch_encode(Vector<int>data, int n, int k);
	};
}

#endif