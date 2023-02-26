#include "header.h"


extern int time_sequence_length;//time_sequence_length=4095;
extern int CP_length_time;//CP_length_time=129;
extern int subcarrier_num;//subcarrier_num=512;
extern int Trans_ofdm_polit_symbols;//Trans_ofdm_polit_symbols=1;
extern int CP_length_frequency;//int CP_length_frequency=128;
extern int Trans_ofdm_data_symbols;//Trans_ofdm_data_symbols=1;
extern int Trans_time_polit_symbols;//Trans_time_polit_symbols=1
extern int sine_symbol_length;//sine_symbol_length=1504;

int rx_get_data_syn_offset=-12;

Matrix<complex<float>>rx_ofdm_pilot_symbol;

Matrix<complex<float>>rx_time_seq_symbol(time_sequence_length,Trans_time_polit_symbols);

Vector<Matrix<complex<float>>> Getsymbols(const char Output_datamode,Vector<complex<float>>Process_final_symbol)
{
	int temp1;
	int temp2;
	Vector<complex <float>>rx_ofdm_data_symbol;

	temp1=sine_symbol_length+rx_get_data_syn_offset;//1504-12
	for(int n=0;n<Trans_time_polit_symbols;n++)
	{
		temp1=temp1+CP_length_time;
		for(int m=0;m<time_sequence_length;m++)
			rx_time_seq_symbol[m][n]=Process_final_symbol[temp1+m];		
		temp1=temp1+time_sequence_length;
	}

	switch (Output_datamode)
	{
		case 'R':
///////////////Ƶ����������������
			rx_ofdm_pilot_symbol.resize(2*subcarrier_num,Trans_ofdm_polit_symbols);

////////////////////////////////////
			for(int n=0;n<Trans_ofdm_polit_symbols;n++)
			{
				temp1=temp1+CP_length_frequency;
		
				for(int m=0;m<2*subcarrier_num;m++)
					rx_ofdm_pilot_symbol[m][n]=Process_final_symbol[temp1+m];
				temp1=temp1+2*subcarrier_num;
			}

			temp2=(2*subcarrier_num+CP_length_frequency)*Trans_ofdm_data_symbols;
			rx_ofdm_data_symbol.resize(temp2);
			for(int i=0;i<temp2;i++)
				rx_ofdm_data_symbol[i]=Process_final_symbol[temp1+i];
			temp1=temp1+temp2;
			break;


		case 'C':
///////////////Ƶ����������������
			rx_ofdm_pilot_symbol.resize(subcarrier_num,Trans_ofdm_polit_symbols);

////////////////////////////////////
			for(int n=0;n<Trans_ofdm_polit_symbols;n++)
			{
				temp1=temp1+CP_length_frequency;
				for(int m=0;m<subcarrier_num;m++)
					rx_ofdm_pilot_symbol[m][n]=Process_final_symbol[temp1+m];
				temp1=temp1+subcarrier_num;
			}
			temp2=(subcarrier_num+CP_length_frequency)*Trans_ofdm_data_symbols;
			rx_ofdm_data_symbol.resize(temp2);
			for(int i=0;i<temp2;i++)
				rx_ofdm_data_symbol[i]=Process_final_symbol[temp1+i];
			temp1=temp1+temp2;
			break;
	}

	Vector<Matrix<complex<float>>> Y;
	Y.resize(2);
	Y[0]=rx_ofdm_pilot_symbol;
	Y[1]=rx_time_seq_symbol;
	return Y;

}