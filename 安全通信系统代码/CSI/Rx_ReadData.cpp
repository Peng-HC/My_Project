#include "header.h"

int Get_datalength=0;
int Rx_offsetdata=0;
int Received_symbols_length_total;

Vector<complex<float>> Rx_ReadData(const char* filename,const char Output_datamode,const char rx_power_normalization)
{
    cout<<filename<<endl;
    std::cout<<"start to Rx_ReadData"<<std::endl;
	Vector<complex<float>> Received_symbols;
	Vector<float> Datatemp;
	int size;
	FILE *fileID_read=fopen(filename,"rb");
	if(fileID_read==NULL)
		cout<<"Read data erro!";
	fseek(fileID_read,0,SEEK_END);
	size=ftell(fileID_read)/sizeof(float);
    std::cout<<"size = "<<size<<std::endl;
	rewind(fileID_read);
//	if(Get_datalength==0)
//		{
			Datatemp.resize(size);
			fread(Datatemp,sizeof(float),size,fileID_read);
			Get_datalength=size;
			fclose(fileID_read);		
//		}
//	else
//		{
//			Datatemp.resize(Get_datalength+Rx_offsetdata);
//			fread(Datatemp,sizeof(float),Get_datalength+Rx_offsetdata,fileID_read);
//			fclose(fileID_read);
//		}

	if( Get_datalength>size )
		Get_datalength=size;

	switch (Output_datamode)
	{
	case 'R':
		Received_symbols.resize(Get_datalength);
		for(int i=0;i<Get_datalength;i++)
		{
			Received_symbols[i]=Datatemp[i+Rx_offsetdata];
		}
		Received_symbols_length_total=Get_datalength;
		break;

	case 'C':
		int temp1=round(Get_datalength/2.0);
		Received_symbols.resize(temp1);
		for(int j=0;j<temp1;j++)
			Received_symbols[j]=complex<float>(Datatemp[Rx_offsetdata+j*2],Datatemp[Rx_offsetdata+(j+1)*2-1]);
		Received_symbols_length_total=temp1;
		break;
	}
	switch (rx_power_normalization)
	{
	case 'O':
		{double Received_power=0;
		for(int n=0;n<Received_symbols_length_total;n++)
			Received_power=Received_power+abs(Received_symbols[n]*Received_symbols[n]);
		Received_power=sqrt(Received_power/Received_symbols_length_total);
		for(int n=0;n<Received_symbols_length_total;n++)
			Received_symbols[n]=complex_divide(Received_symbols[n],Received_power);}
		break;

	case 'N':break;
	}

	return Received_symbols;

	}
