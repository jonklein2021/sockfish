#include "src/ai/Engine.h"
#include "src/core/Position.h"
#include "src/core/types.h"

#include <iostream>
#include <sstream>
#include <string>

int main() {
    Engine engine;
    Position pos{std::string(STARTING_POSITION_FEN)};

    std::string line;

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);

        std::string cmd;
        ss >> cmd;

        // -------- UCI handshake --------

        if (cmd == "uci") {
            std::cout << "id name Sockfish\n";
            std::cout << "id author Jon Klein\n";
            std::cout << "uciok\n";
        } else if (cmd == "isready") {
            std::cout << "readyok\n";
        }

        // -------- New game --------

        else if (cmd == "ucinewgame") {
            pos.parseFen(std::string(STARTING_POSITION_FEN));
        }

        // -------- Position command --------

        else if (cmd == "position") {
            std::string type;
            ss >> type;

            if (type == "startpos") {
                pos.parseFen(std::string(STARTING_POSITION_FEN));
            } else if (type == "fen") {
                std::string fen, tmp;
                fen = "";

                // FEN is exactly 6 space-separated fields
                for (int i = 0; i < 6; i++) {
                    ss >> tmp;
                    fen += tmp + " ";
                }

                pos.parseFen(fen);
            }

            // Apply moves if present
            std::string movesToken;
            ss >> movesToken;

            if (movesToken == "moves") {
                std::string moveStr;
                while (ss >> moveStr) {
                    Move m = Move::fromUciString(moveStr);
                    pos.makeMove(m);
                }
            }
        }

        // -------- Go command --------

        else if (cmd == "go") {
            int depth = 4;  // default fallback

            std::string token;
            while (ss >> token) {
                if (token == "depth") {
                    ss >> depth;
                }
            }

            Move best = engine.getMove(pos, depth);

            std::cout << "bestmove " << best.toUciString() << "\n";
        }

        // -------- Quit --------

        else if (cmd == "quit") {
            break;
        }
    }

    return 0;
}
