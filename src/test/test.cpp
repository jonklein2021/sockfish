#include "../Magic.h"
#include "../MoveGenerator.h"
#include "../Position.h"
#include "../Printers.h"
#include "../Zobrist.h"
#include "src/bit_tools.h"
#include "src/types.h"

#include <iostream>
#include <vector>

using std::string, std::cin, std::cout, std::endl;

std::vector<Move> legalMoves;

Move getMoveFromStdin(Position &pos) {
    Move candidate;
    bool validMove = false;
    bool pawnPromoting = false;

    while (!validMove) {
        // pick a random move to suggest
        const std::string sample = legalMoves[std::rand() % legalMoves.size()].toCoordinateString();

        // DEBUG: print legal moves
        Printers::printMoveList(legalMoves, pos);

        // prompt user for input
        std::cout << "Enter move (example: " << sample << ") or q to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            exit(0);
        }

        if (!validateCoords(input) &&
            !((input == "O-O") || (input == "OO") || (input == "O-O-O") || (input == "OOO"))) {
            std::cout << "Error: Invalid input" << std::endl;
            continue;
        }

        // check if the move is legal before returning it
        // todo: check for castling
        if (validateCoords(input)) {
            candidate = Move::fromCoordinateString(input);
        } else {
            // must be a castling move
            if (input == "O-O" || input == "OO") {
                // Kingside
                candidate = MoveGenerator::createCastlingMove(false, pos.getSideToMove());
            } else {
                // Queenside
                candidate = MoveGenerator::createCastlingMove(true, pos.getSideToMove());
            }
        }

        const Square from = candidate.getFromSquare(), to = candidate.getToSquare();
        const Piece pieceMoved = pos.pieceAt(from);

        // check for pawn promotion
        pawnPromoting = (pieceMoved == WP && to <= h8) || (pieceMoved == BP && to >= a1);

        // check for en passant
        if (pieceToPT(pieceMoved) == PAWN && to == pos.getMetadata().enPassantSquare) {
            candidate.setFlag(Move::EN_PASSANT);
        }

        // temporarily set the promotion piece to a queen
        // so that it can match a legal move
        if (pawnPromoting) {
            candidate.setPromotedPieceType(QUEEN);
        }

        for (Move move : legalMoves) {
            if (candidate == move) {
                validMove = true;
                candidate = move;
                break;
            }
        }

        if (!validMove) {
            std::cout << "Error: Illegal move. Please try again" << std::endl;
        }
    }

    if (pawnPromoting) {
        while (true) {
            std::cout << "Promotion piece (Q, N, R, B): ";
            std::string promotion;
            std::getline(std::cin, promotion);
            if (promotion == "Q" || promotion == "q") {
                candidate.setPromotedPieceType(QUEEN);
                break;
            } else if (promotion == "N" || promotion == "n") {
                candidate.setPromotedPieceType(KNIGHT);
                break;
            } else if (promotion == "R" || promotion == "r") {
                candidate.setPromotedPieceType(ROOK);
                break;
            } else if (promotion == "B" || promotion == "b") {
                candidate.setPromotedPieceType(BISHOP);
                break;
            } else {
                std::cout << "Error: Invalid promotion piece" << std::endl;
            }
        }
    }

    return candidate;
}

void testMakeMove(Position &pos) {
    cout << "Initially:\n";
    Printers::prettyPrintPosition(pos, false, true);
    Printers::printPieceValues(pos);

    // get move
    Move m = getMoveFromStdin(pos);
    cout << "Move: " << m.toString() << "\n";

    // make move
    Position::Metadata md = pos.makeMove(m);
    cout << "After making:\n";
    Printers::prettyPrintPosition(pos, false, true);
    Printers::printPieceValues(pos);

    cout << "Unmake this move? (y/n)\n";
    string choice;
    getline(cin, choice);
    if (choice != "y" && choice != "Y") {
        return;
    }

    // unmake move
    pos.unmakeMove(m, md);
    cout << "After unmaking:\n";
    Printers::prettyPrintPosition(pos, false, true);
    Printers::printPieceValues(pos);
}

void printAttackMap(const Position &pos) {
    puts("Attack map:\n");
    for (Piece p : ALL_PIECES) {
        printf("%s\n", PIECE_NAMES[p].data());
        Bitboard attacks = pos.getMetadata().attackTable[p];
        Printers::printBitboard(attacks);
        puts("");
    }
}

void magic() {
    Bitboard blockers = 0ull;

    setBit(blockers, d4);
    setBit(blockers, b4);
    setBit(blockers, e2);
    setBit(blockers, a1);

    puts("Blockers:");
    Printers::printBitboard(blockers);

    Square src = e4;
    Bitboard attacks = Magic::getRookAttacks(src, blockers);

    printf("Rook on %s:\n", squareToCoordinateString(src).c_str());
    Printers::printBitboard(attacks);
}

int main() {
    cout << "Welcome to the testing suite!\n";
    cout << "Enter a FEN or leave blank for starting position: ";
    string fen;
    getline(cin, fen);
    if (fen.empty()) {
        fen = STARTING_POSITION_FEN;
    }

    Magic::init();
    Zobrist::init();
    Position pos(fen);
    legalMoves = MoveGenerator::generateLegal(pos);

    while (1) {
        Printers::prettyPrintPosition(pos);
        string choice;
        cout << "===============================\n"
                "Enter an operation:\n"
                "q: Quit\n"
                "1: Make a move\n"
                "2: Show legal moves\n"
                "3: Show attack map\n"
                "4: Test magic bitboards"
             << endl;
        getline(cin, choice);

        switch (choice[0]) {
            case 'q': return 0;
            case '1': testMakeMove(pos); break;
            case '2': {
                Printers::prettyPrintPosition(pos);
                Printers::printMoveList(legalMoves, pos);
                break;
            }
            case '3': printAttackMap(pos); break;
            case '4': magic(); break;
            default: break;
        }

        legalMoves = MoveGenerator::generateLegal(pos);
    }
}
