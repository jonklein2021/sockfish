#include <fstream>
#include "Engine.h"

void testPerftFile(const std::string &inputFilename) {
    std::ifstream in(inputFilename, std::ios::in);
    
    Engine cpu;
    GameState state;
    std::string line;
    while (std::getline(in, line)) {
        state = GameState(line);
        cpu.countPositionsBuildup(state, 4);
    }
    in.close();
    exit(0);
}

int main(int argc, char **argv) {
    testPerftFile("testfiles/input");

    std::string fen = argv[1];
    GameState state(fen);
    state.board.prettyPrint();
    std::cout << "White to move: " << state.whiteToMove << std::endl;
    std::cout << "White Kingside Castle: " << state.md.whiteKCastleRights << std::endl;
    std::cout << "White Queenside Castle: " << state.md.whiteQCastleRights << std::endl;
    std::cout << "Black Kingside Castle: " << state.md.blackKCastleRights << std::endl;
    std::cout << "Black Queenside Castle: " << state.md.blackQCastleRights << std::endl;
    std::cout << "En Passant Square: " << state.md.enPassantSquare.x << ", " << state.md.enPassantSquare.y << "\n" << std::endl;

    // generate moves
    std::vector<Move> legalMoves = state.generateMoves();
    for (const Move &m : legalMoves) {
        std::cout << moveToCoords(m) << std::endl;
    }

    // count positions
    Engine *cpu;

    // cpu->countPositionsBuildup(state, 6);

    for (int d = 1; d <= 6; d++) {
        std::cout << "Depth: " << d << std::endl;
        cpu->countPositions(state, d);
    }
        
    return 0;    
}