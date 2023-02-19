#include "ChessGame.h"

ChessGame::ChessGame(Man * man, AI * ai, Chess * chess)
{
	this->man = man;
	this->ai = ai;
	this->chess = chess;

	//对棋手进行初始化
	man->init(chess);
	//对AI进行初始化
	ai->init(chess);
}

ChessGame::ChessGame(Man * man, Man2 * man2, Chess * chess)
{
	this->man = man;
	this->man2 = man2;
	this->chess = chess;

	//对棋手进行初始化
	man->init(chess);
	//对另外一个棋手进行初始化
	man2->init(chess);
}

// 对局（开始五子棋游戏）
void ChessGame::play()
{
	//棋盘初始化
	chess->init();
	while (true) {
			
		//先棋手走
		man->go();
		//判断棋局是否结束
		if (chess->checkGameOver()) {
			chess->init();
			continue;
		}
		//再AI走
		ai->go();
		//man2->go();
		//判断棋局是否结束
		if (chess->checkGameOver()) {
			chess->init();
			continue;
		}
	}
}
