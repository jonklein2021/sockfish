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
    if (argc == 1) {   
        testPerftFile("testfiles/input");
    }

    std::string fen = argv[1];
    
    GameState state(fen);
    state.print();
    
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