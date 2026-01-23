#include "src/core/Position.h"
#include "src/movegen/MoveGenerator.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

uint64_t perft(Position &pos, int depth) {
    if (depth == 0) {
        return 1;
    }

    uint64_t nodes = 0;
    std::vector<Move> moves;
    MoveGenerator::generatePseudolegal(moves, pos);

    for (const Move &m : moves) {
        Position::Metadata md = pos.makeMove(m);

        if (!pos.isLegal()) {
            pos.unmakeMove(m, md);
            continue;
        }

        nodes += perft(pos, depth - 1);
        pos.unmakeMove(m, md);
    }

    return nodes;
}

void runPerftPosition(const std::string &fen, const std::vector<std::pair<int, uint64_t>> &tests) {
    Position pos(fen);
    std::cout << fen << '\n';

    for (auto [depth, expected] : tests) {
        printf("perft(d=%d)\n", depth);
        const auto start = std::chrono::high_resolution_clock::now();
        uint64_t result = perft(pos, depth);

        const auto end = std::chrono::high_resolution_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(end - start).count();

        std::cout << "  D" << depth << ": " << result << " (expected " << expected << ")"
                  << (result == expected ? " ✅" : " ❌") << " [" << ms << " ms]\n";

        if (result != expected) {
            std::cout << "FAILED\n";
            break;
        }
    }

    std::cout << '\n';
}

void runPerftLine(const std::string &filename, int line_no) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open perft file\n";
        return;
    }

    std::string line;

    // skip to the targeted test's line
    int lineIndex = 0;
    while (++lineIndex < line_no && std::getline(file, line));

    std::getline(file, line);

    std::stringstream ss(line);
    std::string fen;
    std::getline(ss, fen, ';');  // read FEN

    std::vector<std::pair<int, uint64_t>> tests;

    std::string token;
    while (std::getline(ss, token, ';')) {
        std::stringstream ts(token);
        char D;
        int depth;
        uint64_t nodes;

        ts >> D >> depth >> nodes;
        tests.emplace_back(depth, nodes);
    }

    std::cout << "Test " << line_no << ": ";
    runPerftPosition(fen, tests);
}

void runPerftFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open perft file\n";
        return;
    }

    std::string line;
    int index = 1;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string fen;
        std::getline(ss, fen, ';');  // read FEN

        std::vector<std::pair<int, uint64_t>> tests;

        std::string token;
        while (std::getline(ss, token, ';')) {
            std::stringstream ts(token);
            char D;
            int depth;
            uint64_t nodes;

            ts >> D >> depth >> nodes;
            tests.emplace_back(depth, nodes);
        }

        std::cout << "Test " << index++ << ": \n";
        runPerftPosition(fen, tests);
    }
}

int main(int argc, char **argv) {
    const std::string input("../src/test/perft.in");
    if (argc == 2) {
        runPerftLine(input, std::stoi(argv[1]));
    } else {
        runPerftFile(input);
    }

    return 0;
}
