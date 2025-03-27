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
    if (argc < 2) {   
        testPerftFile("testfiles/input");
    }

    std::string fen = argv[1];
    GameState state(fen);
    state.print();

    Engine *cpu = new Engine(6);
    
    // evaluate init state
    cpu->get_eval(state);
    exit(0);
    
    // generate moves
    std::vector<Move> legalMoves = state.generateMoves();
    
    // count positions
    for (int d = 1; d <= 2; d++) {
        std::cout << "Depth: " << d << std::endl;
        cpu->countPositions(state, d);
    }
        
    return 0;    
}