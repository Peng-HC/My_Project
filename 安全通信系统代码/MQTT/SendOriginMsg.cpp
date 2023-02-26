//
// Created by phc on 2022/12/1 17:20
//
#include <iostream>
#include "SendOriginMsg.h"

/*生成ZUC算法初始密钥时需要的库文件*/
#include <string.h>
#include "Util/UtilForZuc/util.h"
#include "Util/Digest/Sm3/SM3.h"
#include "Util/Digest/Sha256/SHA256.h"
#include "Util/Encode/ldpc/LdpcCode.h"
#include "Util/File/file.h"
#include "Util/UtilForZuc/util.h"
#include "Util/Math/Convert/Convert.h"

/*加解密所需库文件*/

const int HashMapLen=256;
const int CharArrayLen = 4096;

//// ldpc编码或解码所需要的参数
//int BG = 1;
//int ZC = 32;
//int multiple = 1;
//// 用户通过ROI界面获取码率
//float codeRate = Config::LDPC_RATE[Config::ldpc_rate_index];

/*工具函数*/

//alice发给bob的信息
void SendOriginMsg::fill(const std::string& origin_msg_, const std::string& msg_binstr_len_) {
    origin_msg = origin_msg_;
    msg_binstr_len = msg_binstr_len_;
}

void SendOriginMsg::assign(const char* in) {

    std::istringstream ins(in);

    std::getline(ins, origin_msg);
    std::getline(ins, msg_binstr_len);

    std::cout<<"收到的消息:\n"<<origin_msg<<"消息长度："<<msg_binstr_len<<std::endl;
}

std::string SendOriginMsg::fill_do_to_string(bool print) {
    std::string res;
    res += origin_msg + "\n";
    res += msg_binstr_len + "\n";

    if (print) {
        std::cout << "std::string SendOriginMsgCmd::fill_do_to_string的res = " << res << std::endl;
    }

    return res;
}

vector<string> split(const string &s, const string &seperator){
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while(i != s.size()){
        //找到字符串中首个不等于分隔符的字母；
        int flag = 0;
        while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x) {
                if(s[i] == seperator[x]){
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }
        //找到又一个分隔符，将两个分隔符之间的字符串取出;
        flag = 0;
        string_size j = i;
        while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x) {
                if(s[j] == seperator[x]){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
                ++j;
        }
        if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
        }
    }
    return result;
}




// alice端生成2进制的Ma_Expand
std::string SendOriginMsg::get_ma_expand_bin(std::string &Key_Bin, int randomLen) {
    std::string Ma_Expand = "";
    while(Ma_Expand.size() < ldpc_code.k * ldpc_code.zc) {
        std::string Random_Str_Bin = "";
        srand((unsigned)time(NULL));	// 时间作为随机数种子
        for (int i = 0; i < randomLen; i++)
            Random_Str_Bin += rand() % 2 + '0';
        // Sm3散列函数函数用于增强随机性
        std::string Ma = sm3_hash(Key_Bin + Random_Str_Bin);
        Ma_Expand+=Ma;
    }
    return Ma_Expand.substr(0, ldpc_code.k * ldpc_code.zc);
}

//Alice对01信息进行LDPC编码,然后和本端秘钥异或
std::string SendOriginMsg::encrypted_encoded_msg(std::string &msg_binstr_, bool isharq){

//    //记录源信息长度传输，用作译码截断
//    int len = msg_binstr_.size();
    if (isharq) {
        //重传直接发送冗余比特
        std::string encrypted_redundant_parity(msg_binstr_);
        std::string expand_key(Config::alice_key_buffer);
        for (int i=0;i<encrypted_redundant_parity.size();i++) {
            encrypted_redundant_parity[i] ^= expand_key[i] ^ '0';
        }
//        fill(encrypted_redundant_parity, std::to_string(len));
        Config::alice_send_parity = false;
        //需要重传说明当前密钥缓冲区保存的密钥整体KDR过高，舍弃
        Config::alice_key_buffer.clear();
        Config::alice_lastkey.clear();
        // 返回加密的冗余比特!
        return encrypted_redundant_parity;
    }
    //扩展源信息
    while (msg_binstr_.size() < ldpc_code.k * ldpc_code.zc){
        int n = random()%2;
        msg_binstr_ += n + '0';
    }

    std::cout<<"msg_binstr_="<<msg_binstr_<<std::endl;

    //编码得到01字符串，包含冗余比特
    std::string full_enc = ldpc_redundancy.LdpcEncoder(msg_binstr_);
    std::cout<<"full_enc.size()="<<full_enc.size()<<std::endl;
    std::cout<<"full_enc="<<full_enc<<std::endl;
    std::cout<<"full_enc.c_str()="<<full_enc.c_str()<<std::endl;
    printf("full_enc=%s",full_enc.c_str());
    std::string encoded_msg = full_enc.substr(0, ldpc_code.n * ldpc_code.zc);
    if (ldpc_code.n * ldpc_code.zc != full_enc.size()) Config::redundant_parity = full_enc.substr(ldpc_code.n * ldpc_code.zc);
    else Config::redundant_parity = "";

    //密钥
    // 利用未校验的csi生成密钥,并加入循环密钥缓冲区
    std::string temp_key = Config::key.get_uncheked_key();
//    std::string temp_key = Config::zuc_generate_key_hex[Config::zuc_generate_key_hex.size()-1];
/*******************************************************************************************************/
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
    std::string t(temp_key);
    while (t.size() < encoded_msg.size()) t += temp_key;
/*******************************************************************************************************/

//    std::string expand_key(Config::alice_key_buffer);
    std::string expand_key(t);

/*    std::string expand_key = temp_key;
    std::string unchecked_csi_binstr = expand_key;
    //防止密钥数不够
    while (expand_key.size() < encoded_msg.size()) expand_key += unchecked_csi_binstr;
*/

    //密钥异或编码结果
    std::string encrypted_msg(encoded_msg);
    for (int i=0;i<encoded_msg.size();i++) {
        encrypted_msg[i] ^= expand_key[i] ^ '0';
    }

    //返回加密后的编码信息
    return encrypted_msg;
}

//Bob处理接收到的LDPC加密信息
std::string SendOriginMsg::decrypted_decoded_msg(basic_string<char> encoded_msg, bool isharq) {
//    // 解析收到的消息格式
//    assign(in);
//
//    std::string decrypted_key_hex=Config::zuc_generate_key_hex[Config::zuc_generate_key_hex.size()-1];
//    std::string decrypted_key_bin = HexStrToBinStr(decrypted_key_hex);
//    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
//    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
//    std::cout<<"正在使用ZUC密钥流对文字进行解密-------------------------------"<<std::endl;
//    //密钥异或解密结果
//    std::string decrypted_msg=encoded_msg;
//    for (int i=0;i<decrypted_msg.size();i++) {
//        decrypted_msg[i] ^= decrypted_key_bin[i] ^ '0';
////        decrypted_msg[i] ^= decrypted_key_bin[i];
//    }
//    // 二进制转为字符
//    std::string text_msg = BitStrToStr(decrypted_msg);
//
//    QString text(text_msg.c_str());
//    set_message(text);
//    std::cout<<"原始消息为:"<<encoded_msg<<std::endl;
//    std::cout<<"原始消息的长度为:"<<msg_binstr_len<<std::endl;
//    std::cout<<"解密后的信息为:"<<decrypted_msg<<std::endl;
//    std::cout<<"解密后的信息转为字符串为:"<<text_msg<<std::endl;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int iters = -1;
    Convert converter;

    if (isharq) {
        //当前消息为重传的冗余比特
        std::string decrypted_msg(encoded_msg);
        std::string expand_key(Config::bob_key_buffer);
        for (int i=0;i<encoded_msg.size();i++) {
            decrypted_msg[i] ^= expand_key[i] ^ '0';
        }
        //拼接冗余比特到初传码字后进行译码
        std::string fullsize = Config::bob_last_msg + encoded_msg;
        std::string decoded_msg = ldpc_redundancy.LdpcDecoder(fullsize,iters);
        if (iters == maxiter) {
            QString errstr = "HARQ failed, message lost";
            set_message(errstr);
        }
        else {
            std::string msg = converter.binstr2str(decoded_msg.substr(0,boost::lexical_cast<int>(msg_binstr_len)));
            QString text(msg.c_str());
            set_message(text);
            ++Config::correctcws;
        }
        ++Config::totalcws;
        //需要重传说明缓冲区里的密钥整体KDR过高，舍弃
        Config::bob_key_buffer.clear();
        Config::bob_lastkey.clear();
        Config::bob_send_harq = false;
        return "HARQ failed";/////////////////////////////////////////////////////////
    }
    // 利用未校验的csi生成秘钥,并加入循环密钥缓冲区
    std::string temp_key = Config::key.get_uncheked_key();
//    std::string temp_key = Config::zuc_generate_key_hex[Config::zuc_generate_key_hex.size()-1];
/***********************************************************************************************/
//    if (Config::bob_key_buffer.size() == 0) {
//        std::cout<<"Config::bob_key_buffer.size() == 0"<<std::endl;
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
    std::string t(temp_key);
    while (t.size() < encoded_msg.size()) t += temp_key;
/***********************************************************************************************/

//    Config::bob_key_buffer不等于当前扩展的密钥Kb
/*
 *  初步分析：是由于Config::bob_key_buffer与当前的Config::key.get_uncheked_key()不一致导致的
 */
//    std::string expand_key(Config::bob_key_buffer);
    std::string expand_key(t);
/***********************************************************************/
//    std::string t(temp_key);
//    while (t.size() < encoded_msg.size()) t += temp_key;
//    std::string expand_key(t);

/***********************************************************************/

/*    std::string expand_key = temp_key;
    std::string unchecked_csi_binstr = temp_key;
    //防止密钥数不够
    while (expand_key.size() < encoded_msg.size()) expand_key += unchecked_csi_binstr;
*/

    //密钥异或回来密文
    std::cout<<"Kb_Hex_expand_key="<<BinStrToHexStr(expand_key)<<std::endl;
    std::cout<<"Kb大小"<<expand_key.size()<<"bit"<<std::endl;
    std::string decrypted_msg(encoded_msg);
    for (int i = 0; i < encoded_msg.size(); i++) {
        decrypted_msg[i] ^= expand_key[i] ^ '0';
    }

    Config::bob_last_msg = decrypted_msg;
//    Config::bob_last_msg_len = boost::lexical_cast<int>(msg_binstr_len);
    Config::bob_last_msg_len = boost::lexical_cast<int>(decrypted_msg.size());
    //解密
    std::cout<<"decrypted_msg_Hex="<<BinStrToHexStr(decrypted_msg)<<std::endl;
    std::cout<<"decrypted_msg大小"<<decrypted_msg.size()<<"bit"<<std::endl;

    //解码,得到字符串
    std::string decoded_msg = ldpc_code.LdpcDecoder(decrypted_msg, iters);
    Config::totalcws++;
    if (iters != maxiter) Config::correctcws++;
    else {
        //解码失败,该组密钥KDR过高,需要从缓冲区中删除
        std::cout<<"解码失败!!!"<<std::endl;
        Config::bob_send_harq = true;
        auto buffer_end = Config::bob_key_buffer.end();
        auto buffer_begin = Config::bob_key_buffer.begin();
        auto ptr = buffer_begin + Config::bob_key_buffer_idx;
        std::string temp_key = Config::bob_lastkey;
        for (int i = temp_key.size()-1; i >= 0; i--) {
            *ptr = temp_key[i];
            if (ptr == buffer_begin) ptr = buffer_end;
            --ptr;
        }
        //更新指向缓冲区的位置
        Config::bob_key_buffer_idx = ptr - buffer_begin;
    }
//    //恢复信息
//    std::string msg = converter.binstr2str(decoded_msg);
    std::cout << "正确码字 / 总码字 ： " << Config::correctcws << "/" << Config::totalcws << std::endl;
    return decoded_msg;
//    //恢复信息
//    std::string msg = converter.binstr2str(decoded_msg.substr(0,boost::lexical_cast<int>(msg_binstr_len)));
//    QString text(msg.c_str());
//    set_message(text);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

// alice端生成送入zuc算法的初始密钥(待bob端协商)
void SendOriginMsg::alice_sender_originmsg() {
    cout<<endl<< "***************************alice端正在生成发送到bob端的明文消息***************************" << endl;

    std::cout<<"Alice生成zuc初始密钥的第1阶段所花费的时间："<<(double)(1000.0*(Config::alice_zuc_stage1)/CLOCKS_PER_SEC)<<"ms"<<std::endl;
    // 利用未校验的csi生成秘钥expand_key()
    /* 不明白为什么用未经过校验的csi生成的密钥进行加密的,这样alice和bob端加解密得到的结果一致吗*/
    /* 解答：确实不一致,但是经过ldpc编码可以进行修正 */
    std::string Key_Bin = Config::key.get_uncheked_key();
    std::cout<<"-----------------------------------------Warning：Alice端正在利用未校验的密钥Ka生成Ma-----------------------------------------"<<std::endl;
    std::cout<<"Ka_Hex="<<BinStrToHexStr(Key_Bin)<<std::endl;
    std::cout<<"Ka占"<<Key_Bin.size()<<"bit"<<std::endl;

    std::cout<< "---Alice端生成扩展的zuc初始密钥(未协商)---"<<std::endl;
    clock_t start_time_temp=clock();
    std::string Ma_Expand_Bin = get_ma_expand_bin(Key_Bin,256);
    // 注意：对于alice端，只要它生成了zuc初始密钥，就添加进全局变量（此时未校验）
    Config::init_zuc_key.push_back(Ma_Expand_Bin.substr(0,256));
    std::cout << "Ma_Expand_Hex=" <<BinStrToHexStr(Ma_Expand_Bin)<<std::endl;
    clock_t end_time_temp=clock();

    std::cout<<"time1："<<(double)(1000.0*(end_time_temp-start_time_temp)/CLOCKS_PER_SEC)<<"ms"<<std::endl;

    //Alice进行LDPC编码,然后和本端秘钥异或
    //把msg转化为01字符串
//    Convert converter;
//    std::string msg_binstr = converter.str2binstr(Ma_Expand_Bin);
    std::cout<<"Config::ldpc_rate_index="<<Config::ldpc_rate_index<<endl;
    std::cout<<"当前用户选择的码率为:"<<Config::LDPC_RATE_TABLE[Config::ldpc_rate_index]<<endl;
    std::string Encrypted_Encoded_Msg = encrypted_encoded_msg(Ma_Expand_Bin, false);//传入的参数isharq后期需要修改

    /***************************************************************************************************************************************/
//    LdpcCode ldpc_code;
//    //初始化校验矩阵，支持码率为2/3,1/2,1/3,1/4,1/5
//    int BG = Config::ldpc_rate_index > 2 ? 2 : 1;
//    int ZC=128;
//    int multiple=22;
//    float code_rate = Config::LDPC_RATE[Config::ldpc_rate_index];
//    ldpc_code.InitMatrix(BG,ZC,code_rate);
//    ldpc_code.InitConfig();
//
//    cout << "2.生成符合输入到ldpc编码器所需要的比特流Ma'=Ma1+Ma2+..." << endl;
//    std::cout<<"当前用户选择的码率为:"<<code_rate<<endl;
//    if (BG == 1) {
//        multiple = 22;
//    } else if (BG == 2) {
//        multiple = 10;
//    } else {
//        multiple = 0;
//        cout << "LDPC参数设置错误,BG只能等于1或者2" << endl;
//    }
//
//    // 补位
//    int compensationBin = ZC * multiple - Ma_Bin.size();
//    double enctime = 0, time1 = 0;
//    string ldpcInputMsg, ldpcEncMsg;
//    ldpcInputMsg = Ma_Bin;
//    // 需要重复生成Ma的轮数
//    int wheel = compensationBin / Ma_Bin.size();
//    std::cout<<"需要补充"<<compensationBin<<"bit才能用于ldpc编码器的输入端"<<std::endl;
//    std::cout<<"共计需要生成"<<wheel+1<<"轮Ma"<<std::endl;
//    for (int i = 0; i < wheel; i++) {
//        string tempHex = generateMaHexStr(Key_Hex, 256);
//        ldpcInputMsg += HexStrToBinStr(tempHex);
//    }
//    // 剩余不够一轮的比特数
//    int remainBit = compensationBin - wheel * Ma_Bin.size();
//    std::cout<<"剩余不够一轮的比特数为:"<<remainBit<<"bit"<<std::endl;
//    if (remainBit > 0) {
//        string tempHex = generateMaHexStr(Key_Hex, 256);
//        string tempBin= HexStrToBinStr(tempHex);
//        for (int j = 0; j < remainBit; j++) {
//            ldpcInputMsg += tempBin[j];
//        }
//    }
//
//    cout << "3.根据码率进行ldpc编码生成编码信息" << endl;
//
//    // 取前256bit作为zuc初始密钥
//    std::string init_zuc_key="";
//    for(int i=0;i<256;i++) {
//        init_zuc_key+=ldpcInputMsg[i];
//    }
//    // 注意：对于alice端，只要它生成了zuc初始密钥，就添加进全局变量（此时未校验）
//    Config::init_zuc_key.push_back(init_zuc_key);
//
//    auto start1 = std::chrono::system_clock::now();
//    ldpcEncMsg = ldpc_code.LdpcEncoder(ldpcInputMsg);
//    auto end1 = std::chrono::system_clock::now();
//    enctime += double(std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - start1).count());
//
//    std::cout << "ldpc rate=" << ldpc_code.k << '/' << ldpc_code.n << std::endl;
//    std::cout << "ldpc 编码时间: " << time1 / 1e6 << "ms" << std::endl;
//    std::cout << "BinStrToHexStr(ldpcInputMsg)= " << BinStrToHexStr(ldpcInputMsg) << std::endl<<"ldpcInputMsg.size()="<< ldpcInputMsg.size()<<endl;
//    std::cout << "BinStrToHexStr(ldpcEncMsg)=" << BinStrToHexStr(ldpcEncMsg) << std::endl << "ldpcEncMsg.size()=" << ldpcEncMsg.size() << endl;
//
//    cout << "4.使用Ka对上述编码信息进行加密[ E(Ma')^Ka ]" << endl;
//    //防止密钥数不够
//    string Key_Bin_Expand = Key_Bin;
//    while (Key_Bin_Expand.size() < ldpcEncMsg.size()) Key_Bin_Expand += Key_Bin;
//
//    std::cout << "加密的密钥Ka_Hex=" << BinStrToHexStr(Key_Bin_Expand)<<std::endl;
//    std::cout << "扩展后的密钥长度为(比特):" << Key_Bin_Expand.size()<<std::endl;
//
//    string Encryption_Msg = "";
//    for (int i = 0; i < ldpcEncMsg.size(); i++)
//    {
//        char temp= ldpcEncMsg[i] ^ Key_Bin_Expand[i];
//        if (temp == '\0') {
//            Encryption_Msg += "0";
//        }
//        else {
//            Encryption_Msg += "1";
//        }
//    }
    /***************************************************************************************************************************************/

    std::cout << "对ldpc编码进行加密后得到的结果(16进制):" << BinStrToHexStr(Encrypted_Encoded_Msg)<<std::endl;
    std::cout << "密文长度为(比特):" << Encrypted_Encoded_Msg.size()<<std::endl;

    cout << "将Ma'经过SHA256生成验证信息[ SHA256(Ma') ]" << endl;
    SHA256 hash_SHA256;
    string Ma_Expand_SHA256_Hex = hash_SHA256.hash(Ma_Expand_Bin);
    string Ma_Expand_SHA256_Bin = HexStrToBinStr(Ma_Expand_SHA256_Hex);
    cout << "生成的验证信息:" << Ma_Expand_SHA256_Hex << endl;
    cout << "生成的验证信息大小:" << Ma_Expand_SHA256_Bin.size() <<"bit"<< endl;

    cout << "将加密后的编码信息与验证信息连接起来得到发送给bob端的明文信息[ E(Ma')^Ka+SHA256(Ma') ]" << endl;

//    string Alice_Sender_Msg = Encrypted_Encoded_Msg + Ma_Expand_SHA256_Bin;
    //为了计算KDR,需要传密钥Ka,后期需要删除+

    string Alice_Sender_Msg = Config::key.get_uncheked_key() + Encrypted_Encoded_Msg + Ma_Expand_SHA256_Bin;
    int len=Alice_Sender_Msg.size();
    cout << "Alice端发送的明文信息大小:" << Alice_Sender_Msg.size()<<"bit"<< endl;
    cout << "Alice端发送的明文信息(16进制):" << BinStrToHexStr(Alice_Sender_Msg)<< endl;
    cout << "Alice端发送的明文信息(2进制):" << Alice_Sender_Msg<< endl;

    //封装传输消息
    fill(Alice_Sender_Msg, std::to_string(len));
    std::cout<<std::endl<<"******************************************************"<<std::endl;
}

// bob端处理从alice端发送过来的明文消息,判断此次zuc初始密钥是否有效
void SendOriginMsg::bob_msg_handler(const char* in) {
    // 解析收到的消息格式
     assign(in);

    // bob端收到alice发送的zuc初始密钥协商信息
//    generate_zuc_init_key_start_time = clock();
    cout << endl << endl << "***************************bob端处理接受到的信息并且将获得的初始密钥送入Zuc算法中获得流密钥***************************" << endl << endl;

//    // 减去末尾的2bit,不明白为什么末尾会多出bit
//    std::string temp=Bob_Receiver_Msg;
//    Bob_Receiver_Msg="";
//    for(int i=0;i<temp.size()-2;i++) {
//        Bob_Receiver_Msg+=temp[i];
//    }
    std::string Bob_Receiver_Msg = origin_msg;

    std::cout << "Bob端接收到的消息:" << BinStrToHexStr(Bob_Receiver_Msg)<<std::endl;
    std::cout << "Bob端接收到的消息的长度(bit):" <<Bob_Receiver_Msg.size()<<std::endl;

    //取出Ka,计算KDR
    string Ka = Bob_Receiver_Msg.substr(0,Config::key.get_uncheked_key().size());
    cout<<"Ka_Hex="<<BinStrToHexStr(Ka)<<endl;
    string Kb = Config::key.get_uncheked_key();
    cout<<"Kb_Hex="<<BinStrToHexStr(Kb)<<endl;
    int right_cnt=0;
    int sum_cnt=Config::key.get_uncheked_key().size();
    for(int i=0;i<Kb.size();i++) {
        if(Ka[i]==Kb[i]) {
            right_cnt++;
        }
    }
    cout<<"right_cnt="<<right_cnt<<endl;
    cout<<"sum_cnt="<<sum_cnt<<endl;
//    cout<<"正确率:"<<float(right_cnt/sum_cnt)<<endl;
    float KDR = 100.0*float(sum_cnt-right_cnt)/float(sum_cnt);
    cout<<"KDR:"<<KDR<<"%"<<endl;

//    string Decoder_Bin = decrypted_decoded_msg(Bob_Receiver_Msg.substr(0,boost::lexical_cast<int>(msg_binstr_len)-256),false);
    //注意：如果alice端传递过来的消息中前1024位为Ka，则必须减去256bit后再减去1024才能得到中间的加密后的编码信息！否则会导致测序崩溃!
    std::string getEncodeMsg = Bob_Receiver_Msg.substr(Config::key.get_uncheked_key().size(),boost::lexical_cast<int>(msg_binstr_len)-Ka.size()-256);
    std::cout<<"Bob端获得的编码长度"<<getEncodeMsg.size()<<std::endl;
    std::cout<<"Bob端获得的编码长度"<<getEncodeMsg.size()<<std::endl;
    std::cout<<"Bob端获得的编码长度"<<getEncodeMsg.size()<<std::endl;
    string Decoder_Bin = decrypted_decoded_msg(getEncodeMsg,false);
    std::cout << "Bob端解密解码后的消息长度为:" <<Decoder_Bin.size()<<"bit"<<std::endl;
    std::cout << "Bob端解密解码后的消息为:" <<BinStrToHexStr(Decoder_Bin)<<std::endl;
    SHA256 hash_SHA256;
    string Mb_SHA256_Hex = hash_SHA256.hash(Decoder_Bin);
    string Valid_Msg_Bin = Bob_Receiver_Msg.substr(boost::lexical_cast<int>(msg_binstr_len)-256,256);
    string Valid_Msg_Hex = BinStrToHexStr(Valid_Msg_Bin);
    std::cout << "Bob端收到的的验证消息长度:" <<Valid_Msg_Bin.size()<<"bit"<<std::endl;
    std::cout << "Bob端收到的的验证消息为:" <<Valid_Msg_Hex<<std::endl;
//    for (int i = 0; i < Bob_Receiver_Msg.size() - HashMapLen; i++) {
//        if (Bob_Receiver_Msg[i] == '0') {
//            decoder += "0";
//        }
//        else {
//            decoder += "1";
//        }
//    }


//    for (int i = Bob_Receiver_Msg.size() - HashMapLen; i < Bob_Receiver_Msg.size(); i++) {
//        if (Bob_Receiver_Msg[i] == '0') {
//            Bin_Valid_Msg += "0";
//        }
//        else {
//            Bin_Valid_Msg += "1";
//        }
//    }

//    string Valid_Msg_Hex = BinStrToHexStr(Valid_Msg_Bin);
//    std::string keyB_Bin = Config::key.get_uncheked_key();
//    std::string KeyB_Hex=BinStrToHexStr(keyB_Bin);
//    // 防止密钥数不够
//    string KeyB_Bin_Expand = keyB_Bin;
//    while (KeyB_Bin_Expand.size() < decoder.size()) KeyB_Bin_Expand += keyB_Bin;
//    std::cout << "加密的密钥Kb=" << BinStrToHexStr(KeyB_Bin_Expand)<<std::endl;
//    std::cout << "Kb的长度为:" << keyB_Bin.size()<<"bit"<<std::endl;
//    std::cout << "扩展后的密钥Kb长度为(比特):" << KeyB_Bin_Expand.size()<<std::endl;
//
//    string decryptionMsg = "";
//    for (int i = 0; i < decoder.size(); i++) {
//        char temp = decoder[i] ^ KeyB_Bin_Expand[i];
//        if (temp == '\0') {
//            decryptionMsg += "0";
//        }
//        else {
//            decryptionMsg += '1';
//        }
//    }

//    std::cout << "Bob端对前半部分进行解密后得到的结果:" << BinStrToHexStr(decryptionMsg)<<std::endl;
//    std::cout << "解密后的长度(比特):" << decryptionMsg.size()<<std::endl;
    std::cout<<"Config::ldpc_rate_index="<<Config::ldpc_rate_index<<endl;
    std::cout<<"当前用户选择的码率为:"<<Config::LDPC_RATE_TABLE[Config::ldpc_rate_index]<<endl;

//    string message_enc2 = decryptionMsg;
////    ldpc.InitMatrix(BG,ZC,codeRate);
////    ldpc.InitConfig();
//
//    double time2 = 0;
//    auto start2 = std::chrono::system_clock::now();
//    int iters = -1;
//    string ldpcDecMsg = ldpc_code.LdpcDecoder(message_enc2,iters);
//    auto end2 = std::chrono::system_clock::now();
//    double dectime = 0;
//    dectime += double(std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2).count());
//
//    std::cout << "Rate : " << ldpc_code.k << '/' << ldpc_code.n << std::endl;
//    std::cout << "Decode Time : " << time2 / 1e6 << "ms" << std::endl;
//    std::cout << "ldpcDecMsg.size():" << ldpcDecMsg.size() << endl;
//    cout << "ldpcDecMsg_hex:" << BinStrToHexStr(ldpcDecMsg) << endl;
//    string Mb_Hex = BinStrToHexStr(Decoder_Bin);



//    bool isValidZucInitKey=false; // Bob端得到的Mb是否为有效的ZUC初始密钥
    Config::init_zucKey_seq++;
    cout << "Mb_SHA256_Hex=" << Mb_SHA256_Hex << endl;
    cout << "Valid_Msg_Hex=" << Valid_Msg_Hex << endl;
    if (Mb_SHA256_Hex == Valid_Msg_Hex) {
        cout << "Congratulations,PHC!成功生成ZUC初始密钥"<<endl;
        Config::isSuccess=true;
        cout << endl << endl << "***************************根据zuc初始密钥使用zuc算法生成流密钥的过程***************************" << endl << endl;

        // 取前256bit作为zuc初始密钥
        std::string init_zuc_key="";
        for(int i=0;i<256;i++) {
            init_zuc_key+=Decoder_Bin[i];
        }
        Config::init_zuc_key.push_back(init_zuc_key);
        if(Config::last_init_zucKey_seq!=Config::init_zucKey_seq) {
            Config::bob_zucSuccessSeq[Config::init_zucKey_seq] = init_zuc_key;
            Config::last_init_zucKey_seq = Config::init_zucKey_seq;
        }


        //先不急着存储密钥,等验证成功了再存储
//        save_zucInitKey(init_zuc_key, Config::tmp_zuc_init_key_file, Config::zuc_key_dir, get_global_current_time(), true,true);

        std::string zuc_key_bin=generate_zuc_key(init_zuc_key);
        Config::zuc_generate_key_bin.push_back(zuc_key_bin);
//        save_zucInitKey(zuc_key_hex, Config::tmp_zuc_stream_key_file, Config::zuc_key_dir, get_global_current_time(), false,true);
    }else {
        Config::isSuccess=false;
        cout << "验证信息与SHA256(Mb’)不一致,无法用作ZUC初始密钥!"<<endl;
    }
};

// bob端根据Config::isSuccess生成发送给alice端的zuc初始密钥协商消息
void SendOriginMsg::bob_sender_originmsg() {
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;
    std::string feedback = "";
    int len = 0;
    if(Config::isSuccess) {
        // 发送明文消息告知alice端此次密钥会话成功,可以保留初始zuc密钥并用于计算
        feedback = "T"+to_string(Config::init_zucKey_seq);

    } else {
        // 发送明文消息告知alice端此次密钥会话失败，并删除此次密钥
        feedback = "F"+to_string(Config::init_zucKey_seq);
    }
    //封装传输消息
    len = feedback.size();
    fill(feedback, std::to_string(len));
}


// alice处理从bob端发送过来的明文信息
void SendOriginMsg::alice_msg_handler(const char* in) {
    // 解析收到的消息格式
    assign(in);
//    std::string temp=Alice_Receiver_Msg;
//    Alice_Receiver_Msg="";
//    for(int i=0;i<temp.size();i++) {
//        Alice_Receiver_Msg+=temp[i];
//    }
    std::cout << "Alice端接收到的消息:" << origin_msg<<std::endl;
    std::cout << "Alice端接收到的消息的长度:" <<msg_binstr_len<<std::endl;

    Config::isSuccess= (origin_msg[0] == 'T')?true:false;
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;
    std::cout<<"Config::isSuccess="<<Config::isSuccess<<std::endl;

    Config::init_zucKey_seq=atoi(origin_msg.substr(1).c_str());
    if(Config::isSuccess) {
        string cur_init_zuc_key=Config::init_zuc_key[Config::init_zuc_key.size()-1];
        //Alice端:zuc协商成功的序号
        if(Config::last_init_zucKey_seq!=Config::init_zucKey_seq) {
            Config::alice_zucSuccessSeq.push_back(Config::init_zucKey_seq);
            Config::last_init_zucKey_seq = Config::init_zucKey_seq;
        }


        save_zucInitKey(cur_init_zuc_key, Config::tmp_zuc_init_key_file, Config::zuc_key_dir, get_global_current_time(), true,true);
        std::string zuc_key_bin=generate_zuc_key(cur_init_zuc_key);
        // zuc密钥流直接使用2进制
        Config::zuc_generate_key_bin.push_back(zuc_key_bin);
//        save_zucInitKey(zuc_key_hex, Config::tmp_zuc_stream_key_file, Config::zuc_key_dir, get_global_current_time(), false,true);
    } else {
        // bob端校验zuc初始密钥无效，alice端需要删除变量中存放的zuc初始密钥
        if(!Config::init_zuc_key.empty()) {
            Config::init_zuc_key.pop_back();
        }
    }
};

// Alice端发送zuc协商成功的序号
void SendOriginMsg::alice_sender_zucSucessSeq() {
    cout<<endl<< "***************************alice端正在生成发送到bob端的zuc协商成功序号***************************" << endl;
//    if(Config::alice_zucSuccessSeq.size()<=0) {
//
//    }
    std::string Alice_Sender_ZucSucessSeq;
    int nums = Config::alice_zucSuccessSeq.size();
    for(int i=0;i<nums;i++) {
        Alice_Sender_ZucSucessSeq+= to_string(Config::alice_zucSuccessSeq[i]);
        if(i!=nums-1) {
            Alice_Sender_ZucSucessSeq+=",";
        }
    }
    int len=Alice_Sender_ZucSucessSeq.size();
    //封装传输消息
    fill(Alice_Sender_ZucSucessSeq, std::to_string(len));
    std::cout<<"Alice_Sender_ZucSucessSeq = "<<Alice_Sender_ZucSucessSeq<<std::endl;
    std::cout<<std::endl<<"******************************************************"<<std::endl;
};

// bob端
void SendOriginMsg::bob_zucSuccessSeq_handler(const char* in) {
    // 解析收到的消息格式
    assign(in);

    // bob端收到alice发送的zuc初始密钥协商信息
    cout << endl << endl << "***************************bob端处理接受到的信息并且将获得的初始密钥送入Zuc算法中获得流密钥***************************" << endl << endl;
    std::string Bob_Receiver_Msg = origin_msg;
    vector<std::string> temp;
    temp = split(Bob_Receiver_Msg,",");
    for(int i=0;i<temp.size();i++) {
        Config::alice_zucSuccessSeq.push_back(atoi(temp[i].c_str()));
    }

    std::cout <<std::endl<< "Bob端接收到的消息:" << Bob_Receiver_Msg<<std::endl;

    std::string absolution_key = "";
//    bob端收到序号，开始存储
    for(int i=0;i<Config::alice_zucSuccessSeq.size();i++) {
        int seq = Config::alice_zucSuccessSeq[i];
        if(Config::bob_zucSuccessSeq[seq]!="") {
            absolution_key+=Config::bob_zucSuccessSeq[seq];
            Config::bob_zucSuccessSeq[seq]="";//标记已经存储了
        }
    }
    save_zucInitKey(absolution_key, Config::tmp_zuc_init_key_file, Config::zuc_key_dir, get_global_current_time(), true,true);
    Config::alice_zucSuccessSeq.clear();
};

