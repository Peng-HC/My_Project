#include <cstring>
#include <Config/Config.h>
#include "header.h"

int zc_root_time=128;
int zc_root_frequency=1;
int pilot_subcarrier_num=511;

///////////////////////////////////////////////////////////////ROUND
int round(float r)
{
	return (r>0.0)?floor(r+0.5):ceil(r-0.5);
}

////ZC����
Vector<complex<float>> ZC_sequence(int r,int N)
{
	Vector<complex<float>> seq;
	seq.resize(N);
	for(int i=0;i<N;i++)
		seq[i]=complex<float>(cos(PI*(float)r*(float)i*(float)(i+1)/(float)N),sin(-PI*(float)r*(float)i*(float)(i+1)/(float)N));
	return seq;


}

///M����
Vector<complex<float>> M_sequence(int baseval,int powerval,int shift)
{
	int bitnum=pow(baseval,powerval)-1;
	Vector<int>reg(powerval,1);


	int tap_final[480];
	if (baseval==2)
	{
		switch (powerval)
		{
			case 2:{int tap[480]={1,2};
				   memcpy(tap_final,tap,sizeof(tap));} break;
			case 3:{int tap[480]={1,3,2,3};
				   memcpy(tap_final,tap,sizeof(tap));} break;  
			case 4:{int tap[480]={1,4,3,4};
				   memcpy(tap_final,tap,sizeof(tap));} break;
			case 5:{int tap[480]={2,5,3,5,1,2,3,5,2,3,4,5,1,2,4,5,1,3,4,5};
				   memcpy(tap_final,tap,sizeof(tap)); }break;
			case 6:{int tap[480]={1,6,5,6,1,2,5,6,1,4,5,6,1,3,4,6,2,3,5,6};
				   memcpy(tap_final,tap,sizeof(tap));} break;
			case 7:{int tap[480]={1,7,6,7,3,7,4,7,1,2,3,7,4,5,6,7,1,2,5,7,2,5,6,7,2,3,4,7,3,4,5,7,1,3,5,7,2,4,6,7,1,3,6,7,1,4,6,7,2,3,4,5,6,7,1,2,3,4,5,7,1,2,4,5,6,7,1,2,3,5,6,7};
				   memcpy(tap_final,tap,sizeof(tap));}break;
			case 8:{int tap[480]={1,2,7,8,1,6,7,8,1,3,5,8,3,5,7,8,2,3,4,8,4,5,6,8,2,3,5,8,3,5,6,8,2,3,6,8,2,5,6,8,2,3,7,8,1,5,6,8,1,2,3,4,6,8,2,4,5,6,7,8,1,2,3,6,7,8,1,2,5,6,7,8};
				   memcpy(tap_final,tap,sizeof(tap));}break;
			case 9:{int tap[480]={4,9,5,9,3,4,6,9,3,5,6,9,4,5,8,9,1,4,5,9,1,4,8,9,1,5,8,9,2,3,5,9,4,6,7,9,5,6,8,9,1,3,4,9,2,7,8,9,1,2,7,9,2,4,7,9,2,5,7,9,2,4,8,9,1,5,7,9,1,2,4,5,6,9,3,4,5,7,8,9,1,3,4,6,7,9,2,3,5,6,8,9,3,5,6,7,8,9,1,2,3,4,6,9,1,5,6,7,8,9,1,2,3,4,8,9,1,2,3,7,8,9,1,2,6,7,8,9,1,3,5,6,8,9,1,3,4,6,8,9,1,2,3,5,6,9,3,4,6,7,8,9,2,3,6,7,8,9,1,2,3,6,7,9,1,4,5,6,8,9,1,3,4,5,8,9,1,3,6,7,8,9,1,2,3,6,8,9,2,3,4,5,6,9,3,4,5,6,7,9,2,4,6,7,8,9,1,2,3,5,7,9,2,3,4,5,7,9,2,4,5,6,7,9,1,2,4,5,7,9,2,4,5,6,7,9,1,3,4,5,6,7,8,9,1,2,3,4,5,6,8,9};
				   memcpy(tap_final,tap,sizeof(tap));}break;
			case 10:{int tap[480]={3,10,7,10,2,3,8,10,2,7,8,10,1,3,4,10,6,7,9,10,1,5,8,10,2,5,9,10,4,5,8,10,2,5,6,10,1,4,9,10,1,6,9,10,3,4,8,10,2,6,7,10,2,3,5,10,5,7,8,10,1,2,5,10,5,8,9,10,2,4,9,10,1,6,8,10,3,7,9,10,1,3,7,10,1,2,3,5,6,10,4,5,7,8,9,10,2,3,6,8,9,10,1,2,4,7,8,10,1,5,6,8,9,10,1,2,4,5,9,10,2,5,6,7,8,10,2,3,4,5,8,10,2,4,6,8,9,10,1,2,4,6,8,10,1,2,3,7,8,10,2,3,7,8,9,10,3,4,5,8,9,10,1,2,5,6,7,10,1,4,6,7,9,10,1,3,4,6,9,10,1,2,6,8,9,10,1,2,4,8,9,10,1,4,7,8,9,10,1,2,3,6,9,10,1,2,6,7,8,10,2,3,4,8,9,10,1,2,4,6,7,10,3,4,6,8,9,10,2,4,5,7,9,10,1,3,5,6,8,10,3,4,5,6,9,10,1,4,5,6,7,10,1,3,4,5,6,7,8,10,2,3,4,5,6,7,9,10,3,4,5,6,7,8,9,10,1,2,3,4,5,6,7,10,1,2,3,4,5,6,9,10,1,4,5,6,7,8,9,10,2,3,4,5,6,8,9,10,1,2,4,5,6,7,8,10,1,2,3,4,6,7,9,10,1,3,4,6,7,8,9,10};
					memcpy(tap_final,tap,sizeof(tap));}break;
			case 11:{int tap[480]={9,11};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 12:{int tap[480]={6,8,11,12};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 13:{int tap[480]={9,10,11,12};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 14:{int tap[480]={4,8,13,14};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 15:{int tap[480]={14,15};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 16:{int tap[480]={4,13,15,16};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 17:{int tap[480]={14,17};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 18:{int tap[480]={11,18};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 19:{int tap[480]={14,17,18,19};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 20:{int tap[480]={17,20};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 21:{int tap[480]={19,21};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 22:{int tap[480]={21,22};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 23:{int tap[480]={18,23};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 24:{int tap[480]={17,22,23,24};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 25:{int tap[480]={22,25};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 26:{int tap[480]={20,24,25,26};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 27:{int tap[480]={22,25,26,27};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 28:{int tap[480]={25,28};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 29:{int tap[480]={27,29};
					memcpy(tap_final,tap,sizeof(tap));} break;
			case 30:{int tap[480]={7,28,29,30};
					memcpy(tap_final,tap,sizeof(tap));} break;
		} 
		
	}
	
	else if (baseval==3)
	{
		switch (powerval)
		{
		case 2:{int tap[480]={2,1,1,1};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 3:{int tap[480]={0,1,2,1,0,2,1,2,2,2,1,2};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 4:{int tap[480]={0,0,2,1,0,0,1,1,2,0,0,1,2,2,1,1,2,1,1,1,1,0,0,1,1,2,2,1,1,1,2,1};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 5:{int tap[480]={0,0,0,1,2,0,0,0,1,2,0,0,1,2,2,0,2,1,0,2,0,2,1,1,2,0,1,2,0,2,0,1,1,2,2,2,0,0,1,2,2,0,2,0,2,2,0,2,2,2,2,2,0,2,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1,1,0,2,1,0,0,0,2,1,0,0,2,2,1,0,1,1,2,1,2,2,2,2,1,1,0,1,2,1,1,2,0,2};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 6:{int tap[480]={0,0,0,0,2,1,0,0,0,0,1,1,0,0,2,0,2,1,0,0,1,0,1,1,0,2,0,1,2,1,0,2,0,1,1,1,0,2,2,0,1,1,0,2,2,2,1,1,2,1,1,1,0,1,1,0,0,0,0,1,1,0,2,1,0,1,1,0,1,0,0,1,1,0,1,2,1,1,1,0,1,1,1,1,1,2,0,2,2,1,1,2,0,1,0,1,1,2,2,1,2,1,1,2,1,0,1,1,1,2,1,2,1,1,1,2,1,1,2,1,1,1,2,1,0,1,1,1,1,0,1,1,1,1,1,2,0,1,1,1,1,1,1,1};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 7:{int tap[480]={0,0,0,0,2,1,2,0,0,0,0,1,0,2,0,0,0,2,0,2,2,0,0,0,2,2,2,2,0,0,0,2,1,0,2,0,0,0,1,1,2,2,0,0,0,1,1,1,2,0,0,2,2,2,0,2,0,0,2,2,1,2,2,0,0,2,1,0,0,0,0,2,1,2,2,2,0,0,1,0,2,1,2,0,0,1,0,1,1,2,0,0,1,1,0,1,2,0,0,1,1,2,0,2,0,2,0,0,0,2,2,0,2,0,0,1,0,2,0,2,0,0,1,1,2,0,2,0,2,2,0,2,0,2,0,2,1,2,2,0,2,0,1,1,0,2,0,2,2,0,2,0,2,0,2,2,0,1,2,2,0,2,2,2,2,1,2,0,2,2,2,1,0,2,0,2,2,1,0,1,2,0,2,2,1,2,2,2};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		}
	}
	
	
	else if (baseval==5)
	{
		switch (powerval)
		{
		case 2:{int tap[480]={4,3,3,2,2,2,1,3};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 3:{int tap[480]={0,2,3,4,1,2,3,0,2,3,4,2,3,3,3,3,3,2,3,1,3,2,0,3,2,4,3,2,3,3,2,3,2,2,1,2,1,0,2,1,4,3,1,1,3};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		case 4:{int tap[480]={0,4,3,3,0,4,3,2,0,4,2,3,0,4,2,2,0,1,4,3,0,1,4,2,0,1,1,3,0,1,1,2,4,0,4,2,4,0,3,2,4,0,2,3,4,0,1,3,4,4,4,2,4,3,0,3,4,3,4,3,4,2,0,2,4,2,1,3,4,1,1,2,3,0,4,2,3,0,3,3,3,0,2,2,3,0,1,3,3,4,3,2,3,3,0,2,3,3,3,3,3,2,0,3,3,2,2,3,3,1,2,2,2,0,4,3,2,0,3,2,2,0,2,3,2,0,1,2,2,4,2,2,2,3,0,2,2,3,2,3,2,2,0,3,2,2,3,3,2,1,3,2,1,0,4,3,1,0,3,3,1,0,2,2,1,0,1,2,1,4,1,2,1,3,0,3,1,3,1,3,1,2,0,2,1,2,4,3,1,1,4,2};
			   memcpy(tap_final,tap,sizeof(tap));} break;
		}
	} 

	else
	{
		cout<<"Generate M-sequence Error!"<<endl;
		cout<<"M-sequence"<<baseval<<"-"<<powerval<<"is not defined"<<endl;
	}

	Vector<int>ms(bitnum);
    Vector<int>weight(powerval);

	if(baseval==2)
	{
		switch (powerval)
		{

		case 2:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 3:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 4:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 5:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 6:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 7:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 8:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 9:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 10:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 11:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 12:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 13:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 14:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 15:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 16:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 17:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 18:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 19:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 20:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 21:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 22:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 23:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 24:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 25:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 26:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 27:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;
		case 28:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 29:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;break;
		case 30:weight[tap_final[0]-1]=1;weight[tap_final[1]-1]=1;weight[tap_final[2]-1]=1;weight[tap_final[3]-1]=1;break;

		}
	}
	else if(baseval>2)
		memcpy(weight,tap_final,sizeof(int)*powerval);

	int Vector_sum=0;
	for(int i=0;i<bitnum;i++)
	{
		for(int j=0;j<powerval;j++)
			Vector_sum=Vector_sum+weight[j]*reg[j];
		ms[i]=(Vector_sum+baseval)%baseval;
		for(int j=0;j<powerval-1;j++)
			reg[powerval-1-j]=reg[powerval-2-j];
		reg[0]=ms[i];
		Vector_sum=0;
		
	}

	if(shift>0)
	{
		shift=shift%bitnum;
		int *shift_tmp=new int[bitnum];
		memcpy(shift_tmp,ms,sizeof(int)*bitnum);
		for(int i=0;i<bitnum;i++)
			ms[i]=shift_tmp[(i+shift)%bitnum];
	}

	if(baseval==2)
	{
		for(int i=0;i<bitnum;i++)
			ms[i]=ms[i]*2-1;
	}

	else if(baseval==3)
	{
		for(int i=0;i<bitnum;i++)
		{
			if(ms[i]==2)
				ms[i]=-1;
		}
	}

	else if(baseval==5)
	{
		for(int i=0;i<bitnum;i++)
		{
			if(ms[i]==4)
				ms[i]=-1;
			if(ms[i]==3)
				ms[i]=-2;
		}
	}

	else
		cout<<"wrong baseval"<<endl;

	
	Vector<complex<float>>seq;
	seq.resize(bitnum);
	for(int i=0;i<bitnum;i++)
		seq[i]=ms[i];
	return seq;

}

/////////////////////////////////////////////////////////////����SEQTIME
Vector<complex<float>>generate_SEQtime(int time_sequence_length,const char seq_mode_time)
{
	Vector<complex<float>>SEQ_time;
	switch (seq_mode_time)
	{
	case'M':{int M_sequence_power=round(log10(time_sequence_length+1)/log10(2));
		   SEQ_time=M_sequence(2,M_sequence_power,0);break;}
	
	case 'Z':{ SEQ_time=ZC_sequence(zc_root_time,time_sequence_length);break;}
			 		 
	}

	return SEQ_time;
	
}

/////////////////////////////////////////////////////////////����SEQFREQU
Vector<complex<float>>generate_SEQfrequency(int pilot_subcarriernum,const char seq_mode_frequency)
{
	Vector<complex<float>>SEQ_frequency;
	switch (seq_mode_frequency)
	{
	case'M':{int M_sequence_power=round(log10(pilot_subcarrier_num+1)/log10(2));
		SEQ_frequency=M_sequence(2,M_sequence_power,0);break;}
	
	case 'Z':SEQ_frequency=ZC_sequence(zc_root_frequency,pilot_subcarrier_num);break;
		
	}

	return SEQ_frequency;
	
}

////////////////////////////////////////////////////////////��������
complex<float> complex_divide(complex<float>A ,complex<float>B)
{
	float a=A.real();
	float b=A.imag();
	float c=B.real();
	float d=B.imag();
	float real=(a*c+b*d)/(c*c+d*d);
	float imag=(b*c-a*d)/(c*c+d*d);
    A=complex<float>(real,imag);

	return A;
}

////////////////////////////////////////////////////////////Բ�ܾ��-�����
Vector<complex<float>> xcorr(Vector<complex<float>>a,Vector<complex<float>> b)
{
	int len = a.size();
	Vector<complex<float>> c(len);
	Vector<complex<float>> tmp(len);
	for(int i=0;i<len;i++)
		tmp[i]=b[(len-i)%len];
	c=ifftc2c(fftc2c(a)*fftc2c(tmp));
	c/=(complex<float>)len;
	return c;		
}

/////////////////////////////////////////////////////////////////////////OFDM����
Vector<complex<float>> OFDM_SNR_Estimation(Matrix<complex<float>>OFDM_IN,int row,int column)
{
	Vector<complex<float>>SNR_OUT(row);
	if(column>2)
	{
		Vector<float>SNR_TEMP(row);
		for(int n=0;n<column-1;n++)
		{
			for(int m=0;m<row;m++)
				SNR_TEMP[m]+=abs(OFDM_IN[m][n]-OFDM_IN[m][column-1]);
		}

		SNR_TEMP=SNR_TEMP/(float)(column-1);

		for(int n=0;n<row;n++)
		{
			SNR_OUT[n]=abs(OFDM_IN[n][column-1])/SNR_TEMP[n];
			SNR_OUT[n]=20*log10(real(SNR_OUT[n]))-3;
			SNR_OUT[n]=(float)(-3.678)*(float)pow(10,-5)*pow(SNR_OUT[n],4)+(float)0.003*pow(SNR_OUT[n],3)-(float)0.0872*SNR_OUT[n]*SNR_OUT[n]+(float)2.1258*SNR_OUT[n]-(float)5.5353;
		}
	}
	
	else
	{
		for(int i=0;i<row;i++)
			SNR_OUT[i]=30;

	}
	return SNR_OUT;
}

 Vector<complex<float>> Filter_data(Vector<complex<float>> Data_in,Vector<float>Filter_corr)
{
	int data_length=Data_in.size();
	int Filter_length=Filter_corr.size();	
	Vector<complex<float>> Data_out_temp(Data_in);

	for(int n=0;n<data_length-Filter_length;n++)
	{
		complex<float> temp1=0;
		for(int m=0;m<Filter_length;m++)
			temp1=temp1+Data_out_temp[n+m]*Filter_corr[m];
		Data_out_temp[n]=conj(temp1);
	}
	
	Vector<complex<float>>Data_OUT(Data_out_temp);
	return Data_OUT;

}

//////////////////////////////////////////////////////////////////////////resample
Vector<complex<float>> resample(Vector<complex<float>> X, int p,int q)
{
	FILE* f1=fopen(Config::tmp_filter_matrix_filepath.c_str(),"rb");
	if(f1==NULL)
		cout<<"Open Filter_Corr erro!";
 	fseek(f1,0,SEEK_END);
    int Filter_length=ftell(f1)/sizeof(float);
	rewind(f1);
	Vector<float>Filter_corr(Filter_length);
	fread(Filter_corr,sizeof(float),Filter_length,f1);
	fclose(f1);
	int X_length=X.size();
	int Y_length=ceil(X_length,q);
	int Y_data_length=Y_length*p+2*Filter_length;
	
	Vector<complex<float>>Y_resample_temp(Y_data_length);
	for(int n=0;n<Y_length;n++)
		Y_resample_temp[(Filter_length-1)/2+(n+1)*p-1]=X[n*q];

	Y_resample_temp=Filter_data(Y_resample_temp,Filter_corr);
    
	Vector<complex<float>> Y(Y_length*p);
	for(int i=0;i<Y_length*p;i++)
		Y[i]=Y_resample_temp[i];
	return Y;
}
Vector<complex<float>> smooth(Vector<complex<float>> &hls,int movestep,int size){
    Vector<complex<float>> res(size-movestep);
    for(int i=0;i<hls.size()-movestep;++i){
        complex<float> temp1={0.0,0.0};
        for(int j=0;j<movestep;++j){
            temp1+=hls[i+j];
        }
        temp1 = temp1/ complex<float>(float(movestep),0.0);
        res[i]=temp1;
    }
    return res;
}
vector<float> xcorr_delay(vector<float>a,vector<float> b){
    int len=a.size();
    vector<float> c(2*len-1,0.0);
    vector<float> tmp(2*len-1,0.0);
    for(int i=len-1;i<2*len-1;++i){
        tmp[i]=a[i-(len-1)];
    }
    for(int i=0;i<2*len-1;++i){
        for(int j=0;j<len&&(i+j)<(2*len-1);++j){
            c[i]+=tmp[i+j]*b[j];
        }
    }
    return c;
}
