#include <iostream>

#include "constants.h"
#include "Cli.h"

bool Cli::validInput(const std::string& input) {
    if (input.size() != 5) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') &&
           (input[1] >= '1' && input[1] <= '8') &&
           (input[2] == ' ') &&
           (input[3] >= 'a' && input[3] <= 'h') &&
           (input[4] >= '1' && input[4] <= '8');
}

Move Cli::getMoveFromStdin() {
    Move candidate;
    bool pawnPromoting;
    while (true) {
        // pick a random move to suggest
        std::string sample = moveToCoords(legalMoves[std::rand() % legalMoves.size()]);

        // prompt user for input
        std::cout << "Enter move (example: " + sample + ") or q to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            exit(0);
        }

        if (!validInput(input)) {
            std::cout << "Error: Invalid input" << std::endl;
            continue;
        }

        // check if the move is legal before returning it
        candidate = coordsToMove(input);

        // check for pawn promotion
        pawnPromoting = (candidate.to.y == 0 && state.board.getPieceType(candidate.from) == WP) || (candidate.to.y == 7 && state.board.getPieceType(candidate.from) == BP);

        // temporarily set the promotion piece to a queen
        // so that it can match a legal move
        if (pawnPromoting) {
            candidate.promotionPiece = state.whiteToMove ? WQ : BQ;
        }

        bool validMove = false;
        for (const Move& move : legalMoves) {
            if (candidate.equals(move)) {
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

Cli::Cli() : Game() {}

Cli::Cli(const std::string &fen, int depth) : Game(fen, depth) {}

Cli::Cli(const std::string &fen, int depth, bool playerIsWhite) : Game(fen, depth, playerIsWhite) {}

void Cli::run() {
    legalMoves = state.generateMoves();

    while (!state.isTerminal()) {
        state.board.prettyPrint(playerIsWhite);
        std::cout << cpu.get_eval(state) << std::endl;
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

        std::cout << next.to_string() << std::endl;

        // update state with new move and push hash to history
        state.makeMove(next);
        state.md.history.push_back(state.hash());
        state.board.prettyPrint(playerIsWhite);

        // update set of legal moves
        legalMoves = state.generateMoves();

        playersTurn = !playersTurn;
    }

    if (legalMoves.empty()) {
        if (state.isCheck()) {
            std::cout << "Checkmate! " << (state.whiteToMove ? "Black" : "White") << " wins!" << std::endl;
        } else {
            std::cout << "Stalemate!" << std::endl;
        }
    } else {
        std::cout << "Draw!" << std::endl;
    }
}
