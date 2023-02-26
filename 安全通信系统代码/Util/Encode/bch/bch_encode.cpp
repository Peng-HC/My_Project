#include "bch_encode.h"
#include "BchEncoder.h"

Vector<int> bch_encoder(Vector<int> x) {
	int codelength = x.size();

	/////get [n,k,t]
	int m = 4;//m=2-20
	int t = 2;
	int n = pow(2, m) - 1;;
	int k;
	int d = 2 * t + 1;

	BchEncoder bchEncoder;

	bchEncoder.get_px(m);
	bchEncoder.generate_gf(n, m);
	k = bchEncoder.generate_gx(n, m, t);

	/////add 0 bits
	int blocknum;
	Vector<int>alice1 = bchEncoder.add_0bits(x, blocknum, k);

	/////encoder
	Vector<int>A(blocknum*n);
	Vector<int>msg(k);
	Vector<int>c(n);
	for (int i = 0; i < blocknum; i++)
	{
		for (int j = 0; j < k; j++)
			msg[j] = alice1[i*k + j];
		c = bchEncoder.bch_encode(msg, n, k);
		for (int j = 0; j < n; j++)
			A[i*n + j] = c[j];
	}

	return A;
}