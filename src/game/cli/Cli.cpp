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

Move Cli::coordsToMove(const std::string& input) {
    sf::Vector2<int> from = {input[0] - 'a', '8' - input[1]};
    sf::Vector2<int> to = {input[3] - 'a', '8' - input[4]};
    return Move(from, to, None, false);
}

std::string Cli::moveToCoords(const Move& move) {
    std::string out = "";
    out += 'a' + move.from.x;
    out += '8' - move.from.y;
    out += ' ';
    out += 'a' + move.to.x;
    out += '8' - move.to.y;
    return out;
}

Move Cli::getMoveFromStdin(std::vector<Move>& legalMoves) {
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
        Move candidate = coordsToMove(input);

        // check for pawn promotion
        if (
            (candidate.to.y == 0 && state.board.getPieceType(candidate.from) == WP) ||
            (candidate.to.y == 7 && state.board.getPieceType(candidate.from) == BP)
        ) {
            while (true) {
                std::cout << "Promotion piece (Q, R, B, N): ";
                std::string promotion;
                std::getline(std::cin, promotion);
                if (promotion == "Q" || promotion == "q") {
                    candidate.promotionPiece = state.whiteToMove ? WQ : BQ;
                    break;
                } else if (promotion == "R" || promotion == "r") {
                    candidate.promotionPiece = state.whiteToMove ? WR : BR;
                    break;
                } else if (promotion == "B" || promotion == "b") {
                    candidate.promotionPiece = state.whiteToMove ? WB : BB;
                    break;
                } else if (promotion == "N" || promotion == "n") {
                    candidate.promotionPiece = state.whiteToMove ? WN : BN;
                    break;
                } else {
                    std::cout << "Error: Invalid promotion piece" << std::endl;
                }
            }
        }

        for (const Move& move : legalMoves) {
            if (candidate.equals(move)) {
                return move;
            }
        }

        std::cout << "Error: Illegal move" << std::endl;
    }
}

Cli::Cli() : Cli(defaultFEN) {}

Cli::Cli(const std::string &fen) {
    // generate seed for random move generator
    std::srand(std::time(nullptr));

    state = GameState(fen);
    legalMoves = state.generateMoves();
}

void Cli::run() {
    std::cout << state.board.to_string() << std::endl;
    std::cout << (state.whiteToMove ? "White" : "Black") << " to move\n" << std::endl;
    
    while (!legalMoves.empty()) {
        Move candidate = getMoveFromStdin(legalMoves);

        state.applyMove(candidate);

        // print the board
        std::cout << state.board.to_string() << std::endl;
        std::cout << (state.whiteToMove ? "White" : "Black") << " to move\n" << std::endl;
        
        // update set of legal moves
        legalMoves = state.generateMoves();
    }
}
