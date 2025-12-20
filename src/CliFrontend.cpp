#include "CliFrontend.h"

#include "bit_tools.h"
#include "types.h"

#include <iostream>

CliFrontend::CliFrontend(GameController &game)
    : game(game) {}

Move CliFrontend::getMoveFromStdin() {
    std::vector<Move> legalMoves;
    while (true) {
        // pick a random move to suggest
        const std::string sample = legalMoves[std::rand() % legalMoves.size()].toCoordinateString();

        // prompt user for input
        std::cout << "Enter move (example: " << sample << ") or q to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            exit(0);
        }

        if (!validateCoords(input)) {
            std::cout << "Error: Invalid input" << std::endl;
            continue;
        }

        // check if the move is legal before returning it
        // todo: check for castling
        Move candidate = coordsToMove(input);

        // check for pawn promotion
        bool pawnPromoting = (candidate.to.y == 0 && state.pieceAt(candidate.from) == WP) ||
                             (candidate.to.y == 7 && state.pieceAt(candidate.from) == BP);

        // temporarily set the promotion piece to a queen
        // so that it can match a legal move
        if (pawnPromoting) {
            candidate.promotionPiece = state.whiteToMove ? WQ : BQ;
        }

        bool validMove = false;
        for (const Move &move : legalMoves) {
            if (candidate == move) {
                validMove = true;
                candidate = move;
                break;
            }
        }

        if (validMove) {
            break;
        }

        std::cout << "Error: Illegal move" << std::endl;
    }

    if (pawnPromoting) {
        while (true) {
            std::cout << "Promotion piece (Q, N, R, B): ";
            std::string promotion;
            std::getline(std::cin, promotion);
            if (promotion == "Q" || promotion == "q") {
                candidate.promotionPiece = state.whiteToMove ? WQ : BQ;
                break;
            } else if (promotion == "N" || promotion == "n") {
                candidate.promotionPiece = state.whiteToMove ? WN : BN;
                break;
            } else if (promotion == "R" || promotion == "r") {
                candidate.promotionPiece = state.whiteToMove ? WR : BR;
                break;
            } else if (promotion == "B" || promotion == "b") {
                candidate.promotionPiece = state.whiteToMove ? WB : BB;
                break;
            } else {
                std::cout << "Error: Invalid promotion piece" << std::endl;
            }
        }
    }

    return candidate;
}

void CliFrontend::run() {
    legalMoves = state.generateMoves();

    while (!state.isTerminal()) {
        state.print();

        // std::cout << cpu.get_eval(state) << std::endl;
        std::cout << (state.whiteToMove ? "White" : "Black") << " to move\n" << std::endl;

        Move next;
        if (playersTurn) {
            // get move from stdin
            next = getMoveFromStdin();
        } else {
            // get move from engine
            next = cpu.getMove(state, legalMoves);
            std::cout << "CPU's move: " << moveToCoords(next) << std::endl;
        }

        std::cout << next.toString() << std::endl;

        // update state with new move and push hash to history
        state.makeMove(next);
        state.md.history.push_back(state.hash());
        prettyPrintPosition(state.pieceBitsData(), playerIsWhite);

        // update set of legal moves
        legalMoves = state.generateMoves();

        playersTurn = !playersTurn;
    }

    if (legalMoves.empty()) {
        if (state.isCheck()) {
            std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!"
                      << std::endl;
        } else {
            std::cout << "Stalemate!" << std::endl;
        }
    } else {
        std::cout << "Draw!" << std::endl;
    }
}
