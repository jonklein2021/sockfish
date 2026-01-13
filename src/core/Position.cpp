#include "Position.h"

#include "src/bitboard/Zobrist.h"
#include "src/core/types.h"
#include "src/movegen/MoveComputers.h"

#include <ostream>
#include <sstream>

Position::Position(const std::string &fen) {
    parseFen(fen);
    updateAllAttacks();
}

void Position::parseFen(const std::string &fen) {
    const size_t n = fen.size();

    // 1: position data
    size_t i = 0;
    int x = 0, y = 0;
    for (; i < n && fen[i] != ' '; i++) {
        const char c = fen[i];
        if (c == '/') {  // move to next row
            x = 0;
            y++;
        } else if (isdigit(c)) {  // empty square; skip x squares
            x += c - '0';
        } else {  // piece
            const Piece p = fenCharToPiece(c);
            const Square sq = xyToSquare(x, y);

            board.addPiece(p, sq);
            md.hash ^= Zobrist::getPieceSquareHash(p, sq);
            x++;
        }
    }

    board.updateOccupancies();

    if (i >= n) {
        return;
    }

    // 2: side to move
    if (fen[++i] == 'w') {
        sideToMove = WHITE;
    } else {
        sideToMove = BLACK;
        md.hash ^= Zobrist::getSideToMoveHash();
    }

    // 3: castling rights (0b0000qkQK)
    i += 2;
    for (; fen[i] != ' ' && i < n; i++) {
        if (fen[i] == 'K') {
            addCastleRights(md.castleRights, WHITE_OO);
        }
        if (fen[i] == 'Q') {
            addCastleRights(md.castleRights, WHITE_OOO);
        }
        if (fen[i] == 'k') {
            addCastleRights(md.castleRights, BLACK_OO);
        }
        if (fen[i] == 'q') {
            addCastleRights(md.castleRights, BLACK_OOO);
        }
    }
    md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);

    if (i >= n) {
        return;
    }

    // 4: en passant square
    if (fen[++i] != '-') {
        md.enPassantSquare = coordinateStringToSquare(fen.substr(i, i + 2));
        md.hash ^= Zobrist::getEnPassantHash(fileOf(md.enPassantSquare));
    }

    if (i >= n) {
        return;
    }

    // 5: halfmove clock
    i += 2;
    std::string halfmoveClock = "";
    for (; i < n && fen[i] != ' '; i++) {
        halfmoveClock += fen[i];
    }
    md.movesSinceCapture = halfmoveClock.empty() ? 0 : std::stoi(halfmoveClock);

    // 6: fullmove number (not used)
}

Position::Metadata Position::makeMove(const Move &move) {
    // save metadata about this state before making move
    Metadata oldMD = md;

    /*** BOARD CHANGES ***/
    const Square from = move.getFromSquare(), to = move.getToSquare();
    const Piece pieceMoved = board.pieceAt(from);

    // handle traditional captures first
    Piece capturedPiece = board.pieceAt(to);
    if (capturedPiece != NO_PIECE) {
        // note that this wont run for en passant captures
        board.removePiece(capturedPiece, to);
        md.hash ^= Zobrist::getPieceSquareHash(capturedPiece, to);
    }

    // handle en passant
    if (move.isEnPassant()) {
        static constexpr int SOUTH_NORTH[2] = {SOUTH, NORTH};
        const Piece capturedPawn = ptToPiece(PAWN, otherColor(sideToMove));

        // represents the pawn to remove
        Square removeSq = Square(md.enPassantSquare + SOUTH_NORTH[sideToMove]);

        // remove the captured pawn
        board.removePiece(capturedPawn, removeSq);
        md.hash ^= Zobrist::getPieceSquareHash(capturedPawn, removeSq);

        // log capture for metadata capture
        capturedPiece = capturedPawn;
    }

    // handle king movement for castling, rook movement is handled separately
    if (move.isCastles()) {
        const Piece king = ptToPiece(KING, sideToMove);
        Square kingFrom, kingTo;
        CastleRights toRemove;
        if (from == h1) {
            // white kingside castle
            kingFrom = e1, kingTo = g1;
            toRemove = WHITE_CASTLING;
        } else if (from == a1) {
            // white queenside castle
            kingFrom = e1, kingTo = c1;
            toRemove = WHITE_CASTLING;
        } else if (from == h8) {
            // black kingside castle
            kingFrom = e8, kingTo = g8;
            toRemove = BLACK_CASTLING;
        } else if (from == a8) {
            // black kingside castle
            kingFrom = e8, kingTo = c8;
            toRemove = BLACK_CASTLING;
        } else {
            // unreachable so long as move gen is correct; this is included only to avoid a warning
            throw std::runtime_error("Unreachable castle branch in Position::makeMove");
        }
        board.movePiece(king, kingFrom, kingTo);
        removeCastleRights(md.castleRights, toRemove);
        md.hash ^= Zobrist::getPieceSquareHash(king, from) ^ Zobrist::getPieceSquareHash(king, to);
        md.hash ^= Zobrist::getCastleRightsHash(toRemove) ^ md.castleRights;
    }

    // move the piece to its new destination
    board.movePiece(pieceMoved, from, to);
    md.hash ^= Zobrist::getPieceSquareHash(pieceMoved, from);
    md.hash ^= Zobrist::getPieceSquareHash(pieceMoved, to);

    // finally, handle pawn promotion
    if (move.isPromotion()) {
        // removes pawn from the edge and add promoted piece
        const Piece promotedPiece = ptToPiece(move.getPromotedPieceType(), sideToMove);
        board.swapPiece(to, pieceMoved, promotedPiece);
        md.hash ^= Zobrist::getPieceSquareHash(pieceMoved, to);
        md.hash ^= Zobrist::getPieceSquareHash(promotedPiece, to);
    }

    // update occupancies now that board operations are done
    board.updateOccupancies();

    /*** OTHER METADATA CHANGES ***/

    // prevent white castling
    if (pieceMoved == WK && !move.isCastles()) {
        removeCastleRights(md.castleRights, WHITE_CASTLING);
        md.hash ^= Zobrist::getCastleRightsHash(WHITE_CASTLING);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // prevent black castling
    if (pieceMoved == BK && !move.isCastles()) {
        removeCastleRights(md.castleRights, BLACK_CASTLING);
        md.hash ^= Zobrist::getCastleRightsHash(BLACK_CASTLING);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // prevent black queenside castle
    if (to == a8 || (from == a8 && pieceMoved == BR)) {
        removeCastleRights(md.castleRights, BLACK_OOO);
        md.hash ^= Zobrist::getCastleRightsHash(BLACK_OOO);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // prevent black kingside castle
    if (to == h8 || (from == h8 && pieceMoved == BR)) {
        removeCastleRights(md.castleRights, BLACK_OO);
        md.hash ^= Zobrist::getCastleRightsHash(BLACK_OO);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // prevent white queenside castle
    if (to == a1 || (from == a1 && pieceMoved == WR)) {
        removeCastleRights(md.castleRights, WHITE_OOO);
        md.hash ^= Zobrist::getCastleRightsHash(WHITE_OOO);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // prevent white kingside castle
    if (to == h1 || (from == h1 && pieceMoved == WR)) {
        removeCastleRights(md.castleRights, WHITE_OO);
        md.hash ^= Zobrist::getCastleRightsHash(WHITE_OO);
        md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);
    }

    // update en passant square
    int oldEpSqFile = fileOf(md.enPassantSquare);
    if (pieceMoved == WP && from + 2 * NORTH == to) {
        // white pawn moved 2 squares
        md.enPassantSquare = Square(+from + NORTH);
    } else if (pieceMoved == BP && from + 2 * SOUTH == to) {
        // black pawn moved 2 squares
        md.enPassantSquare = Square(+from + SOUTH);
    } else {
        // ensure no stale en passant squares
        md.enPassantSquare = NO_SQ;
    }
    md.hash ^= Zobrist::getEnPassantHash(oldEpSqFile);
    md.hash ^= Zobrist::getEnPassantHash(fileOf(md.enPassantSquare));

    // update capturedPiece; this will be NONE if there was no capture
    md.capturedPiece = capturedPiece;

    // update 50 move rule
    if (capturedPiece != NO_PIECE || pieceMoved == WP || pieceMoved == BP) {
        md.movesSinceCapture = 0;
    } else {
        md.movesSinceCapture++;
    }

    // update attack table
    updateAllAttacks();

    // change turns
    sideToMove = otherColor(sideToMove);
    md.hash ^= Zobrist::getSideToMoveHash();

    return oldMD;
}

void Position::unmakeMove(const Move &move, const Metadata &prevMD) {
    /* BOARD RESTORATION */

    // useful constants
    const Square from = move.getFromSquare(), to = move.getToSquare();
    Piece pieceMoved = board.pieceAt(to);

    // first, undo pawn promotion in the reverse order done in makeMove
    if (move.isPromotion()) {
        // N.B: sideToMove holds the CURRENT player's turn; we need to restore the PREVIOUS player's
        // promotion
        const Piece pawn = ptToPiece(PAWN, otherColor(sideToMove));
        const Piece promotedPiece = pieceMoved;

        // replace promoted piece with pawn
        board.swapPiece(to, promotedPiece, pawn);

        // without this line, pieceMoved will continue to store the promotedPiece
        pieceMoved = pawn;
    }

    // next, move the piece back to its original position
    board.movePiece(pieceMoved, to, from);

    // restore king position if castled
    if (move.isCastles()) {
        if (from == h1) {
            // white kingside castle
            board.movePiece(WK, g1, e1);
        } else if (from == a1) {
            // white queenside castle
            board.movePiece(WK, c1, e1);
        } else if (from == h8) {
            // black kingside castle
            board.movePiece(BK, g8, e8);
        } else if (from == a8) {
            // black kingside castle
            board.movePiece(BK, c8, e8);
        }
    }

    // finally, restore captures, including en passant
    if (md.capturedPiece != NO_PIECE) {
        // flipped from makeMove because sideToMove did not make this move that we are undoing
        static constexpr int NORTH_SOUTH[2] = {NORTH, SOUTH};
        const Square capturedSq =
            move.isEnPassant() ? Square(prevMD.enPassantSquare + NORTH_SOUTH[sideToMove]) : to;
        board.addPiece(md.capturedPiece, capturedSq);
    }

    // update occupancies now that the board is restored
    board.updateOccupancies();

    /* METADATA RESTORATION */

    // restore turn
    sideToMove = otherColor(sideToMove);

    // restore metadata
    // N.B: this takes care of the hash and attackTable
    md = prevMD;
}

void Position::updatePieceAttacks(Piece p) {
    Bitboard pieceBB = board.getPieceBB(p);
    md.attackTable[p] = 0ull;

    while (pieceBB) {
        Square sq = Square(getLsbIndex(pieceBB));
        md.attackTable[p] |= MoveComputers::moveAttackComputers[pieceToPT(p)](*this, sq);
        pieceBB &= pieceBB - 1;
    }
}

void Position::updateSideAttacks(Color c) {
    for (Piece p : COLOR_TO_PIECES[c]) {
        updatePieceAttacks(p);
    }
}

void Position::updateAllAttacks() {
    for (Piece p : ALL_PIECES) {
        updatePieceAttacks(p);
    }
}

// TODO: thoroughly check for correctness
std::string Position::toFenString() const {
    std::ostringstream fen;

    // 1. Board
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;

        for (int file = 0; file < 8; file++) {
            Square sq = xyToSquare(file, rank);
            Piece p = pieceAt(sq);

            if (p == NO_PIECE) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen << emptyCount;
                    emptyCount = 0;
                }
                fen << pieceToFenChar[p];
            }
        }

        if (emptyCount > 0) {
            fen << emptyCount;
        }

        if (rank > 0) {
            fen << '/';
        }
    }

    // 2. Side to move
    fen << ' ' << (sideToMove == WHITE ? 'w' : 'b');

    // 3. Castling rights
    fen << ' ';
    if (md.castleRights == NO_CASTLING) {
        fen << '-';
    } else {
        if (md.castleRights & WHITE_OO) {
            fen << 'K';
        }
        if (md.castleRights & WHITE_OOO) {
            fen << 'Q';
        }
        if (md.castleRights & BLACK_OO) {
            fen << 'k';
        }
        if (md.castleRights & BLACK_OOO) {
            fen << 'q';
        }
    }

    // 4. En passant
    fen << ' ';
    if (md.enPassantSquare == NO_SQ) {
        fen << '-';
    } else {
        fen << squareToCoordinateString(md.enPassantSquare);
    }

    // 5. Halfmove clock
    fen << ' ' << md.movesSinceCapture;

    // 6. Fullmove number (not tracked — default to 1)
    fen << " 1";

    return fen.str();
}
