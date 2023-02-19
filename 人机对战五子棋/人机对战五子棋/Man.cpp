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
	//���ӣ�Ĭ�������ú���
	chess->chessDown(&pos, CHESS_BLACK);

}
