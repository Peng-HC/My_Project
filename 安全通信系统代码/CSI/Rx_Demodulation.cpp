//
// Created by pc on 2022/1/12.
//

#include "header.h"
extern int PSSCH_fft_offset;
extern int v2x_fft_size;
extern int subcarrier_num_v2x;
extern int cplen0;
extern int cplenR;
Vector<complex<float>> Rx_Demodulation(Vector<complex<float>> Received_symbols){
    std::cout<<"start to Rx_Demodulation with symbols size: "<< Received_symbols.size()<<std::endl;
    /***用DMRS计算频偏***/
    Vector<complex<float>> Get_CFO_Estimation_Wave(v2x_fft_size+cplenR);
    for(int i=0;i<Get_CFO_Estimation_Wave.size();++i){
        Get_CFO_Estimation_Wave[i]=Received_symbols[i+10976]*exp(complex<float>(0.0,-2.0)*complex<float>(PI)* complex<float>(i-cplenR)/complex<float>(v2x_fft_size*2));//10976=cplen0+cplenR*4+fft_size*5
    }
    float Mean_Thres = 0.2;
    Vector<complex<float>> Syn_Results_Raw(cplenR);
    for(int i=0;i<cplenR;++i){
        Syn_Results_Raw[i]=Get_CFO_Estimation_Wave[i]* conj(Get_CFO_Estimation_Wave[i+v2x_fft_size]);
    }
    float Power_sum=0;
    for(int i=0;i<Syn_Results_Raw.size();++i){
        Power_sum+=abs(Syn_Results_Raw[i]);
    }
    float Temp_1=0;
    float Temp_2=0;
    float Mean_Power=Power_sum/Syn_Results_Raw.size();
    for(int i=0;i<Syn_Results_Raw.size();++i){
        if(float(abs(Syn_Results_Raw[i])>Mean_Power*Mean_Thres)){
            Temp_2+=atan2(Syn_Results_Raw[i].imag(),Syn_Results_Raw[i].real());
            Temp_1+=1.0;
        }
    }
    float Est_Freq_Offset=Temp_2/Temp_1/v2x_fft_size;
    complex<float>xi=complex<float>(0,1);
    /***频偏补偿***/
    for(int i=0;i<Received_symbols.size();++i){
        Received_symbols[i]=Received_symbols[i]* exp(xi*Est_Freq_Offset*float(i));
    }


    FFTMR<float> Four;
    int cp=0;
    int counter=0;
    Vector<complex<float>> tmp(v2x_fft_size);

    Vector<complex<float>> tmp_fft(1200*14);
    for (int j=0;j<14;j++){
        if(j==0||j==7){
            cp=cplen0;
        } else{
            cp=cplenR;
        }
        for(int k=0;k<v2x_fft_size;k++){
            tmp[k]=Received_symbols[counter+k+cp-2]*exp(complex<float>(-2.0)*complex<float>(PI)*xi* complex<float>(k-2)/complex<float>(2048*2));
        }
        Four.fft(tmp);
        tmp=complex<float>( sqrt(1200.0)/2048.0)*tmp;
        tmp=circshift(tmp,-1024);
        for(int l=0;l<1200;l++){
            tmp_fft[j*1200+l]=tmp[l+PSSCH_fft_offset];
        }
//        for(int l=0;l<1200;l++){
//            if(l<600){
//                tmp_fft[j*1200+l]=tmp[1448+l];
//            }else{
//                tmp_fft[j*1200+l]=tmp[l-600];
//            }
//
//        }
        counter=counter+v2x_fft_size+cp;
    }
    Vector<complex<float>> DMRS(subcarrier_num_v2x*4);
    int PSSCH_DMRS_position[]={2,5,8,11};
    for(int k=0;k<4;k++){
        for(int i=0;i<subcarrier_num_v2x;i++){
            DMRS[k*subcarrier_num_v2x+i]=tmp_fft[PSSCH_DMRS_position[k]*1200+36+i];
        }

    }

    return DMRS;
}