#include "Config.h"

#include <iostream>

void Config::parseArgs(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            exit(0);
        } else if (arg == "-g" || arg == "--gui") {
            useGui = true;
        } else if (arg == "-w" || arg == "--white") {
            randomColor = false;
            humanSide = WHITE;
        } else if (arg == "-b" || arg == "--black") {
            randomColor = false;
            humanSide = BLACK;
        } else if (arg == "-t") {
            if (i + 1 < argc) {
                // TODO: validate this theme
                pieceTheme = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -t option requires a theme" << std::endl;
                printUsage();
                exit(1);
            }
        } else if (arg == "-f") {
            if (i + 1 < argc) {
                fen = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -f option requires a "
                             "FEN string"
                          << std::endl;
                printUsage();
                exit(1);
            }
        } else if (arg == "-d") {
            if (i + 1 < argc) {
                searchDepth = std::stoi(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Error: -d option requires a depth" << std::endl;
                printUsage();
                exit(1);
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            printUsage();
            exit(1);
        }
    }

    if (randomColor) {
        randomizeSides();
    }
}

void Config::randomizeSides() {
    humanSide = std::rand() % 2 == 0 ? WHITE : BLACK;
}

void Config::printUsage() {
    std::cout << "Usage: ./main [options]\n"
                 "Options:\n"
                 "  -h, --help      Display this message\n"
                 "  -g, --gui       Run the game in GUI mode\n"
                 "  -w, --white     Play with the white pieces\n"
                 "  -b, --black     Play with the black pieces\n"
                 "  -f  <string>    Specify a custom FEN string for the starting "
                 "position\n"
                 "  -d  <depth>     Specify the depth of the minimax search "
                 "(default: "
                 "4)\n"
                 "  -t  <theme>     Specify the piece theme for the GUI\n"
              << std::endl;
}

void Config::print() const {
    std::cout << "Initializing game in " << (useGui ? "CLI" : "GUI") << " mode...\n";
    std::cout << "  Player color: " << (humanSide == WHITE ? "white" : "black")
              << (randomColor ? " (random)" : "") << "\n";
    std::cout << "  FEN: " << fen << "\n";
    std::cout << "  Depth: " << searchDepth << "\n";
    std::cout << "  Piece theme: " << pieceTheme << std::endl;
}
