#include "GameController.h"

#include "src/core/Notation.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <chrono>
#include <iomanip>
#include <iostream>

GameController::GameController(Position &startPos, std::unique_ptr<Engine> engine, Color humanSide)
    : pos(startPos),
      engine(std::move(engine)),
      outFile(DEFAULT_OUT_FILE.data(), std::ios::out),
      humanSide(humanSide) {
    initPGN();
    MoveGenerator::generateLegal(legalMoves, startPos);
}

void GameController::initPGN() {
    if (!outFile.is_open()) {
        std::cerr << "Error: Failed to open PGN file\n";
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm *tm = std::localtime(&t);

    outFile << "[Event \"Local Game\"]\n";
    outFile << "[Site \"Local\"]\n";
    outFile << "[Date \"" << std::put_time(tm, "%Y.%m.%d") << "\"]\n";
    outFile << "[Round \"1\"]\n";
    outFile << "[White \"" << (humanSide == WHITE ? "Human" : "Sockfish") << "\"]\n";
    outFile << "[Black \"" << (humanSide == BLACK ? "Human" : "Sockfish") << "\"]\n";
    outFile << "[Result \"*\"]\n\n";
}

void GameController::appendToPGN(const std::string &sanString) {
    if (plyCount % 2 == 0) {
        outFile << (plyCount / 2 + 1) << ". ";
    }

    outFile << sanString << " ";
    plyCount++;
}

bool GameController::isGameOver() {
    return PositionUtil::isTerminal(pos);
}

std::vector<Move> GameController::getLegalMoves() const {
    return legalMoves;
}

void GameController::makeHumanMove(Move move) {
    // write SAN string to PGN file BEFORE move is made
    std::string moveSAN = Notation::moveToSAN(move, pos);
    appendToPGN(moveSAN);

    // make move and update legal moves
    pos.makeMove(move);
    MoveGenerator::generateLegal(legalMoves, pos);

    // log move
    std::cout << moveSAN << " played\n";
    std::cout << COLOR_NAMES[pos.getSideToMove()] << " to move\n";
}

void GameController::makeAIMove() {
    Move best = engine->getMove(pos);

    // write SAN string to PGN file BEFORE move is made
    std::string moveSAN = Notation::moveToSAN(best, pos);
    appendToPGN(moveSAN);

    // make move and update legal moves
    pos.makeMove(best);
    MoveGenerator::generateLegal(legalMoves, pos);

    // log move
    std::cout << moveSAN << " played\n";
    std::cout << COLOR_NAMES[pos.getSideToMove()] << " to move\n";
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

    outFile << result << "\n";
    outFile << "\n";
    outFile.close();
}
