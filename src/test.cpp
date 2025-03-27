#include "Engine.h"
#include <fstream>

void testMove(GameState &state) {
    std::cout << " -- state.pieceAttacks[WN] --\n";
    printBitboard(state.pieceAttacks[WN]);
    std::cout << " -- state.pieceAttacks[WK] --\n";
    printBitboard(state.pieceAttacks[WK]);
    std::cout << " -- state.pieceAttacks[BR] --\n";
    printBitboard(state.pieceAttacks[BR]);
    std::cout << " -- state.pieceAttacks[BK] --\n";
    printBitboard(state.pieceAttacks[BK]);

    Move move{{7, 6}, {6, 6}, WK};
    GameState copy = state;
    Metadata md = copy.makeMove(move);

    copy.print();

    std::cout << " -- state.pieceAttacks[WN] after move --\n";
    printBitboard(state.pieceAttacks[WN]);
    std::cout << " -- copy.pieceAttacks[WK] after move --\n";
    printBitboard(copy.pieceAttacks[WK]);
    std::cout << " -- copy.pieceAttacks[BR] after move --\n";
    printBitboard(copy.pieceAttacks[BR]);
    std::cout << " -- copy.pieceAttacks[BK] after move --\n";
    printBitboard(copy.pieceAttacks[BK]);

    std::cout << copy.underAttack(copy.pieceBits[WK], false) << std::endl;

    copy.unmakeMove(move, md);
    copy.print();

    std::cout << " -- state.pieceAttacks[WN] after unmove --\n";
    printBitboard(state.pieceAttacks[WN]);
    std::cout << " -- copy.pieceAttacks[WK] after unmove --\n";
    printBitboard(copy.pieceAttacks[WK]);
    std::cout << " -- copy.pieceAttacks[BR] after unmove --\n";
    printBitboard(copy.pieceAttacks[BR]);
    std::cout << " -- copy.pieceAttacks[BK] after unmove --\n";
    printBitboard(copy.pieceAttacks[BK]);

    std::cout << (state.hash() == copy.hash()) << std::endl;
}

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
    if (argc == 1) {
        testPerftFile("testfiles/input");
    }

    std::string fen = argv[1];

    GameState state(fen);
    state.print();

    // testMove(state);

    // generate moves
    std::vector<Move> legalMoves = state.generateMoves();

    std::cout << legalMoves.size() << " legal moves" << std::endl;
    for (const Move &m : legalMoves) {
        std::cout << m.to_string() << std::endl;
    }

    exit(0);

    Engine *cpu = new Engine(6);

    // count positions
    for (int d = 1; d <= 2; d++) {
        std::cout << "Depth: " << d << std::endl;
        cpu->countPositions(state, d);
    }

    return 0;
}
