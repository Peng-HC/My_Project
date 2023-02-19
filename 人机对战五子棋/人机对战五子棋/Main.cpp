/*
	开发日志：
	1.创建项目，导入素材
	2.设计项目的框架（设计哪些类）
*/

/*
	使用图形库来显示棋盘、棋子等：easyx
	EasyX 是针对 C/C++ 的图形库，可以帮助使用C/C++语言的程序员快速上手图形和游戏编程。
	https://docs.easyx.cn/zh-cn/intro
*/

#include<iostream>
#include "ChessGame.h"
using namespace std;

int main() {
	Man man = Man();
	Man2 man2 = Man2();
	AI ai = AI();
	Chess chess(13, 44, 43, 67.4);

	ChessGame game(&man, &ai, &chess);
	//ChessGame game(&man, &man2, &chess);
	game.play();
	return 0;
}