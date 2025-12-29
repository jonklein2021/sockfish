#include "../CliFrontend.h"
#include "../GameController.h"
#include "../Position.h"

#include <iostream>
#include <vector>

using std::string, std::cin, std::cout, std::endl;

void print_bitboard(Bitboard bitboard) {
    // print offset
    cout << "\n";

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++) {
        // loop over board files
        for (int file = 0; file < 8; file++) {
            // convert file & rank into square index
            Square sq = xyToSquare(file, rank * 8);

            // print ranks
            if (!file) {
                cout << "  " << 8 - rank << "  ";
            }

            // print bit state (either 1 or 0)
            cout << " " << (bitboard & (1 << sq) ? 1 : 0);
        }

        // print new line every rank
        cout << "\n";
    }

    // print board files
    cout << "\n     a b c d e f g h\n\n";

    // print bitboard as unsigned decimal number
    cout << "     Bitboard: " << bitboard << "\n\n";
}

void testMakeMove(CliFrontend &cli) {}

void showLegalMoves(const std::vector<Move> &legalMoves) {
    cout << legalMoves.size() << " legal moves" << endl;
    for (const Move &move : legalMoves) {
        cout << move.toString() << endl;
    }
}

int main() {
    cout << "Welcome to the testing suite!\n";
    cout << "Enter a FEN or leave blank for starting position: ";
    string fen;
    getline(cin, fen);
    if (fen.empty()) {
        fen = STARTING_POSITION_FEN;
    }

    std::shared_ptr<Position> pos = std::make_shared<Position>(fen);
    std::shared_ptr<MoveGenerator> moveGen = std::make_shared<MoveGenerator>(pos);
    std::unique_ptr<MoveGenerator> engineMoveGen = std::make_unique<MoveGenerator>(pos);
    std::unique_ptr<Engine> engine = std::make_unique<Engine>(std::move(engineMoveGen), 4);
    GameController game(pos, moveGen, std::move(engine), WHITE);
    CliFrontend cli(game);

    while (1) {
        string choice;
        cout << "===============================\n"
                "Enter an operation:\n"
                "0: Quit\n"
                "1: Make a move\n"
                "2: Show legal moves in this position\n"
                "3: Evaluate this position\n";
        getline(cin, choice);

        const std::vector<Move> legalMoves = game.legalMoves();
        switch (choice[0]) {
            case '0': return 0;
            case '1': testMakeMove(cli); break;
            case '2': showLegalMoves(legalMoves); break;
            case '3':
                // evaluate this position
                break;
            default: break;
        }
    }
}
