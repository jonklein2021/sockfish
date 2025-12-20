#include "GameState.h"
#include "bit_tools.h"
#include "constants.h"

#include <iostream>

using std::string, std::cin, std::cout, std::endl;

void testMakeMove(Position &gs, const std::vector<Move> &legalMoves) {
    // make a move
    Move candidate;
    bool pawnPromoting;

    // prompt user for input and validate it
    cout << "Please enter a move (ex: e2 e4): ";
    std::string input;
    std::getline(std::cin, input);

    if (!validateCoords(input)) {
        cout << "Error: Invalid input" << endl;
        return;
    }

    candidate = coordsToMove(input);

    // check for pawn promotion
    pawnPromoting = (candidate.to.y == 0 && gs.pieceAt(candidate.from) == WP) ||
                    (candidate.to.y == 7 && gs.pieceAt(candidate.from) == BP);

    // temporarily set the promotion piece to a queen
    // so that it can match a legal move
    if (pawnPromoting) {
        candidate.promotionPiece = gs.whiteToMove ? WQ : BQ;
    }

    // match user move to a legal one
    bool validMove = false;
    for (const Move &move : legalMoves) {
        if (candidate.equals(move)) {
            validMove = true;
            candidate = move;
            break;
        }
    }

    if (!validMove) {
        cout << "Error: " + candidate.toString() + " is illegal" << endl;
        return;
    }

    if (pawnPromoting) {
        while (true) {
            cout << "Promotion piece (Q, N, R, B): ";
            std::string promotion;
            std::getline(std::cin, promotion);
            if (promotion == "Q" || promotion == "q") {
                candidate.promotionPiece = gs.whiteToMove ? WQ : BQ;
                break;
            } else if (promotion == "N" || promotion == "n") {
                candidate.promotionPiece = gs.whiteToMove ? WN : BN;
                break;
            } else if (promotion == "R" || promotion == "r") {
                candidate.promotionPiece = gs.whiteToMove ? WR : BR;
                break;
            } else if (promotion == "B" || promotion == "b") {
                candidate.promotionPiece = gs.whiteToMove ? WB : BB;
                break;
            } else {
                cout << "Error: Invalid promotion piece" << endl;
            }
        }
    }

    // make move and print
    Metadata oldMetadata = gs.makeMove(candidate);
    gs.print();

    // give the option to undo the move
    cout << "Undo move? (y/n): ";
    std::string undo;
    getline(std::cin, undo);
    if (undo == "y" || undo == "Y") {
        gs.unmakeMove(candidate, oldMetadata);
        gs.print();
    }
}

void showLegalMoves(const std::vector<Move> &legalMoves) {
    cout << legalMoves.size() << " legal moves" << endl;
    for (const Move &move : legalMoves) {
        cout << move.toString() << endl;
    }
}

void showAttackedSquares(const Position &gs) {
    cout << "Which side do you want to see the attacked squares? (w/b)\n";
    std::string side;
    getline(std::cin, side);
    bool showWhiteAttacks = side == "w" || side == "W" || side.empty();

    uint64_t attacks = gs.computeAllSidesAttacks(showWhiteAttacks);
    printBitboard(attacks);
}

int main() {
    cout << "Welcome to the testing suite!\n";
    cout << "Enter a FEN or leave blank for starting position: ";
    string fen;
    getline(cin, fen);
    if (fen.empty())
        fen = defaultFEN;
    Position gs(fen);
    gs.print();

    while (1) {
        string choice;
        cout << "===============================\n"
             << "Enter an operation:\n"
             << "0: Quit\n"
             << "1: Make a move\n"
             << "2: Show legal moves in this position\n"
             << "3: Show all attacking squares\n"
             << "4: Evaluate this position\n";
        getline(cin, choice);

        std::vector<Move> legalMoves = gs.generateMoves();
        switch (choice[0]) {
        case '0':
            return 0;
        case '1':
            testMakeMove(gs, legalMoves);
            break;
        case '2':
            showLegalMoves(legalMoves);
            break;
        case '3':
            showAttackedSquares(gs);
            break;
        case '4':
            // evaluate this position
            break;
        default:
            break;
        }
    }
}
