#pragma once

#include "Util.h"

#define PROMOTE_NONE -1

#define CASTLE_NONE -1
#define CASTLE_KING 0
#define CASTLE_QUEEN 1

typedef struct Move {
    int8_t startingSquare = -1;
    int8_t endingSquare = -1;

    bool CAPTURE_FLAG = false;
    bool PAWN_DOUBLE_MOVE_FLAG = false;
    int8_t PROMOTE_FLAG = PROMOTE_NONE;
    int8_t CASTLE_FLAG = CASTLE_NONE;
    bool EN_PASSANT_FLAG = false;

    Move() = default;

    Move(int startingSquare, int endingSquare) {
        this->startingSquare = startingSquare;
        this->endingSquare = endingSquare;
    }
} Move;