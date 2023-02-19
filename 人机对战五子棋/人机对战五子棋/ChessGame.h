#pragma once
#include "Man.h"
#include "Man2.h"
#include "AI.h"
#include "Chess.h"

class ChessGame
{
public:
	//构造函数对数据进行初始化
	ChessGame(Man* man, AI* ai, Chess* chess);
	ChessGame(Man* man, Man2* man2, Chess* chess);
	//开始对局
	void play();
private:
	Man* man;
	Man2* man2;
	AI* ai;
	Chess* chess;
};
