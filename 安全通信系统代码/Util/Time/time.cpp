//
// Created by calt on 18-7-21.
//

#include "time.h"

std::string get_current_time () {
    time_t t = time(0);
    char ch[64];
    strftime(ch, sizeof(ch), "%Y-%m-%d-%H-%M-%S", localtime(&t)); //年-月-日 时-分-秒
    return std::string(ch);
}
