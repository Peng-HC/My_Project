//
// Created by calt on 18-6-25.
//

#include "Crc.h"

Crc::Crc(long data, long generate):data(data), generate(generate){}

void Crc::calc(long data_, long generate_) {
    data = data_;
    generate = generate_;
    calc();
}

void Crc::calc() {
    Convert converter;

    std::string divisor = converter.dec2bin(generate);
    long x = data << (divisor.size() - 1);

    std::string dividend = converter.dec2bin(x);

    while (dividend.size() >= divisor.size()) {
        long tmp = generate << (dividend.size() - divisor.size());
        x = x ^ tmp;
        dividend = converter.dec2bin(x);
    }

    redundance = converter.dec2bin(x, divisor.size() - 1);//计算出冗余码
    origin = converter.dec2bin(data);//计算出信息位
    code = origin + redundance;//得到发送的信息数据
}

void Crc::calc(std::string dinary_data, std::string generate_) {

    Convert converter;
    data = converter.binstr2dec(dinary_data);//原二进制信息比特流字符串转化成十进制
    generate = converter.binstr2dec(generate_);//原二进制生成多项式字符串转化为十进制

    calc();//计算出冗余码，信息位，加上冗余码的信息

    //补零
    if (code.size() - redundance.size() < dinary_data.size()) {
        std::string padding(dinary_data.size() - (code.size() - redundance.size()), '0');
        code = padding + code;
    }

    if (origin.size() < dinary_data.size()) {
        std::string padding(dinary_data.size() - origin.size(), '0');
        origin = padding + origin;
    }
}