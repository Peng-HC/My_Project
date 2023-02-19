#include "AI.h"

void AI::init(Chess * chess)
{
	this->chess = chess;

	int size = chess->getChessboardSize();
	for (int i = 0; i < size; i++) {
		vector<int> row;
		for (int j = 0; j < size; j++) {
			row.push_back(0);
		}
		AI_scores.push_back(row);
	}
}

void AI::go()
{
	ChessPos pos=think();
	Sleep(1000);//AI��װ˼��
	chess->chessDown(&pos,CHESS_WHITE);
}

//AI�����������ڸ����հ׵�����ķ���
void AI::calculateScore()
{
	//����������������
	for (int i = 0; i < AI_scores.size(); i++) {
		for (int j = 0; j < AI_scores[i].size(); j++) {
			AI_scores[i][j] = 0;
		}
	}
	int size = chess->getChessboardSize();

	//�������̵ĸ����հ�λ��
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			//��λ�ÿհף���������
			if (chess->getChessData(row, col) == 0) {
				//�Ӱ˸��������ж�������
				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						int personNum = 0; //������������
						int AINum = 0; //������������
						int emptyNum = 0; //�հ�����
						if (x == 0 && y == 0) continue;//��������

						//ÿ����������4��
						for (int i = 1; i <= 4; i++) {
							int nextRow = row + i * y;
							int nextCol = col + i * x;

							//�����հ׻��߰�������÷���ı���
							//�Ժ�������
							if (nextRow >= 0 && nextRow < size&&nextCol >= 0 && nextCol < size) {
								if (chess->getChessData(nextRow, nextCol) == 1) {//����
									personNum++;
								}
								else if (chess->getChessData(nextRow, nextCol) == -1) {//����
									AINum++;
									break;
								}
								else {//�հ�
									emptyNum++;
									break;
								}
							}
							else {
								//Խ����
								break;
							}
						}
						//ĳ����������������������з���
						if (personNum == 1) { //��2
							AI_scores[row][col] += 10;
						}
						else if (personNum == 2) { //��3
							//��3
							if (emptyNum == 1) {
								AI_scores[row][col] += 40;
							}
							else if (AINum == 1) {
								//��3
								AI_scores[row][col] += 30;
							}
						}
						else if (personNum == 3) {//��4
							//��4
							if (emptyNum == 1) {
								AI_scores[row][col] += 200;
							}
							else if (AINum == 1) {
								//��4
								AI_scores[row][col] += 60;
							}
						}
						else if (personNum == 4) {//��5
							AI_scores[row][col] += 20000;
						}

						AINum = 0;
						emptyNum = 0;
						personNum = 0;


						//ÿ����������4��
						for (int i = 1; i <= 4; i++) {
							int nextRow = row + i * y;
							int nextCol = col + i * x;

							//�����հ�������÷���ı���
							//�԰�������
							if (nextRow >= 0 && nextRow < size&&nextCol >= 0 && nextCol < size) {
								if (chess->getChessData(nextRow, nextCol) == -1) {//����
									AINum++;
								}
								else if (chess->getChessData(nextRow, nextCol) == 1) {//����
									personNum++;
									break;
								}
								else {//�հ�
									emptyNum++;
									break;
								}
							}
							else {
								//Խ����
								break;
							}
						}
						//ĳ����������������������з���
						if (AINum == 0) { //��ͨ����
							AI_scores[row][col] += 5;
						}
						else if (AINum == 1) { //��2
							AI_scores[row][col] += 10;
						}
						else if (AINum == 2) { //��3
							//��3
							if (emptyNum == 1) {
								AI_scores[row][col] += 50;
							}
							else if (personNum == 1) {
								//��3
								AI_scores[row][col] += 25;
							}
						}
						else if (AINum == 3) { //��4
							//��4
							if (emptyNum == 1) {
								AI_scores[row][col] += 10000;
							}
							else if (personNum == 1) {
								//��4
								AI_scores[row][col] += 55;
							}
						}
						else if (AINum>= 4) { //��5(��ɱ)
							AI_scores[row][col] += 30000;
						}

					}
				}
			}

		}
	}
}

ChessPos AI::think()
{
	//AI��������
	calculateScore();
	//���������ҵ����ֵ
	int max_AI_score = 0;
	//�����ֵ��Ӧ������ֵ�洢��ȥ������ͬ�֣�
	vector<ChessPos> max_score_pos;


	int size = chess->getChessboardSize();
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			if (AI_scores[row][col] > max_AI_score) {
				max_AI_score = AI_scores[row][col];
				max_score_pos.clear();
				max_score_pos.push_back(ChessPos(row, col));
			}
			else if (AI_scores[row][col] == max_AI_score) {
				//����ͬ�ֵ����
				max_score_pos.push_back(ChessPos(row, col));
			}
		}
	}
	//��ͬ����������������
	int index = rand() % max_score_pos.size();
	return max_score_pos[index];
}
