#include"tur_decoder.h"

Vector<double> logmap(Vector<double>rsc_out, Matrix<double>G, Vector<double>L_a, int index_dec)
{
	/////////la:先验概率 index_dec:decoder的标号

	int L_total = rsc_out.size() / 2;
	int n = G.rows();
	int k = G.cols();//约束长度
	int m = k - 1;//寄存器个数
	int nstates = pow(2, m);
	double infty = pow(10, 10);

	Matrix<double>next_out(nstates, 4);
	Matrix<double>last_out(nstates, 4);
	Matrix<double>next_state(nstates, 2);
	Matrix<double>last_state(nstates, 2);

	Trellis(next_out, next_state, last_out, last_state, G);///set up the trellis

	Matrix<double>Alpha(L_total + 1, nstates);//前向递推概率ak(s)
	Matrix<double>Beta(L_total, nstates);//后向递推概率Bk(s)
	Vector<double>gamma(nstates);//转移概率r(s',s)
	Vector<double>L_all(L_total);//似然比
	Vector<double>tempmax(L_total + 1);
	Vector<double>ones(nstates, 1);


	/////initializion of Apha/ Beta
	Alpha.setRow(-infty*ones, 0);
	Alpha[0][0] = 0;

	if (index_dec == 1)
		Beta.setRow(Alpha.getRow(0), L_total - 1);
	else if (index_dec == 2)
		Beta.setRow(0, L_total - 1);
	else
		cout << "index_dec is limited to 1 an 2" << endl;

	//////trace forward,compute Alpha
	Vector<double>row_a(nstates);
	for (int i = 1; i <L_total + 1; i++)
	{
		for (int j = 0; j < nstates; j++)
		{
			gamma = -infty*ones;
			gamma[last_state[j][0]] = -rsc_out[2 * i - 2] + rsc_out[2 * i - 1] * last_out[j][1] - log(1 + exp(L_a[i - 1]));
			gamma[last_state[j][1]] = rsc_out[2 * i - 2] + rsc_out[2 * i - 1] * last_out[j][3] + L_a[i - 1] - log(1 + exp(L_a[i - 1]));
			if (sum(exp(gamma + Alpha.getRow(i - 1)))<pow(10, -300))
				Alpha[i][j] = -infty;
			else
				Alpha[i][j] = log(sum(exp(gamma + Alpha.getRow(i - 1))));
		}

		tempmax[i] = max(Alpha.getRow(i));
		for (int j = 0; j < nstates; j++)
			Alpha[i][j] = Alpha[i][j] - tempmax[i];
	}


	//////trace backward,compute Beta
	for (int i = L_total - 2; i >= 0; i--)
	{
		for (int j = 0; j < nstates; j++)
		{
			gamma = -infty*ones;
			gamma[next_state[j][0]] = (-rsc_out[2 * i + 2] + rsc_out[2 * i + 3] * next_out[j][1]) - log(1 + exp(L_a[i + 1]));
			gamma[next_state[j][1]] = (rsc_out[2 * i + 2] + rsc_out[2 * i + 3] * next_out[j][3]) + L_a[i + 1] - log(1 + exp(L_a[i + 1]));
			if (sum(exp(gamma + Beta.getRow(i + 1))) < pow(10, -300))
				Beta[i][j] = -infty;
			else
				Beta[i][j] = log(sum(exp(gamma + Beta.getRow(i + 1))));
	
		}

		for (int j = 0; j < nstates; j++)
			Beta[i][j] = Beta[i][j] - tempmax[i + 1];
	}



	//////soft output,log-likelihood ratio of symbols in the frame
	for (int i = 0; i <L_total; i++)
	{
		Vector<double>temp0(nstates);
		Vector<double>temp1(nstates);
		for (int j = 0; j < nstates; j++)
		{
			double gamma0 = -rsc_out[2 * i] + rsc_out[2 * i + 1] * last_out[j][1] - log(1 + exp(L_a[i]));
			double gamma1 = rsc_out[2 * i] + rsc_out[2 * i + 1] * last_out[j][3] + L_a[i] - log(1 + exp(L_a[i]));
			int k0 = last_state[j][0];
			int k1 = last_state[j][1];
			temp0[j] = exp(gamma0 + Alpha[i][k0] + Beta[i][j]);
			temp1[j] = exp(gamma1 + Alpha[i][k1] + Beta[i][j]);
		}
		L_all[i] = log(sum(temp1)) - log(sum(temp0));
	}

	return L_all;

}