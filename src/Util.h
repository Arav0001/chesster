#pragma once

#include <bits/stdc++.h>

#define WHITE true
#define BLACK false

#define EMPTY        -1
#define WHITE_PAWN   0
#define WHITE_KNIGHT 1
#define WHITE_BISHOP 2
#define WHITE_ROOK   3
#define WHITE_QUEEN  4
#define WHITE_KING   5
#define BLACK_PAWN   6
#define BLACK_KNIGHT 7
#define BLACK_BISHOP 8
#define BLACK_ROOK   9
#define BLACK_QUEEN 10
#define BLACK_KING  11

#define PAWN    0
#define KNIGHT  1
#define BISHOP  2
#define ROOK    3
#define QUEEN   4
#define KING    5

#define UP 2
#define DOWN 3
#define LEFT 5
#define RIGHT 7

inline bool getBit(uint64_t num, int index) {
    return (num >> index) & (uint64_t)1;
}

inline int squareToIndex(const std::string& square) {
    if (square.length() != 2)
        return -1;

    char file = square[0];
    char rank = square[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        return -1;

    int fileIndex = file - 'a';
    int rankIndex = '8' - rank;

    return rankIndex * 8 + fileIndex;
}