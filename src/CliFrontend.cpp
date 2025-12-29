#include "CliFrontend.h"

#include "GameController.h"
#include "Move.h"
#include "types.h"

#include <iostream>
#include <sstream>

CliFrontend::CliFrontend(GameController &game)
    : game(std::move(game)) {}

Move CliFrontend::getMoveFromStdin() {
    const std::vector<Move> legalMoves = game.legalMoves();
    Move candidate;
    bool validMove = false;
    bool pawnPromoting = false;

    while (!validMove) {
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
        candidate = Move::fromCoordinateString(input);
        const Square from = candidate.fromSquare(), to = candidate.toSquare();
        const Piece pieceMoved = game.getPosition().getBoard().pieceAt(from);

        // check for pawn promotion
        pawnPromoting = (pieceMoved == WP && to <= h8) || (pieceMoved == BP && to >= a1);

        // temporarily set the promotion piece to a queen
        // so that it can match a legal move
        if (pawnPromoting) {
            candidate.setPromotedPiece(QUEEN);
        }

        for (const Move &move : legalMoves) {
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
                candidate.setPromotedPiece(QUEEN);
                break;
            } else if (promotion == "N" || promotion == "n") {
                candidate.setPromotedPiece(KNIGHT);
                break;
            } else if (promotion == "R" || promotion == "r") {
                candidate.setPromotedPiece(ROOK);
                break;
            } else if (promotion == "B" || promotion == "b") {
                candidate.setPromotedPiece(BISHOP);
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

    puts("legalMoves:");
    for (Move &m : legalMoves) {
        printf("%s\n", m.toString().c_str());
    }

    while (!game.isGameOver()) {
        printBoard();

        // std::cout << cpu.get_eval(state) << std::endl;
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

// TODO
void CliFrontend::printBoard(bool flip) {
    std::ostringstream out("\n");

    for (int i = 0; i < 8; i++) {
        int rank = flip ? 7 - i : i;

        // row border
        out << "   +----+----+----+----+----+----+----+----+\n";

        // rank number
        out << " " << (8 - rank) << " ";

        for (int j = 0; j < 8; j++) {
            int file = flip ? 7 - j : j;
            Square sq = xyToSquare(file, rank);

            out << "| ";

            Piece p = game.getPosition().getBoard().pieceAt(sq);
            if (p == NONE) {
                out << "   ";
            } else {
                out << pieceFilenames[p] << " ";
            }
        }

        out << "|\n";
    }

    // bottom border
    out << "   +----+----+----+----+----+----+----+----+\n";

    // file indices
    out << "     ";
    if (flip) {
        out << "h    g    f    e    d    c    b    a  \n";
    } else {
        out << "a    b    c    d    e    f    g    h  \n";
    }

    std::cout << out.str() << std::endl;
}
