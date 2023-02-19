#pragma once

//棋子类型
enum ChessType {
    CHESS_BLACK = -1,
    CHESS_NULL = 0,
    CHESS_WHITE = 1
};

//棋盘信息
struct ChessPieceInfo {
    int row;
    int col;
    int type;
};

//玩家黑白棋交换
ChessType reverse(ChessType typeIn);

