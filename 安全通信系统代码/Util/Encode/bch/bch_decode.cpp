#include"BchDecoder.h"

Vector<int> bch_decoder(Vector<int>y, int mes_len)
{
	/////get nkt
	int m = 4;//m=2~20
	int t = 2;
	int n = pow(2, m) - 1;
	int k;
	int d = 2 * t + 1;

	BchDecoder bchDecoder;
	bchDecoder.get_px(m);
	bchDecoder.generate_gf(n, m);
	k = bchDecoder.generate_gx(n, m, t);

	/////decode
	int blocknum = y.size() / n;
	Vector<int>x(blocknum*k);
	Vector<int>msg_decode(n);
	Vector<int>c(n);
	for (int i = 0; i < blocknum; i++)
	{
		for (int j = 0; j < n; j++)
			c[j] = y[i*n + j];
		msg_decode = bchDecoder.bch_decode(c, n, k, t);
		for (int j = 0; j < k; j++)
			x[i*k + j] = msg_decode[j];
	}

	Vector<int>x_decode(mes_len, x);
	return x_decode;
}