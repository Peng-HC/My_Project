//
// Created by ruiy on 21-1-24.
//

#include "SendEncryptedImageMsg.h"
#include <iostream>
#include <fstream>
#include <QtGui/QImage>
#include <Util/Image/toolLet.h>

#include <Util/Digest/Sha512/Sha512.h>

/**
 * 图片信息加密部分
 */



/**
 * 工具函数
 */
std::string SendEncryptedImageMsg::file_do_to_string(bool print) {
    std::string res;
    res += send_file + "\n";
    res += recv_file + "\n";
    res += size + "\n";
    // 每一行以\n结尾
    res += content + "\n";

    if (print) {
        std::cout << "SendEncryptedMsg::file_do_to_string的res = " << res << std::endl;
    }

    return res;
}

void SendEncryptedImageMsg::print_mem() {
    std::cout << "send_file: " << send_file << std::endl;
    std::cout << "recv_file: " << recv_file << std::endl;
    std::cout << "size: " << size << std::endl;
    std::cout << "content: " << content << std::endl;
}

//封装alice发给bob的加密信息:文件夹名称+内容+size
void SendEncryptedImageMsg::fill_file(std::string send_file_,std::string recv_file_){

    send_file = send_file_;
    recv_file = recv_file_;

    std::ifstream is(send_file, std::ios::in | std::ios::binary);
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        content.append(buf, is.gcount());
    }

    is.close();

    //int转字符串
    size = std::to_string(content.size());
}

//接收解析部分
std::vector<std::string> SendEncryptedImageMsg::separated(const char* in){

    //分隔符(换行符)
    const char* delim = "\n";
    //转换类型，const char* -> char*
    char* in2 = const_cast<char*>(in);

    std::vector<std::string> res;
    char* s = strtok(in2,delim);
    //按照分隔符将
    while (s != NULL){
        res.push_back(s);
        s = strtok(NULL,delim);
    }
    return res;
}

//Bob接收到信息之后封装到本地
void SendEncryptedImageMsg::assign(const char* in) {

    std::istringstream ins(in);

    std::getline(ins,send_file);
    std::getline(ins,recv_file);
    std::getline(ins,size);

    //字符串转int
    int file_size = boost::lexical_cast<int>(size);

    //总体是将string转化为char数组
    //先把string转换为const char*,然后复制到char数组
    char* file_char = new char[file_size];
    ins.read(file_char, file_size);

    for (int i = 0; i < file_size; i++){
        if (file_char[i] == 'A')
            file_char[i] = '\0';
    }

    content.clear();
    content.append(file_char,file_size);
    delete [] file_char;

    //将content放进recv_file中,生成本地的Config::tmp_encrypted_image文件
    std::ofstream out_file(recv_file, std::ios::out | std::ios::binary);
    out_file.write(content.c_str(), content.size());
    out_file.close();
}

//Bob处理接收到的图片加密信息, 解密之后显示到UI
void SendEncryptedImageMsg::server_process(const char* in) {

//    assign(in);
//    std::cout << "我在server画图里------------------" << std::endl;
//
//    std::string key_path = Config::tmp_key_file;
//    std::ifstream is(key_path, std::ios::in | std::ios::binary);
//    char buf[512];
//    std::string key;
//    while (is.read(buf, sizeof(buf)).gcount() > 0) {
//        key.append(buf, is.gcount());
//    }
//    is.close();
//
//    QImage origin_img(Config::tmp_encrypted_image.c_str());
//    QImage xor_img = substitute(origin_img, key, Config::gray_color_table, true);
//    QImage xor_img2 = substitute(xor_img, key, Config::gray_color_table, true);
//    xor_img2.save(Config::tmp_decrypted_image.c_str(), "bmp");
//
//    emit plot_decrypted_image_signal();

/**********************************************************************************************************/

    assign(in);
    std::cout << "我在server画图里------------------" << std::endl;
    std::string decrypted_key=Config::zuc_generate_key_bin[Config::zuc_generate_key_bin.size()-1];
//    std::string decrypted_key=Config::init_zuc_key[Config::init_zuc_key.size()-1];

//    std::string decrypted_key_sha512 = sha512(decrypted_key, 8);

//    std::cout<<"********************************************************"<<std::endl;

//    std::cout<<"第"<<Config::init_zucKey_seq<<"次生成的zuc解密密钥(16进制):"<<decrypted_key<<std::endl;
//    std::cout<<"第"<<Config::init_zucKey_seq<<"次生成的zuc解密密钥作sha512得到:"<<decrypted_key_sha512<<std::endl;
//
//    std::cout<<"********************************************************"<<std::endl;

    QImage origin_img(Config::tmp_encrypted_image.c_str());

    // 直接对图片进行异或解密
//    EncryptionImage(Config::tmp_encrypted_image.c_str(),Config::tmp_decrypted_image.c_str(),decrypted_key_sha512);

    QImage xor_img = substitute(origin_img, decrypted_key, Config::gray_color_table, true);
//    QImage xor_img2 = substitute(xor_img, decrypted_key_sha512, Config::gray_color_table, true);
//    xor_img2.save(Config::tmp_decrypted_image.c_str(), "bmp");
    xor_img.save(Config::tmp_decrypted_image.c_str(), "bmp");

    emit plot_decrypted_image_signal();

}

//Alice处理完原始图片得到加密图片后进行显示UI
void SendEncryptedImageMsg::client_process(std::string &key_code) {
    std::cout << "我在client画图里------------------" << std::endl;
    // 绘制图片
    QImage origin_img(Config::image_path.c_str());


    // 直接对图片进行异或加密
//    EncryptionImage(Config::image_path.c_str(),Config::tmp_encrypted_image.c_str(),key_code);

    QImage xor_img = substitute(origin_img, key_code, Config::gray_color_table, false);
//    QImage xor_img2 = substitute(xor_img, key_code, Config::gray_color_table, false);
//
//    xor_img2.save(Config::tmp_encrypted_image.c_str(), "bmp");
    xor_img.save(Config::tmp_encrypted_image.c_str(), "bmp");

    emit plot_crypted_image_signal();
}

// 加密图片的函数
void SendEncryptedImageMsg::EncryptionImage(const char *in_fname, const char *out_fname, std::string &pwd) {
    FILE *in_file,*out_file;
    register char ch;   // register修饰符暗示编译程序相应的变量将被频繁地使用，如果可能的话，应将其保存在CPU的寄存器中
    int x = pwd.length();
    in_file=fopen(in_fname,"rb");//以读的方式打开二进制文件
    if(in_file==NULL)//如果打开失败
    {
        std::cout<<"打开文件 "<<in_fname<<" 失败"<<std::endl;
        exit(1);
    }
    out_file=fopen(out_fname,"wb");//以写的方式打开二进制文件
    if(out_file==NULL)//如果创建失败
    {
        std::cout<<"创建文件 "<<out_file<<" 失败"<<std::endl;
        exit(1);
    }
    int i=0;
    while(!feof(in_file))
    {
        fputc(ch^pwd[i>=x?i=0:i++],out_file);
        ch=fgetc(in_file);
    }
    fclose(in_file);
    fclose(out_file);
}

// 解密图片的函数
void SendEncryptedImageMsg::DecryptionImage(const char *in_fname, const char *out_fname, std::string &pwd) {
    FILE *in_file,*out_file;
    register char ch;   // register修饰符暗示编译程序相应的变量将被频繁地使用，如果可能的话，应将其保存在CPU的寄存器中
    int x = pwd.length();
    in_file=fopen(in_fname,"rb");//以读的方式打开二进制文件
    if(in_file==NULL)//如果打开失败
    {
        std::cout<<"打开文件 "<<in_fname<<" 失败"<<std::endl;
        exit(1);
    }
    out_file=fopen(out_fname,"wb");//以写的方式打开二进制文件
    if(out_file==NULL)//如果创建失败
    {
        std::cout<<"创建文件 "<<out_file<<" 失败"<<std::endl;
        exit(1);
    }
    int i=0;
    while(!feof(in_file))
    {
        fputc(ch^pwd[i>=x?i=0:i++],out_file);
        ch=fgetc(in_file);
    }
    fclose(in_file);
    fclose(out_file);
}

