// 设定操作系统版本为 Win10（需要放到 #include <graphics.h> 前面）
#define WINVER 0x0A00
#define WIN32_WINNT 0x0A00
#include <ShellScalingApi.h>		// 引用头文件
#pragma comment(lib, "Shcore.lib")	// 链接库文件

#include "Chess.h"
#include "mmsystem.h"
#pragma comment(lib,"Winmm.lib")

#include <conio.h>


Chess::Chess(int chessboardSize, int marginX, int marginY, float chessSize)
{
	this->chessboardSize = chessboardSize; //格
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
	//创建游戏窗口
	/*
		由于现在的屏幕分辨率都比较高，为了使屏幕字体不至于太小，
		通常在 Win10 的显示设置里面设为放大 125% 或 150% 或更大，
		这样就会导致像素实际上被放大了。
		解决方法：https://codebus.cn/yangw/dpi-awareness
	*/
	// 设置该进程不受系统 DPI 设置影响
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	initgraph(897,895, EX_SHOWCONSOLE);
	//initgraph(450,450);
	//显示棋盘图片(自定义图片)
	loadimage(NULL, _T("res/棋盘2.jpg"));
	//进入对局则播放提示音
	//mciSendString是用来播放多媒体文件的API指令，可以播放MPEG,AVI,WAV,MP3等。其实就是向API发送字符串命令行
	mciSendString("play res/start.wav", NULL, 0, NULL);

	//加载黑棋和白棋棋子的图片
	loadimage(&chessBlackImg, "res/black.png", chessSize, chessSize, true);
	loadimage(&chessWhiteImg, "res/white.png", chessSize, chessSize, true);

	//棋盘清零
	for (int i = 0; i < chessMap.size(); i++) {
		for (int j = 0; j < chessMap[i].size(); j++) {
			chessMap[i][j] = 0;
		}
	}
	playerRole = true; //黑棋先下
}

/*
	棋子背后有黑色背景。这是因为easyx图形库默认不支持背景透明的png格式图片，
	把透明部分直接渲染为黑色了。
*/
void putimagePNG(int x, int y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + x) + (iy + y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

bool Chess::clickBoard(int x, int y, ChessPos* pos)
{
	int left_top_col = (x - margin_x) / chessSize;
	int left_top_row = (y - margin_y) / chessSize;
	float Threshold = chessSize * 0.3; //设定鼠标点击的位置与棋盘最近的方格顶点的距离阈值
	
	//左上顶点
	float left_top_x = margin_x + left_top_col * chessSize;
	float left_top_y = margin_y + left_top_row * chessSize;
	float left_top_dis_square = (x - left_top_x)*(x - left_top_x) + (y - left_top_y)*(y - left_top_y);
	if (left_top_dis_square <= Threshold * Threshold && left_top_row >= 0 && left_top_row < getChessboardSize() && left_top_col >= 0 && left_top_col < getChessboardSize() && chessMap[left_top_row][left_top_col] == 0) {
		pos->row = left_top_row;
		pos->col = left_top_col;
		return true;
	}
	//右上顶点
	float right_top_x = margin_x + (left_top_col + 1) * chessSize;
	float right_top_y = margin_y + left_top_row * chessSize;
	float right_top_dis_square = (x - right_top_x)*(x - right_top_x) + (y - right_top_y)*(y - right_top_y);
	if (right_top_dis_square <= Threshold * Threshold && left_top_row >= 0 && left_top_row < getChessboardSize() && left_top_col + 1 >= 0 && left_top_col + 1 < getChessboardSize() && chessMap[left_top_row][left_top_col + 1] == 0) {
		pos->row = left_top_row;
		pos->col = left_top_col+1;
		return true;
	}
	//左下顶点
	float left_bottom_x = margin_x + left_top_col * chessSize;
	float left_bottom_y = margin_y + (left_top_row + 1) * chessSize;
	float left_bottom_dis_square = (x - left_bottom_x)*(x - left_bottom_x) + (y - left_bottom_y)*(y - left_bottom_y);
	if (left_bottom_dis_square <= Threshold * Threshold && left_top_row + 1 >= 0 && left_top_row + 1 < getChessboardSize() && left_top_col >= 0 && left_top_col < getChessboardSize() && chessMap[left_top_row + 1][left_top_col] == 0) {
		pos->row = left_top_row + 1;
		pos->col = left_top_col;
		return true;
	}
	//右下顶点
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
	//更新最近落子位置
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

//判断棋局是否结束
bool Chess::checkGameOver()
{
	bool game_over = false;
	//最后落子的位置
	int curRow = lastPos.row;
	int curCol = lastPos.col;
	//四个方向:横、竖、左斜/、右斜\

	//1.横（左右分别延伸4格）
	for (int i = 0; i < 5; i++) {
		//往左4个，往右匹配4个子，5种情况
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
	//2.竖（上下分别延伸4格）
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
	//3.左斜线（/）
	for (int i = 0; i < 5; i++) {
		if (curRow + i-4 >= 0 && curRow + i  < chessboardSize&&curCol-i>=0&&curCol-i+4< chessboardSize) {
			//// 第[row+i]行，第[col-i]的棋子，与右上方连续4个棋子都相同
			if (chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 1][curCol - i + 1]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 2][curCol - i + 2]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 3][curCol - i + 3]
				&& chessMap[curRow + i][curCol - i] == chessMap[curRow + i - 4][curCol - i + 4]) {
				game_over = true;
				goto over;
			}
		}
	}
	//4.右斜线（\）
	for (int i = 0; i < 5; i++) {
		if (curRow - i >= 0 && curRow - i + 4 < chessboardSize&&curCol - i >= 0 && curCol - i + 4 < chessboardSize) {
			// 第[row + i]行，第[col - i]的棋子，与右下方连续4个棋子都相同
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
	//判断棋局结束后具体谁赢了
	if (game_over) {
		Sleep(1500);
		if (playerRole == false) { //轮到白棋落子，说明黑棋（棋手）获胜
			mciSendString("play res/不错.mp3", 0, 0, 0);
			loadimage(0, "res/胜利.jpg");
		}
		else {
			mciSendString("play res/失败.mp3", 0, 0, 0);
			loadimage(0, "res/失败.jpg");
		}
		_getch(); //暂停
	}
	return game_over;
}

void Chess::updateChessMap(ChessPos * pos)
{
	chessMap[pos->row][pos->col] = playerRole ? CHESS_BLACK : CHESS_WHITE;
	playerRole = !playerRole; // 换手

}
