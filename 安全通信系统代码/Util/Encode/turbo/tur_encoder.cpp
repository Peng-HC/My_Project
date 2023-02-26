#include "tur_encoder.h"
#include "element.h"
#define N 999

bool compare(element&a, element&b)
{
	return a.data < b.data;
}

Vector<int>Sort(Vector<double>temp)
{
	element X[3000];
	int len = temp.size();
	Vector<int>index(len);
	for (int i = 0; i < len; i++)
	{
		X[i].data = temp[i];
		X[i].index = i;
	}
	sort(X, X + len, compare);
	for (int i = 0; i < len; i++)
		index[i] = X[i].index;;
	return index;
}

Vector<int> Encode_bits(Matrix<int>G, int ak, Vector<int>&state)
{
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	Vector<int>c_state(k);
	Vector<int>encode_bits(n);
	//////encode_bits=[G(0);G(1)]*[ak,M(k-1:k-L)]
	c_state[0] = ak;
	for (int i = 0; i<m; i++)
		c_state[i + 1] = state[i];
	for (int i = 0; i<n; i++)
		encode_bits[i] = mod(dotProd(G.getRow(i), c_state), 2);

	for (int i = 0; i < m; i++)
		state[i] = c_state[i];
	return encode_bits;

}

Vector<int> rsc_encode(Matrix<int>G, Vector<int>x, int terminated)
{
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	int L_info;
	int L_total;
	if (terminated>0)
	{
		L_info = x.size();
		L_total = L_info + m;
	}
	else
	{
		L_total = x.size();
		L_info = L_total - m;

	}

	Vector<int>state(m);//寄存器D1.D2
	Vector<int>_state(k);//寄存器结果mk,mk-1,mk-2
	int dk;//当前时刻传入的比特mk
	int ak;//反馈输出，即信息位
	Vector<int>encode_bits(n);//当前输出比特 encode_bits[0]:mk;  encode_bits[1]:parity bits
	Vector<int>output_bits(n*L_total);

	for (int i = 0; i<L_total; i++)
	{
		if ((terminated<0) || (terminated>0 && i<L_info))
			dk = x[i];
		else if (terminated>0 && i >= L_info)
		{
			Vector<int>g0(m);
			for (int i = 0; i<m; i++)
				g0[i] = G[0][i + 1];
			dk = mod(dotProd(g0, state), 2);
		}
		_state[0] = dk;  //mk
		for (int i = 0; i<m; i++)
			_state[i + 1] = state[i];
		ak = mod(dotProd(G.getRow(0), _state), 2); //ak=G(0)*M
		encode_bits = Encode_bits(G, ak, state);
		encode_bits[0] = dk;
		for (int j = 0; j<n; j++)
			output_bits[n*i + j] = encode_bits[j];
	}

	return output_bits;
}

Vector<int> encoderm(Vector<int>x, Matrix<int>G, Vector<int>alpha, int puncture)
{
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	int L_info = x.size();
	int L_total = L_info + m;

	Vector<int>input(x);//输入1#编码器序列mk
	Vector<int>input2(L_total);//输入2#编码器序列 交织后的interleave(mk')
	Vector<int>output1(n*L_total);//1#编码后序列[mk',ck1]
	Vector<int>output2(n*L_total);//2#编码后序列[interleave(mk'),ck2]
	Matrix<int>s_output(3, L_total);//s_output[0]:mk'; s_output[1]:ck1; s_output[2]:ck2
	Vector<int>encoderm_out;

	////////////////////////////////1#
	output1 = rsc_encode(G, input, 1);
	//序列分离 mk'和ck1
	for (int j = 0; j<L_total; j++)
	{
		s_output[0][j] = output1[2 * j];
		s_output[1][j] = output1[2 * j + 1];
	}

	//////////////////////////////////2#
	///interleave input to second rsc_endcoder
	for (int i = 0; i<L_total; i++)
		input2[i] = s_output[0][alpha[i]];
	output2 = rsc_encode(G, input2, -1);
	//序列分离,得到ck2
	for (int j = 0; j<L_total; j++)
		s_output[2][j] = output2[2 * j + 1];

	////////////////////////////////////删余和复用单元
	//puncture=0: rate=1/2
	//puncture=1: rate=1/3

	if (puncture>0) //rate=1/3
	{
		encoderm_out.resize(3 * L_total);
		for (int i = 0; i<L_total; i++)
		{
			for (int j = 0; j<3; j++)
				encoderm_out[3 * i + j] = s_output[j][i];
		}

	}
	else   //rate=1/2
	{
		encoderm_out.resize(2 * L_total);
		for (int i = 0; i<L_total; i++)
		{
			encoderm_out[2 * i] = s_output[0][i];
			//odd check bits from RSC1
			if (!mod(i, 2))
				encoderm_out[2 * i + 1] = s_output[1][i];
			//even check bits from RSC1
			else
				encoderm_out[2 * i + 1] = s_output[2][i];
		}
	}

	return encoderm_out;
}

Vector<int>tur_encoder(Vector<int>alice,Vector<int>&alpha)
{
	const int  n = 2;
	const int  k = 1;
	const int  m = 2;//使用（2，1，2）卷积码
	Matrix<int>G(n, m + 1);//生成矩阵
	int nstates = pow(2, m);//状态个数
	int puncture = 0;//0:punctured;1:unpunctured
	double rate = 1 / (double)(2 + puncture);//码率

	////////////g=[07,05]
	int g0[m + 1] = { 1, 1, 1 };
	int g1[m + 1] = { 1, 0, 1 };
	Vector<int> G0(m + 1, g0);//反馈多项式
	Vector<int> G1(m + 1, g1);//生成多项式
	G.setRow(G0, 0); G.setRow(G1, 1);

	int L_total = alice.size() + m;
	//Vector<int>alpha(L_total);//交织器
	Vector<int> encode_out((puncture + 2)*L_total);

	
	//随机交织映射	
	srand(time(0));
	Vector<double> temp(L_total);
	alpha.resize(L_total);
	for (int i = 0; i < L_total; i++)
		temp[i] = rand() % N + 1;
	alpha = Sort(temp);

	//encoder -1/+1
	encode_out = encoderm(alice, G, alpha, puncture);

	return encode_out;

}