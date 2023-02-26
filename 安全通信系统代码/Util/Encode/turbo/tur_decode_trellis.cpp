#include"tur_decoder.h"

Vector<double> Encode_bits(Matrix<double>G, double ak, Vector<double>&state)
{
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	Vector<double>c_state(k);
	Vector<double>encode_bits(n);
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

Vector<double> binary_state(int integer, int m)
{
	Vector<double>state(m);
	for (int i = 0; i < m; i++)
	{
		state[i] = integer % 2;
		integer /= 2;
	}
	return flip(state);

}

double int_state(Vector<double> state)
{
	int m = state.size();
	double int_state;
	Vector<double>vec(m);
	for (int i = 0; i < m; i++)
		vec[i] = pow(2, m - i - 1);
	int_state = dotProd(state, vec);

	return int_state;
}

void Trellis(Matrix<double>&next_out, Matrix<double>&next_state, Matrix<double>&last_out, Matrix<double>&last_state, Matrix<double>G)
{
	///////next_state:状态转移矩阵，
	//////////////////行数i=当前状态; 列数j=dk(0/1);
	////////////////////next_state(i,j)=下一状态（00,01,10,11）;
	//////last_state:状态转移矩阵，
	/////////////////行数i=当前状态; 列数j=dk(0/1);
	/////////////////last_state(i,j)=上一状态（00,01,10,11）;
	//////next_out:下一状态输入输出矩阵，
	///////////////行数i=当前状态;列数j=dk(0/1);[mk,ck1,mk,ck2];
	///////////////next_out(i,j)=mk or ck;
	//////last_out:上一状态输入输出矩阵，
	///////////////行数i=当前状态;列数j=dk(0/1);(s:[mk,ck1,mk,ck2])
	///////////////last_out(i,j)=mk or ck;
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	int nstates = pow(2, m);

	for (int i = 0; i < nstates; i++)
	{
		////////conver nstates to binary  0--00;1--01;2--10;3--11
		Vector<double>state(m);
		state = binary_state(i, m);

		////dk=0
		double dk0 = 0;
		double ak0;
		Vector<double>state0(state);
		Vector<double>_state0(m + 1);
		Vector<double>encode_bits0(n);//0:mk; 1:ck
		_state0[0] = dk0;
		for (int i = 0; i<m; i++)
			_state0[i + 1] = state0[i];
		ak0 = mod(dotProd(G.getRow(0), _state0), 2); //ak=G(0)*M
		encode_bits0 = Encode_bits(G, ak0, state0);
		double istate0 = int_state(state0);

		////dk=1
		double dk1 = 1;
		double ak1;
		Vector<double>state1(state);
		Vector<double>_state1(m + 1);
		Vector<double>encode_bits1(n);//0:mk; 1:ck
		_state1[0] = dk1;
		for (int i = 0; i<m; i++)
			_state1[i + 1] = state1[i];
		ak1 = mod(dotProd(G.getRow(0), _state1), 2); //ak=G(0)*M
		encode_bits1 = Encode_bits(G, ak1, state1);
		double istate1 = int_state(state1);

		double istate[2] = { istate0, istate1 };
		for (int j = 0; j < 2; j++)
		{
			next_out[i][j] = 2 * encode_bits0[j] - 1;
			next_out[i][j + 2] = 2 * encode_bits1[j] - 1;
			next_state[i][j] = istate[j];
		}
	}

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < nstates; i++)
		{
			last_state[(next_state[i][j])][j] = i;
			last_out[(next_state[i][j])][j * 2] = next_out[i][j * 2];
			last_out[(next_state[i][j])][j * 2 + 1] = next_out[i][j * 2 + 1];

		}
	}
}