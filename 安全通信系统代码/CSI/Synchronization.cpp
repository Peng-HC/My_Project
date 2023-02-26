#include <cstring>
#include "header.h"

extern int Received_symbols_length_total;
extern int transmitted_symbol_length;
extern int syn_sine_frequency_index;
extern int time_sequence_length;
extern int rx_fft_synchronization_window;
extern int CP_length_time;
extern int sine_symbol_length;
extern int sine_frequency;
extern int process_total_length;

int rx_PN_seq_synchronization_theo=2;
int rx_fre_corr_number=300;
int rx_frequency_estimation_iteration_max=3;
int rx_fre_corr_start=200; 
double system_samping_rate=25*pow(10.0,(int)6);
int rx_fft_synchronization_theo=8;
int rx_fft_synchronization_sine_self_theo=10000;

Vector<complex<float>> Synchronization (const char Output_datamode,const char seq_mode_time,Vector<complex<float>> Received_symbols,Vector<complex<float>> Process_final_symbol)
{
	float temp1;
	float temp2;
	float temp3;
	int get_data_index;
	int get_data_ok;

////////////////////////////////////////////////////////frame synchronization with FFT///////////////////////////////////////////
//	//fft_syn_temp1=new complex<float>[rx_fft_synchronization_window];
//	//memset(fft_syn_temp1,0,sizeof(float)*2*rx_fft_synchronization_window);
//	//fft_syn_temp2=new complex<float>[rx_fft_synchronization_window];
//	//memset(fft_syn_temp2,0,sizeof(float)*2*rx_fft_synchronization_window);
//	temp1=1;
//	get_data_index=sine_symbol_length;
//	while (temp1>0)
//	{
//
//		for(int i=0;i<rx_fft_synchronization_window;i++)
//		{
//			fft_syn_temp1.push_back(Received_symbols[i+get_data_index]);
//			//fft_syn_temp2.push_back(Received_symbols[i+get_data_index+transmitted_symbol_length-sine_symbol_length]);
//			fft_syn_temp2.push_back(Received_symbols[i]);
//			//sum1_temp3=sum1_temp3+real(fft_syn_temp1[i]);
//			//sum2_temp3=sum2_temp3+real(fft_syn_temp2[i]);
//		}
//		
//		get_data_index=get_data_index+rx_fft_synchronization_window;
//
//		/*if(get_data_index+rx_fft_synchronization_window+transmitted_symbol_length>Received_symbols_length_total)
//			temp1=0;*/
//
//		if(get_data_index+rx_fft_synchronization_window>Received_symbols_length_total)
//			temp1=0;
//
//		FFT(fft_syn_temp1,rx_fft_synchronization_window);
//		FFT(fft_syn_temp2,rx_fft_synchronization_window);
//
//		for(int i=0;i<rx_fft_synchronization_window;i++)
//		{
//			fft_syn_temp1[i]=abs(fft_syn_temp1[i]);
//			fft_syn_temp2[i]=abs(fft_syn_temp2[i]);
//		}
//
//		temp2=real(fft_syn_temp1[syn_sine_frequency_index-1]+fft_syn_temp1[syn_sine_frequency_index-2]+fft_syn_temp1[syn_sine_frequency_index]);
//		float sum1_temp3=0;
//		for(int i=0;i<rx_fft_synchronization_window;i++)
//			sum1_temp3=sum1_temp3+real(fft_syn_temp1[i]);
//		temp3=sum1_temp3/rx_fft_synchronization_window;
//		if( (temp2/temp3)>rx_fft_synchronization_theo&&temp2<rx_fft_synchronization_sine_self_theo )
//		{
//			temp2=real(fft_syn_temp2[syn_sine_frequency_index-1]+fft_syn_temp2[syn_sine_frequency_index-2]+fft_syn_temp2[syn_sine_frequency_index]);
//			
//			float sum2_temp3=0;
//			for(int i=0;i<rx_fft_synchronization_window;i++)
//				sum2_temp3=sum2_temp3+real(fft_syn_temp2[i]);
//			temp3=sum2_temp3/rx_fft_synchronization_window;
//			if( (temp2/temp3)>rx_fft_synchronization_theo&&temp2<rx_fft_synchronization_sine_self_theo )
//			{
//				temp1=0;
//				get_data_index=get_data_index-rx_fft_synchronization_window;
//			}
//		}
//
//	}
//
//	if((get_data_index+process_total_length)>Received_symbols_length_total)
//	{
//		/*Process_final_symbol=new complex<float>[process_total_length];
//		memset(Process_final_symbol,0,sizeof(float)*2*process_total_length);*/
//		//for(int i=0;i<process_total_length;i++)
//		//{
//		//	
//		//	Process_final_symbol[i]=Received_symbols[get_data_index+i];
//		//	
//		//}
//		get_data_ok=1;
//	}
//	else
//		 get_data_ok=0;
//

//////////////////////////////////////////////////time synchronization//////////////////////////////////////////////
	get_data_ok=1;
	get_data_index=0;
	Vector<complex<float>>SEQ_time=generate_SEQtime(time_sequence_length,seq_mode_time);
	for(int i=0;i<transmitted_symbol_length;i++)
		Process_final_symbol[i]=Received_symbols[i];
	if(get_data_ok==1)
	{
		temp1=rx_fft_synchronization_window+2*CP_length_time;
		Vector<complex<float>>time_syn_ref((int)temp1);
	
		for(int i=0;i<(int)temp1;i++)
		{
			for(int j=0;j<time_sequence_length;j++)
				time_syn_ref[i]=time_syn_ref[i]+Process_final_symbol[sine_symbol_length-rx_fft_synchronization_window+i+j]*SEQ_time[j];
			
			time_syn_ref[i]=abs(time_syn_ref[i]);

		}
		temp2=0;
		temp3=0;
		float sum_temp1=0;
		for(int i=0;i<(int)temp1;i++)
		{
			sum_temp1+=real(time_syn_ref[i]);
			if(real(time_syn_ref[i])>temp2)
			{
				temp2=real(time_syn_ref[i]);
				temp3=i;
			}
		}

		temp1=temp2/(sum_temp1/temp1);
		if(temp1>rx_PN_seq_synchronization_theo)
		{
			get_data_ok=1;
			get_data_index=get_data_index+(int)temp3+1-CP_length_time-1-rx_fft_synchronization_window;
			if(get_data_index>0)
			{
				for(int i=0;i<transmitted_symbol_length-get_data_index+1;i++)
					Process_final_symbol[i]=Received_symbols[get_data_index-1+i];

				for(int i=transmitted_symbol_length-get_data_index+1;i<transmitted_symbol_length;i++)
					Process_final_symbol[i]=Received_symbols[i-transmitted_symbol_length+get_data_index-1];

			}
			else
			{
				for(int i=0;i<-get_data_index+1;i++)
					Process_final_symbol[i]=Received_symbols[transmitted_symbol_length+get_data_index-1+i];
				for(int i=0;i<transmitted_symbol_length+get_data_index-1;i++)
					Process_final_symbol[-get_data_index+1+i]=Received_symbols[i];
				//get_data_ok=0;
			}
		}
		else
			get_data_ok=0;
		
	}

//////////////////////////////////////frequency offset estimation/////////////////////////////////////////////////////////////
	Vector<complex<float>>Sine_waveform1(sine_symbol_length);
	Vector<complex<float>>Sine_waveform2(sine_symbol_length);

	int iteration_index;
	float corr_frequency_offset;
	float ref_zero_sin_phase=-2*PI/sine_frequency;
	float ref_sine_compensate_ratio=PI;

	for(int j=0;j<sine_symbol_length;j++)
	{
		Sine_waveform1[j]=Process_final_symbol[j];
		Sine_waveform2[j]=Process_final_symbol[transmitted_symbol_length-sine_symbol_length+j];
	}

	complex<float> sum_sine_wareform1=0;
	complex<float> sum_sine_wareform2=0;
	for(int j=0;j<sine_symbol_length;j++)
	{
		sum_sine_wareform1+=Sine_waveform1[j];
		sum_sine_wareform2+=Sine_waveform2[j];
	}

	sum_sine_wareform1=complex_divide(sum_sine_wareform1,sine_symbol_length);
	sum_sine_wareform2=complex_divide(sum_sine_wareform2,sine_symbol_length);

	
	for(int j=0;j<sine_symbol_length;j++)
	{
		

		Sine_waveform1[j]=Sine_waveform1[j]-sum_sine_wareform1;
		Sine_waveform2[j]=Sine_waveform2[j]-sum_sine_wareform2;
	}

	if(get_data_ok==1)
	{
//////////////////////////large frequency offset estimation/////////////////////////////////
		iteration_index=0;
		corr_frequency_offset=0;
		while(iteration_index<rx_frequency_estimation_iteration_max)
		{

			Vector<complex<float>>corr_temp(rx_fre_corr_number+1);


			for(int m=0;m<rx_fre_corr_number;m++)
			{
				for(int n=floor(sine_symbol_length/2)-1;n<sine_symbol_length-11-m;n++)
					corr_temp[m]=corr_temp[m]+(conj(Sine_waveform1[n]))*Sine_waveform1[n+m+1];
				corr_temp[m]=complex_divide(corr_temp[m],floor(sine_symbol_length/2)-m-11);
/////////////////////angle				
				corr_temp[m]= arg(corr_temp[m]);	

			}

			
////////////////////////////////////////////////��֤��С����
			if(real(corr_temp[1])>real(corr_temp[0]))
			{
				for(int m=0;m<rx_fre_corr_number-1;m++)
				{
					if(real(corr_temp[m])>real(corr_temp[m+1]))
					{
						for(int n=m+1;n<rx_fre_corr_number;n++)
							corr_temp[n]=corr_temp[n]+complex<float>(2*PI,0);
					}

				}
			}
//////////////////////////////////////////////��֤�Ӵ�С
			else
			{
				for(int m=0;m<rx_fre_corr_number-1;m++)
				{
					if(real(corr_temp[m])<real(corr_temp[m+1]))
					{
						for(int n=m+1;n<rx_fre_corr_number;n++)
							corr_temp[n]=corr_temp[n]-complex<float>(2*PI,0);
					}

				}

			}

			
			for(int m=0;m<rx_fre_corr_number;m++)
				corr_temp[m]=complex_divide(corr_temp[m],(m+1));
			
			float real_sum=0;
			for(int m=rx_fre_corr_start-1;m<rx_fre_corr_number-1;m++)
				real_sum=real_sum+real(corr_temp[m]);

			corr_temp[rx_fre_corr_number]=real_sum/(rx_fre_corr_number-rx_fre_corr_start);
			
			complex<float> corr_sine1=corr_temp[rx_fre_corr_number];

			temp1=0;
			for(int n=rx_fre_corr_start-1;n<rx_fre_corr_number;n++)
				temp1= temp1 + abs(corr_temp[n]-corr_sine1)*abs(corr_temp[n]-corr_sine1);

			temp1=temp1/(rx_fre_corr_number-rx_fre_corr_start+1);

///////////////////////////get sine waveform2 and estimation frequency offset/////////////////////////////////
			memset(corr_temp,0,sizeof(float)*2*(rx_fre_corr_number+1));
			for(int m=0;m<rx_fre_corr_number;m++)
			{
				for (int n=9;n<sine_symbol_length-m-11;n++)
					corr_temp[m]=corr_temp[m]+(conj(Sine_waveform2[n]))*Sine_waveform2[n+m+1];
				corr_temp[m]=complex_divide(corr_temp[m],sine_symbol_length-m-21);
///////////////////////angle				
				corr_temp[m]= atan2(imag(corr_temp[m]),real(corr_temp[m]));
			}
////////////////////////////////////////////////////////////
			if(real(corr_temp[1])>real(corr_temp[0]))
			{
				for(int m=0;m<rx_fre_corr_number-1;m++)
				{
					if(real(corr_temp[m])>real(corr_temp[m+1]))
					{
						for(int n=m+1;n<rx_fre_corr_number;n++)
							corr_temp[n]=corr_temp[n]+complex<float>(2*PI,0);
					}

				}
			}

			else
			{
				for(int m=0;m<rx_fre_corr_number-1;m++)
				{
					if(real(corr_temp[m])<real(corr_temp[m+1]))
					{
						for(int n=m+1;n<rx_fre_corr_number;n++)
							corr_temp[n]=corr_temp[n]-complex<float>(2*PI,0);
					}

				}

			}


			for(int m=0;m<rx_fre_corr_number;m++)
				corr_temp[m]=complex_divide(corr_temp[m],(m+1));	

			real_sum=0;
			for(int m=rx_fre_corr_start-1;m<rx_fre_corr_number-1;m++)
				real_sum=real_sum+real(corr_temp[m]);

			corr_temp[rx_fre_corr_number]=real_sum/(rx_fre_corr_number-rx_fre_corr_start);
			complex<float> corr_sine2=corr_temp[rx_fre_corr_number];
			temp2=0;
			for(int n=rx_fre_corr_start-1;n<rx_fre_corr_number;n++)
				temp2= temp2 + abs(corr_temp[n]-corr_sine2)*abs(corr_temp[n]-corr_sine2);

			temp2=temp2/(rx_fre_corr_number-rx_fre_corr_start+1);
////////////////////////////////////////
			if(temp1>0.0002)
				temp1=0;
			else
				temp1=1;
			if(temp2>0.0002)
				temp2=0;
			else
				temp2=1;


			float corr_frequency_offset_temp=0;
			complex<float> corr_sine;
			if((temp1+temp2)>0)
			{
				corr_sine=(temp1*corr_sine1+temp2*corr_sine2)/(temp1+temp2);
				corr_frequency_offset_temp=-(real(corr_sine)-ref_zero_sin_phase)/ref_sine_compensate_ratio/2;
				for(int n=0;n<transmitted_symbol_length;n++)
					Process_final_symbol[n]=Process_final_symbol[n]*exp(complex<float>(0,2*PI*corr_frequency_offset_temp*(n+1)));
			}
			else
				iteration_index=rx_frequency_estimation_iteration_max;
///////////////////////////////get sin wareform///////////////////////
			for(int j=0;j<sine_symbol_length;j++)
			{
				Sine_waveform1[j]=Process_final_symbol[j];
				Sine_waveform2[j]=Process_final_symbol[transmitted_symbol_length-sine_symbol_length+j];
			}

			iteration_index++;
			corr_frequency_offset+=corr_frequency_offset_temp;
						
		}

		float frequency_offset=-corr_frequency_offset*system_samping_rate;

	}

	return Process_final_symbol;

}
