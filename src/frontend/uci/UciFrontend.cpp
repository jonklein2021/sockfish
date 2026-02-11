#include "UciFrontend.h"

#include "src/core/Notation.h"
#include "src/core/types.h"

#include <iostream>
#include <sstream>
#include <thread>

void UciFrontend::searchWorker() {
    while (true) {
        // spin until a "go" command is received in the main thread
        while (searchDepth.load(std::memory_order_acquire) == -1) {
            std::this_thread::yield();
        }

        // write the result move to field
        Move best = engine.getMove(pos, searchDepth);
        std::cout << "bestmove " << Notation::moveToUci(best) << std::endl;

        // write -1 to searchDepth to prevent search from rerunning
        searchDepth.store(-1, std::memory_order_relaxed);
    }
}

void UciFrontend::run() {
    // initialize search thread
    std::thread searchThread(&UciFrontend::searchWorker, this);

    std::string line;
    bool running = true;
    while (running && std::getline(std::cin, line)) {
        std::stringstream ss(line);

        std::string cmd;
        ss >> cmd;

        // -------- UCI handshake --------
        if (cmd == "uci") {
            std::cout << "id name Sockfish\n";
            std::cout << "id author Jon Klein\n";
            std::cout << "uciok\n";
        }

        // -------- Ready command --------
        else if (cmd == "isready") {
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

            // write this depth to searchDepth; this will trigger the search worker to run and print
            // the best move
            searchDepth.store(depth, std::memory_order_relaxed);
        }

        // -------- Stop command --------
        else if (cmd == "stop") {
            // this signals to the Searcher class to terminate ASAP
            stopper.overrideAndAbort();
        }

        // -------- Quit --------
        else if (cmd == "quit") {
            running = false;
        }
    }

    // clean up search thread
    searchThread.join();
}
