#pragma once
#include "Chess.h"
class AI
{
public:
	void init(Chess *chess);
	//AI走棋
	void go();
	//AI对棋盘的每个位置进行评分
	void calculateScore();
	//AI思考(返回落子的行列)
	ChessPos think();
private:
	Chess *chess;
	//存储AI在棋盘上每个位置上下棋后获得的分数，最后取最高的分数对应的位置落子
	vector<vector<int>> AI_scores;
};