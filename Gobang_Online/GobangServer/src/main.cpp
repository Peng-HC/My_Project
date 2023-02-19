#include "gobangserver.h"

#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

GobangServer server;

void handleCtrlC(int num);
void exitFunc();

int main(int argc, char** argv) {
    //atexit注册相对应的回调函数
    atexit(exitFunc);
    //当用户按下ctrl+C键后发送服务器中断信号
    signal(SIGINT, handleCtrlC);

    srand(time(NULL));

    //服务器端口默认为10032
    if (!server.start(10032)) {
        std::cerr << "Start failed!" << std::endl;
        return 1;
    }

    return 0;
}

void handleCtrlC(int num) {
    exit(0);
}

void exitFunc() {
    server.stop();
}

