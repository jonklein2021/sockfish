#include "types.h"

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

struct Config {
    std::string fen = std::string(STARTING_POSITION_FEN);
    std::string pieceTheme = std::string(DEFAULT_THEME_PATH);
    int searchDepth = 4;
    Color humanSide = WHITE;
    bool randomColor = true;
    bool useGui = true;

    void parseArgs(int argc, char **argv) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-h" || arg == "--help") {
                Config::printUsage();
                exit(0);
            } else if (arg == "-c" || arg == "--cli") {
                useGui = false;
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
                    pieceTheme = argv[i + 1];
                    i++;
                } else {
                    std::cerr << "Error: -t option requires a theme" << std::endl;
                    Config::printUsage();
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
                    Config::printUsage();
                    exit(1);
                }
            } else if (arg == "-d") {
                if (i + 1 < argc) {
                    searchDepth = std::stoi(argv[i + 1]);
                    i++;
                } else {
                    std::cerr << "Error: -d option requires a depth" << std::endl;
                    Config::printUsage();
                    exit(1);
                }
            } else {
                std::cerr << "Error: Unknown option " << arg << std::endl;
                Config::printUsage();
                exit(1);
            }
        }

        // randomly assign colors if no override
        if (randomColor) {
            humanSide = std::rand() % 2 == 0 ? WHITE : BLACK;
        }
    }

    static void printUsage() {
        std::ostringstream ss;

        ss << "Usage: ./build/main [options]\n";
        ss << "Options:\n";
        ss << "  -h, --help      Display this message\n";
        ss << "  -c, --cli       Run the game in CLI mode (default)\n";
        ss << "  -g, --gui       Run the game in GUI mode\n";
        ss << "  -w, --white     Play with the white pieces\n";
        ss << "  -b, --black     Play with the black pieces\n";
        ss << "  -f  <string>    Specify a custom FEN string for the starting "
              "position\n";
        ss << "  -d  <depth>     Specify the depth of the minimax search "
              "(default: "
              "4)\n";
        ss << "  -t  <theme>     Specify the piece theme for the GUI\n";

        std::cout << ss.str() << std::endl;
    }

    void print() {
        std::cout << "Initializing game in " << (useGui ? "CLI" : "GUI") << " mode...\n";
        std::cout << "  Player color: " << (humanSide == WHITE ? "white" : "black")
                  << (randomColor ? " (random)" : "") << "\n";
        std::cout << "  FEN: " << fen << "\n";
        std::cout << "  Depth: " << searchDepth << "\n";
        std::cout << "  Piece theme: " << pieceTheme << std::endl;
    }
};
