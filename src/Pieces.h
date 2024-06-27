#pragma once

#include "res/w_pawn.h"
#include "res/w_knight.h"
#include "res/w_bishop.h"
#include "res/w_rook.h"
#include "res/w_queen.h"
#include "res/w_king.h"
#include "res/b_pawn.h"
#include "res/b_knight.h"
#include "res/b_bishop.h"
#include "res/b_rook.h"
#include "res/b_queen.h"
#include "res/b_king.h"

inline const unsigned char* getPixels(int pieceType) {
	switch (pieceType) {
		case WHITE_PAWN:
			return w_pawn::pixels;
			break;
		case WHITE_KNIGHT:
			return w_knight::pixels;
			break;
		case WHITE_BISHOP:
			return w_bishop::pixels;
			break;
		case WHITE_ROOK:
			return w_rook::pixels;
			break;
		case WHITE_QUEEN:
			return w_queen::pixels;
			break;
		case WHITE_KING:
			return w_king::pixels;
			break;
		case BLACK_PAWN:
			return b_pawn::pixels;
			break;
		case BLACK_KNIGHT:
			return b_knight::pixels;
			break;
		case BLACK_BISHOP:
			return b_bishop::pixels;
			break;
		case BLACK_ROOK:
			return b_rook::pixels;
			break;
		case BLACK_QUEEN:
			return b_queen::pixels;
			break;
		case BLACK_KING:
			return b_king::pixels;
			break;
		default:
			return nullptr;
	}
}