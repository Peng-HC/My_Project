//
// Created by pc on 2022/1/13.
//

#include <Config/Config.h>
#include "header.h"
Vector<complex<float>> RX_OUT_V2X(const char * argv)
{
    std::cout<<"start to RX_OUT_V2X"<<std::endl;

    const char Output_datamode='C';

    const char rx_power_normalization='N';

    Vector<complex<float>> res;
    Vector<complex<float>> Received_symbols;
    Vector<complex<float>> Process_final_symbol;
    Vector<Matrix<complex<float>>> Y;
    Vector<complex<float>> DMRS_sequence;
    Vector<complex<float>> Y_OUT;


//    res= Intialiazation_V2X(Output_datamode);

    Received_symbols=Rx_ReadData(argv,Output_datamode,rx_power_normalization);
    //
    cout<<"Received_symbols size is "<<Received_symbols.size()<<endl;
    DMRS_sequence= Rx_Demodulation(Received_symbols);

//    Y=Getsymbols(Output_datamode,Process_final_symbol);//生成matrix[0]存储导频信号，matrix[1]存储数据信号

    Y_OUT=Rx_channel_estimation_V2X(DMRS_sequence);
    return Y_OUT;
}