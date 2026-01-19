#include "CliFrontend.h"

#include "src/core/Move.h"
#include "src/core/Printers.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <iostream>

CliFrontend::CliFrontend(GameController &game)
    : game(game) {}

bool CliFrontend::validateMoveInput(const std::string &input) {
    return validateCoords(input) || (input == "O-O") || (input == "OO") || (input == "O-O-O") ||
           (input == "OOO");
}

std::string CliFrontend::formatMove(const Move m) {
    if (m.isKCastles()) {
        return "O-O";
    } else if (m.isQCastles()) {
        return "O-O-O";
    } else {
        return m.toCoordinateString();
    }
}

std::string CliFrontend::getMoveSuggestion(const std::vector<Move> &legalMoves) {
    Move random = legalMoves[std::rand() % legalMoves.size()];
    return formatMove(random);
}

Move CliFrontend::getMoveFromStdin() {
    const std::vector<Move> legalMoves = game.getLegalMoves();
    Move candidate;
    bool validMove = false;
    bool pawnPromoting = false;

    while (!validMove) {
        // pick a random move to suggest
        std::cout << "Enter move (example: " << getMoveSuggestion(legalMoves) << ") or q to quit: ";

        // prompt user for input
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

        // check if the move is legal before returning it
        for (Move move : legalMoves) {
            if (candidate.softEquals(move)) {
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
                std::cout << "Error: Invalid promotion piece. Please try again." << std::endl;
            }
        }
    }

    return candidate;
}

void CliFrontend::run() {
    // print initial position
    Printers::prettyPrintPosition(game.getPosition());

    // game loop
    while (!game.isGameOver()) {
        Move m;
        if (game.getSideToMove() == game.getHumanSide()) {
            // get move from stdin
            m = getMoveFromStdin();
            game.makeHumanMove(m);
        } else {
            // get move from engine
            m = game.makeAIMove();
        }

        // print resultant position
        Printers::prettyPrintPosition(game.getPosition());
    }

    game.handleEnd();
}
