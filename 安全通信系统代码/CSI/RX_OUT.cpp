#include <Config/Config.h>
#include "header.h"

int sine_frequency;

Matrix<complex<float>> RX_OUT(const char* argv)
{
    sine_frequency = Config::sine_freq;

	const char Output_datamode='C';
    const char seq_mode_time='M';
	const char seq_mode_frequency='Z';
	const char rx_power_normalization='N';

	Vector<complex<float>> res;
	Vector<complex<float>> Received_symbols;
	Vector<complex<float>> Process_final_symbol;
	Vector<Matrix<complex<float>>> Y;
    Vector<complex<float>> DMRS_sequence;
	Matrix<complex<float>> Y_OUT;

	//res=Intialiazation(Output_datamode);
    res= Intialiazation_V2X(Output_datamode);

//	Received_symbols=Rx_ReadData(argv,Output_datamode,rx_power_normalization);
    //
//	Process_final_symbol=Synchronization (Output_datamode,seq_mode_time,Received_symbols,res);
//    DMRS_sequence= Rx_Demodulation(Received_symbols);

//    Y=Getsymbols(Output_datamode,Process_final_symbol);//生成matrix[0]存储导频信号，matrix[1]存储数据信号
//	Y_OUT=Rx_channel_estimation(Output_datamode,seq_mode_frequency,seq_mode_time,Y);
  //  Y_OUT=Rx_channel_estimation_V2X(DMRS_sequence);
	return Y_OUT;	
}

