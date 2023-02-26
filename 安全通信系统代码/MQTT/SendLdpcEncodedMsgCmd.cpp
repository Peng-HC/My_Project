//
// Created by louis on 22-5-21.
//
#include <Util/Math/Convert/Convert.h>
#include <boost/lexical_cast.hpp>
#include <Config/Config.h>
#include "SendLdpcEncodedMsgCmd.h"
#include <QtCore/QString>
#include <iostream>
#include <MQTT/MQTTThread.h>
#include <immintrin.h>

/**
 * LDPC加密信息
 */
// 二进制字符串转化成字符串
#include "Util/UtilForZuc/util.h"
#include <iostream>
#include <bitset>
#include <string>
#include <algorithm>
using namespace std;


std::string SendLdpcEncodedMsgCmd::fill_do_to_string(bool print) {
    std::string res;
    res += encoded_msg + "\n";
    res += msg_binstr_len + "\n";

    if (print) {
        std::cout << "std::string SendLdpcEncodedMsgCmd::fill_do_to_string的res = " << res << std::endl;
    }

    return res;
}

void SendLdpcEncodedMsgCmd::print_mem() {
    std::cout << "encoded_msg: " << encoded_msg << std::endl;
    std::cout << "msg_binstr_len: " << msg_binstr_len << std::endl;
}

//alice发给bob的加密信息
void SendLdpcEncodedMsgCmd::fill(const std::string& encoded_msg_, const std::string& msg_binstr_len_) {
    encoded_msg = encoded_msg_;
    msg_binstr_len = msg_binstr_len_;
}

void SendLdpcEncodedMsgCmd::assign(const char* in) {

    std::istringstream ins(in);

    std::getline(ins, encoded_msg);
    std::getline(ins, msg_binstr_len);
}


//将8位字符串转化为64bit
string StrToBitStr(string str)
{
    bitset<64> bstr ;
    for(int i=0;i<8;i++)
    {
        bitset<8> bits =  bitset<8>(str[i]);
        cout<<str[i]<<endl;
        for(int j = 0;j<8;j++)
        {
            bstr[i*8+j] = bits[7-j];
        }
    }
    string s = bstr.to_string();
    //添加一个翻转操作
    reverse(begin(s),end(s));
    return s;
}
//将64bit二进制字符串转化为字符串
string BitStrToStr(string bstr)
{
    string str="";
    //每八位转化成十进制，然后将数字结果转化成字符
    int sum;
    for(int i=0;i<bstr.size();i+=8)
    {
        sum=0;
        for(int j=0;j<8;j++)
            if(bstr[i+j] == '1')
                sum = sum*2+1;
            else
                sum = sum*2;
        str =str+char(sum);
    }
    return str;

}


//Bob处理接收到的LDPC加密信息,并显示UI
void SendLdpcEncodedMsgCmd::server_process(const char* in, bool isharq) {
    // 解析收到的消息格式
    assign(in);

    std::string decrypted_key_bin=Config::zuc_generate_key_bin[Config::zuc_generate_key_bin.size()-1];
    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
    //密钥异或解密结果
    std::string decrypted_msg=encoded_msg;
    for (int i=0;i<decrypted_msg.size();i++) {
        decrypted_msg[i] ^= decrypted_key_bin[i] ^ '0';
//        decrypted_msg[i] ^= decrypted_key_bin[i];
    }
    // 二进制转为字符
    std::string text_msg = BitStrToStr(decrypted_msg);

    QString text(text_msg.c_str());
    set_message(text);
    std::cout<<"原始消息为:"<<encoded_msg<<std::endl;
    std::cout<<"原始消息的长度为:"<<msg_binstr_len<<std::endl;
    std::cout<<"解密后的信息为:"<<decrypted_msg<<std::endl;
    std::cout<<"解密后的信息转为字符串为:"<<text_msg<<std::endl;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    int iters = -1;
//    Convert converter;
//
//    // 解析收到的消息格式
//    assign(in);
//    if (isharq) {
//        //当前消息为重传的冗余比特
//        std::string decrypted_msg(encoded_msg);
//        std::string expand_key(Config::bob_key_buffer);
//        for (int i=0;i<encoded_msg.size();i++) {
//            decrypted_msg[i] ^= expand_key[i] ^ '0';
//        }
//        //拼接冗余比特到初传码字后进行译码
//        std::string fullsize = Config::bob_last_msg + encoded_msg;
//        std::string decoded_msg = ldpc_redundancy.LdpcDecoder(fullsize,iters);
//        if (iters == maxiter) {
//            QString errstr = "HARQ failed, message lost";
//            set_message(errstr);
//        }
//        else {
//            std::string msg = converter.binstr2str(decoded_msg.substr(0,boost::lexical_cast<int>(msg_binstr_len)));
//            QString text(msg.c_str());
//            set_message(text);
//            ++Config::correctcws;
//        }
//        ++Config::totalcws;
//        //需要重传说明缓冲区里的密钥整体KDR过高，舍弃
//        Config::bob_key_buffer.clear();
//        Config::bob_lastkey.clear();
//        Config::bob_send_harq = false;
//        return;
//    }
//    // 利用未校验的csi生成秘钥,并加入循环密钥缓冲区
//    std::string temp_key = Config::key.get_uncheked_key();
////    std::string temp_key = Config::zuc_generate_key_hex[Config::zuc_generate_key_hex.size()-1];
//    if (Config::bob_key_buffer.size() == 0) {
//        //缓冲区为空则将当前密钥复制后拼接再使用
//        std::string t(temp_key);
//        while (t.size() < encoded_msg.size()) t += temp_key;
//        Config::bob_key_buffer = t.substr(0, ldpc_code.n * ldpc_code.zc);
//        Config::bob_key_buffer_idx = temp_key.size();
//        Config::bob_lastkey = temp_key;
//    }
//    else {
//        //缓冲区非空则先把密钥写入缓冲区
//        auto buffer_end = Config::bob_key_buffer.end();
//        auto buffer_begin = Config::bob_key_buffer.begin();
//        auto ptr = buffer_begin + Config::bob_key_buffer_idx;
//        std::string lastkey(temp_key.size(), 0);
//        for (int i = 0; i < temp_key.size(); i++) {
//            lastkey[i] = *ptr;
//            *ptr = temp_key[i];
//            if (++ptr == buffer_end) ptr = buffer_begin;
//        }
//        Config::bob_key_buffer_idx = ptr - buffer_begin;
//        Config::bob_lastkey = lastkey;
//    }
//    std::string expand_key(Config::bob_key_buffer);
//
///*    std::string expand_key = temp_key;
//    std::string unchecked_csi_binstr = temp_key;
//    //防止密钥数不够
//    while (expand_key.size() < encoded_msg.size()) expand_key += unchecked_csi_binstr;
//*/
//
//    //密钥异或回来密文
//    std::string decrypted_msg(encoded_msg);
//    for (int i = 0; i < encoded_msg.size(); i++) {
//        decrypted_msg[i] ^= expand_key[i] ^ '0';
//    }
//
//    Config::bob_last_msg = decrypted_msg;
//    Config::bob_last_msg_len = boost::lexical_cast<int>(msg_binstr_len);
//
//    //解码,得到字符串
//    std::string decoded_msg = ldpc_code.LdpcDecoder(decrypted_msg, iters);
//    Config::totalcws++;
//    if (iters != maxiter) Config::correctcws++;
//    else {
//        //解码失败,该组密钥KDR过高,需要从缓冲区中删除
//        Config::bob_send_harq = true;
//        auto buffer_end = Config::bob_key_buffer.end();
//        auto buffer_begin = Config::bob_key_buffer.begin();
//        auto ptr = buffer_begin + Config::bob_key_buffer_idx;
//        std::string temp_key = Config::bob_lastkey;
//        for (int i = temp_key.size()-1; i >= 0; i--) {
//            *ptr = temp_key[i];
//            if (ptr == buffer_begin) ptr = buffer_end;
//            --ptr;
//        }
//        //更新指向缓冲区的位置
//        Config::bob_key_buffer_idx = ptr - buffer_begin;
//    }
//    std::cout << "正确码字 / 总码字 ： " << Config::correctcws << "/" << Config::totalcws << std::endl;
//    //恢复信息
//    std::string msg = converter.binstr2str(decoded_msg.substr(0,boost::lexical_cast<int>(msg_binstr_len)));
//    QString text(msg.c_str());
//    set_message(text);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

//Alice对01信息进行LDPC编码,然后和本端秘钥异或后发送
void SendLdpcEncodedMsgCmd::client_process(std::string msg_binstr_, bool isharq) {
    int len = msg_binstr_.size();
    std::string encrypted_key_bin=Config::zuc_generate_key_bin[Config::zuc_generate_key_bin.size()-1];
//    std::string encrypted_key_bin = HexStrToBinStr(encrypted_key_hex);
    std::cout<<"正在使用ZUC密钥流对文字进行加密-------------------------------"<<std::endl;
    std::cout<<"正在使用ZUC密钥流对文字进行加密-------------------------------"<<std::endl;
    std::cout<<"正在使用ZUC密钥流对文字进行加密-------------------------------"<<std::endl;
    //密钥异或编码结果
    std::string encrypted_msg=msg_binstr_;
    for (int i=0;i<encrypted_msg.size();i++) {
        encrypted_msg[i] ^= encrypted_key_bin[i] ^ '0';
//        encrypted_msg[i] ^= encrypted_key_bin[i];
    }
    std::cout<<"原始消息为:"<<msg_binstr_<<std::endl;
    std::cout<<"原始消息的长度为:"<<len<<std::endl;
    std::cout<<"加密后的信息为:"<<encrypted_msg<<std::endl;

    //保存发送内容

    //封装传输消息
    fill(encrypted_msg, std::to_string(len));


//    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //记录源信息长度传输，用作译码截断
//    int len = msg_binstr_.size();
//
//    if (isharq) {
//        //重传直接发送冗余比特
//        std::string encrypted_redundant_parity(msg_binstr_);
//        std::string expand_key(Config::alice_key_buffer);
//        for (int i=0;i<encrypted_redundant_parity.size();i++) {
//            encrypted_redundant_parity[i] ^= expand_key[i] ^ '0';
//        }
//        fill(encrypted_redundant_parity, std::to_string(len));
//        Config::alice_send_parity = false;
//        //需要重传说明当前密钥缓冲区保存的密钥整体KDR过高，舍弃
//        Config::alice_key_buffer.clear();
//        Config::alice_lastkey.clear();
//        return;
//    }
//
//    //扩展源信息
//    while (msg_binstr_.size() < ldpc_code.k * ldpc_code.zc){
//        int n = random()%2;
//        msg_binstr_ += n + '0';
//    }
//
//    //编码得到01字符串，包含冗余比特
//    std::string full_enc = ldpc_redundancy.LdpcEncoder(msg_binstr_);
//    std::string encoded_msg = full_enc.substr(0, ldpc_code.n * ldpc_code.zc);
//    if (ldpc_code.n * ldpc_code.zc != full_enc.size()) Config::redundant_parity = full_enc.substr(ldpc_code.n * ldpc_code.zc);
//    else Config::redundant_parity = "";
//
//    //密钥
//    // 利用未校验的csi生成密钥,并加入循环密钥缓冲区
//    std::string temp_key = Config::key.get_uncheked_key();
////    std::string temp_key = Config::zuc_generate_key_hex[Config::zuc_generate_key_hex.size()-1];
//    if (Config::alice_key_buffer.size() == 0) {
//        //缓冲区为空则将当前密钥复制后拼接再使用
//        std::string t(temp_key);
//        while (t.size() < encoded_msg.size()) t += temp_key;
//        Config::alice_key_buffer = t.substr(0, ldpc_code.n * ldpc_code.zc);
//        Config::alice_key_buffer_idx = temp_key.size();
//        Config::alice_lastkey = temp_key;
//    }
//    else {
//        //缓冲区非空则先把密钥写入缓冲区
//        auto buffer_end = Config::alice_key_buffer.end();
//        auto buffer_begin = Config::alice_key_buffer.begin();
//        auto ptr = buffer_begin + Config::alice_key_buffer_idx;
//        std::string lastkey(temp_key.size(), 0);
//        for (int i = 0; i < temp_key.size(); i++) {
//            lastkey[i] = *ptr;
//            *ptr = temp_key[i];
//            if (++ptr == buffer_end) ptr = buffer_begin;
//        }
//        Config::alice_key_buffer_idx = ptr - buffer_begin;
//        Config::alice_lastkey = lastkey;
//    }
//    std::string expand_key(Config::alice_key_buffer);
//
///*    std::string expand_key = temp_key;
//    std::string unchecked_csi_binstr = expand_key;
//    //防止密钥数不够
//    while (expand_key.size() < encoded_msg.size()) expand_key += unchecked_csi_binstr;
//*/
//
//    //密钥异或编码结果
//    std::string encrypted_encoded_msg(encoded_msg);
//    for (int i=0;i<encoded_msg.size();i++) {
//        encrypted_encoded_msg[i] ^= expand_key[i] ^ '0';
//    }
//
//    //保存发送内容
//
//    //封装传输消息
//    fill(encrypted_encoded_msg, std::to_string(len));
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
