#include <fstream>
#include "Engine.h"

void testPerftFile(const std::string &inputFilename) {
    std::ifstream in(inputFilename, std::ios::in);
    
    Engine cpu;
    GameState state;
    std::string line;
    while (std::getline(in, line)) {
        state = GameState(line);
        cpu.countPositionsBuildup(state, 6);
    }
    in.close();
    exit(0);
}

int main(int argc, char **argv) {
    std::string fen = argv[1];
    GameState state(fen);
    state.board.prettyPrint();
    std::cout << "White to move: " << state.whiteToMove << std::endl;
    std::cout << "White King Moved: " << state.whiteKingMoved << std::endl;
    std::cout << "Black King Moved: " << state.blackKingMoved << std::endl;
    std::cout << "White A Rook Moved: " << state.whiteRookAMoved << std::endl;
    std::cout << "White H Rook Moved: " << state.whiteRookHMoved << std::endl;
    std::cout << "Black A Rook Moved: " << state.blackRookAMoved << std::endl;
    std::cout << "Black H Rook Moved: " << state.blackRookHMoved << std::endl;
    std::cout << "En Passant Square: " << state.enPassantSquare.x << ", " << state.enPassantSquare.y << "\n" << std::endl;

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