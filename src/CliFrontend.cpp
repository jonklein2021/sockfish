#include "CliFrontend.h"

#include "GameController.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "Printers.h"
#include "types.h"

#include <iostream>

CliFrontend::CliFrontend(GameController &game)
    : game(std::move(game)) {}

bool CliFrontend::validateMoveInput(const std::string &input) {
    return validateCoords(input) || (input == "O-O") || (input == "OO") || (input == "O-O-O") ||
           (input == "OOO");
}

Move CliFrontend::getMoveFromStdin() {
    const std::vector<Move> legalMoves = game.legalMoves();
    Move candidate;
    bool validMove = false;
    bool pawnPromoting = false;

    while (!validMove) {
        // pick a random move to suggest
        const std::string sample = legalMoves[std::rand() % legalMoves.size()].toCoordinateString();

        // DEBUG: print legal moves
        Printers::printMoveList(legalMoves, game.getPosition());

        // prompt user for input
        std::cout << "Enter move (example: " << sample << ") or q to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            exit(0);
        }

        if (!validateMoveInput(input)) {
            std::cout << "Error: Invalid input" << std::endl;
            continue;
        }

        if (validateCoords(input)) {
            candidate = Move::fromCoordinateString(input);
        } else {
            // must be a castling move
            if (input == "O-O" || input == "OO") {
                // Kingside
                candidate = MoveGenerator::createCastlingMove(false, game.getHumanSide());
            } else {
                // Queenside
                candidate = MoveGenerator::createCastlingMove(true, game.getHumanSide());
            }
        }
        const Square from = candidate.getFromSquare(), to = candidate.getToSquare();
        const Piece pieceMoved = game.getPosition().pieceAt(from);

        // check for pawn promotion
        pawnPromoting = (pieceMoved == WP && to <= h8) || (pieceMoved == BP && to >= a1);

        // temporarily set the promotion piece to a queen
        // so that it can match a legal move
        // TODO: Uncomment this once move generation is complete
        // if (pawnPromoting) {
        //     candidate.setPromotedPieceType(QUEEN);
        // }

        // check if the move is legal before returning it
        for (Move move : legalMoves) {
            if (candidate == move) {
                validMove = true;
                candidate = move;
                break;
            }
        }

        if (!validMove) {
            std::cout << "Error: Illegal move. Please try again" << std::endl;
        }
    }

    if (pawnPromoting) {
        while (true) {
            std::cout << "Promotion piece (Q, N, R, B): ";
            std::string promotion;
            std::getline(std::cin, promotion);
            if (promotion == "Q" || promotion == "q") {
                candidate.setPromotedPieceType(QUEEN);
                break;
            } else if (promotion == "N" || promotion == "n") {
                candidate.setPromotedPieceType(KNIGHT);
                break;
            } else if (promotion == "R" || promotion == "r") {
                candidate.setPromotedPieceType(ROOK);
                break;
            } else if (promotion == "B" || promotion == "b") {
                candidate.setPromotedPieceType(BISHOP);
                break;
            } else {
                std::cout << "Error: Invalid promotion piece" << std::endl;
            }
        }
    }

    return candidate;
}

void CliFrontend::run() {
    auto legalMoves = game.legalMoves();

    while (!game.isGameOver()) {
        Printers::prettyPrintPosition(game.getPosition());

        std::cout << (game.getSideToMove() == WHITE ? "White" : "Black") << " to move\n"
                  << std::endl;

        if (game.getSideToMove() == game.getHumanSide()) {
            // get move from stdin
            game.makeHumanMove(getMoveFromStdin());
        } else {
            // get move from engine
            // game.makeAIMove();
            game.makeHumanMove(getMoveFromStdin());
        }
    }

    std::cout << "Game over!" << std::endl;
}
