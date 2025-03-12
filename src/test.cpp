#include <iostream>
#include <string>
#include <vector>

#include "constants.h"
#include "game.h"

int main() {
    std::string initFen = "5k2/8/8/8/7b/6P1/5P2/4K3";
    BitBoard board = fenToBitBoard(initFen);
    GameState state(board, true, false, false, false, false, false, false);

    std::cout << "BOARD:" << std::endl;
    prettyPrint(board);

    std::vector<Move> moves = state.generateMoves();

    std::cout << "LEGAL MOVES:" << std::endl;
    for (const Move &m : moves) {
        std::cout << to_string(m) << std::endl;
    }

    return 0;
}