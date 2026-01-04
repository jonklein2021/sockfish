#include "Printers.h"

#include <bitset>
#include <iomanip>
#include <iostream>

void Printers::printBitboard(const Bitboard bitboard) {
    std::ostringstream out;
    for (int rank = 0; rank < 8; rank++) {
        out << (8 - rank) << "  ";
        for (int file = 0; file < 8; file++) {
            out << (getBit(bitboard, xyToSquare(file, rank)) ? "1" : "0") << " ";
        }
        out << "\n";
    }

    out << "\n   a b c d e f g h\n";
    std::cout << out.str() << std::endl;
}

void Printers::prettyPrintPosition(const Position &pos, bool flip, bool debug) {
    std::ostringstream out("\n");

    for (int i = 0; i < 8; i++) {
        int rank = flip ? 7 - i : i;

        // row border
        out << "   +----+----+----+----+----+----+----+----+\n";

        // rank number
        out << " " << (8 - rank) << " ";

        for (int j = 0; j < 8; j++) {
            int file = flip ? 7 - j : j;
            Square sq = xyToSquare(file, rank);

            out << "| ";

            Piece p = pos.pieceAt(sq);
            if (p == NO_PIECE) {
                out << "   ";
            } else {
                out << pieceFilenames[p] << " ";
            }
        }

        out << "|\n";
    }

    // bottom border
    out << "   +----+----+----+----+----+----+----+----+\n";

    // file indices
    out << "     ";
    if (flip) {
        out << "h    g    f    e    d    c    b    a  \n";
    } else {
        out << "a    b    c    d    e    f    g    h  \n";
    }

    // print turn
    out << (pos.getSideToMove() == WHITE ? "\nWhite" : "\nBlack") << " to move\n";

    // print debug data
    if (debug) {
        const Position::Metadata md = pos.getMetadata();
        out << "\nCaptured piece: " << pieceNames[md.capturedPiece];
        out << "\nMoves since capture: " << md.movesSinceCapture;
        out << "\nCastle rights: 0b" << std::bitset<4>(md.castleRights);
        out << "\nEn passant square: "
            << ((md.enPassantSquare == NO_SQ) ? "-" : squareToCoordinateString(md.enPassantSquare));
        out << "\nHash: 0x" << std::hex << std::setw(16) << std::setfill('0') << pos.getHash();
    }

    std::cout << out.str() << std::endl;
}

void Printers::printPieceValues(const Position &pos) {
    std::ostringstream out;
    for (PieceType pt : PIECE_TYPES) {
        const Piece white = ptToPiece(pt, WHITE);
        const Bitboard whiteBB = pos.getPieceBB(white);
        out << pieceFilenames[white] << ":";
        out << "0x" << std::hex << std::setw(16) << std::setfill('0') << whiteBB;
        out << "\t";

        const Piece black = ptToPiece(pt, BLACK);
        const Bitboard blackBB = pos.getPieceBB(black);
        out << pieceFilenames[black] << ":";
        out << "0x" << std::hex << std::setw(16) << std::setfill('0') << blackBB;
        out << "\n";
    }
    std::cout << out.str() << std::endl;
}

void Printers::printMoveList(const std::vector<Move> &moveList, const Position &pos) {
    // srcSq -> {Piece, {dstSq0, ..., dstSqN}}
    std::array<std::pair<Piece, std::vector<Square>>, 64> movesFromSquares;
    bool kCastle = false, qCastle = false;

    // group moves by the piece moved
    for (const Move &m : moveList) {
        const Square from = m.getFromSquare();
        const Square to = m.getToSquare();
        const Piece pieceMoved = pos.getBoard().pieceAt(from);
        if (m.isCastles()) {
            if (from == h1 || from == h8) {
                kCastle = true;
            } else {
                qCastle = true;
            }
        } else {
            movesFromSquares[from].first = pieceMoved;
            movesFromSquares[from].second.push_back(to);
        }
    }

    // build output string
    std::ostringstream ss;
    ss << std::to_string(moveList.size()) + " Moves:\n";
    for (Square sq : ALL_SQUARES) {
        if (movesFromSquares[sq].second.empty()) {
            continue;
        }
        Piece moved = movesFromSquares[sq].first;
        ss << pieceNames[moved] << " on " << squareToCoordinateString(sq) << " -> { ";

        for (Square dst : movesFromSquares[sq].second) {
            ss << squareToCoordinateString(dst) << " ";
        }

        ss << "}\n";
    }

    if (kCastle) {
        ss << "O-O\n";
    }

    if (qCastle) {
        ss << "O-O-O\n";
    }

    std::cout << ss.str();
}
