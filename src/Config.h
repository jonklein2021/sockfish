#include "types.h"

#include <ctime>
#include <string>

struct Config {
    std::string fen = std::string(STARTING_POSITION_FEN);
    std::string pieceTheme = std::string(DEFAULT_THEME_PATH);
    int searchDepth = 4;
    Color humanSide = WHITE;
    bool randomColor = true;
    bool useGui = false;

    void parseArgs(int argc, char **argv);
    static void printUsage();
    void print() const;
    void randomizeSides();
};
