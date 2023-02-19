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
	Sleep(1000);//AI假装思考
	chess->chessDown(&pos,CHESS_WHITE);
}

//AI计算棋盘中在各个空白点下棋的分数
void AI::calculateScore()
{
	//评分向量数组清零
	for (int i = 0; i < AI_scores.size(); i++) {
		for (int j = 0; j < AI_scores[i].size(); j++) {
			AI_scores[i][j] = 0;
		}
	}
	int size = chess->getChessboardSize();

	//遍历棋盘的各个空白位置
	for (int row = 0; row < size; row++) {
		for (int col = 0; col < size; col++) {
			//该位置空白，可以落子
			if (chess->getChessData(row, col) == 0) {
				//从八个方向来判断棋局情况
				for (int y = -1; y <= 1; y++) {
					for (int x = -1; x <= 1; x++) {
						int personNum = 0; //连续黑棋数量
						int AINum = 0; //连续白棋数量
						int emptyNum = 0; //空白数量
						if (x == 0 && y == 0) continue;//本身不考虑

						//每个方向延伸4格
						for (int i = 1; i <= 4; i++) {
							int nextRow = row + i * y;
							int nextCol = col + i * x;

							//遇到空白或者白棋结束该方向的遍历
							//对黑棋评分
							if (nextRow >= 0 && nextRow < size&&nextCol >= 0 && nextCol < size) {
								if (chess->getChessData(nextRow, nextCol) == 1) {//黑棋
									personNum++;
								}
								else if (chess->getChessData(nextRow, nextCol) == -1) {//白棋
									AINum++;
									break;
								}
								else {//空白
									emptyNum++;
									break;
								}
							}
							else {
								//越界了
								break;
							}
						}
						//某个方向遍历完后根据棋形评判分数
						if (personNum == 1) { //连2
							AI_scores[row][col] += 10;
						}
						else if (personNum == 2) { //连3
							//活3
							if (emptyNum == 1) {
								AI_scores[row][col] += 40;
							}
							else if (AINum == 1) {
								//死3
								AI_scores[row][col] += 30;
							}
						}
						else if (personNum == 3) {//连4
							//活4
							if (emptyNum == 1) {
								AI_scores[row][col] += 200;
							}
							else if (AINum == 1) {
								//死4
								AI_scores[row][col] += 60;
							}
						}
						else if (personNum == 4) {//连5
							AI_scores[row][col] += 20000;
						}

						AINum = 0;
						emptyNum = 0;
						personNum = 0;


						//每个方向延伸4格
						for (int i = 1; i <= 4; i++) {
							int nextRow = row + i * y;
							int nextCol = col + i * x;

							//遇到空白则结束该方向的遍历
							//对白棋评分
							if (nextRow >= 0 && nextRow < size&&nextCol >= 0 && nextCol < size) {
								if (chess->getChessData(nextRow, nextCol) == -1) {//白棋
									AINum++;
								}
								else if (chess->getChessData(nextRow, nextCol) == 1) {//黑棋
									personNum++;
									break;
								}
								else {//空白
									emptyNum++;
									break;
								}
							}
							else {
								//越界了
								break;
							}
						}
						//某个方向遍历完后根据棋形评判分数
						if (AINum == 0) { //普通落子
							AI_scores[row][col] += 5;
						}
						else if (AINum == 1) { //连2
							AI_scores[row][col] += 10;
						}
						else if (AINum == 2) { //连3
							//活3
							if (emptyNum == 1) {
								AI_scores[row][col] += 50;
							}
							else if (personNum == 1) {
								//死3
								AI_scores[row][col] += 25;
							}
						}
						else if (AINum == 3) { //连4
							//活4
							if (emptyNum == 1) {
								AI_scores[row][col] += 10000;
							}
							else if (personNum == 1) {
								//死4
								AI_scores[row][col] += 55;
							}
						}
						else if (AINum>= 4) { //连5(绝杀)
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
	//AI落子评分
	calculateScore();
	//从评分中找到最大值
	int max_AI_score = 0;
	//将最大值对应的行列值存储进去（包含同分）
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
				//出现同分的情况
				max_score_pos.push_back(ChessPos(row, col));
			}
		}
	}
	//对同分现象进行随机落子
	int index = rand() % max_score_pos.size();
	return max_score_pos[index];
}
