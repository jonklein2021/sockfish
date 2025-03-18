#include "GameState.h"

class CliGame {
private:
    GameState state;
    std::vector<Move> legalMoves;

    bool validInput(const std::string& input);
    Move coordsToMove(const std::string& input);
    std::string moveToCoords(const Move& move);
    Move getMoveFromStdin(std::vector<Move>& legalMoves);
public:
    CliGame();
    void run();
};