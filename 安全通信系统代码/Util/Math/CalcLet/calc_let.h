//
// Created by ruiy on 18-6-26.
//

#ifndef ROI_CALC_LET_H
#define ROI_CALC_LET_H

#include <vector>
#include <cfloat>
#include <string>

float find_min(const std::vector<float> &data);
float find_max(const std::vector<float> &data);
float avg(const std::vector<float> &data);
int mod_binary(long data, long generate);
std::string string_xor(const std::string& text, const std::string& key);
#endif //ROI_CALC_LET_H
