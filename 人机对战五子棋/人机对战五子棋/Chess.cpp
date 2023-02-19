// �趨����ϵͳ�汾Ϊ Win10����Ҫ�ŵ� #include <graphics.h> ǰ�棩
#define WINVER 0x0A00
#define WIN32_WINNT 0x0A00
#include <ShellScalingApi.h>		// ����ͷ�ļ�
#pragma comment(lib, "Shcore.lib")	// ���ӿ��ļ�

#include "Chess.h"
#include "mmsystem.h"
#pragma comment(lib,"Winmm.lib")

#include <conio.h>


Chess::Chess(int chessboardSize, int marginX, int marginY, float chessSize)
{
	this->chessboardSize = chessboardSize; //��
	this->margin_x = marginX;
	this->margin_y = marginY;
	this->chessSize = chessSize;
	playerRole = CHESS_BLACK;
	for (int i = 0; i < chessboardSize; i++) {
		vector<int> row;
		for (int j = 0; j < chessboardSize; j++) {
			row.push_back(0);
		}
		chessMap.push_back(row);
	}
}

void Chess::init()
{
	//������Ϸ����
	/*
		�������ڵ���Ļ�ֱ��ʶ��Ƚϸߣ�Ϊ��ʹ��Ļ���岻����̫С��
		ͨ���� Win10 ����ʾ����������Ϊ�Ŵ� 125% �� 150% �����
		�����ͻᵼ������ʵ���ϱ��Ŵ��ˡ�
		���������https://codebus.cn/yangw/dpi-awareness
	*/
	// ���øý��̲���ϵͳ DPI ����Ӱ��
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	initgraph(897,895, EX_SHOWCONSOLE);
	//initgraph(450,450);
	//��ʾ����ͼƬ(�Զ���ͼƬ)
	loadimage(NULL, _T("res/����2.jpg"));
	//����Ծ��򲥷���ʾ��
	//mciSendString���������Ŷ�ý���ļ���APIָ����Բ���MPEG,AVI,WAV,MP3�ȡ���ʵ������API�����ַ���������
	mciSendString("play res/start.wav", NULL, 0, NULL);

	//���غ���Ͱ������ӵ�ͼƬ
	loadimage(&chessBlackImg, "res/black.png", chessSize, chessSize, true);
	loadimage(&chessWhiteImg, "res/white.png", chessSize, chessSize, true);

	//��������
	for (int i = 0; i < chessMap.size(); i++) {
		for (int j = 0; j < chessMap[i].size(); j++) {
			chessMap[i][j] = 0;
		}
	}
	playerRole = true; //��������
}

/*
	���ӱ����к�ɫ������������Ϊeasyxͼ�ο�Ĭ�ϲ�֧�ֱ���͸����png��ʽͼƬ��
	��͸������ֱ����ȾΪ��ɫ�ˡ�
*/
void putimagePNG(int x, int y, IMAGE* picture) //xΪ����ͼƬ��X���꣬yΪY����
{
	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + x) + (iy + y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}

bool Chess::clickBoard(int x, int y, ChessPos* pos)
{
	int left_top_col = (x - margin_x) / chessSize;
	int left_top_row = (y - margin_y) / chessSize;
	float Threshold = chessSize * 0.3; //�趨�������λ������������ķ��񶥵�ľ�����ֵ
	
	//���϶���
	float left_top_x = margin_x + left_top_col * chessSize;
	float left_top_y = margin_y + left_top_row * chessSize;
	float left_top_dis_square = (x - left_top_x)*(x - left_top_x) + (y - left_top_y)*(y - left_top_y);
	if (left_top_dis_square <= Threshold * Threshold && left_top_row >= 0 && left_top_row < getChessboardSize() && left_top_col >= 0 && left_top_col < getChessboardSize() && chessMap[left_top_row][left_top_col] == 0) {
		pos->row = left_top_row;
		pos->col = left_top_col;
		return true;
	}
	//���϶���
	float right_top_x = margin_x + (left_top_col + 1) * chessSize;
	float right_top_y = margin_y + left_top_row * chessSize;
	float right_top_dis_square = (x - right_top_x)*(x - right_top_x) + (y - right_top_y)*(y - right_top_y);
	if (right_top_dis_square <= Threshold * Threshold && left_top_row >= 0 && left_top_row < getChessboardSize() && left_top_col + 1 >= 0 && left_top_col + 1 < getChessboardSize() && chessMap[left_top_row][left_top_col + 1] == 0) {
		pos->row = left_top_row;
		pos->col = left_top_col+1;
		return true;
	}
	//���¶���
	float left_bottom_x = margin_x + left_top_col * chessSize;
	float left_bottom_y = margin_y + (left_top_row + 1) * chessSize;
	float left_bottom_dis_square = (x - left_bottom_x)*(x - left_bottom_x) + (y - left_bottom_y)*(y - left_bottom_y);
	if (left_bottom_dis_square <= Threshold * Threshold && left_top_row + 1 >= 0 && left_top_row + 1 < getChessboardSize() && left_top_col >= 0 && left_top_col < getChessboardSize() && chessMap[left_top_row + 1][left_top_col] == 0) {
		pos->row = left_top_row + 1;
		pos->col = left_top_col;
		return true;
	}
	//���¶���
	float right_bottom_x = margin_x + (left_top_col + 1) * chessSize;
	float right_bottom_y = margin_y + (left_top_row + 1) * chessSize;
	float right_bottom_dis_square = (x - right_bottom_x)*(x - right_bottom_x) + (y - right_bottom_y)*(y - right_bottom_y);
	if (right_bottom_dis_square <= Threshold * Threshold && left_top_row + 1 >= 0 && left_top_row + 1 < getChessboardSize() && left_top_col + 1 >= 0 && left_top_col + 1 < getChessboardSize() && chessMap[left_top_row + 1][left_top_col + 1] == 0) {
		pos->row = left_top_row + 1;
		pos->col = left_top_col + 1;
		return true;
	}
	return false;
}

void Chess::chessDown(ChessPos* pos, ChessType chess_type)
{
	float x = margin_x + pos->col * chessSize - 0.5*chessSize;
	float y = margin_y + pos->row * chessSize - 0.5*chessSize;
	if (chess_type == CHESS_BLACK) {
		putimagePNG(x, y, &chessBlackImg);
	}
	else {
		putimagePNG(x, y, &chessWhiteImg);
	}
	mciSendString("play res/down7.WAV", 0, 0, 0);
	updateChessMap(pos);
	//�����������λ��
	lastPos = *pos;
}

int Chess::getChessboardSize()
{
	return this->chessboardSize;
}

int Chess::getChessData(ChessPos * pos)
{
	return chessMap[pos->row][pos->col];
}

int Chess::getChessData(int row, int col)
{
	return chessMap[row][col];
}

//�ж�����Ƿ����
bool Chess::checkGameOver()
{
	bool game_over = false;
	//������ӵ�λ��
	int curRow = lastPos.row;
	int curCol = lastPos.col;
	//�ĸ�����:�ᡢ������б/����б\

	//1.�ᣨ���ҷֱ�����4��
	for (int i = 0; i < 5; i++) {
		//����4��������ƥ��4���ӣ�5�����
		if (curCol - i >= 0 && curCol - i + 4 < chessboardSize) {
			if (chessMap[curRow][curCol - i] == chessMap[curRow][curCol - i + 1]
				&& chessMap[curRow][curCol - i] == chessMap[curRow][curCol - i + 2]
				&& chessMap[curRow][curCol - i] == chessMap[curRow][curCol - i + 3]
				&& chessMap[curRow][curCol - i] == chessMap[curRow][curCol - i + 4]) {
				game_over = true;
				goto over;
			}
		}
	}
	//2.�������·ֱ�����4��
	for (int i = 0; i < 5; i++) {
		if (curRow - i >= 0 && curRow - i + 4 < chessboardSize) {
			if (chessMap[curRow - i][curCol] == chessMap[curRow - i + 1][curCol]
				&& chessMap[curRow - i][curCol] == chessMap[curRow - i + 2][curCol]
				&& chessMap[curRow - i][curCol] == chessMap[curRow - i + 3][curCol]
				&& chessMap[curRow - i][curCol] == chessMap[curRow - i + 4][curCol]) {
				game_over = true;
				goto over;
			}
		}
	}
	//3.��б�ߣ�/��
	for (int i = 0; i < 5; i++) {
		if (curRow + i-4 >= 0 && curRow + i  < chessboardSize&&curCol-i>=0&&curCol-i+4< chessboardSize) {
			//// ��[row+i]�У���[col-i]�����ӣ������Ϸ�����4�����Ӷ���ͬ
			if (chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 1][curCol - i + 1]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 2][curCol - i + 2]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 3][curCol - i + 3]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 4][curCol - i + 4]) {
				game_over = true;
				goto over;
			}
		}
	}
	//4.��б�ߣ�\��
	for (int i = 0; i < 5; i++) {
		if (curRow - i >= 0 && curRow - i + 4 < chessboardSize&&curCol - i >= 0 && curCol - i + 4 < chessboardSize) {
			// ��[row + i]�У���[col - i]�����ӣ������·�����4�����Ӷ���ͬ
			if (chessMap[curRow - i][curCol - i] == chessMap[curRow - i + 1][curCol - i + 1]
				&& chessMap[curRow - i][curCol - i] == chessMap[curRow - i + 2][curCol - i + 2]
				&& chessMap[curRow - i][curCol - i] == chessMap[curRow - i + 3][curCol - i + 3]
				&& chessMap[curRow - i][curCol - i] == chessMap[curRow - i + 4][curCol - i + 4]) {
				game_over = true;
				goto over;
			}
		}
	}
	over:
	//�ж���ֽ��������˭Ӯ��
	if (game_over) {
		Sleep(1500);
		if (playerRole == false) { //�ֵ��������ӣ�˵�����壨���֣���ʤ
			mciSendString("play res/����.mp3", 0, 0, 0);
			loadimage(0, "res/ʤ��.jpg");
		}
		else {
			mciSendString("play res/ʧ��.mp3", 0, 0, 0);
			loadimage(0, "res/ʧ��.jpg");
		}
		_getch(); //��ͣ
	}
	return game_over;
}

void Chess::updateChessMap(ChessPos * pos)
{
	chessMap[pos->row][pos->col] = playerRole ? CHESS_BLACK : CHESS_WHITE;
	playerRole = !playerRole; // ����

}
