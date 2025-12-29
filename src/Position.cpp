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
            const Square sq = Square(8 * y + x);

            board.addPiece(p, sq);

            x++;
        }
    }

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

// TODO: Fix the misusage of board.getPieces()
Position::Metadata Position::makeMove(const Move &move) {
    // save metadata about this state before making move
    Metadata oldMD = md;

    /*** BITBOARD CHANGES ***/

    const Square from = move.fromSquare(), to = move.toSquare();
    const Piece pieceMoved = board.pieceAt(from);
    Bitboard &pieceMovedBB = board.getPieces(pieceMoved);

    // useful constants
    const Bitboard fromBB = 1ull << from;
    const Bitboard toBB = 1ull << to;
    const Bitboard fromTo = fromBB | toBB;

    // "move" the bit of the piece's old location to its new location
    pieceMovedBB ^= fromTo;

    // handle king movement for castling, rook
    // has already moved at this point
    if (move.isCastles() && from == h1) {
        // white kingside castle
        board.getPieces(WK) ^= (1ull << e1) | (1ull << g1);
    } else if (move.isCastles() && from == a1) {
        // white queenside castle
        board.getPieces(WK) ^= (1ull << e1) | (1ull << c1);
    } else if (move.isCastles() && from == h8) {
        // black kingside castle
        board.getPieces(BK) ^= (1ull << e8) | (1ull << g8);
    } else if (move.isCastles() && from == a8) {
        // black kingside castle
        board.getPieces(BK) ^= (1ull << e8) | (1ull << c8);
    }

    // handle en passant
    if (move.isEnPassant()) {
        // represents the pawn to remove
        const Bitboard removeBit = sideToMove == WHITE ? (1ull << (md.enPassantSquare + SOUTH))
                                                       : (1ull << (md.enPassantSquare + NORTH));
        // remove the captured pawn
        board.getPieces(sideToMove == WHITE ? BP : WP) &= ~removeBit;
    }

    // handle traditional captures
    Piece capturedPiece = NONE;
    Bitboard removedPieceBB;
    const auto opponentPieces = sideToMove == WHITE ? BLACK_PIECES : WHITE_PIECES;
    for (Piece p : opponentPieces) {
        if ((removedPieceBB = board.getPieces(p)) & toBB) {
            // zero out the captured piece's bit
            removedPieceBB &= ~to;
            capturedPiece = p;
            break;
        }
    }

    // handle pawn promotion
    if (move.isPromotion()) {
        pieceMovedBB &= ~to;                              // remove pawn
        board.getPieces(move.promotedPieceType()) |= to;  // add promoted piece
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

    // update 50 move rule and capturedPiece
    if (capturedPiece != NONE) {
        md.movesSinceCapture = 0;
        md.capturedPiece = capturedPiece;
    } else {
        md.movesSinceCapture++;
    }

    // change turns
    sideToMove = Color((sideToMove + 1) % 2);

    return oldMD;
}

void Position::unmakeMove(const Move &move, const Metadata &prevMD) {
    /* BITBOARD RESTORATION */

    const Square from = move.fromSquare(), to = move.toSquare();
    const Piece pieceMoved = board.pieceAt(move.fromSquare());
    Bitboard &pieceMovedBB = board.getPieces(pieceMoved);

    // useful constants
    const Bitboard fromBB = 1ull << move.fromSquare();
    const Bitboard toBB = 1ull << move.toSquare();
    const Bitboard fromTo = fromBB | toBB;

    // "Move" this piece's bit to its original position
    pieceMovedBB ^= fromTo;

    // restore king position if castled
    if (move.isCastles() && from == h1) {
        // white kingside castle
        board.getPieces(WK) ^= (1ull << e1) | (1ull << g1);
    } else if (move.isCastles() && from == a1) {
        // white queenside castle
        board.getPieces(WK) ^= (1ull << e1) | (1ull << c1);
    } else if (move.isCastles() && from == h8) {
        // black kingside castle
        board.getPieces(BK) ^= (1ull << e8) | (1ull << g8);
    } else if (move.isCastles() && from == a8) {
        // black kingside castle
        board.getPieces(BK) ^= (1ull << e8) | (1ull << c8);
    }

    // restore capture
    if (md.capturedPiece != NONE) {
        const Bitboard capturedBit =
            move.isEnPassant() ? (1ull << xyToSquare(to % 8, from / 8)) : toBB;
        board.getPieces(md.capturedPiece) |= capturedBit;
    }

    // undo pawn promotion
    if (move.isPromotion()) {
        pieceMovedBB |= fromBB;  // restore original pawn
        pieceMovedBB &= ~toBB;   // compensate for pieces[move.piece] ^= fromTo;

        // N.B: sideToMove holds the CURRENT player's turn; we need to restore the PREVIOUS player's
        // promotion
        const Piece promotedPiece = sideToMove == WHITE ? Piece(move.promotedPieceType() + 6)
                                                        : Piece(move.promotedPieceType());

        board.getPieces(promotedPiece) &= ~to;  // remove promoted piece
    }

    board.updateOccupancies();

    /* METADATA RESTORATION */

    // restore turn
    sideToMove = Color((sideToMove + 1) % 2);

    // restore metadata
    md = prevMD;
}
