//
// Created by pc on 2022/1/13.
//
#include "header.h"
extern int subcarrier_num_v2x;
extern int movestep;

extern Vector<complex<float>> Rx_channel_estimation_V2X(const splab::Vector<complex<float>> Y){
    std::cout<<"start to Rx_channel_estimation_V2X"<<std::endl;
    const char Output_datamode='C';
    const char rx_power_normalization='N';
//    string file_dmrs="/home/pln/ROI-V2X-ZUC/data/corr_data/PSSCH_DMRS_Seq";
    // 采用相对路径(会报错)
//    string file_dmrs="data/corr_data/PSSCH_DMRS_Seq";
    char tmp[MAX_PATH];
    string source_path=getcwd(tmp,MAX_PATH);
//    string file_dmrs="/home/pc/ROI/data/corr_data/PSSCH_DMRS_Seq";
    string file_dmrs=source_path+"/PSSCH_DMRS_Seq";
    const char* filename_dmrs=file_dmrs.c_str();

    Vector<complex<float>> DMRS_symbols;
    DMRS_symbols=Rx_ReadData(filename_dmrs,Output_datamode,rx_power_normalization);
    Vector<complex<float>> CSI(subcarrier_num_v2x*4);
    for(int i=0;i<DMRS_symbols.size();i++){
        CSI[i]=complex_divide(Y[i],DMRS_symbols[i]);
//          CSI[i]=DMRS[i]* conj(DMRS_symbols[i]);
    }
    Vector<complex<float>> CSI_2(subcarrier_num_v2x);
    Vector<complex<float>> CSI_5(subcarrier_num_v2x);
    Vector<complex<float>> CSI_8(subcarrier_num_v2x);
    Vector<complex<float>> CSI_11(subcarrier_num_v2x);
    for(int i=0;i<subcarrier_num_v2x;++i){
        CSI_2[i]=CSI[i];
        CSI_5[i]=CSI[i+subcarrier_num_v2x];
        CSI_8[i]=CSI[i+subcarrier_num_v2x*2];
        CSI_11[i]=CSI[i+subcarrier_num_v2x*3];
    }
    Vector<complex<float>> CSI_2_smooth= smooth(CSI_2,movestep,subcarrier_num_v2x);
    Vector<complex<float>> CSI_5_smooth= smooth(CSI_5,movestep,subcarrier_num_v2x);
    Vector<complex<float>> CSI_8_smooth= smooth(CSI_8,movestep,subcarrier_num_v2x);
    Vector<complex<float>> CSI_11_smooth= smooth(CSI_11,movestep,subcarrier_num_v2x);
    Vector<complex<float>> CSI_smooth((subcarrier_num_v2x-movestep)*4);
    for(int i=0;i<subcarrier_num_v2x-movestep;++i){
        CSI_smooth[i]=CSI_2_smooth[i];
        CSI_smooth[i+subcarrier_num_v2x-movestep]=CSI_5_smooth[i];
        CSI_smooth[i+(subcarrier_num_v2x-movestep)*2]=CSI_8_smooth[i];
        CSI_smooth[i+(subcarrier_num_v2x-movestep)*3]=CSI_11_smooth[i];
    }

    return CSI_smooth;

}
