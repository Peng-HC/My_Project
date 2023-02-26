#ifndef _HEADER_H_
#define _HEADER_H_



//#include<math.h>
#include<iostream>
#include<iomanip>
#include<complex>
#include <utilities.h>
#include <vector.h>
#include <matrix.h>
#include <fft.h>
#include <timing.h>
#include <vector>
#include <unistd.h>
using namespace std;
using namespace splab;

#define PI 3.1416
#define MAX_N 8192

#define MAX_PATH 1000
extern Vector<complex<float>>generate_SEQtime(int time_sequence_length,const char seq_mode_time);
extern Vector<complex<float>>generate_SEQfrequency(int pilot_subcarriernum,const char seq_mode_frequency);
extern int round(float r);
extern complex<float> complex_divide(complex<float>A ,complex<float>B);
extern Vector<complex<float>> xcorr(Vector<complex<float>>a,Vector<complex<float>> b);
extern Vector<complex<float>> OFDM_SNR_Estimation(Matrix<complex<float>>OFDM_IN,int row,int column);
extern Vector<complex<float>> resample(Vector<complex<float>> X, int p,int q);
extern Vector<complex<float>> smooth(splab::Vector<complex<float>> &hls,int movestep,int size);

extern vector<float> xcorr_delay(vector<float>a,vector<float> b);
extern vector<float> winner_filter(int m, vector<float>x,vector<float>y);
extern Vector<complex<float>> Intialiazation(const char Output_datamode);
extern Vector<complex<float>> Intialiazation_V2X(const char Output_datamode);
extern Vector<complex<float>> Rx_ReadData(const char* filename,const char Output_datamode,const char rx_power_normalization);
extern Vector<complex<float>> Synchronization (const char Output_datamode,const char seq_mode_time,Vector<complex<float>> Received_symbols,Vector<complex<float>> Process_final_symbol);
extern Vector<Matrix<complex<float>>> Getsymbols(const char Output_datamode,Vector<complex<float>>Process_final_symbol);
extern Matrix<complex<float>> Rx_channel_estimation(const char Output_datamode,const char seq_mode_frequency,const char seq_mode_time,Vector<Matrix<complex<float>>>Y);
extern Matrix<complex<float>> RX_OUT(const char* argv);
extern Vector<complex<float>> RX_OUT_V2X(const char * argv);
extern Vector<complex<float>> Rx_Demodulation(Vector<complex<float>> Received_symbols);
extern Vector<complex<float>> Rx_channel_estimation_V2X(const splab::Vector<complex<float>> Y);

#endif