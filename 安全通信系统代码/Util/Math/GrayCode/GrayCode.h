//
// Created by calt on 18-6-24.
//

#ifndef ROI_GRAYCODE_H
#define ROI_GRAYCODE_H

#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <iostream>

class GrayCode {
public:
    std::vector<std::string> getWholeGrayStr(int n) {
        int m = std::pow(2, n);
        std::vector<std::string> str(m);

        if (n < 1) return {};

        if(n == 1){
            str[0]="0";
            str[1]="1";
            return str;
        }

        std::vector<std::string> pre_gray = getWholeGrayStr(n-1);
        for(int i=0;i<pre_gray.size();i++){
            str[i]="0" + pre_gray[i];
            str[m-1-i]="1" + pre_gray[i];
        }
        return str;
    }

    std::vector<int> getWholeGrayInt(int n) {
        std::vector<int> res;
        for(int i=0;i<(1<<n);i++){
            res.push_back(i^(i>>1));
        }
        return res;
    }

    //获得格雷码的int型
    int dec2gray_int (int n) {
        return n^(n>>1);
    }

    std::string dec2gray_str (int n, int digit) {
        int gray_int = dec2gray_int(n);
        //int转为32为比特序列
        std::bitset<sizeof(int) * 8> t(gray_int);
        //取字符串的后三位作为格雷码
        return t.to_string().substr(sizeof(int) * 8 - digit);
    }
};


#endif //ROI_GRAYCODE_H
