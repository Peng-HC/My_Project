//
// Created by calt on 18-6-25.
//

#ifndef ROI_CRC_H
#define ROI_CRC_H

#include <string>
#include <Util/Math/Convert/Convert.h>
#include <iostream>

class Crc {
    long data;
    long generate;

    std::string origin;
    std::string redundance;
    std::string code;



public:
    Crc(){};
    Crc(long data, long generate);

    /**
    *
     * @param data
     * @param generate
     * - 4 位 25 =》 11001 =》 x4+x3+1
     * - 12位 6157 =》 1100000001101 =》 x12 + x11 + x3 + x2 + 1
     *
     * */
    void calc(long data_, long generate_);
    void calc () ;
    void calc(std::string dinary_data, std::string generate_);

    std::string get_origin() {return origin;}
    std::string get_redundance() {return redundance;}
    std::string get_code() {return code;}

};

#endif //ROI_CRC_H
