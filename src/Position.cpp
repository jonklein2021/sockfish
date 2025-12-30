#include "Position.h"

#include "src/types.h"

Position::Position(const std::string &fen)
    : md({
          a1,                         // enPassantSquare
          NONE,                       // capturedPiece
          0,                          // movesSinceCapture
          CastleRights::NO_CASTLING,  // castleRights
      }) {

    // parse the fen and update game state members
    parseFen(fen);
}

const Board &Position::getBoard() const {
    return board;
}

Position::Metadata Position::getMetadata() const {
    return md;
}

Color Position::getSideToMove() const {
    return sideToMove;
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
            x++;
        }
    }

    board.updateOccupancies();

    if (i >= n) {
        return;
    }

    // 2: whose turn it is
    sideToMove = (fen[++i] == 'w' ? WHITE : BLACK);

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

    if (i >= n) {
        return;
    }

    // 4: en passant square
    if (fen[++i] != '-') {
        const int file = fen[i] - 'a';
        const int rank = '8' - fen[i + 1];
        md.enPassantSquare = Square(rank * 8 + file);
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

    /*** BITBOARD CHANGES ***/

    const Square from = move.fromSquare(), to = move.toSquare();
    const Piece pieceMoved = board.pieceAt(from);

    // "move" the bit of the piece's old location to its new location
    board.movePiece(pieceMoved, from, to);

    // handle king movement for castling, rook
    // has already moved at this point
    if (move.isCastles() && from == h1) {
        // white kingside castle
        board.movePiece(WK, e1, g1);
    } else if (move.isCastles() && from == a1) {
        // white queenside castle
        board.movePiece(WK, e1, c1);
    } else if (move.isCastles() && from == h8) {
        // black kingside castle
        board.movePiece(BK, e8, g8);
    } else if (move.isCastles() && from == a8) {
        // black kingside castle
        board.movePiece(BK, e8, c8);
    }

    // handle traditional captures
    Piece capturedPiece = board.pieceAt(to);
    if (capturedPiece != NONE) {
        board.removePiece(capturedPiece, to);
    }

    // handle en passant
    if (move.isEnPassant()) {
        static constexpr Direction SOUTH_NORTH[2] = {SOUTH, NORTH};
        const Piece capturedPawn = ptToPiece(PAWN, otherColor(sideToMove));

        // represents the pawn to remove
        Square removeSq = Square(md.enPassantSquare + SOUTH_NORTH[sideToMove]);

        // remove the captured pawn
        board.removePiece(capturedPawn, removeSq);

        // log capture for metadata capture
        capturedPiece = capturedPawn;
    }

    // handle pawn promotion
    if (move.isPromotion()) {
        board.removePiece(pieceMoved, to);  // remove pawn from the edge
        board.addPiece(ptToPiece(move.promotedPieceType(), sideToMove), to);  // add promoted piece
    }

    // update occupancies bitboards
    board.updateOccupancies();

    /*** METADATA CHANGES ***/

    // white cannot castle to either side
    if (pieceMoved == WK || move.isCastles()) {
        md.castleRights = NO_CASTLING;
    }

    // black cannot castle to either side
    if (pieceMoved == BK || move.isCastles()) {
        md.castleRights = NO_CASTLING;
    }

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

    // update en passant square
    if (pieceMoved == WP && from + 2 * NORTH == to) {
        // white pawn moved 2 squares
        md.enPassantSquare = Square(from + NORTH);
    } else if (pieceMoved == BP && from + 2 * SOUTH == to) {
        // black pawn moved 2 squares
        md.enPassantSquare = Square(from + SOUTH);
    } else {
        // ensure no stale en passant squares
        md.enPassantSquare = a1;
    }

    // update capturedPiece; this will be NONE if there was no capture
    md.capturedPiece = capturedPiece;

    // update 50 move rule and capturedPiece
    if (capturedPiece != NONE) {
        md.movesSinceCapture = 0;
    } else {
        md.movesSinceCapture++;
    }

    // change turns
    sideToMove = otherColor(sideToMove);

    return oldMD;
}

void Position::unmakeMove(const Move &move, const Metadata &prevMD) {
    /* BITBOARD RESTORATION */

    // useful constants
    const Square from = move.fromSquare(), to = move.toSquare();
    const Piece pieceMoved = board.pieceAt(to);

    // restore king position if castled

    // handle king movement for castling, rook
    // has already moved at this point
    if (move.isCastles() && from == h1) {
        // white kingside castle
        board.movePiece(WK, e1, g1);
    } else if (move.isCastles() && from == a1) {
        // white queenside castle
        board.movePiece(WK, e1, c1);
    } else if (move.isCastles() && from == h8) {
        // black kingside castle
        board.movePiece(BK, e8, g8);
    } else if (move.isCastles() && from == a8) {
        // black kingside castle
        board.movePiece(BK, e8, c8);
    }

    // restore capture
    if (prevMD.capturedPiece != NONE) {
        // flipped from makeMove because sideToMove did not make this move that we are undoing
        static constexpr Direction NORTH_SOUTH[2] = {NORTH, SOUTH};
        const Square capturedSq =
            move.isEnPassant() ? Square(prevMD.enPassantSquare + NORTH_SOUTH[sideToMove]) : to;
        board.addPiece(md.capturedPiece, capturedSq);
    }

    // undo pawn promotion in the reverse order done in makeMove
    if (move.isPromotion()) {
        // N.B: sideToMove holds the CURRENT player's turn; we need to restore the PREVIOUS player's
        // promotion
        const Piece promotedPiece = ptToPiece(move.promotedPieceType(), otherColor(sideToMove));

        board.removePiece(promotedPiece, to);  // remove promoted piece

        // replace promotedPiece with pawn
        board.addPiece(pieceMoved, to);
    }

    // finally, move the piece back to its original position
    board.movePiece(pieceMoved, to, from);

    board.updateOccupancies();

    /* METADATA RESTORATION */

    // restore turn
    sideToMove = otherColor(sideToMove);

    // restore metadata
    md = prevMD;
}
