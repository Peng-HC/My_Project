#pragma once
#include "Chess.h"
class AI
{
public:
	void init(Chess *chess);
	//AI����
	void go();
	//AI�����̵�ÿ��λ�ý�������
	void calculateScore();
	//AI˼��(�������ӵ�����)
	ChessPos think();
private:
	Chess *chess;
	//�洢AI��������ÿ��λ����������õķ��������ȡ��ߵķ�����Ӧ��λ������
	vector<vector<int>> AI_scores;
};