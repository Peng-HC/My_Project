#pragma once

#include "base.h"
#include <string>


struct Player {
    //构造函数
    Player() {}
    Player(const std::string& name, int fd, ChessType type) :
        name(name), socketfd(fd), type(type) {}
    Player(const Player& phc) {
        name = phc.name;
        socketfd = phc.socketfd;
        type = phc.type;
        prepare = phc.prepare;
    }
    //重载=操作符
    Player& operator=(const Player& phc) {
        if (this != &phc) {
            this->name = phc.name;
            this->socketfd = phc.socketfd;
            this->type = phc.type;
            this->prepare = phc.prepare;
        }
        return *this;
    }
    //成员变量
    std::string name;
    int socketfd = 0;
    ChessType type = CHESS_BLACK;
    bool prepare = false;
};

struct Watcher {
    Watcher(const std::string& name, int fd) :
        name(name), socketfd(fd) {}
    Watcher(const Watcher& phc) {
        name = phc.name;
        socketfd = phc.socketfd;
    }
    //成员变量
    std::string name;
    int socketfd;
};

