#include "ChessGame.h"

ChessGame::ChessGame(Man * man, AI * ai, Chess * chess)
{
	this->man = man;
	this->ai = ai;
	this->chess = chess;

	//�����ֽ��г�ʼ��
	man->init(chess);
	//��AI���г�ʼ��
	ai->init(chess);
}

ChessGame::ChessGame(Man * man, Man2 * man2, Chess * chess)
{
	this->man = man;
	this->man2 = man2;
	this->chess = chess;

	//�����ֽ��г�ʼ��
	man->init(chess);
	//������һ�����ֽ��г�ʼ��
	man2->init(chess);
}

// �Ծ֣���ʼ��������Ϸ��
void ChessGame::play()
{
	//���̳�ʼ��
	chess->init();
	while (true) {
			
		//��������
		man->go();
		//�ж�����Ƿ����
		if (chess->checkGameOver()) {
			chess->init();
			continue;
		}
		//��AI��
		ai->go();
		//man2->go();
		//�ж�����Ƿ����
		if (chess->checkGameOver()) {
			chess->init();
			continue;
		}
	}
}
