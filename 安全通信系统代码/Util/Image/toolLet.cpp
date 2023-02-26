//
// Created by calt on 18-10-18.
//

#include <Util/Digest/Sha512/Sha512.h>
#include "toolLet.h"
#include "Util/UtilForZuc/util.h"


/**
 * 按照key进行对图片进行置换
 * reverse为true，则从最后一个像素开始变换，通常来说加密和解密分别为false和true
 * @return
 */
QImage substitute(const QImage &origin_img, const std::string &key, const QVector<QRgb> &gray_color_table, bool reverse) {
    if (key.empty()) return origin_img;
    Convert convert;
//    std::cout<<"====================================================================="<<key<<std::endl;
//    std::cout<<"key="<<key<<std::endl;
//    std::cout<<"====================================================================="<<key<<std::endl;
    unsigned char *keys = (unsigned char *) key.c_str();//

    QSize origin_img_size = origin_img.size();
    int width = origin_img_size.rwidth();
    int height = origin_img_size.rheight();

    // 返回指向第一个像素数据的指针
    const unsigned char *origin_img_data = origin_img.bits();
    // QImage::Format_Indexed8->使用8位索引将图像存储到彩色图中
    // https://blog.csdn.net/weixin_43294620/article/details/122419099
    QImage substitute_img(origin_img_size, QImage::Format_Indexed8);

    // 返回指向第一个像素数据的指针
    unsigned char *xor_img_data = substitute_img.bits();

    // 将原始图片的像素数据origin_img_data传到xor_img_data中
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            *xor_img_data = *origin_img_data;
            xor_img_data++;
            origin_img_data++;
        }
    }

    // 回到第一个像素点
    xor_img_data = substitute_img.bits();
//    std::cout<<"<------------------------------------"<<Config::init_zucKey_seq<<"------------------------------------>"<<std::endl;
//    if (!reverse) {
////        std::cout<<"转成char字符的加密图片的密钥为"<<convert.binstr2str(key)<<std::endl;
////        std::cout<<"转成16进制的加密图片的密钥为"<<BinStrToHexStr(key)<<std::endl;
//        std::cout<<"转成2进制的加密图片的密钥为"<<key<<std::endl;
//        std::cout<<"加密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//        std::cout<<"加密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//        std::cout<<"加密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//    }else {
//        //解密
////        std::cout<<"转成char字符的解密图片的密钥为"<<convert.binstr2str(key)<<std::endl;
////        std::cout<<"转成16进制的解密图片的密钥为"<<BinStrToHexStr(key)<<std::endl;
//        std::cout<<"转成2进制的解密图片的密钥为"<<key<<std::endl;
//        std::cout<<"解密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//        std::cout<<"解密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//        std::cout<<"解密图片的密钥长度为"<<key.size()<<" bit"<<std::endl;
//    }
        int key_cnt = 0;
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++){
                //优化
                unsigned char *current_pixel = xor_img_data + i * width + j;
                std::string current_pixel_bin = convert.dec2bin(*current_pixel,8);
                std::string current_pixel_xor_bin = "";
                for (int  k= 0; k < current_pixel_bin.size(); k++) {
                    int index = (key_cnt + k) % key.size();
                    char temp = current_pixel_bin[k] ^ keys[index];
//                    char temp = keys[index];
                    if (temp == 0) {
                        current_pixel_xor_bin += "0";
                    }
                    else {
                        current_pixel_xor_bin += "1";
                    }
                }
                *current_pixel = (unsigned char)convert.binstr2dec(current_pixel_xor_bin);

                key_cnt+=current_pixel_xor_bin.size();
            }
        }


    substitute_img.setColorTable(gray_color_table);

    return substitute_img;
}

/**
 * image xor
 * @param origin_img
 * @param key
 * @param use_origin: if use_origin == true, then use origin to as last; or not, use xor to as last;
 *        usage: when encryption, use_origin should be false; when decryption, use_orgin should be true
 * @return
 */
QImage gray_xor(const QImage &origin_img, const std::string &key, bool use_origin, const QVector<QRgb> &gray_color_table) {

    if (key.empty()) return origin_img;

    QSize origin_img_size = origin_img.size();
    int width = origin_img_size.rwidth();
    int height = origin_img_size.rheight();
    const unsigned char *origin_img_data = origin_img.bits();

    QImage xor_img(origin_img_size, QImage::Format_Indexed8);
    unsigned char *xor_img_data = xor_img.bits();

    int bits = origin_img.byteCount() * 8;

    int partition_num = bits / 512 + 1;
    std::string key_expand = sha512(key, partition_num);

    //下边这个for循环是直接取用green绿色分量值 来作为gray索引值
    int key_cnt = 0;
    Convert converter;
    const unsigned char *last_xor_img_data;
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
//                unsigned char bit8 = converter.binstr2dec(key_expand.substr(key_cnt, 8))
            unsigned char bit8 = key_expand[key_cnt] << 4 | key_expand[key_cnt+1];
            *xor_img_data = (0xFF ^ *origin_img_data) ^ bit8;
//            if (key_cnt != 0) {
//                *xor_img_data = *last_xor_img_data ^ *xor_img_data;
//            }
//            if (use_origin) {
//                last_xor_img_data = origin_img_data;
//            } else {
//                last_xor_img_data = xor_img_data;
//            }
            origin_img_data += 1;
            xor_img_data++;
            key_cnt += 2;
        }
    }

    xor_img.setColorTable(gray_color_table);

    return xor_img;
};
