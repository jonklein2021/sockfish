#include "UciFrontend.h"

#include "src/core/Notation.h"
#include "src/core/types.h"

#include <iostream>
#include <sstream>
#include <thread>

void UciFrontend::searchWorker() {
    std::unique_lock<std::mutex> lock(mtx);

    while (!terminateWorker) {
        // sleep until search is requested
        cv.wait(lock, [this] { return searchDepth != -1 || terminateWorker; });

        // break out of loop if "quit" command received
        if (terminateWorker) {
            break;
        }

        // unlock while searching to allow parsing commands in UCI loop
        lock.unlock();

        Move best = useOwnBook ? engine.getMove(pos, searchDepth)
                               : engine.getSearchedMove(pos, searchDepth);
        std::cout << "bestmove " << Notation::moveToUci(best) << std::endl;

        lock.lock();

        // reset flag
        searchDepth = -1;
    }
}

int UciFrontend::calculateTimeToMove(int remainingTime, int increment) {
    return remainingTime / 15 + increment / 2;
}

void UciFrontend::run() {
    // initialize search thread
    std::thread searchThread(&UciFrontend::searchWorker, this);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);

        std::string cmd;
        ss >> cmd;

        // -------- UCI handshake --------
        if (cmd == "uci") {
            std::cout << "id name Sockfish" << std::endl;
            std::cout << "id author Jon Klein" << std::endl;
            std::cout << "option name OwnBook type check default true" << std::endl;
            std::cout << "uciok" << std::endl;
        }

        // -------- Set Option command --------
        else if (cmd == "setoption") {
            std::string name;
            ss >> name;
            if (name != "name") {
                continue;
            }

            std::string option;
            ss >> option;

            if (option == "OwnBook") {
                std::string value;
                ss >> value;
                if (value != "value") {
                    continue;
                }

                std::string checked;
                ss >> checked;
                if (checked == "false") {
                    useOwnBook = false;
                }
            }
        }

        // -------- Ready command --------
        else if (cmd == "isready") {
            std::cout << "readyok" << std::endl;
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

            // Clear repetition table to avoid overflowing its index
            engine.clearHistory();

            // Apply moves if present
            std::string movesToken;
            ss >> movesToken;

            if (movesToken == "moves") {
                std::string moveStr;
                while (ss >> moveStr) {
                    // add the hash BEFORE the move is made because search checks the hash against
                    // the repetition table before examining any moves
                    engine.addToHashHistory(pos.getHash());

                    // decode UCI move and apply it
                    Move m = Notation::uciToMove(pos, moveStr);
                    pos.makeMove(m);
                }
            }
        }

        // -------- Go command --------
        else if (cmd == "go") {
            int depth = 64;

            // indexed by side to move, units are ms
            std::array<int, 2> time = {-1, -1};
            std::array<int, 2> inc = {0, 0};

            std::string token;
            while (ss >> token) {
                if (token == "depth") {
                    ss >> depth;
                } else if (token == "wtime") {
                    ss >> time[WHITE];
                } else if (token == "btime") {
                    ss >> time[BLACK];
                } else if (token == "winc") {
                    ss >> inc[WHITE];
                } else if (token == "binc") {
                    ss >> inc[BLACK];
                }
            }

            // default fallback; necessary to handle "go depth inf"
            if (depth <= 0) {
                depth = 64;
            }

            Color sideToMove = pos.getSideToMove();
            if (time[sideToMove] != -1) {
                // times provided; set up time management
                int timeToMove = calculateTimeToMove(time[sideToMove], inc[sideToMove]);
                timerStopper.setTimeLimit(timeToMove);
                engine.setSearchStopper(&timerStopper);
                puts("using time management stoppper");
            } else {
                // no time limit, use manual search stopper
                engine.setSearchStopper(&manualStopper);
                puts("using manual stoppper");
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                searchDepth = depth;
            }

            // trigger the search worker to run
            cv.notify_one();
        }

        // -------- Stop command --------
        else if (cmd == "stop") {
            // this signals to the Searcher class to terminate ASAP
            engine.abortSearch();
        }

        // -------- Quit --------
        else if (cmd == "quit") {
            {
                std::lock_guard<std::mutex> lock(mtx);
                terminateWorker = true;

                // allows for quitting during search
                engine.abortSearch();
            }

            // clean up search thread
            cv.notify_one();
            searchThread.join();

            // break out of loop
            break;
        }
    }
}
