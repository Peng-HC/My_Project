#include "Man2.h"

void Man2::init(Chess * chess)
{
	this->chess = chess;
}

void Man2::go()
{
	MOUSEMSG mouse_msg;
	ChessPos pos;

	while (true) {
		//��ȡ�������Ϣ
		mouse_msg = GetMouseMsg();
		//�������Ƿ���
		if (mouse_msg.uMsg == WM_LBUTTONDOWN) {
			printf("(%d��%d)\n", mouse_msg.x, mouse_msg.y);
			//�ж������Ƿ���Ч
			if (chess->clickBoard(mouse_msg.x, mouse_msg.y, &pos)) {
				break;
			}
		}
	}
	//���ӣ�����һ�������ð���
	chess->chessDown(&pos, CHESS_WHITE);
}
