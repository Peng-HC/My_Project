//
// Created by louis on 22-5-21.
//

#ifndef ROI_SENDLDPCENCODEDMSGCMD_H
#define ROI_SENDLDPCENCODEDMSGCMD_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <QtCore/QObject>
#include <Util/Encode/ldpc/LdpcCode.h>

 #include "Config/Config.h"

class SendLdpcEncodedMsgCmd: public QObject {

public:

    SendLdpcEncodedMsgCmd(){
        //初始化校验矩阵，支持码率为2/3,1/2,1/3,1/4,1/5
        int bg = Config::ldpc_rate_index > 2 ? 2 : 1;
        ldpc_code.InitMatrix(bg,256,Config::LDPC_RATE_TABLE[Config::ldpc_rate_index]);
        ldpc_code.InitConfig();
        double t = bg == 1 ? 0.3333333 : 0.2;
        ldpc_redundancy.InitMatrix(bg,256,t);
        ldpc_redundancy.InitConfig();
    }

    LdpcCode ldpc_code, ldpc_redundancy;

    std::string encoded_msg;
    std::string msg_binstr_len;

    void assign(const char *in);
    void server_process(const char* in, bool);
    void client_process(std::string msg_binstr_, bool);

    void fill(const std::string& encoded_msg_, const std::string& msg_binstr_len_);

    std::string fill_do_to_string(bool print);

    void print_mem();

};

#endif //ROI_SENDLDPCENCODEDMSGCMD_H
