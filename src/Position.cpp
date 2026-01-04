#include "Position.h"

#include "Zobrist.h"
#include "src/types.h"

#include <sstream>

Position::Position(const std::string &fen) {
    parseFen(fen);
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
            const Piece p = fenToPiece(c);
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
        static constexpr Direction SOUTH_NORTH[2] = {SOUTH, NORTH};
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
        if (from == h1) {
            // white kingside castle
            kingFrom = e1, kingTo = g1;
            removeCastleRights(md.castleRights, WHITE_CASTLING);
        } else if (from == a1) {
            // white queenside castle
            kingFrom = e1, kingTo = c1;
            removeCastleRights(md.castleRights, WHITE_CASTLING);
        } else if (from == h8) {
            // black kingside castle
            kingFrom = e8, kingTo = g8;
            removeCastleRights(md.castleRights, BLACK_CASTLING);
        } else if (from == a8) {
            // black kingside castle
            kingFrom = e8, kingTo = c8;
            removeCastleRights(md.castleRights, BLACK_CASTLING);
        } else {
            // unreachable so long as move gen is correct; this is included only to avoid a warning
            throw std::runtime_error("Unreachable castle branch in Position::makeMove");
        }
        board.movePiece(king, kingFrom, kingTo);
        md.hash ^= Zobrist::getPieceSquareHash(king, from) ^ Zobrist::getPieceSquareHash(king, to);
    }

    // move the piece to its new destination
    board.movePiece(pieceMoved, from, to);
    md.hash ^=
        Zobrist::getPieceSquareHash(pieceMoved, from) ^ Zobrist::getPieceSquareHash(pieceMoved, to);

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

    // prevent black queenside castle
    if (to == a8 || pieceMoved == BR) {
        removeCastleRights(md.castleRights, BLACK_OOO);
    }

    // prevent black kingside castle
    if (to == h8 || pieceMoved == BR) {
        removeCastleRights(md.castleRights, BLACK_OO);
    }

    // prevent white queenside castle
    if (to == a1 || pieceMoved == WR) {
        removeCastleRights(md.castleRights, WHITE_OOO);
    }

    // prevent white kingside castle
    if (to == a8 || pieceMoved == WR) {
        removeCastleRights(md.castleRights, WHITE_OO);
    }

    // update hash with new castle rights
    md.hash ^= Zobrist::getCastleRightsHash(md.castleRights);

    // update en passant square
    if (pieceMoved == WP && from + 2 * NORTH == to) {
        // white pawn moved 2 squares
        md.enPassantSquare = Square(from + NORTH);
    } else if (pieceMoved == BP && from + 2 * SOUTH == to) {
        // black pawn moved 2 squares
        md.enPassantSquare = Square(from + SOUTH);
    } else {
        // ensure no stale en passant squares
        md.enPassantSquare = NO_SQ;
    }
    md.hash ^= Zobrist::getEnPassantHash(fileOf(md.enPassantSquare));

    // update capturedPiece; this will be NONE if there was no capture
    md.capturedPiece = capturedPiece;

    // update 50 move rule
    md.movesSinceCapture = capturedPiece != NO_PIECE ? 0 : md.movesSinceCapture + 1;

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
        static constexpr Direction NORTH_SOUTH[2] = {NORTH, SOUTH};
        const Square capturedSq =
            move.isEnPassant() ? Square(prevMD.enPassantSquare + NORTH_SOUTH[sideToMove]) : to;
        board.addPiece(md.capturedPiece, capturedSq);
    }

    board.updateOccupancies();

    /* METADATA RESTORATION */

    // restore turn
    sideToMove = otherColor(sideToMove);

    // restore metadata
    // N.B: this takes care of md.hash for us
    md = prevMD;
}

// TODO
std::string Position::toFenString() const {
    std::ostringstream out;

    return out.str();
}
