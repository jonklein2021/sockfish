#include "UciFrontend.h"

#include "src/core/Notation.h"
#include "src/core/types.h"

#include <iostream>
#include <sstream>
#include <thread>

void UciFrontend::start() {
    engine.setStopFlagPtr(&stopFlag);
    inputThread = std::thread([this] { run(); });

    if (inputThread.joinable()) {
        inputThread.join();
    }
}

void UciFrontend::stop() {
    running.store(false, std::memory_order_relaxed);
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

bool UciFrontend::stopRequested() const {
    return stopFlag.load(std::memory_order_relaxed);
}

void UciFrontend::run() {
    std::string line;
    while (running.load(std::memory_order_relaxed) && std::getline(std::cin, line)) {
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
                    Move m = Notation::uciToMove(pos, moveStr);
                    pos.makeMove(m);
                }
            }
        }

        // -------- Go command --------
        else if (cmd == "go") {
            int depth = 64;

            std::string token;
            while (ss >> token) {
                if (token == "depth") {
                    ss >> depth;
                }
            }

            if (depth <= 0) {
                depth = 64;  // default fallback
            }

            Move best = engine.getMove(pos, depth);
            std::cout << "bestmove " << Notation::moveToUci(best) << "\n";
        }

        // -------- Stop command --------
        else if (cmd == "stop") {
            stopFlag.store(true, std::memory_order_relaxed);
        }

        // -------- Quit --------
        else if (cmd == "quit") {
            stopFlag.store(true, std::memory_order_relaxed);
            running.store(false, std::memory_order_relaxed);
        }
    }
}
