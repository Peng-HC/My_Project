#include "tur_decoder.h"


Vector<double>sova(Vector<double>rsc_out, Matrix<double>G, Vector<double>L_a, int index_dec)
{
	int L_total = rsc_out.size() / 2;
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	int nstates = pow(2, m);
	double infty = pow(10, 10);
	int delta = 30;//SOVA window size

	Matrix<double>next_out(nstates, 4);
	Matrix<double>last_out(nstates, 4);
	Matrix<double>next_state(nstates, 2);
	Matrix<double>last_state(nstates, 2);
	Trellis(next_out, next_state, last_out, last_state, G);///set up the trellis

	Matrix<double>path_matrix(nstates, 1 + L_total, -infty);//初始化路径矩阵
	Matrix<double>Mdiff(nstates, 1 + L_total);//度量差
	Matrix<double>prev_bit(nstates, 1 + L_total);//mk
	Vector<double>mlstate(1 + L_total);
	Vector<double>est(L_total);
	Vector<double>L_all(L_total);//似然比

	/////trace forward to compute all the path matrix
	path_matrix[0][0] = 0;
	for (int i = 0; i < L_total; i++)
	{
		Vector<double>y(2);
		y[0] = rsc_out[2 * i]; y[1] = rsc_out[2 * i + 1];
		for (int j = 0; j < nstates; j++)
		{
			Vector<double>sym0(2);
			Vector<double>sym1(2);
			double state0;
			double state1;
			double MK0;//mk=0的度量概率
			double MK1;//mk=1的度量概率
			double d;
			sym0[0] = last_out[j][0]; sym0[1] = last_out[j][1];
			sym1[0] = last_out[j][2]; sym1[1] = last_out[j][3];
			state0 = last_state[j][0];
			state1 = last_state[j][1];
			MK0 = (dotProd(y, sym0) - L_a[i] / 2 + path_matrix[state0][i]);
			MK1 = (dotProd(y, sym1) + L_a[i] / 2 + path_matrix[state1][i]);
			d = dotProd(y, sym0) - dotProd(y, sym1) - L_a[i];
			if (path_matrix[state0][i] == path_matrix[state1][i])
			{

				if (d>0)
				{
					path_matrix[j][i + 1] = MK0;
					Mdiff[j][i + 1] = d;
					prev_bit[j][i + 1] = 0;
				}
				else
				{
					path_matrix[j][i + 1] = MK1;
					Mdiff[j][i + 1] = -d;
					prev_bit[j][i + 1] = 1;
				}
			}
			else
			{
				if (MK0>MK1)
				{
					path_matrix[j][i + 1] = MK0;
					Mdiff[j][i + 1] = d + path_matrix[state0][i] - path_matrix[state1][i];
					prev_bit[j][i + 1] = 0;
				}
				else
				{
					path_matrix[j][i + 1] = MK1;
					Mdiff[j][i + 1] = -d + path_matrix[state1][i] - path_matrix[state0][i];
					prev_bit[j][i + 1] = 1;
				}
			}

		}

	}

	////////decoder#1,trace back from all 0 state
	////////decoder#2,trace back from the most likely state
	if (index_dec == 1)
		mlstate[L_total] = 0;
	else
	{
		double max_num;
		for (int i = 0; i < nstates; i++)
		{
			if (path_matrix.getColumn(L_total)[i] == max(path_matrix.getColumn(L_total)))
			{
				max_num = i;
				break;
			}
		}
		mlstate[L_total] = max_num;
	}
	for (int t = L_total - 1; t >= 0; t--)
	{
		est[t] = prev_bit[mlstate[t + 1]][t + 1];
		mlstate[t] = last_state[mlstate[t + 1]][(int)est[t]];
	}

	for (int t = 0; t < L_total; t++)
	{
		double llr = infty;
		for (int i = 0; i <= delta; i++)
		{
			if (t + i < L_total)
			{
				double bit = 1 - est[t + i];
				double temp_state = last_state[mlstate[t + i + 1]][(int)bit];
				for (int j = i - 1; j >= 0; j--)
				{
					bit = prev_bit[(int)temp_state][t + j + 1];
					temp_state = last_state[(int)temp_state][(int)bit];
				}
				if (bit != est[t])
					llr = min(llr, Mdiff[mlstate[t + i + 1]][t + i + 1]);
			}
		}
		L_all[t] = (2 * est[t] - 1)*llr;
	}
	return L_all;
}
