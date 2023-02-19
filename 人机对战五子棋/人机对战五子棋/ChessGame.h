#pragma once
#include "Man.h"
#include "Man2.h"
#include "AI.h"
#include "Chess.h"

class ChessGame
{
public:
	//���캯�������ݽ��г�ʼ��
	ChessGame(Man* man, AI* ai, Chess* chess);
	ChessGame(Man* man, Man2* man2, Chess* chess);
	//��ʼ�Ծ�
	void play();
private:
	Man* man;
	Man2* man2;
	AI* ai;
	Chess* chess;
};
