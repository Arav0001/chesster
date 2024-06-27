#include "Board.h"

int main(int argc, char *argv[]) {
    // if (argc != 3) {
    //     std::cout << "Usage: chesster -play *starting side or valid FEN string of starting position* -evaluate *PGN file path*\n" << std::endl;
    //     exit(99);
    // }

    // if (strcmp(argv[1], "-play") == 0) {
    //     std::cout << "playing\n";
    // } else if (strcmp(argv[1], "-evaluate") == 0) {
    //     std::cout << "evaluating\n";
    // } else {
    //     std::cout << "Usage: chesster -play *starting side or valid FEN string of starting position* -evaluate *PGN file path*" << std::endl;
    //     exit(99);
    // }

    sf::RenderWindow window(sf::VideoMode(BOARD_SIZE + (unsigned int)EVAL_WIDTH, BOARD_SIZE), "chesster", sf::Style::Titlebar | sf::Style::Close);

    Board board = Board();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            board.handleEvent(event, window);
        }

        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}
