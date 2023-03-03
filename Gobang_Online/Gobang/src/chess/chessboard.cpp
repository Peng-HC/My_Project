#include "chessboard.h"

#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QDebug>

#include <cmath>

//初始化
ChessBoard::ChessBoard(QWidget *parent) :
        QWidget(parent)
{
    imgLabel = new QLabel(this);
    imgLabel->setFixedSize(625, 625);
    this->setFixedSize(625, 625);

    pixmap = QPixmap();
    //初始化最后一个落子为空
    lastPiece = { 0, 0, CHESS_NULL };
    init();
}

void ChessBoard::init() {
    numPieces = 0;
    lastPiece = { 0, 0, CHESS_NULL };
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < ROWS; ++col) {
            chessPieces[row][col] = CHESS_NULL;
        }
    }
    //绘画棋盘
    drawChessboard();
}

// 指定棋子填充棋盘
void ChessBoard::init(int pieces[][ROWS], ChessPieceInfo lastPieceInfo) {
    numPieces = 0;
    lastPiece = lastPieceInfo;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < ROWS; ++col) {
            chessPieces[row][col] = pieces[row][col];
            if (chessPieces[row][col] != CHESS_NULL)
                numPieces++;
        }
    }
    flush();
}

// 刷新棋局，重新给棋盘绘上棋子
void ChessBoard::flush() {
    drawChessboard();

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < ROWS; ++col) {
            //画棋子
            drawPiece(row, col, ChessType(chessPieces[row][col]), false);
        }
    }

    drawPiece(lastPiece.row, lastPiece.col, lastPiece.type, true);
    //label加载图片pixmap
    imgLabel->setPixmap(pixmap);
}

//画棋盘
void ChessBoard::drawChessboard() {
    // 背景颜色
    //   1. pure color
    //   2. picture
    fillBackground();

    // Draw chess board
    QPainter painter(&pixmap);
    //设置抗锯齿能力，绘制的更为仔细，没有毛边
    painter.setRenderHint(QPainter::Antialiasing, true);
    //设置画笔颜色
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine));

    int left = startX + boardWidth;
    int right = left + gridWidth * (ROWS - 1);
    int top = startY + boardWidth;
    int bottom = top + gridWidth * (ROWS - 1);

    for (int i = 0; i < ROWS; ++i) {
        // 水平线
        painter.drawLine(left, top + gridWidth * i, right, top + gridWidth * i);
        // 竖直线
        painter.drawLine(left + gridWidth * i, top, left + gridWidth * i, bottom);

        // top & bottom  text (A, B, C, ... , S)
        painter.drawText(QPoint(left + gridWidth * i, startY * 2 / 3), QString('A' + i));
        painter.drawText(QPoint(left + gridWidth * i, bottom + boardWidth + startY / 2), QString('A' + i));

        // left & right  text (1, 2, 3, ... , 15)
        painter.drawText(QPoint(startX / 3, top + gridWidth * i), QString::number(1 + i));
        painter.drawText(QPoint(right + boardWidth + startX / 3, top + gridWidth * i), QString::number(1 + i));
    }

    // 棋盘内部线条
    left = startX;
    top = startY;
    right = left + boardWidth * 2 + gridWidth * (ROWS - 1);
    bottom = top + boardWidth * 2 + gridWidth * (ROWS - 1);
    painter.drawLine(left, top, right, top);
    painter.drawLine(right, top, right, bottom);
    painter.drawLine(left, bottom, right, bottom);
    painter.drawLine(left, top, left, bottom);

    // 5个控制点
    painter.setPen(QColor(Qt::black));
    painter.setBrush(QBrush(Qt::black));
    int centerX = startX + boardWidth + (ROWS / 2) * gridWidth;
    int centerY = startY + boardWidth + (ROWS / 2) * gridWidth;
    //绘制小黑点(中心点、行列偏移4格)
    painter.drawEllipse(QPoint(centerX, centerY), 4, 4);
    painter.drawEllipse(QPoint(centerX - 4 * gridWidth, centerY - 4 * gridWidth), 4, 4);
    painter.drawEllipse(QPoint(centerX + 4 * gridWidth, centerY - 4 * gridWidth), 4, 4);
    painter.drawEllipse(QPoint(centerX - 4 * gridWidth, centerY + 4 * gridWidth), 4, 4);
    painter.drawEllipse(QPoint(centerX + 4 * gridWidth, centerY + 4 * gridWidth), 4, 4);

    imgLabel->setPixmap(pixmap);
    painter.end();
}

// 落子
void ChessBoard::setPiece(int row, int col, ChessType type) {
    chessPieces[row][col] = type;

    drawPiece(lastPiece.row, lastPiece.col, lastPiece.type, false);
    // 绘制刚落子后棋子的"准星"
    drawPiece(row, col, type, true);

    lastPiece = { row, col, type };

    // judge
    if (type != CHESS_NULL) {
        int status = judge(row, col);
        if (status == S_WIN)
            emit sigWin(type);
        else if (status == S_DRAW)
            emit sigDraw();
    }
}

//判断落子对应的行列是否大于等于0且小于ROWS，同时该位置需为空
bool ChessBoard::isValid(int row, int col) {
    return (row >= 0 && row < ROWS && col >= 0 && col < ROWS &&
            chessPieces[row][col] == CHESS_NULL);
}

//获得鼠标点击的坐标对应的行列
//如果点击的位置距离左上角的格子位置小于10px,则可得到对应的行列
bool ChessBoard::getRowCol(QPoint cursorPos, int &row, int &col) {
    int left = startX + boardWidth;
    int right = left + gridWidth * (ROWS - 1);
    int top = startY + boardWidth;
    int bottom = top + gridWidth * (ROWS - 1);

    int x = cursorPos.x();
    int y = cursorPos.y();

    if (x < left - EPS || x > right + EPS)
        return false;
    if (y < top - EPS || y > bottom + EPS)
        return false;

    int countX = abs(x - left) / gridWidth;
    int countY = abs(y - top) / gridWidth;

    int gridLeft = left + countX * gridWidth;
    int gridRight = gridLeft + gridWidth;
    int gridTop = top + countY * gridWidth;
    int gridBottom = gridTop + gridWidth;

    if (abs(x - gridLeft) <= EPS) {
        if (abs(y - gridTop) <= EPS) {
            row = countY;
            col = countX;
            return true;
        }
        else if (abs(y - gridBottom) <= EPS) {
            row = countY + 1;
            col = countX;
            return true;
        }
        else {
            return false;
        }
    }
    else if (abs(x - gridRight) <= EPS) {
        if (abs(y - gridTop) <= EPS) {
            row = countY;
            col = countX + 1;
            return true;
        }
        else if (abs(y - gridBottom) <= EPS) {
            row = countY + 1;
            col = countX + 1;
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}


// 填充背景
//   1. pure color
//   2. picture
void ChessBoard::fillBackground() {
    if (bgType == BG_PURE_COLOR) {
        pixmap = QPixmap(imgLabel->size());
        pixmap.fill(bgColor);
    }
    else {
        pixmap = bgPicture;
        QPixmap temp(pixmap.size());
        temp.fill(Qt::transparent);
        QPainter painter(&temp);
        //给图片添加一个蒙层
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.drawPixmap(0, 0, pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.fillRect(temp.rect(), QColor(0, 0, 0, bgTransparency));
        painter.end();

        pixmap = temp.scaled(625, 625, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imgLabel->setScaledContents(true);
    }
    imgLabel->setPixmap(pixmap);
}

//画棋子
void ChessBoard::drawPiece(int row, int col, ChessType type, bool highlight) {
    QPainter painter(&pixmap);
    //设置抗锯齿能力，绘制的更为仔细，没有毛边
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    if (type == CHESS_NULL)
        return;
    else if (type == CHESS_BLACK) {
        painter.setPen(Qt::black);
        // 设置画刷
        painter.setBrush(Qt::black);
    }
    else {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
    }

    int x = startX + boardWidth + gridWidth * col;
    int y = startY + boardWidth + gridWidth * row;

    // 画棋子(圆)
    painter.drawEllipse(QPoint(x, y), pieceRadius, pieceRadius);

    // 绘制刚落子后棋子的"准星"
    if (highlight) {
        QPen pen;
        pen.setWidth(3);
        if (type == CHESS_BLACK)
            pen.setColor(Qt::white);
        else
            pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawLine(x + 4, y, x + 6, y);
        painter.drawLine(x - 4, y, x - 6, y);
        painter.drawLine(x, y + 4, x, y + 6);
        painter.drawLine(x, y - 4, x, y - 6);
    }
    painter.end();
    imgLabel->setPixmap(pixmap);
    numPieces++;
}

//判断棋局是否结束(出现连5的情况)
int ChessBoard::judge(int row, int col) {
    int chessType = chessPieces[row][col];

    bool ret = (judgeInRow(row, col, chessType) ||
               judgeInCol(row, col, chessType) ||
               judgeFromTopleftToBottomRight(row, col, chessType) ||
               judgeFromBottomleftToTopright(row, col, chessType)
              );

    if (ret)
        return S_WIN;
    if (numPieces == ROWS * ROWS)
        return S_DRAW;
    else
        return S_CONTINUE;
}

//行5
bool ChessBoard::judgeInRow(int row, int col, int chessType) {
    int count = 1;
    int l = col - 1, r = col + 1;
    while (l > -1 || r < ROWS) {
        if (l > -1) {
            if (chessPieces[row][l] == chessType) {
                count++;
                l--;
            }
            else {
                l = -1;
            }
        }
        if (r < ROWS) {
            if (chessPieces[row][r] == chessType) {
                count++;
                r++;
            }
            else {
                r = ROWS;
            }
        }
    }

    return count >= 5;
}

//列5
bool ChessBoard::judgeInCol(int row, int col, int chessType) {
    int count = 1;
    int t = row - 1, b = row + 1;
    while (t > -1 || b < ROWS) {
        if (t > -1) {
            if (chessPieces[t][col] == chessType) {
                count++;
                t--;
            }
            else {
                t = -1;
            }
        }
        if (b < ROWS) {
            if (chessPieces[b][col] == chessType) {
                count++;
                b++;
            }
            else {
                b = ROWS;
            }
        }
    }

    return count >= 5;
}

//左斜5(\)
bool ChessBoard::judgeFromTopleftToBottomRight(int row, int col, int chessType) {
    int count = 1;
    int l = col - 1, r = col + 1;
    int t = row - 1, b = row + 1;
    bool bTL = (t > -1 && l > -1);
    bool bBR = (b < ROWS && r < ROWS);

    while (bTL || bBR) {
        if (bTL) {
            if (chessPieces[t][l] == chessType) {
                count++;
                t--;
                l--;
            }
            else {
                t = -1;
            }
        }
        if (bBR) {
            if (chessPieces[b][r] == chessType) {
                count++;
                b++;
                r++;
            }
            else {
                b = ROWS;
            }
        }
        bTL = (t > -1 && l > -1);
        bBR = (b < ROWS && r < ROWS);
    }

    return count >= 5;
}

//右斜5(/)
bool ChessBoard::judgeFromBottomleftToTopright(int row, int col, int chessType) {
    int count = 1;
    int l = col - 1, r = col + 1;
    int t = row - 1, b = row + 1;
    bool bBL = (b > -1 && l > -1);
    bool bTR = (t < ROWS && r < ROWS);

    while (bBL || bTR) {
        if (bBL) {
            if (chessPieces[b][l] == chessType) {
                count++;
                b++;
                l--;
            }
            else {
                b = -1;
            }
        }
        if (bTR) {
            if (chessPieces[t][r] == chessType) {
                count++;
                t--;
                r++;
            }
            else {
                t = ROWS;
            }
        }
        bBL = (b > -1 && l > -1);
        bTR = (t < ROWS && r < ROWS);
    }

    return count >= 5;
}
