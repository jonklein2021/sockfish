#include <iostream>
#include "GameState.h"
#include "constants.h"

bool validInput(const std::string& input) {
    if (input.size() != 5) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') &&
           (input[1] >= '1' && input[1] <= '8') &&
           (input[2] == ' ') &&
           (input[3] >= 'a' && input[3] <= 'h') &&
           (input[4] >= '1' && input[4] <= '8');
}

Move coordsToMove(const std::string& input) {
    int x0 = input[0] - 'a';
    int y0 = '8' - input[1];
    int x1 = input[3] - 'a';
    int y1 = '8' - input[4];

    return Move{{x0, y0}, {x1, y1}, None, false};
}

std::string moveToCoords(const Move& move) {
    std::string out = "";
    out += 'a' + move.from.x;
    out += '8' - move.from.y;
    out += ' ';
    out += 'a' + move.to.x;
    out += '8' - move.to.y;
    return out;
}

Move getMoveFromStdin(std::vector<Move>& legalMoves) {
    while (true) {
        // pick a random move to suggest
        std::string sample = moveToCoords(legalMoves[rand() % legalMoves.size()]);

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
        for (const Move& move : legalMoves) {
            if (candidate.equals(move)) {
                return move;
            }
        }

        if (candidate.pieceMoved == None) {
            std::cout << "Error: Illegal move" << std::endl;
        }
    }
}

int main() {
    GameState state(startingFen);
    std::cout << state.board.to_string() << std::endl;
    std::cout << (state.whiteToMove ? "White" : "Black") << " to move\n" << std::endl;
    std::vector<Move> moves = state.generateMoves();

    while (!moves.empty()) {
        Move candidate = getMoveFromStdin(moves);

        state.applyMove(candidate);

        // print the board
        std::cout << state.board.to_string() << std::endl;
        std::cout << (state.whiteToMove ? "White" : "Black") << " to move\n" << std::endl;
        
        // generate new moves for the next player
        moves = state.generateMoves();
    }

    return 0;
}