/*
	������־��
	1.������Ŀ�������ز�
	2.�����Ŀ�Ŀ�ܣ������Щ�ࣩ
*/

/*
	ʹ��ͼ�ο�����ʾ���̡����ӵȣ�easyx
	EasyX ����� C/C++ ��ͼ�ο⣬���԰���ʹ��C/C++���Եĳ���Ա��������ͼ�κ���Ϸ��̡�
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