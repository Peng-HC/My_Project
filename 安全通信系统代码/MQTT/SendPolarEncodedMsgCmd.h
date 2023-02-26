//
// Created by DYJ on 2020-11-26.
//

#ifndef ROI_SENDPOLARENCODEDMSGCMD_H
#define ROI_SENDPOLARENCODEDMSGCMD_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <QtCore/QObject>
#include <Util/Encode/polar/PolarCode.h>

class SendPolarEncodedMsgCmd: public QObject {

public:

    SendPolarEncodedMsgCmd():
    polar_code(n, info_length, design_epsilon, crc_size)
    {}


    uint8_t n = 10;
    uint16_t info_length = (uint16_t) (1 << (n - 1));

    uint16_t crc_size = 0;
    double design_epsilon = 0.32;
    uint8_t list_size_vec = 4;
    double ebno_vec = 3.0;
    PolarCode polar_code;


    std::string encoded_msg;
    std::string msg_binstr_len;

    void assign(const char *in);
    void server_process(const char* in);
    void client_process(std::string msg_binstr_);

    void fill(const std::string& encoded_msg_, const std::string& msg_binstr_len_);

    std::string fill_do_to_string(bool print);

    void print_mem();
	
};



#endif //ROI_SENDPOLARENCODEDMSGCMD_H
