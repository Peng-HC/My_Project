#include "tur_decoder.h"

Matrix<double> demultiplex(Vector<int>r, Vector<int>alpha, int puncture)
{
	int len = r.size();
	int t_len = len / (puncture + 2);
	Matrix<double>rsc_out(2, 2 * t_len);//rsc_out[0]=[mk,ck1]; rsc_out[1]=[interleave(mk),ck2]
	Vector<double>systembits(t_len);//mk

	Vector<int>ones(len,1);
	r=2*r-ones;

	if (puncture == 1)//1/3
	{
		for (int i = 0; i<t_len; i++)
		{
			systembits[i] = r[3 * i];
			rsc_out[0][2 * i + 1] = r[3 * i + 1];
			rsc_out[1][2 * i + 1] = r[3 * i + 2];
		}

	}
	else//1/2
	{
		for (int i = 0; i<t_len; i++)
		{
			systembits[i] = r[2 * i];
			if (mod(i, 2))
				rsc_out[1][2 * i + 1] = r[2 * i + 1];
			else
				rsc_out[0][2 * i + 1] = r[2 * i + 1];
		}
	}

	for (int i = 0; i<t_len; i++)
	{
		rsc_out[0][2 * i] = systembits[i];
		rsc_out[1][2 * i] = systembits[alpha[i]];
	}
	return rsc_out;
}

int Sign(double A)
{
	int sign ;
	if (A > 0)
		sign = 1;
	else if (A == 0)
		sign = 0;
	else if (A < 0)
		sign = -1;
	return sign;
}

Vector<int>tur_decoder(Vector<int>bob_recieved,Vector<int>alpha)
{

	
	const int  n = 2;
	const int  k = 1;
	const int  m = 2;//ʹ�ã�2��1��2�������
	Matrix<double>G(n, m + 1);//���ɾ���
	int puncture = 0;//0:punctured;1:unpunctured
	double rate = 1 / (double)(2 + puncture);//����
	int L_total = bob_recieved.size() / (2 + puncture);
	int niter = 5;//��������
	int dec_alg = 0;
	int A = 1;//�ŵ�˥�����
	double SNR = 2.5;//�����-db
	double L_c = 4 * A*SNR*rate;//�ŵ���������


	////////////g=[07,05]
	double g0[m + 1] = { 1, 1, 1 };
	double g1[m + 1] = { 1, 0, 1 };
	Vector<double>G0(m + 1, g0);//��������ʽ
	Vector<double>G1(m + 1, g1);//���ɶ���ʽ
	G.setRow(G0, 0); G.setRow(G1, 1);

	Matrix<double>mk_ck(2, 2 * L_total);//depart bits to get #1[mk,ck1]; #2[interleave(mk),ck2];
	Matrix<double>rsc(2, 2 * L_total);//scale the received bits
	Vector<int>bob_temp(L_total);//bob����sequence


	//demultiplex
	mk_ck = demultiplex(bob_recieved, alpha, puncture);
	rsc = 0.5*L_c*mk_ck;//scale the received bits

	//decoder +1/-1
	Vector<double>L_e(L_total);/////��ʼ������Ϣ
	Vector<double> L_a(L_total);//������Ϣ
	Vector<double>L_all(L_total);//��Ȼ��
	for (int iter = 0; iter < niter; iter++)
	{
		///decoder #1
		for (int i = 0; i < L_total; i++)//#2�õ�������Ϣ��������֯��Ϊ#1��������Ϣ
			L_a[alpha[i]] = L_e[i];

		if (dec_alg == 0)
			L_all = logmap(rsc.getRow(0), G, L_a, 1);
		else
			L_all = sova(rsc.getRow(0), G, L_a, 1);
		Vector<double>yk1(L_total);
		for (int i = 0; i < L_total; i++)
			yk1[i] = 2 * rsc[0][2 * i];
		L_e = L_all - yk1 - L_a;

		////decoder #2
		for (int i = 0; i < L_total; i++)//#1�õ�������Ϣ��������֯��Ϊ#2��������Ϣ
			L_a[i] = L_e[alpha[i]];

		if (dec_alg == 0)
			L_all = logmap(rsc.getRow(1), G, L_a, 2);
		else
			L_all = sova(rsc.getRow(1), G, L_a, 2);
		Vector<double>yk2(L_total);
		for (int i = 0; i < L_total; i++)
			yk2[i] = 2 * rsc[1][2 * i];
		L_e = L_all - yk2 - L_a;

		//////estimate the information bits
		for (int i = 0; i < L_total; i++)
			bob_temp[alpha[i]] = (Sign(L_all[i]) + 1) / 2;

	}

	Vector<int>bob(L_total - m, bob_temp);
	return bob;
}