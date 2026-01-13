#include "src/core/types.h"

#include <ctime>
#include <string>

struct Config {
    std::string fen = std::string(STARTING_POSITION_FEN);
    std::string pieceTheme = std::string(DEFAULT_PIECE_THEME);
    int searchDepth = 4;
    Color humanSide = WHITE;
    bool randomColor = true;
    bool useGui = false;

    static void printUsage();
    void parseArgs(int argc, char **argv);
    void print() const;
    void randomizeSides();
};
