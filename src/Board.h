#pragma once

#include <SFML/Graphics.hpp>
#include "Position.h"
#include "Pieces.h"

#define GRID_SIZE   60
#define GRID_SIZE_F (float)GRID_SIZE

#define EVAL_WIDTH GRID_SIZE / 2
#define EVAL_WIDTH_F (float)EVAL_WIDTH

#define BOARD_SIZE GRID_SIZE * 8
#define BOARD_SIZE_F (float)BOARD_SIZE

#define LIGHT_SQUARE sf::Color(0xffce9eff)
#define DARK_SQUARE  sf::Color(0xd18b47ff)
#define DARKENED     sf::Color(0x0f0f0f00)
#define HIGHLIGHT    sf::Color(0xffff94ff)
#define MOVE_DOT     sf::Color(0x06060640)
#define WHITE_BAR    sf::Color(0xf8f8f8ff)
#define BLACK_BAR    sf::Color(0x1f1f1fff)

#define MOVE_DOT_RADIUS GRID_SIZE_F / 5.0f

class Board {
private:
    Position position;
    sf::Texture textures[12];
    
    int highlightedSquare = -1;
    int promotionFile = -1;

    std::vector<Move> highlightedMoves;

    void drawBoard(sf::RenderWindow& window) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                sf::Color color;
                
                if (position.getSquare(7 - j, i) == highlightedSquare) {
                    color = HIGHLIGHT;
                } else {
                    color = (i + j) % 2 ? DARK_SQUARE : LIGHT_SQUARE;
                }

                float innerRectOffset = GRID_SIZE_F / 15.0f;
                float innerRectSize = GRID_SIZE_F - 2 * innerRectOffset;
                
                sf::RectangleShape outerRect(sf::Vector2f(GRID_SIZE_F, GRID_SIZE_F));
                sf::RectangleShape innerRect(sf::Vector2f(innerRectSize, innerRectSize));

                outerRect.setFillColor(color - DARKENED);
                innerRect.setFillColor(color);
                
                outerRect.setPosition(sf::Vector2f(GRID_SIZE_F * i, GRID_SIZE_F * j));
                innerRect.setPosition(sf::Vector2f(GRID_SIZE_F * i, GRID_SIZE_F * j) + sf::Vector2f(innerRectOffset, innerRectOffset));
                
                window.draw(outerRect);
                window.draw(innerRect);
            }
        }
    }

    void drawPieces(sf::RenderWindow& window) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                int square = i * 8 + j;
                int pieceType = position.getPieceAt(square);

                if (pieceType != EMPTY) {
                    sf::Sprite piece(textures[pieceType]);
                    piece.setPosition(sf::Vector2f(GRID_SIZE_F * j, GRID_SIZE_F * (7 - i)));

                    window.draw(piece);
                }
            }
        }
    }

    void drawMoves(sf::RenderWindow& window) {
        sf::CircleShape moveDot(MOVE_DOT_RADIUS, 64ULL);
        moveDot.setFillColor(MOVE_DOT);
        
        for (int i = 0; i < highlightedMoves.size(); i++) {
            int moveSquare = highlightedMoves[i].endingSquare;

            sf::Vector2f coords = getCoordsFromSquare(moveSquare);
            float offset = GRID_SIZE_F / 2 - MOVE_DOT_RADIUS;
            coords += sf::Vector2f(offset, offset);

            moveDot.setPosition(coords);
            window.draw(moveDot);
        }
    }

    void drawEval(sf::RenderWindow& window) {
        float eval = position.evaluate();

        float height = eval * BOARD_SIZE;
        sf::RectangleShape whiteEvalRect(sf::Vector2f(EVAL_WIDTH_F, height));
        sf::RectangleShape blackEvalRect(sf::Vector2f(EVAL_WIDTH_F, BOARD_SIZE_F - height));

        whiteEvalRect.setFillColor(WHITE_BAR);
        whiteEvalRect.setPosition(sf::Vector2f(BOARD_SIZE_F, BOARD_SIZE_F - height));

        blackEvalRect.setFillColor(BLACK_BAR);
        blackEvalRect.setPosition(sf::Vector2f(BOARD_SIZE_F, 0.0f));

        window.draw(whiteEvalRect);
        window.draw(blackEvalRect);
    }

    void handlePromotion(Move& move) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
            move.PROMOTE_FLAG = ROOK;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::B)) {
            move.PROMOTE_FLAG = BISHOP;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) {
            move.PROMOTE_FLAG = KNIGHT;
        } else {
            move.PROMOTE_FLAG = QUEEN;
        }
    }

    void initTextures() {
        for (int i = 0; i < 12; i++)  {
            textures[i] = sf::Texture();
            textures[i].create(GRID_SIZE_F, GRID_SIZE_F);
            textures[i].update(getPixels(i));
        }
    }

    int getSquareFromCoords(int x, int y) {
        if (x >= BOARD_SIZE) return -2;
        return position.getSquare(7 - y / GRID_SIZE, x / GRID_SIZE);
    }

    sf::Vector2f getCoordsFromSquare(int square) {
        return sf::Vector2f(GRID_SIZE_F * position.getFile(square), GRID_SIZE_F * (7 - position.getRank(square)));
    }
public:
    Board() {
        position = Position();
        initTextures();
    }
    
    Board(const std::string& FENstring) {
        position = Position(FENstring);
        initTextures();
    };
    
    void draw(sf::RenderWindow& window) {
        drawBoard(window);
        drawPieces(window);
        drawMoves(window);
        drawEval(window);
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) {
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonReleased:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int targetSquare = getSquareFromCoords(event.mouseButton.x, event.mouseButton.y);
                    if (position.isOwnPiece(targetSquare) == 1) {
                        highlightedMoves.clear();
                        if (targetSquare == highlightedSquare) {
                            highlightedSquare = -1;
                        } else {
                            highlightedSquare = targetSquare;
                            position.getMoves(highlightedMoves, highlightedSquare);
                        }
                    } else {
                        bool foundMove = false;
                        int i = 0;
                        while (i < highlightedMoves.size() && !foundMove) {
                            Move highlightedMove = highlightedMoves[i];
                            if (highlightedMove.startingSquare == highlightedSquare && highlightedMove.endingSquare == targetSquare && position.isLegalMove(highlightedMove)) {
                                if (highlightedMove.PROMOTE_FLAG != PROMOTE_NONE) handlePromotion(highlightedMove);
                                position.applyMove(highlightedMove);
                                foundMove = true;
                            }
                            i++;
                        }
                        highlightedMoves.clear();
                    }
                }
                break;
            case sf::Event::KeyReleased:
                if (event.key.code == sf::Keyboard::Key::R) position.reset();
                break;
        }
    }
};
