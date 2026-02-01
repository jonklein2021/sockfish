#include "src/ai/Engine.h"
#include "src/core/Notation.h"
#include "src/core/Position.h"
#include "src/core/Printers.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

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
        const std::string sample =
            Notation::moveToCoords(legalMoves[std::rand() % legalMoves.size()]);

        // DEBUG: print legal moves
        Printers::printMoveList(legalMoves, pos);

        // prompt user for input
        std::cout << "Enter move (example: " << sample << ") or q to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            exit(0);
        }

        if (!Notation::validateCoords(input) &&
            !((input == "O-O") || (input == "OO") || (input == "O-O-O") || (input == "OOO"))) {
            std::cout << "Error: Invalid input" << std::endl;
            continue;
        }

        // check if the move is legal before returning it
        // todo: check for castling
        if (Notation::validateCoords(input)) {
            candidate = Notation::coordsToMove(input);
        } else {
            // must be a castling move
            if (input == "O-O" || input == "OO") {
                // Kingside
                candidate = pos.getSideToMove() == WHITE
                                ? MoveGenerator::createCastlingMove<false, WHITE>()
                                : MoveGenerator::createCastlingMove<false, BLACK>();
            } else {
                // Queenside
                candidate = pos.getSideToMove() == WHITE
                                ? MoveGenerator::createCastlingMove<true, WHITE>()
                                : MoveGenerator::createCastlingMove<true, BLACK>();
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
    cout << "Move: " << Notation::moveToUci(m) << "\n";

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

void getEvaluation(Position &pos, Evaluator &evaluator) {
    Eval evaluation = evaluator.run(pos);
    printf("Final evaluation: %d\n", evaluation);
}

void getBestMove(Position &pos, Engine &engine) {
    Move m = engine.getMove(pos);
    printf("Best move: %s\n", Notation::moveToSAN(m, pos).c_str());
}

void getBookMove(Position &pos, PolyglotBook &openingBook) {
    Move m = openingBook.getMove(pos);
    if (m != Move::none()) {
        std::vector<Move> bookMoves = openingBook.getMoves(pos);
        puts("Possible book moves:");
        for (Move m : bookMoves) {
            printf("%s\n", Notation::moveToSAN(m, pos).c_str());
        }
        printf("\nSelected book move: %s\n", Notation::moveToSAN(m, pos).c_str());
    } else {
        puts("No book move");
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

    Position pos(fen);
    Evaluator evaluator;
    Engine engine;
    PolyglotBook openingBook;
    MoveGenerator::generateLegal(legalMoves, pos);

    while (1) {
        Printers::prettyPrintPosition(pos);
        string choice;
        cout << "===============================\n"
                "Enter an operation:\n"
                "q: Quit\n"
                "1: Make a move\n"
                "2: Show legal moves\n"
                "3: Evaluate this position\n"
                "4: Get engine move\n"
                "5: Get book move"
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
            case '3': getEvaluation(pos, evaluator); break;
            case '4': getBestMove(pos, engine); break;
            case '5': getBookMove(pos, openingBook); break;
            default: break;
        }

        MoveGenerator::generateLegal(legalMoves, pos);
    }
}
