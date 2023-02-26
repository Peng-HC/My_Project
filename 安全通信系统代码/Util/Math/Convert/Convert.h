//
// Created by calt on 18-6-25.
//

#ifndef ROI_CONVERT_H
#define ROI_CONVERT_H


#include <string>
#include <algorithm>
#include <iostream>
#include <map>

class Convert {

public:
    std::map<char, std::string> get_hex_bin () {
        std::map<char, std::string> hex_bin;
        hex_bin['0'] = "0000";
        hex_bin['1'] = "0001";
        hex_bin['2'] = "0010";
        hex_bin['3'] = "0011";
        hex_bin['4'] = "0100";
        hex_bin['5'] = "0101";
        hex_bin['6'] = "0110";
        hex_bin['7'] = "0111";
        hex_bin['8'] = "1000";
        hex_bin['9'] = "1001";
        hex_bin['a'] = "1010";
        hex_bin['b'] = "1011";
        hex_bin['c'] = "1100";
        hex_bin['d'] = "1101";
        hex_bin['e'] = "1110";
        hex_bin['f'] = "1111";
        return hex_bin;
    }

    std::string dec2bin(long n) {
        std::string res;
        while (n) {
            res.push_back((n & 1) + '0');
            n >>= 1;
        }
        if (res.empty()) {
            res = "0";
        } else{
            std::reverse(res.begin(), res.end());
        }
        return res;
    };

    std::string dec2bin(long n, int digit) {
        std::string res = dec2bin(n);
        if (res.size() < digit) {
            std::string padding(digit - res.size(), '0');
            res = padding + res;
        }
        return res;
    }

    long binstr2dec(std::string binary_str) {
        long res = 0;
        for (int i=0;i<binary_str.size();i++) {
            if (binary_str[i] == '1') {
                res += std::pow(2, binary_str.size() - i - 1);
            }
        }
        return res;
    }

    std::string str2binstr (std::string str) {
        std::string binstr;
        for (int i=0;i<str.size();i++) { // 依次将每个char变为01字符串
            binstr += ((str[i] & 0x80) >> 7)? '1': '0';
            binstr += ((str[i] & 0x40) >> 6)? '1': '0';
            binstr += ((str[i] & 0x20) >> 5)? '1': '0';
            binstr += ((str[i] & 0x10) >> 4)? '1': '0';
            binstr += ((str[i] & 0x08) >> 3)? '1': '0';
            binstr += ((str[i] & 0x04) >> 2)? '1': '0';
            binstr += ((str[i] & 0x02) >> 1)? '1': '0';
            binstr += ((str[i] & 0x01) >> 0)? '1': '0';
        }
        return binstr;
    }

    std::string binstr2str (std::string binstr) {
        std::string str = "";
        for (int i=0;i<binstr.size();i+=8) { // 依次将每8个01字符还原为一个char
            char t = ((binstr[i] == '1'? 1: 0) << 7)
                ^ ((binstr[i+1] == '1'? 1: 0) << 6)
                ^ ((binstr[i+2] == '1'? 1: 0) << 5)
                ^ ((binstr[i+3] == '1'? 1: 0) << 4)
                ^ ((binstr[i+4] == '1'? 1: 0) << 3)
                ^ ((binstr[i+5] == '1'? 1: 0) << 2)
                ^ ((binstr[i+6] == '1'? 1: 0) << 1)
                ^ ((binstr[i+7] == '1'? 1: 0) << 0);
            str += t;
        }
        return str;
    }
};


#endif //ROI_CONVERT_H
