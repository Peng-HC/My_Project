#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QPixmap>
#include <QLabel>
#include <QColor>

#include "base.h"
#include "../environment.h"

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    //explicit:只能显示调用构造函数
    explicit ChessBoard(QWidget *parent = nullptr);

    enum { ROWS = 15 }; //棋盘大小:15X15
    enum { S_CONTINUE = -2, S_LOSE = -1, S_DRAW = 0, S_WIN = 1 };

    //落子信息
    struct ChessPieceInfo {
        int row;
        int col;
        ChessType type;
    };

public:
    // 清除棋盘上的全部棋子
    void init();
    // 指定棋子填充棋盘
    void init(int pieces[][ROWS], ChessPieceInfo lastPieceType);

    // 刷新棋局，重新给棋盘绘上棋子(不是清除棋盘上的棋子)
    void flush();

    // 落子
    void setPiece(int row, int col, ChessType type);

    // 设置背景颜色
    //    1. pure color (纯色)
    //    2. picture    (图片)
    BackgroundType getBgType() const { return bgType; }
    QPixmap getBgPicture() const { return bgPicture; }
    QColor getBgColor() const { return bgColor; }
    int getBgTransparency() const { return bgTransparency; }

    void setBgType(BackgroundType type)  { bgType = type; }
    void setBgPicture(const QPixmap& picture) { bgPicture = picture; }
    void setBgColor(const QColor& color) { bgColor = color; }
    void setBgTransparecny(int val) { bgTransparency = val; }

    // 获取棋盘图像，包括棋子分布情况
    QPixmap& getImage() { return pixmap; }

signals:
    void sigWin(int type);
    void sigDraw();

protected:
    //画棋盘
    void drawChessboard();
    //填充背景
    void fillBackground();
    //画棋子
    void drawPiece(int row, int col, ChessType type, bool highlight);   // (0 <= row, col <= ROWS)
    //判断落子对应的行列是否大于等于0且小于ROWS，同时该位置需为空
    bool isValid(int row, int col);

    //获得鼠标点击的坐标对应的行列
    //如果点击的位置距离左上角的格子位置小于10px,则可得到对应的行列
    bool getRowCol(QPoint cursorPos, int& row, int& col);

    ////判断棋局是否结束(出现连5的情况)
    int judge(int row, int col);
    bool judgeInRow(int row, int col, int chessType);
    bool judgeInCol(int row, int col, int chessType);
    bool judgeFromTopleftToBottomRight(int row, int col, int chessType);
    bool judgeFromBottomleftToTopright(int row, int col, int chessType);

protected:

    // 棋局
    int chessPieces[ROWS][ROWS];
    //棋子个数
    int numPieces = 0;

    const int gridWidth = 40; //棋盘每一格的长宽
    const int startX = 27; //棋盘左上角起始位置
    const int startY = 27;
    const int boardWidth = 5; //边框空白部分
    const int pieceRadius = 13; //棋子大小
    const int EPS = 10;

    QPixmap pixmap;     // 包括背景图像(或颜色)和棋子
    QLabel* imgLabel;

    BackgroundType bgType = BG_PURE_COLOR;
    QColor bgColor = qRgb(200, 150, 100);
    QPixmap bgPicture;
    //透明度
    int bgTransparency = 220;

    //最后一次落子位置
    ChessPieceInfo lastPiece;
};

#endif // CHESSBOARD_H
