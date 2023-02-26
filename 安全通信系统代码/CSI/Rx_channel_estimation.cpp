#include "header.h"

int pilot_subcarrier_start=1;
int circle_shift_num=256;
int rx_fre_moving_average_length=5;
int est_channel_length=32;

extern int Trans_ofdm_polit_symbols;//Trans_ofdm_polit_symbols=1;
extern int subcarrier_num;//subcarrier_num=512;
extern int pilot_subcarrier_num;//pilot_subcarrier_num=511;
extern int time_sequence_length;//time_sequence_length=4095;
extern int Trans_time_polit_symbols;//Trans_time_polit_symbols=1;
Matrix<complex<float>> Rx_channel_estimation(const char Output_datamode,const char seq_mode_frequency,const char seq_mode_time,Vector<Matrix<complex<float>>>Y)
{

    Matrix<complex<float>>rx_ofdm_pilot_symbol=Y[0];
	Matrix<complex<float>>rx_time_seq_symbol=Y[1];

	Matrix<complex <float>> rx_freq_est(pilot_subcarrier_num,Trans_ofdm_polit_symbols+1);
	Matrix<complex <float>> rx_timeseq_est(time_sequence_length,Trans_time_polit_symbols+1);
	Matrix<complex <float>> rx_timeseq_est_tmp(est_channel_length,Trans_time_polit_symbols+1);
	Matrix<complex <float>>rx_timefre_est(subcarrier_num,Trans_time_polit_symbols+1);
	Matrix<complex <float>> rx_timefre_est_tmp(subcarrier_num,Trans_time_polit_symbols+1);

	

////////////////////get frequency pilot channel estimation///////////////////////
	

	Vector<complex<float>>SEQ_frequency=generate_SEQfrequency(pilot_subcarrier_num,seq_mode_frequency);
	switch (Output_datamode)
	{
	case 'R':
		for(int n=0;n<Trans_ofdm_polit_symbols;n++)
		{
			Vector<complex<float>>fft_in(rx_ofdm_pilot_symbol.getColumn(n));
			Vector<complex<float>>fft_out=fft(fft_in)/complex<float>(sqrt(2*subcarrier_num),0);
			rx_ofdm_pilot_symbol.setColumn(fft_out,n);
			
			for(int m=0;m<pilot_subcarrier_num; m++)
			{
				rx_freq_est[m][n]=rx_ofdm_pilot_symbol[m+pilot_subcarrier_start][n];
				rx_freq_est[m][n]=complex_divide(rx_freq_est[m][n],SEQ_frequency[m]);
			}

		}break;
				
				
	case 'C':
		for(int n=0;n<Trans_ofdm_polit_symbols;n++)
		{
			Vector<complex<float>>fft_in(rx_ofdm_pilot_symbol.getColumn(n));
			Vector<complex<float>>fft_out=fft(fft_in)/complex<float>(sqrt(subcarrier_num),0);

			fft_out=circshift(fft_out,-circle_shift_num);
			rx_ofdm_pilot_symbol.setColumn(fft_out,n);
			for(int m=0;m<pilot_subcarrier_num; m++)
			{
				rx_freq_est[m][n]=rx_ofdm_pilot_symbol[m+pilot_subcarrier_start][n];
				rx_freq_est[m][n]=complex_divide(rx_freq_est[m][n],SEQ_frequency[m]);//矩阵除法，频偏估计
			}

		}break;
	}

////////////////////////////////////////////////////////////////////////////////////////�������һ��
	for(int m=0;m<pilot_subcarrier_num; m++)
	{
		for(int n=0;n<Trans_ofdm_polit_symbols;n++)
			rx_freq_est[m][Trans_ofdm_polit_symbols]=rx_freq_est[m][Trans_ofdm_polit_symbols]+rx_freq_est[m][n];
		rx_freq_est[m][Trans_ofdm_polit_symbols]=complex_divide(rx_freq_est[m][Trans_ofdm_polit_symbols],Trans_ofdm_polit_symbols);
	}


/////////////////////get time sequence channel estimation///////////////////////////



	for(int n=0;n<Trans_time_polit_symbols;n++)
	{
		Vector<complex<float>>temp_1(rx_time_seq_symbol.getColumn(n));
		Vector<complex<float>>SEQ_time=generate_SEQtime(time_sequence_length,seq_mode_time);
		temp_1=xcorr(temp_1,SEQ_time);
		Matrix <complex<float>> time_seq_Q_inv(est_channel_length,est_channel_length);
		complex<float> time_seq_A=1+(est_channel_length-1)/(float)(time_sequence_length*time_sequence_length+2*time_sequence_length-time_sequence_length*est_channel_length-est_channel_length+1);
		complex<float> time_seq_B=(time_sequence_length)/(float)(time_sequence_length*time_sequence_length+2*time_sequence_length-time_sequence_length*est_channel_length-est_channel_length+1);
		Matrix <complex<float>> one(est_channel_length,est_channel_length,1);
		Vector<complex<float>>x(est_channel_length,1);
		Matrix<complex<float>>A=diag(x);
		time_seq_Q_inv=time_seq_B*(one-A )+A*time_seq_A;
		Vector <complex<float>> temp_2(est_channel_length);
		Vector <complex<float>> tmp(est_channel_length,temp_1);
		temp_2=time_seq_Q_inv*tmp;
		for(int i=0;i<est_channel_length;i++)
			temp_2[i]=conj(temp_2[i]);
		rx_timeseq_est.setColumn(temp_1,n);
		rx_timeseq_est_tmp.setColumn(temp_2,n);

		Vector<complex<float>> temp_1a(time_sequence_length+1);
		for(int m=0;m<time_sequence_length;m++)
			temp_1a[m]=rx_timeseq_est[m][n];
		temp_1a=fft(temp_1a);
		int temp_2a=round((time_sequence_length+1)/subcarrier_num);
		Vector<complex<float>>temp_1a_out=resample(temp_1a,1,temp_2a);	
		temp_1a_out=circshift(temp_1a_out,circle_shift_num);
		rx_timefre_est.setColumn(temp_1a_out,n);
	
		Vector<complex<float>>temp_1b(subcarrier_num);
		for(int m=0;m<est_channel_length;m++)
			temp_1b[m]=rx_timeseq_est_tmp[m][n];
		temp_1b=fft(temp_1b);
		temp_1b=flip(temp_1b);
		temp_1b=circshift(temp_1b,circle_shift_num);
		rx_timefre_est_tmp.setColumn(temp_1b,n);


	}
///////////////////////////////////////////////////////////////////////////////////////////////�������һ��
	for(int m=0;m<time_sequence_length; m++)
	{
		for(int n=0;n<Trans_time_polit_symbols;n++)
			rx_timeseq_est[m][Trans_time_polit_symbols]+=rx_timeseq_est[m][n];
		rx_timeseq_est[m][Trans_time_polit_symbols]=complex_divide(rx_timeseq_est[m][Trans_time_polit_symbols],Trans_time_polit_symbols);
	}

	for(int m=0;m<est_channel_length; m++)
	{
		for(int n=0;n<Trans_time_polit_symbols;n++)
			rx_timeseq_est_tmp[m][Trans_time_polit_symbols]+=rx_timeseq_est_tmp[m][n];
		rx_timeseq_est_tmp[m][Trans_time_polit_symbols]=complex_divide(rx_timeseq_est_tmp[m][Trans_time_polit_symbols],Trans_time_polit_symbols);

	}

	for(int m=0;m<pilot_subcarrier_num; m++)
	{
		for(int n=0;n<Trans_time_polit_symbols;n++)
			rx_timefre_est[m][Trans_time_polit_symbols]+=rx_timefre_est[m][n];
		rx_timefre_est[m][Trans_time_polit_symbols]=complex_divide(rx_timefre_est[m][Trans_time_polit_symbols],Trans_time_polit_symbols);
	}

	for(int m=0;m<pilot_subcarrier_num; m++)
	{
		for(int n=0;n<Trans_time_polit_symbols;n++)
			rx_timefre_est_tmp[m][Trans_time_polit_symbols]+=rx_timefre_est_tmp[m][n];
		rx_timefre_est_tmp[m][Trans_time_polit_symbols]=complex_divide(rx_timefre_est_tmp[m][Trans_time_polit_symbols],Trans_time_polit_symbols);
	}

/////////////////////////////////////result processing///////////////////////////////////////////////////////////////
	
	int row=floor(subcarrier_num/2);
	Vector<complex<float>> SNROUT(pilot_subcarrier_num);
	Vector<complex<float>> SNROUT_pilot(pilot_subcarrier_num);
	Vector<complex<float>> rx_freq_est_pilot(pilot_subcarrier_num);
	Vector<complex<float>> rx_timefre_est_pilot(pilot_subcarrier_num);
	Vector<complex<float>> rx_timefre_est_tmp_pilot(pilot_subcarrier_num);
	

	rx_freq_est[row][Trans_ofdm_polit_symbols]=complex_divide(rx_freq_est[row-1][Trans_ofdm_polit_symbols]+rx_freq_est[row+1][Trans_ofdm_polit_symbols],2);
	rx_timefre_est[row+1][Trans_ofdm_polit_symbols]=complex_divide(rx_timefre_est[row][Trans_ofdm_polit_symbols]+rx_timefre_est[row+2][Trans_ofdm_polit_symbols],2);

	SNROUT=OFDM_SNR_Estimation(rx_freq_est,pilot_subcarrier_num,Trans_ofdm_polit_symbols+1);

	for(int n=0;n<(pilot_subcarrier_num-rx_fre_moving_average_length);n++)
	{
		complex<float>tmp1(0,0);
		complex<float>tmp2(0,0);
		complex<float>tmp3(0,0);
		complex<float>tmp4(0,0);
		
		for(int m=0;m<rx_fre_moving_average_length;m++)
		{
			tmp1+=rx_freq_est[m+n][Trans_ofdm_polit_symbols];
			tmp2+=rx_timefre_est[m+n][Trans_time_polit_symbols];
			tmp3+=rx_timefre_est_tmp[m+n][Trans_time_polit_symbols];
			tmp4+=SNROUT[m+n];
		}
		
		tmp1=complex_divide(tmp1,rx_fre_moving_average_length);
		tmp2=complex_divide(tmp2,rx_fre_moving_average_length);
		tmp3=complex_divide(tmp3,rx_fre_moving_average_length);
		tmp4=complex_divide(tmp4,rx_fre_moving_average_length);
		
		rx_freq_est_pilot[n]=tmp1;
		rx_timefre_est_pilot[n]=tmp2;
		rx_timefre_est_tmp_pilot[n]=tmp3;
		SNROUT_pilot[n]=tmp4;
	}
	for(int n=pilot_subcarrier_num-rx_fre_moving_average_length;n<pilot_subcarrier_num;n++)
	{
		rx_freq_est_pilot[n]=rx_freq_est[n][Trans_ofdm_polit_symbols];
		rx_timefre_est_pilot[n]=rx_timefre_est[n][Trans_ofdm_polit_symbols];
		rx_timefre_est_tmp_pilot[n]=rx_timefre_est_tmp[n][Trans_ofdm_polit_symbols];
		SNROUT_pilot[n]=SNROUT[n];

	}
	Matrix<complex<float>> Y_OUT(pilot_subcarrier_num,3);
	Y_OUT.setColumn(rx_freq_est_pilot,0);
	Y_OUT.setColumn(rx_timefre_est_pilot,1);
	Y_OUT.setColumn(rx_timefre_est_tmp_pilot,2);
	return Y_OUT;

}