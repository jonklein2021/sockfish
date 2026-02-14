#include "GameController.h"

#include "src/core/PGNWriter.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <iostream>

GameController::GameController(Position &startPos, Engine &engine, Color humanSide)
    : pos(startPos),
      engine(engine),
      pgnWriter(humanSide == WHITE ? "Human" : "Sockfish",
                humanSide == BLACK ? "Human" : "Sockfish"),
      humanSide(humanSide) {
    MoveGenerator::generateLegal(legalMoves, startPos);
}

bool GameController::isGameOver() {
    return PositionUtil::isTerminal(pos);
}

std::vector<Move> GameController::getLegalMoves() const {
    return legalMoves;
}

void GameController::makeManualMove(Move move) {
    // write to PGN file BEFORE move is made
    pgnWriter.writeMove(pos, move);

    // make move and update legal moves
    pos.makeMove(move);
    MoveGenerator::generateLegal(legalMoves, pos);
}

void GameController::makeAIMove() {
    makeManualMove(engine.getMove(pos));
}

void GameController::handleEnd() {
    // exit early if position isn't terminal
    if (!PositionUtil::isTerminal(pos)) {
        return;
    }

    std::cout << "Game over!\n";
    std::string result = "*";
    switch (PositionUtil::getGameStatus(pos)) {
        case CHECKMATE:
            result = (pos.getSideToMove() == WHITE) ? "0-1" : "1-0";
            std::cout << COLOR_NAMES[otherColor(pos.getSideToMove())] << " won by checkmate.\n";
            break;
        case DRAW_BY_REPETITION: {
            result = "1/2-1/2";
            std::cout << "Draw by threefold repetition.\n";
            break;
        }
        case DRAW_BY_STALEMATE: {
            result = "1/2-1/2";
            std::cout << "Draw by stalemate.\n";
            break;
        }
        case DRAW_BY_INSUFFICIENT_MATERIAL: {
            result = "1/2-1/2";
            std::cout << "Draw by insufficient material.\n";
            break;
        }
        case DRAW_BY_50_MOVE_RULE: {
            result = "1/2-1/2";
            std::cout << "Draw by 50 move rule.\n";
            break;
        }
        default: {
            std::cout << "Not sure how the game ended but hope you had fun :)\n";
            break;
        }
    }

    pgnWriter.writeRawString(result + "\n");
    pgnWriter.closeFile();
}
