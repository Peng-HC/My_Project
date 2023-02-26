//
// Created by phc on 2022/12/1.
//

#ifndef ROI_SENDORIGINMSG_H
#define ROI_SENDORIGINMSG_H

#include "Config/Config.h"
#include "Util/Encode/ldpc/LdpcCode.h"

class SendOriginMsg {

public:

    SendOriginMsg(){
        //初始化校验矩阵，支持码率为2/3,1/2,1/3,1/4,1/5
        int bg = Config::ldpc_rate_index > 2 ? 2 : 1;
        int zc = 256;
        ldpc_code.InitMatrix(bg,zc,Config::LDPC_RATE_TABLE[Config::ldpc_rate_index]);
        ldpc_code.InitConfig();
        double t = bg == 1 ? 0.3333333 : 0.2;
        ldpc_redundancy.InitMatrix(bg,zc,t);
        ldpc_redundancy.InitConfig();
    }
    LdpcCode ldpc_code, ldpc_redundancy;

    std::string origin_msg;
    std::string msg_binstr_len;

    // alice端生成2进制的Ma_Expand
    std::string get_ma_expand_bin(std::string &Key_Bin, int randomLen);
    //Alice对01信息进行LDPC编码,然后和本端秘钥异或
    std::string encrypted_encoded_msg(std::string &msg_binstr_, bool isharq);

    // alice端生成送入zuc算法的初始密钥(待bob端协商)
    void alice_sender_originmsg();
    //Bob处理接收到的LDPC加密信息
    std::string decrypted_decoded_msg(std::basic_string<char> encoded_msg, bool isharq);

    // bob端处理从alice端发送过来的明文消息,判断此次zuc初始密钥是否有效
    void bob_msg_handler(const char* in);

    // bob端根据Config::isSuccess生成发送给alice端的zuc初始密钥协商消息
    void bob_sender_originmsg();

    // alice处理从bob端发送过来的明文信息
    void alice_msg_handler(const char* in);

    void alice_sender_zucSucessSeq();
    void bob_zucSuccessSeq_handler(const char* in);

    void assign(const char *in);
    void fill(const std::string& origin_msg_, const std::string& msg_binstr_len_);
    std::string fill_do_to_string(bool print);

};


#endif //ROI_SENDORIGINMSG_H
