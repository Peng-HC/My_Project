#include <Config/Config.h>
#include "header.h"


int transmitted_symbol_length;
int syn_sine_frequency_index;

int time_sequence_length=4095;
int Trans_ofdm_data_symbols=1;
int subcarrier_num=512;
int CP_length_frequency=128;
int Trans_ofdm_polit_symbols=1;
int Trans_time_polit_symbols=1;	
int CP_length_time=129;
int sine_symbol_length=1504;
//int process_total_length=20000;
int Trans_symbol_repeation=10;
int rx_fft_synchronization_window=128;
extern int sine_frequency;
int process_total_length;

int PSSCH_symbol_len=30720;
int PSSCH_fft_offset=424;
int v2x_fft_size=2048;
int subcarrier_num_v2x=1152;
int cplen0=160;
int cplenR=144;
int movestep=30;
Vector<complex<float>> Intialiazation(const char Output_datamode)
{
	int total_transmit_ofdm_symbol_length;
	
	switch (Output_datamode)
	{
	case 'C':
		total_transmit_ofdm_symbol_length=Trans_ofdm_data_symbols*(subcarrier_num+CP_length_frequency)+Trans_ofdm_polit_symbols*(subcarrier_num+CP_length_frequency);
		break;
	case 'R':
		total_transmit_ofdm_symbol_length=Trans_ofdm_data_symbols*(2*subcarrier_num+CP_length_frequency)+Trans_ofdm_polit_symbols*(subcarrier_num+CP_length_frequency);
	   break;
	}

	transmitted_symbol_length=total_transmit_ofdm_symbol_length+Trans_time_polit_symbols*(time_sequence_length+CP_length_time)+2*sine_symbol_length;
	process_total_length=transmitted_symbol_length;
	Vector<complex<float>> Process_final_symbol(process_total_length);
	Vector<complex<float>> Process_final_transmit_symbol(process_total_length*Trans_symbol_repeation);
	syn_sine_frequency_index=round(rx_fft_synchronization_window-rx_fft_synchronization_window/sine_frequency+1);

	return Process_final_symbol;

}
Vector<complex<float>> Intialiazation_V2X(const char Output_datamode){
    std::cout<<"start to Intialiazation_V2X"<<std::endl;
    int total_transmit_PSSCH_symbol_length;
    switch (Output_datamode)
    {
        case 'C':
            total_transmit_PSSCH_symbol_length=PSSCH_symbol_len;
            break;
        case 'R':
            total_transmit_PSSCH_symbol_length=PSSCH_symbol_len;
            break;
    }
    process_total_length=total_transmit_PSSCH_symbol_length;
    Vector<complex<float>> Process_final_symbol(process_total_length);

    return Process_final_symbol;
}
