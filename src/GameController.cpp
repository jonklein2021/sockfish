#include "GameController.h"

#include "MoveGenerator.h"

#include <iostream>

GameController::GameController(std::shared_ptr<Position> startPos,
                               std::unique_ptr<Engine> engine,
                               Color humanSide)
    : pos(startPos), engine(std::move(engine)), humanSide(humanSide) {}

bool GameController::isGameOver() {
    return PositionUtil::isTerminal(pos);
}

const Position &GameController::getPosition() const {
    return *pos;
}

std::vector<Move> GameController::legalMoves() {
    return MoveGenerator::generateLegal(pos);
}

void GameController::makeHumanMove(Move move) {
    pos->makeMove(move);
}

// Returns the move made by the engine, might be able to make void
Move GameController::makeAIMove() {
    Move best = engine->getMove(pos, legalMoves());
    pos->makeMove(best);
    return best;
}

// TODO
void GameController::handleEnd() {
    // print a user-friendly message depending on how the game ended
    std::cout << "Game over!\n";
}
