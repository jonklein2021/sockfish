#include "../Game.h"

class Cli : public Game {
private:
    bool validInput(const std::string& input);
    Move coordsToMove(const std::string& input);
    std::string moveToCoords(const Move& move);
    Move getMoveFromStdin(std::vector<Move>& legalMoves);
public:
    Cli();
    Cli(const std::string &fen);
    void run();
};