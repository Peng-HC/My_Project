#pragma once
#include<graphics.h> //easyxͼ�ο��ͷ�ļ�
#include<vector>
using namespace std;

//���ӵ�λ��
struct ChessPos {
	int row; //��
	int col; //��
	//���캯��
	ChessPos(int r = 0, int c = 0) :row(r), col(c) {}
};

//��������
typedef enum {
	CHESS_WHITE = -1,//����
	CHESS_BLACK = 1	//����
}ChessType;

//������
class Chess
{
public:
	Chess(int chessboardSize, int marginX, int marginY, float chessSize);
	//���̳�ʼ��:�������̵�ͼƬ��Դ����ʼ�����̵��������
	void init();
	//�ж��Ƿ�Ϊ��Ч���
	bool clickBoard(int x, int y, ChessPos *pos);
	//����
	void chessDown(ChessPos *pos, ChessType chess_type);
	//��ȡ���̴�С(13,15,17,19)
	int getChessboardSize();
	//��ȡָ��λ���Ǻ��廹�ǰ��壬���ǿհ�
	int getChessData(ChessPos *pos);
	int getChessData(int row, int col);
	//�������Ƿ����
	bool checkGameOver();

private:
	IMAGE chessBlackImg; //��������
	IMAGE chessWhiteImg; //��������

	int chessboardSize; //���̴�С(13��15��17��19)
	int margin_x; //���̵����߽�
	int margin_y; //���̵Ķ����߽�
	float chessSize; //���ӵĴ�С(����С����Ĵ�С)
	ChessPos lastPos; //�������λ��

	//�洢��ǰ��ֵ����ӷֲ����
	//���磺chessMap[3][6] ��ʾ���̵ĵ�3�е�5�е�������� 0:�հף�1:���ӣ�-1:����
	vector<vector<int>> chessMap;

	//��ʾ���ڸ��ĸ���ɫ���壨���ӣ���
	bool playerRole; //true:�����ߣ�false:������
	//�޸���������
	void updateChessMap(ChessPos *pos);
};