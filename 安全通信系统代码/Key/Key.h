//
// Created by ruiy on 18-6-26.
//

#ifndef ROI_KEY_H
#define ROI_KEY_H

#include <complex>
#include <vector>
#include <string>
#include <Util/Math/CalcLet/calc_let.h>
#include <boost/any.hpp>


class Key {
    std::vector<std::complex<float>> csi;
    std::vector<float> csi_abs;
    std::vector<std::string> crc_origins;
    std::vector<std::string> crc_redundances;
    std::vector<std::string> crc_marks;
    int right_num;
    std::vector<std::string> keys;

    std::map<std::string, boost::any> extends;
    std::string unchecked_key;


public:
//    Key(const std::vector<std::complex<float>>& csi):
//        csi(csi)
//        {}
    Key(const std::vector<float>&csi_abs):
        csi_abs(csi_abs)
        {}
    void calc_uncheck_key();
    float check_by_redundance(std::vector<std::string> crc_redundance);
    void check_by_marks(std::vector<std::string> crc_marks_);
    std::vector<std::string> get_crc_origins() { return  crc_origins;}
    std::vector<std::string> get_crc_redundances() { return  crc_redundances;}
    std::vector<std::string> get_codes();
    std::vector<std::string> get_crc_marks() { return  crc_marks;}
    std::vector<std::string> get_keys() {return keys;}

    void calc_uncheck_key2();
    std::vector<std::string> separated(const char* in);
    void mixed(std::string code);
    std::string get_uncheked_key() { return unchecked_key;}

//    std::string unchecked_key;

    //将kv移植到这里，因为现有网络部分没有crccheckcmd这个交集处理部分
    void set_extends (std::string key, const boost::any &value){
        extends[key] = value;
    }

    boost::any get_extends (std::string key){
        return extends[key];
    }
};


#endif //ROI_KEY_H
