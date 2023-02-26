//
// Created by seu on 18-11-26.
//
#include <iomanip>// std::setprecision
#include <Util/Encode/polar/PolarCode.h>
#include <Util/Math/Convert/Convert.h>
#include <boost/lexical_cast.hpp>
#include <Config/Config.h>
#include "SendPolarEncodedMsgCmd.h"
#include <iostream>

/**
 * Polar加密信息
 */

std::string SendPolarEncodedMsgCmd::fill_do_to_string(bool print) {
    std::string res;
    res += encoded_msg + "\n";
    res += msg_binstr_len + "\n";

    if (print) {
        std::cout << "std::string SendPolarEncodedMsgCmd::fill_do_to_string的res = " << res << std::endl;
    }

    return res;
}

void SendPolarEncodedMsgCmd::print_mem() {
    std::cout << "encoded_msg: " << encoded_msg << std::endl;
    std::cout << "msg_binstr_len: " << msg_binstr_len << std::endl;
}

//alice发给bob的加密信息
void SendPolarEncodedMsgCmd::fill(const std::string& encoded_msg_, const std::string& msg_binstr_len_) {
    encoded_msg = encoded_msg_;
    msg_binstr_len = msg_binstr_len_;
}

void SendPolarEncodedMsgCmd::assign(const char* in) {

    std::istringstream ins(in);

    std::getline(ins, encoded_msg);
    std::getline(ins, msg_binstr_len);
}

//Bob处理接收到的Polar加密信息,并显示UI
void SendPolarEncodedMsgCmd::server_process(const char* in) {
    // 解析收到的消息格式
    assign(in);

    // 利用未校验的csi生成秘钥expand_key
    std::string expand_key = Config::key.get_uncheked_key();
    std::string unchecked_csi_binstr = expand_key;
    //防止密钥数不够
    while (expand_key.size() < encoded_msg.size()) expand_key += unchecked_csi_binstr;

    //密钥异或回来密文
    Convert converter;
    std::vector<int> encoded_vec(encoded_msg.size());
    for (int i = 0; i < encoded_vec.size(); i++) {
        encoded_vec[i] = ((encoded_msg[i] - '1' + expand_key[i] - '1') % 2) ? 1 : 0;
    }
    //将密文转变为uint8_t向量作为译码输入
    std::vector<uint8_t> encoded_vec2(encoded_vec.size());
    for (int i = 0; i < encoded_vec2.size(); i++)
        encoded_vec2[i] = (uint8_t)encoded_vec[i];

    //解码
    std::vector<uint8_t> decoded_vec = polar_code.decode(ebno_vec, list_size_vec, encoded_vec2);

    //转化为字符串
    std::string decoded_msg_binstr;
    for (int i = 0; i < decoded_vec.size(); i++) {
        decoded_msg_binstr += (decoded_vec.at(i) == 1 ? '1' : '0'); // 解码后的01字符串
    }
    //恢复信息
    std::string decoded_msg = converter.binstr2str(decoded_msg_binstr.substr(0,boost::lexical_cast<int>(msg_binstr_len)));

    set_message(decoded_msg.c_str());
}

//Alice对01信息进行Polar编码,然后和本端秘钥异或后发送
void SendPolarEncodedMsgCmd::client_process(std::string msg_binstr_) {
    //记录源信息长度传输，用作译码截断
    int len = msg_binstr_.size();

    //扩展源信息至1024
    while (msg_binstr_.size() < 1024){
        int n = random()%2;
        msg_binstr_ += std::to_string(n);
    }

    //将信息转变为uint8_t向量作为编码输入
    std::vector<uint8_t> msg_vec(msg_binstr_.size());
    for (int i=0;i<msg_vec.size();i++) {
        msg_vec[i] = msg_binstr_[i];
    }

    //编码
    std::vector<uint8_t> encoded_vec = polar_code.encode(msg_vec); // 编码结果

    //向量变为字符串
    std::string encoded_msg2; // 编码后的01字符串
    for (int i=0;i<encoded_vec.size();i++) {
        encoded_msg2 += ((encoded_vec[i] - '0') % 2)? '1': '0';
    }

    //密钥
    // 利用未校验的csi生成秘钥expand_key
    std::string expand_key = Config::key.get_uncheked_key();
    std::string unchecked_csi_binstr = expand_key;
    //防止密钥数不够
    while (expand_key.size() < encoded_vec.size()) expand_key += unchecked_csi_binstr;

    //密钥异或密文
    std::string encoded_msg3;
    for (int i=0;i<encoded_vec.size();i++) {
        encoded_msg3 += ((encoded_vec[i] + expand_key[i] - '0') % 2)? '1': '0';
    }

    //封装传输消息
    fill(encoded_msg3, std::to_string(len));
}