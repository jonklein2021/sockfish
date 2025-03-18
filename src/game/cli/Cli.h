#include "../Game.h"

class Cli : public Game {
private:
    bool validInput(const std::string& input);
    Move coordsToMove(const std::string& input);
    std::string moveToCoords(const Move& move);
    Move getMoveFromStdin();
public:
    Cli();
    Cli(const std::string &fen);
    void run();
};