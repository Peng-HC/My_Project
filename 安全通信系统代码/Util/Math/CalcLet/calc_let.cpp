//
// Created by ruiy on 18-6-26.
//

#include <numeric>
#include <Util/Math/Convert/Convert.h>
#include "calc_let.h"

float find_max(const std::vector<float> &data) {
    float max_value = DBL_MIN;
    for (const float &d: data) {max_value = std::max(max_value, d);}
    return max_value;
}

float find_min(const std::vector<float> &data) {
    float min_value = DBL_MAX;
    for (const float &d: data) {min_value = std::min(min_value, d);}
    return min_value;
}

float avg(const std::vector<float> &data) {
    float sum;
    sum = std::accumulate(data.begin(), data.end(), 0.0);
    sum = sum / static_cast<float>(data.size());
    return sum;
}

int mod_binary(long data, long generate) {
    long x =  data;
    Convert converter;
    std::string divisor = converter.dec2bin(generate);

    std::string dividend = converter.dec2bin(x);
    while (dividend.size() >= divisor.size()) {
        long tmp = generate << (dividend.size() - divisor.size());
        x = x ^ tmp;
        dividend = converter.dec2bin(x);
    }

    return x;
}

std::string string_xor(const std::string& text, const std::string& key) {
    std::string expand_key(key);
    while(key.size() < text.size()) expand_key += key;
    std::string res(text);
    
    for (int i=0;i<text.size();i++) {
        res[i] = text[i] ^ key[i];
    }
    return res;
}