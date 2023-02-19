#pragma once
#include<graphics.h> //easyx图形库的头文件
#include<vector>
using namespace std;

//棋子的位置
struct ChessPos {
	int row; //行
	int col; //列
	//构造函数
	ChessPos(int r = 0, int c = 0) :row(r), col(c) {}
};

//棋子类型
typedef enum {
	CHESS_WHITE = -1,//白棋
	CHESS_BLACK = 1	//黑棋
}ChessType;

//棋盘类
class Chess
{
public:
	Chess(int chessboardSize, int marginX, int marginY, float chessSize);
	//棋盘初始化:加载棋盘的图片资源，初始化棋盘的相关数据
	void init();
	//判断是否为有效点击
	bool clickBoard(int x, int y, ChessPos *pos);
	//落子
	void chessDown(ChessPos *pos, ChessType chess_type);
	//获取棋盘大小(13,15,17,19)
	int getChessboardSize();
	//获取指定位置是黑棋还是白棋，还是空白
	int getChessData(ChessPos *pos);
	int getChessData(int row, int col);
	//检查棋局是否结束
	bool checkGameOver();

private:
	IMAGE chessBlackImg; //黑棋棋子
	IMAGE chessWhiteImg; //黑棋棋子

	int chessboardSize; //棋盘大小(13，15，17，19)
	int margin_x; //棋盘的左侧边界
	int margin_y; //棋盘的顶部边界
	float chessSize; //棋子的大小(棋盘小方格的大小)
	ChessPos lastPos; //最近落子位置

	//存储当前棋局的棋子分布情况
	//例如：chessMap[3][6] 表示棋盘的第3行第5列的落子情况 0:空白，1:黑子，-1:白子
	vector<vector<int>> chessMap;

	//表示现在该哪个角色下棋（落子）了
	bool playerRole; //true:黑子走，false:白子走
	//修改棋盘数据
	void updateChessMap(ChessPos *pos);
};