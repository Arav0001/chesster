#pragma once

#include "Util.h"
#include "Move.h"

class Position {
public:
private:
    uint64_t* bitboards = new uint64_t[12];

    bool turn = WHITE;

    bool whiteCanCastleKing = true;
    bool whiteCanCastleQueen = true;
    bool blackCanCastleKing = true;
    bool blackCanCastleQueen = true;

    int8_t enPassantSquare = -1;

    std::vector<Move> possibleMoves;

    std::vector<std::string> parseFENstring(const std::string& FENstring) {
        std::string token;
        std::stringstream tokenStream(FENstring);
        std::vector<std::string> tokens;

        while (std::getline(tokenStream, token, ' ')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 6) {
            std::cout << "Error: input a valid FEN string" << std::endl;
            exit(100);
        }

        return tokens;
    }

    uint64_t* parseFENboard(const std::string& FENboard) {
        uint64_t* bitboards = new uint64_t[12]();

        int file = 0;
        int rank = 7;
        
        for (char c : FENboard) {
            if (c == '/') {
                file = 0;
                rank--;
            } else if (isdigit(c)) {
                file += (c - '0');
            } else {
                int pieceIndex = -1;

                switch (c) {
                    case 'P': pieceIndex = WHITE_PAWN;   break;
                    case 'N': pieceIndex = WHITE_KNIGHT; break;
                    case 'B': pieceIndex = WHITE_BISHOP; break;
                    case 'R': pieceIndex = WHITE_ROOK;   break;
                    case 'Q': pieceIndex = WHITE_QUEEN;  break;
                    case 'K': pieceIndex = WHITE_KING;   break;
                    case 'p': pieceIndex = BLACK_PAWN;   break;
                    case 'n': pieceIndex = BLACK_KNIGHT; break;
                    case 'b': pieceIndex = BLACK_BISHOP; break;
                    case 'r': pieceIndex = BLACK_ROOK;   break;
                    case 'q': pieceIndex = BLACK_QUEEN;  break;
                    case 'k': pieceIndex = BLACK_KING;   break;
                }

                if (pieceIndex != -1) {
                    bitboards[pieceIndex] |= (1ULL << getSquare(rank, file));
                    file++;
                }
            }
        }

        return bitboards;
    }

    int getSquareNextTo(int square, int direction) {
        if (square == -1) return -1;
        
        switch (direction) {
            case RIGHT:
                if (getRank(square) == getRank(square + 1)) return square + 1;
                else return -1;
            case LEFT:
                if (getRank(square) == getRank(square - 1)) return square - 1;
                else return -1;
            case UP:
                if (square + 8 >= 0 && square + 8 < 64) return square + 8;
                else return -1;
                break;
            case DOWN:
                if (square - 8 >= 0 && square - 8 < 64) return square - 8;
                else return -1;
                break;
            case UP * RIGHT:
                return getSquareNextTo(getSquareNextTo(square, UP), RIGHT);
            case UP * LEFT:
                return getSquareNextTo(getSquareNextTo(square, UP), LEFT);
            case DOWN * RIGHT:
                return getSquareNextTo(getSquareNextTo(square, DOWN), RIGHT);
            case DOWN * LEFT:
                return getSquareNextTo(getSquareNextTo(square, DOWN), LEFT);
            default:
                return -1;
        }
    }

    int getSquareNextTo(int square, int direction, int amount) {
        for (int i = 0; i < amount; i++) {
            square = getSquareNextTo(square, direction);
        }

        return square;
    }

    void getAllMoves(std::vector<Move>& moves) {
        moves.clear();
        for (int i = 0; i < 64; i++) {
            getMoves(moves, i);
        }
    }

    void getPawnMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_PAWN] : bitboards[BLACK_PAWN];
        
        if (getBit(bitboard, square)) {
            int squareAbove = turn == WHITE ? getSquareNextTo(square, UP) : getSquareNextTo(square, DOWN);
            int square2Above = turn == WHITE ? getSquareNextTo(square, UP, 2) : getSquareNextTo(square, DOWN, 2);
            int squareRightDiag = turn == WHITE ? getSquareNextTo(square, UP * RIGHT) : getSquareNextTo(square, DOWN * LEFT);
            int squareLeftDiag = turn == WHITE ? getSquareNextTo(square, UP * LEFT) : getSquareNextTo(square, DOWN * RIGHT);

            int promotingRank = turn == WHITE ? 6 : 1;

            if (getPieceAt(squareAbove) == EMPTY) {
                int startingRank = turn == WHITE ? 1 : 6;
                if (getRank(square) == startingRank && getPieceAt(square2Above) == EMPTY) {
                    Move doubleMove(square, square2Above);
                    doubleMove.PAWN_DOUBLE_MOVE_FLAG = true;
                    moves.push_back(doubleMove);
                }

                if (getRank(square) == promotingRank) {
                    for (int j = 1; j <= 4; j++) {
                        Move promoteMove(square, squareAbove);
                        promoteMove.PROMOTE_FLAG = j;
                        moves.push_back(promoteMove);
                    }
                } else {
                    moves.push_back(Move(square, squareAbove));
                }
            }

            if (squareRightDiag != -1 && isOwnPiece(squareRightDiag) == 0) {
                if (getRank(square) == promotingRank) {
                    for (int j = 1; j <= 4; j++) {
                        Move promotingCaptureMove(square, squareRightDiag);
                        promotingCaptureMove.PROMOTE_FLAG = j;
                        promotingCaptureMove.CAPTURE_FLAG = true;
                        moves.push_back(promotingCaptureMove);
                    }
                } else {
                    Move captureMove(square, squareRightDiag);
                    captureMove.CAPTURE_FLAG = true;
                    moves.push_back(captureMove);
                }
            }

            if (squareLeftDiag != -1 && isOwnPiece(squareLeftDiag) == 0) {
                if (getRank(square) == promotingRank) {
                    for (int j = 1; j <= 4; j++) {
                        Move promotingCaptureMove(square, squareLeftDiag);
                        promotingCaptureMove.PROMOTE_FLAG = j;
                        promotingCaptureMove.CAPTURE_FLAG = true;
                        moves.push_back(promotingCaptureMove);
                    }
                } else {
                    Move captureMove(square, squareLeftDiag);
                    captureMove.CAPTURE_FLAG = true;
                    moves.push_back(captureMove);
                }
            }

            if (enPassantSquare != -1) {
                int squareRight = turn == WHITE ? getSquareNextTo(square, RIGHT) : getSquareNextTo(square, LEFT);
                int squareLeft = turn == WHITE ? getSquareNextTo(square, LEFT) : getSquareNextTo(square, RIGHT);

                Move enPassantMove;

                if (squareRight != -1 && squareRight == enPassantSquare) {
                    enPassantMove = Move(square, squareRightDiag);
                    enPassantMove.EN_PASSANT_FLAG = true;
                    enPassantMove.CAPTURE_FLAG = true;
                    moves.push_back(enPassantMove);
                } else if (squareLeft != -1 && squareLeft == enPassantSquare) {
                    enPassantMove = Move(square, squareLeftDiag);
                    enPassantMove.EN_PASSANT_FLAG = true;
                    enPassantMove.CAPTURE_FLAG = true;
                    moves.push_back(enPassantMove);
                }
            }
        }
    }

    void getKnightMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_KNIGHT] : bitboards[BLACK_KNIGHT];
        
        if (getBit(bitboard, square)) {
            int options[] = {
                getSquareNextTo(getSquareNextTo(square, UP, 2), RIGHT),
                getSquareNextTo(getSquareNextTo(square, UP, 2), LEFT),
                getSquareNextTo(getSquareNextTo(square, DOWN, 2), RIGHT),
                getSquareNextTo(getSquareNextTo(square, DOWN, 2), LEFT),
                getSquareNextTo(getSquareNextTo(square, RIGHT, 2), UP),
                getSquareNextTo(getSquareNextTo(square, RIGHT, 2), DOWN),
                getSquareNextTo(getSquareNextTo(square, LEFT, 2), UP),
                getSquareNextTo(getSquareNextTo(square, LEFT, 2), DOWN),
            };

            for (int j = 0; j < 8; j++) {
                int option = options[j];
                if (option != -1 && isOwnPiece(option) != 1) {
                    Move knightMove(square, option);
                    
                    if (isOwnPiece(option) == 0) {
                        knightMove.CAPTURE_FLAG = true;
                    }

                    moves.push_back(knightMove);
                }
            }
        }
    }

    void getMoveChains(std::vector<Move>& moves, int square, int direction) {
        int currentSquare = getSquareNextTo(square, direction);
        bool reachedBlock = false;
        while (currentSquare != -1 && !reachedBlock) {
            Move chainMove(square, currentSquare);

            int ownPiece = isOwnPiece(currentSquare);

            if (ownPiece != -1) {
                reachedBlock = true;
                if (ownPiece == 0) {
                    chainMove.CAPTURE_FLAG = true;
                    moves.push_back(chainMove);
                }
            } else {
                moves.push_back(chainMove);
            }

            currentSquare = getSquareNextTo(currentSquare, direction);
        }
    }

    void getBishopMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_BISHOP] : bitboards[BLACK_BISHOP];
        
        if (getBit(bitboard, square)) {
            getMoveChains(moves, square, UP * RIGHT);
            getMoveChains(moves, square, UP * LEFT);
            getMoveChains(moves, square, DOWN * RIGHT);
            getMoveChains(moves, square, DOWN * LEFT);
        }
    }

    void getRookMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_ROOK] : bitboards[BLACK_ROOK];

        if (getBit(bitboard, square)) {
            getMoveChains(moves, square, UP);
            getMoveChains(moves, square, DOWN);
            getMoveChains(moves, square, RIGHT);
            getMoveChains(moves, square, LEFT);
        }
    }

    void getQueenMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_QUEEN] : bitboards[BLACK_QUEEN];

        if (getBit(bitboard, square)) {
            getMoveChains(moves, square, UP);
            getMoveChains(moves, square, DOWN);
            getMoveChains(moves, square, RIGHT);
            getMoveChains(moves, square, LEFT);
            getMoveChains(moves, square, UP * RIGHT);
            getMoveChains(moves, square, UP * LEFT);
            getMoveChains(moves, square, DOWN * RIGHT);
            getMoveChains(moves, square, DOWN * LEFT);
        }
    }

    void getKingMoves(std::vector<Move>& moves, int square) {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_KING] : bitboards[BLACK_KING];
        bool canCastleKingSide = turn == WHITE ? whiteCanCastleKing : blackCanCastleKing;
        bool canCastleQueenSide = turn == WHITE ? whiteCanCastleQueen : blackCanCastleQueen;

        if (getBit(bitboard, square)) {
            int options[] = {
                getSquareNextTo(square, UP),
                getSquareNextTo(square, DOWN),
                getSquareNextTo(square, RIGHT),
                getSquareNextTo(square, LEFT),
                getSquareNextTo(square, UP * RIGHT),
                getSquareNextTo(square, UP * LEFT),
                getSquareNextTo(square, DOWN * RIGHT),
                getSquareNextTo(square, DOWN * LEFT),
            };

            for (int j = 0; j < 8; j++) {
                int option = options[j];
                if (option != -1 && isOwnPiece(option) != 1) {
                    Move kingMove(square, option);
                    
                    if (isOwnPiece(option) == 0) {
                        kingMove.CAPTURE_FLAG = true;
                    }

                    moves.push_back(kingMove);
                }
            }

            if (canCastleKingSide || canCastleQueenSide) {
                Move castleMove;
                int castleSquareKingSide = getSquareNextTo(square, RIGHT, 2);
                int castleSquareQueenSide = getSquareNextTo(square, LEFT, 2);

                bool whiteKingSideCastlingEmpty = turn == WHITE && getPieceAt(5) == EMPTY && getPieceAt(6) == EMPTY;
                bool blackKingSideCastlingEmpty = turn == BLACK && getPieceAt(61) == EMPTY && getPieceAt(62) == EMPTY;
                bool whiteQueenSideCastlingEmpty = turn == WHITE && getPieceAt(1) == EMPTY && getPieceAt(2) == EMPTY && getPieceAt(3) == EMPTY;
                bool blackQueenSideCastlingEmpty = turn == BLACK && getPieceAt(57) == EMPTY && getPieceAt(58) == EMPTY && getPieceAt(59) == EMPTY;
                
                if (canCastleKingSide && (whiteKingSideCastlingEmpty || blackKingSideCastlingEmpty)) {
                    castleMove = Move(square, castleSquareKingSide);
                    castleMove.CASTLE_FLAG = CASTLE_KING;
                } else if (canCastleQueenSide && (whiteQueenSideCastlingEmpty || blackQueenSideCastlingEmpty)) {
                    castleMove = Move(square, castleSquareQueenSide);
                    castleMove.CASTLE_FLAG = CASTLE_QUEEN;
                }

                moves.push_back(castleMove);
            }
        }
    }

    int findKingSquare() {
        uint64_t bitboard = turn == WHITE ? bitboards[WHITE_KING] : bitboards[BLACK_KING];
        
        int square = 0;
        while (bitboard >>= 1) square++;

        return square;
    }

    void copyBitboards(const uint64_t* original, uint64_t*& copy) {
        for (int i = 0; i < 12; i++) copy[i] = original[i];
    }
public:
    Position() {
        bitboards = parseFENboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    }

    Position(Position* position) {
        copyBitboards(position->bitboards, this->bitboards);

        this->turn = position->turn;

        this->whiteCanCastleKing = position->whiteCanCastleKing;
        this->whiteCanCastleQueen = position->whiteCanCastleQueen;
        this->blackCanCastleKing = position->blackCanCastleKing;
        this->blackCanCastleQueen = position->blackCanCastleQueen;

        this->enPassantSquare = position->enPassantSquare;
    }

    void setPositionWithFEN(const std::string& FENstring) {
        std::vector<std::string> FENtokens = parseFENstring(FENstring);
        bitboards = parseFENboard(FENtokens[0]);
        
        const char* FENside = FENtokens[1].c_str();

        if (strcmp(FENside, "w") == 0) {
            turn = WHITE;
        } else if (strcmp(FENside, "b") == 0) {
            turn = BLACK;
        } else {
            std::cout << "Error: input a valid FEN string" << std::endl;
            exit(100);
        }

        std::string FENcastleAvailability = FENtokens[2];
        
        if (FENcastleAvailability.find("K") == std::string::npos) whiteCanCastleKing = false;
        if (FENcastleAvailability.find("Q") == std::string::npos) whiteCanCastleQueen = false;
        if (FENcastleAvailability.find("k") == std::string::npos) blackCanCastleKing = false;
        if (FENcastleAvailability.find("q") == std::string::npos) blackCanCastleQueen = false;

        std::string FENenPassant = FENtokens[3];

        enPassantSquare = squareToIndex(FENenPassant);
    }

    int getPieceAt(int square) {
        int piece = EMPTY;

        if (square < 0) return piece;

        for (int i = 0; i < 12; i++) {
            if (getBit(bitboards[i], square)) piece = i;
        }

        return piece;
    }

    int isOwnPiece(int square) {
        int piece = getPieceAt(square);

        int side = -1;

        if (getPieceAt(square) != EMPTY && piece < 6) side = WHITE;
        if (getPieceAt(square) != EMPTY && piece > 5) side = BLACK;

        if (side == -1) return -1;
        else return (side == turn);
    }

    int getRank(int square) {
        return square / 8;
    }

    int getFile(int square) {
        return square % 8;
    }

    int getSquare(int rank, int file) {
        return rank * 8 + file;
    }

    bool getTurn() {
        return turn;
    }

    bool verifyBoardIntegrity() {
        for (int i = 0; i < 12; i++) {
            for (int j = 0; j < i; j++) {
                if ((bitboards[i] & bitboards[j]) != (uint64_t)0) return false;
            }
        }

        return true;
    }

    void reset() {
        bitboards = parseFENboard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
        turn = WHITE;
    }

    void getMoves(std::vector<Move>& moves, int square) {
        int pieceType = getPieceAt(square);

        if (pieceType == EMPTY) return;
        
        switch (pieceType % 6) {
            case PAWN: getPawnMoves(moves, square); break;
            case KNIGHT: getKnightMoves(moves, square); break;
            case BISHOP: getBishopMoves(moves, square); break;
            case ROOK: getRookMoves(moves, square); break;
            case QUEEN: getQueenMoves(moves, square); break;
            case KING: getKingMoves(moves, square); break;
        }
    }

    /*
    * Checks if the side that has the current turn is in check
    */
    bool isInCheck() {
        turn = !turn;
        std::vector<Move> moves;
        getAllMoves(moves);
        turn = !turn;

        int kingSquare = findKingSquare();

        for (auto& move : moves) {
            if (move.endingSquare == kingSquare) {
                return true;
            }
        }

        return false;
    }

    bool isLegalMove(const Move& move) {
        auto tempPosition = Position(this);
        tempPosition.applyMove(move);
        tempPosition.turn = !tempPosition.turn;

        return !tempPosition.isInCheck();
    }

    void applyMove(const Move& move) {
        int pieceType = getPieceAt(move.startingSquare);
        bitboards[pieceType] &= ~((uint64_t)1 << move.startingSquare);

        if (move.CAPTURE_FLAG) {
            int capturedPieceType = getPieceAt(enPassantSquare);
            if (!move.EN_PASSANT_FLAG) {
                capturedPieceType = getPieceAt(move.endingSquare);
                bitboards[capturedPieceType] &= ~((uint64_t)1 << move.endingSquare);
            } else {
                bitboards[capturedPieceType] &= ~((uint64_t)1 << enPassantSquare);
            }

            if (capturedPieceType % 6 == ROOK) {
                if (capturedPieceType < 6) {
                    if (move.endingSquare == 0) {
                        whiteCanCastleQueen = false;
                    } else if (move.endingSquare == 7) {
                        whiteCanCastleKing = false;
                    }
                } else if (capturedPieceType > 5) {
                    if (move.endingSquare == 56) {
                        blackCanCastleQueen = false;
                    } else if (move.endingSquare == 63) {
                        blackCanCastleKing = false;
                    }
                }
            }
        }

        if (move.PROMOTE_FLAG != PROMOTE_NONE) {
            int promotingPieceType = turn == WHITE ? move.PROMOTE_FLAG : move.PROMOTE_FLAG + 6;
            bitboards[promotingPieceType] |= ((uint64_t)1 << move.endingSquare);
        } else {
            bitboards[pieceType] |= ((uint64_t)1 << move.endingSquare);
        }

        if (move.PAWN_DOUBLE_MOVE_FLAG) {
            enPassantSquare = move.endingSquare;
        } else {
            enPassantSquare = -1;
        }

        if (move.CASTLE_FLAG != CASTLE_NONE) {
            int rookType = turn == WHITE ? WHITE_ROOK : BLACK_ROOK;

            int rookStartingSquare;
            int rookEndingSquare;

            if (turn == WHITE) {
                whiteCanCastleKing = false;
                whiteCanCastleQueen = false;
            } else {
                blackCanCastleKing = false;
                blackCanCastleQueen = false;
            }

            if (move.CASTLE_FLAG == CASTLE_KING) {
                rookStartingSquare = turn == WHITE ? 7 : 63;
                rookEndingSquare = turn == WHITE ? 5 : 61;
            } else {
                rookStartingSquare = turn == WHITE ? 0 : 56;
                rookEndingSquare = turn == WHITE ? 3 : 59;
            }

            bitboards[rookType] &= ~((uint64_t)1 << rookStartingSquare);
            bitboards[rookType] |= ((uint64_t)1 << rookEndingSquare);
        }

        if (pieceType % 6 == KING) {
            if (pieceType < 6) {
                whiteCanCastleKing = false;
                whiteCanCastleQueen = false;
            } else if (pieceType > 5) {
                blackCanCastleKing = false;
                blackCanCastleQueen = false;
            }
        }

        if (pieceType % 6 == ROOK) {
            if (pieceType < 6) {
                if (move.startingSquare == 0) {
                    whiteCanCastleQueen = false;
                } else if (move.startingSquare == 7) {
                    whiteCanCastleKing = false;
                }
            } else if (pieceType > 5) {
                if (move.startingSquare == 56) {
                    blackCanCastleQueen = false;
                } else if (move.startingSquare == 63) {
                    blackCanCastleKing = false;
                }
            }
        }
        
        turn = !turn;
    }

    float evaluate() {
        return 0.5f;
    }
};