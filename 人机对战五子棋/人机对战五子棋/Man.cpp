#include "Man.h"
void Man::init(Chess * chess)
{
	this->chess = chess;
}

void Man::go()
{
	MOUSEMSG mouse_msg;
	ChessPos pos;
	
	while (true) {
		//获取鼠标点击消息
		mouse_msg = GetMouseMsg();
		//鼠标左键是否按下
		if (mouse_msg.uMsg == WM_LBUTTONDOWN) {
			printf("(%d，%d)\n", mouse_msg.x, mouse_msg.y);
			//判断落子是否有效
			if (chess->clickBoard(mouse_msg.x, mouse_msg.y, &pos)) {
				break;
			}
		}
	}
	//落子，默认棋手拿黑棋
	chess->chessDown(&pos, CHESS_BLACK);

}
