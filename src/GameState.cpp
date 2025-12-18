#include "GameState.h"

#include "bit_tools.h"

#include <algorithm>
#include <cstdint>
#include <iostream>

GameState::GameState()
    : GameState(defaultFEN) {}

GameState::GameState(const std::string &fen)
    : md({
          std::vector<uint64_t>(),  // history (not used yet)
          0u,                       // enPassantSquare
          0u,                       // movesSinceCapture
          0u,                       // castleRights
      }) {

    // zero out bitboards
    std::fill(pieceBits, pieceBits + 12, 0);
    std::fill(pieceAttacks, pieceAttacks + 12, 0);
    std::fill(occupancies, occupancies + 4, 0);

    // parse the fen and update game state members
    parseFen(fen);
}

void GameState::parseFen(const std::string &fen) {
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
            const Piece pt = fenPieceMap.at(c);
            const uint64_t pieceBit = coordsToBit(x, y);

            // set this piece's bit in its bitboard
            pieceBits[pt] |= pieceBit;

            x++;
        }
    }

    // update occupancies bitboards
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] |
                         pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] |
                         pieceBits[BQ] | pieceBits[BK];
    occupancies[ALL] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NEITHER] = ~occupancies[ALL];

    // update piece attacks
    for (int p = 0; p < 12; p++) {
        pieceAttacks[p] = computePieceAttacks(static_cast<Piece>(p));
    }

    if (i >= n) {
        return;
    }

    // 2: whose turn it is
    whiteToMove = fen[++i] == 'w';

    // 3: castling rights (castleRights = 0b0000qkQK)
    i += 2;
    for (; fen[i] != ' ' && i < n; i++) {
        if (fen[i] == 'K') {
            md.castleRights |= 0b0001;
        }
        if (fen[i] == 'Q') {
            md.castleRights |= 0b0010;
        }
        if (fen[i] == 'k') {
            md.castleRights |= 0b0100;
        }
        if (fen[i] == 'q') {
            md.castleRights |= 0b1000;
        }
    }

    if (i >= n) {
        return;
    }

    // 4: en passant square
    if (fen[++i] != '-') {
        int file = fen[i] - 'a';
        int rank = '8' - fen[i + 1];
        md.enPassantBitOffset = (rank * 8 + file);
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

uint64_t GameState::computeAllSidesAttacks() const {
    return computeAllSidesAttacks(whiteToMove);
}

uint64_t GameState::computeAllSidesAttacks(bool white) const {
    uint64_t attacks = 0;
    const int startIndex = white ? 0 : 6;
    const int endIndex = white ? 6 : 12;
    for (int p = startIndex; p < endIndex; p++) {
        attacks |= computePieceAttacks(static_cast<Piece>(p));
    }
    return attacks;
}

uint64_t GameState::computePieceAttacks(Piece piece) const {
    uint64_t attacks = 0;
    uint64_t pieceBitboard = pieceBits[piece];
    while (pieceBitboard) {
        const uint64_t pieceBit = pieceBitboard & -pieceBitboard;  // isolate the LSB

        // compute attacks for this piece
        switch (piece) {
            case WP:
            case BP: attacks |= computePawnAttacks(pieceBit, piece == WP); break;
            case WN:
            case BN: attacks |= computeKnightAttacks(pieceBit); break;
            case WB:
            case BB: attacks |= computeBishopAttacks(pieceBit); break;
            case WR:
            case BR: attacks |= computeRookAttacks(pieceBit); break;
            case WQ:
            case BQ: attacks |= computeQueenAttacks(pieceBit); break;
            case WK:
            case BK: attacks |= computeKingAttacks(pieceBit); break;
            default: return 0;
        }

        pieceBitboard ^= pieceBit;  // remove the LSB from the bitboard
    }
    return attacks;
}

uint64_t GameState::computePawnAttacks(const uint64_t squareBit, const bool white) const {
    if (white) {
        return ((squareBit >> 7) & not_file_a) |
               ((squareBit >> 9) & not_file_h);  // up right, up left
    } else {
        return ((squareBit << 7) & not_file_h) |
               ((squareBit << 9) & not_file_a);  // down left, down right
    }
}

uint64_t GameState::computeKnightAttacks(const uint64_t squareBit) const {
    const uint64_t ddl = (squareBit & not_rank_12 & not_file_a) << 15;  // down 2, left 1
    const uint64_t ddr = (squareBit & not_rank_12 & not_file_h) << 17;  // down 2, right 1
    const uint64_t drr = (squareBit & not_rank_1 & not_file_gh) << 10;  // down 1, right 2
    const uint64_t dll = (squareBit & not_rank_1 & not_file_ab) << 6;   // down 1, left 2
    const uint64_t uur = (squareBit & not_rank_78 & not_file_h) >> 15;  // up 2, right 1
    const uint64_t uul = (squareBit & not_rank_78 & not_file_a) >> 17;  // up 2, left 1
    const uint64_t ull = (squareBit & not_rank_8 & not_file_ab) >> 10;  // up 1, left 2
    const uint64_t urr = (squareBit & not_rank_8 & not_file_gh) >> 6;   // up 1, right 2

    return ddl | ddr | drr | dll | uur | uul | ull | urr;
}

uint64_t GameState::computeBishopAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const auto [r0, c0] = bitToCoords(squareBit);

    // up right
    for (int r = r0 + 1, c = c0 + 1; r <= 7 && c <= 7; r++, c++) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // up left
    for (int r = r0 + 1, c = c0 - 1; r <= 7 && c >= 0; r++, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down right
    for (int r = r0 - 1, c = c0 + 1; r >= 0 && c <= 7; r--, c++) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down left
    for (int r = r0 - 1, c = c0 - 1; r >= 0 && c >= 0; r--, c--) {
        const uint64_t attackedSquare = coordsToBit(c, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    return attacks;
}

uint64_t GameState::computeRookAttacks(const uint64_t squareBit) const {
    uint64_t attacks = 0;

    // initial row and column
    // N.B: (0, 0) is top left corner
    const auto [r0, c0] = bitToCoords(squareBit);

    // up
    for (int r = r0 + 1; r <= 7; r++) {
        const uint64_t attackedSquare = coordsToBit(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // down
    for (int r = r0 - 1; r >= 0; r--) {
        const uint64_t attackedSquare = coordsToBit(c0, r);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // left
    for (int c = c0 - 1; c >= 0; c--) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    // right
    for (int c = c0 + 1; c <= 7; c++) {
        const uint64_t attackedSquare = coordsToBit(c, r0);
        attacks |= attackedSquare;
        if (attackedSquare & occupancies[ALL]) {
            break;
        }
    }

    return attacks;
}

uint64_t GameState::computeQueenAttacks(const uint64_t squareBit) const {
    return computeBishopAttacks(squareBit) | computeRookAttacks(squareBit);  // heheh
}

uint64_t GameState::computeKingAttacks(const uint64_t squareBit) const {
    const uint64_t d = (squareBit & not_rank_1) << 8;                // down
    const uint64_t u = (squareBit & not_rank_8) >> 8;                // up
    const uint64_t l = (squareBit & not_file_a) >> 1;                // left
    const uint64_t r = (squareBit & not_file_h) << 1;                // right
    const uint64_t dl = (squareBit & not_rank_1 & not_file_a) << 7;  // down left
    const uint64_t dr = (squareBit & not_rank_1 & not_file_h) << 9;  // down right
    const uint64_t ul = (squareBit & not_rank_8 & not_file_a) >> 9;  // up left
    const uint64_t ur = (squareBit & not_rank_8 & not_file_h) >> 7;  // up right

    return d | u | l | r | dl | dr | ul | ur;
}

Piece GameState::getCapturedPiece(const uint64_t toBit,
                                             const std::vector<Piece> &oppPieces) const {
    for (Piece p : oppPieces) {
        if (pieceBits[p] & toBit) {
            return p;
        }
    }
    return NO_PIECE;
}

Piece GameState::pieceAt(sf::Vector2<int> square) const {
    uint64_t targetBit = coordsToBit(square);
    for (int i = 0; i < 12; i++) {
        if (pieceBits[i] & targetBit) {
            return static_cast<Piece>(i);
        }
    }
    return NO_PIECE;
}

Metadata GameState::makeMove(const Move &move) {
    // save metadata about this state before making move
    Metadata oldMD = md;

    /*** BITBOARD CHANGES ***/

    // useful constants
    const uint64_t from = coordsToBit(move.from);
    const uint64_t to = coordsToBit(move.to);
    const uint64_t fromTo = from | to;

    // "move" the bit of the piece's old location to its new location
    pieceBits[move.piece] ^= fromTo;

    // handle additional rook movement for castling
    if (move.piece == WK && move.isKCastle) {
        // white kingside castle
        pieceBits[WR] ^= (1ull << h1) | (1ull << f1);
    } else if (move.piece == BK && move.isKCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << h8) | (1ull << f8);
    } else if (move.piece == WK && move.isQCastle) {
        // white queenside castle
        pieceBits[WR] ^= (1ull << a1) | (1ull << d1);
    } else if (move.piece == BK && move.isQCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << a8) | (1ull << d8);
    }

    // handle captures
    if (move.capturedPiece != NO_PIECE) {
        if (move.isEnPassant) {
            // the piece to remove is at the attacker pawn's new x and old y
            uint64_t removeBit = coordsToBit(move.to.x, move.from.y);

            // remove the captured pawn
            pieceBits[move.piece == WP ? BP : WP] &= ~removeBit;
        } else {
            // determine what piece to remove
            for (PieceType p : {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK}) {
                if (p != move.piece && (pieceBits[p] & to)) {
                    // zero out the captured piece's bit
                    pieceBits[p] &= ~to;
                    break;
                }
            }
        }
    }

    // handle pawn promotion
    if (move.promotionPiece != NO_PIECE) {
        pieceBits[move.piece] &= ~to;          // remove pawn
        pieceBits[move.promotionPiece] |= to;  // add promoted piece
    }

    // update occupancies bitboards
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] |
                         pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] |
                         pieceBits[BQ] | pieceBits[BK];
    occupancies[ALL] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NEITHER] = ~occupancies[ALL];

    // update attack squares
    // TODO: make this more efficient
    for (int i = 0; i < 12; i++) {
        pieceAttacks[i] = computePieceAttacks(static_cast<Piece>(i));
    }

    /*** METADATA CHANGES ***/

    // covers standard king moves and castling
    if (whiteToMove) {
        if (move.piece == WK) {
            // white cannot castle to either side
            md.castleRights &= ~0b0011;
        }
    } else {
        if (move.piece == BK) {
            // black cannot castle to either side
            md.castleRights &= ~0b1100;
        }
    }

    // prevent black queenside castle
    if ((move.to.x == 0 && move.to.y == 0) || (move.piece == BR && move.from.x == 0)) {
        md.castleRights &= ~0b1000;
    }

    // prevent black kingside castle
    if ((move.to.x == 7 && move.to.y == 0) || (move.piece == BR && move.from.x == 7)) {
        md.castleRights &= ~0b0100;
    }

    // prevent white queenside castle
    if ((move.to.x == 0 && move.to.y == 7) || (move.piece == WR && move.from.x == 0)) {
        md.castleRights &= ~0b0010;
    }

    // prevent white kingside castle
    if ((move.to.x == 7 && move.to.y == 7) || (move.piece == WR && move.from.x == 7)) {
        md.castleRights &= ~0b0001;
    }

    // update en passant square if pawn has moved two squares
    if (move.piece == WP && move.from.y == 6 && move.to.y == 4) {
        md.enPassantBitOffset = (move.from.x + 8 * 5);  // {x, 5}
    } else if (move.piece == BP && move.from.y == 1 && move.to.y == 3) {
        md.enPassantBitOffset = (move.from.x + 8 * 2);  // {x, 2}
    } else {
        // this ensures no stale en passant squares
        md.enPassantBitOffset = 0;
    }

    // update 50 move rule
    if (move.capturedPiece != NO_PIECE) {
        md.movesSinceCapture = 0;
    } else {
        md.movesSinceCapture++;
    }

    // change turns
    whiteToMove = !whiteToMove;

    return oldMD;
}

void GameState::unmakeMove(const Move &move, const Metadata &prevMD) {
    /* BITBOARD RESTORATION */

    // useful constants
    const uint64_t from = coordsToBit(move.from);
    const uint64_t to = coordsToBit(move.to);
    const uint64_t fromTo = from | to;

    // "Move" this piece's bit to its original position
    pieceBits[move.piece] ^= fromTo;

    // restore rook position if castling
    if (move.piece == WK && move.isKCastle) {
        // white kingside castle
        pieceBits[WR] ^= (1ull << h1) | (1ull << f1);
    } else if (move.piece == BK && move.isKCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << h8) | (1ull << f8);
    } else if (move.piece == WK && move.isQCastle) {
        // white queenside castle
        pieceBits[WR] ^= (1ull << a1) | (1ull << d1);
    } else if (move.piece == BK && move.isQCastle) {
        // black kingside castle
        pieceBits[BR] ^= (1ull << a8) | (1ull << d8);
    }

    // restored captures
    if (move.capturedPiece != NO_PIECE) {
        if (move.isEnPassant) {
            uint64_t restoreBit = coordsToBit(move.to.x, move.from.y);
            pieceBits[move.piece == WP ? BP : WP] |= restoreBit;
        } else {
            pieceBits[move.capturedPiece] |= to;  // restore captured piece
        }
    }

    // undo pawn promotion
    if (move.promotionPiece != NO_PIECE) {
        pieceBits[move.piece] |= from;          // restore original pawn
        pieceBits[move.piece] &= ~to;           // compensate for pieceBits[move.piece] ^= fromTo;
        pieceBits[move.promotionPiece] &= ~to;  // remove promoted piece
    }

    // update occupancies
    occupancies[WHITE] = pieceBits[WP] | pieceBits[WN] | pieceBits[WB] | pieceBits[WR] |
                         pieceBits[WQ] | pieceBits[WK];
    occupancies[BLACK] = pieceBits[BP] | pieceBits[BN] | pieceBits[BB] | pieceBits[BR] |
                         pieceBits[BQ] | pieceBits[BK];
    occupancies[ALL] = occupancies[WHITE] | occupancies[BLACK];
    occupancies[NEITHER] = ~occupancies[ALL];

    // restore attack squares
    // TODO: make this more efficient
    for (int i = 0; i < 12; i++) {
        pieceAttacks[i] = computePieceAttacks(static_cast<Piece>(i));
    }

    /* METADATA RESTORATION */

    // restore turn order
    whiteToMove = !whiteToMove;

    // restore metadata
    md = prevMD;
}

/**
 * Returns return if and only if the given square is under attack by the given
 * side
 * @param squareBit The square to check
 * @param white The attacking side. True for white, false for black
 */
bool GameState::underAttack(const uint64_t squareBit, const bool white) const {
    return squareBit & (white ? (pieceAttacks[WP] | pieceAttacks[WN] | pieceAttacks[WB] |
                                 pieceAttacks[WR] | pieceAttacks[WQ] | pieceAttacks[WK])
                              : (pieceAttacks[BP] | pieceAttacks[BN] | pieceAttacks[BB] |
                                 pieceAttacks[BR] | pieceAttacks[BQ] | pieceAttacks[BK]));
}

/**
 * Returns return if and only if the given square is under attack by the
 * opponent
 * @param squareBit The square to check
 */
bool GameState::underAttack(const uint64_t squareBit) const {
    return underAttack(squareBit, !whiteToMove);
}

bool GameState::insufficientMaterial() const {
    // count all pieces
    const int pieceCount[12] = {
        __builtin_popcountll(pieceBits[WP]),
        __builtin_popcountll(pieceBits[WN]),
        __builtin_popcountll(pieceBits[WB]),
        __builtin_popcountll(pieceBits[WR]),
        __builtin_popcountll(pieceBits[WQ]),
        1,  // always 1 white king
        __builtin_popcountll(pieceBits[BP]),
        __builtin_popcountll(pieceBits[BN]),
        __builtin_popcountll(pieceBits[BB]),
        __builtin_popcountll(pieceBits[BR]),
        __builtin_popcountll(pieceBits[BQ]),
        1  // always 1 black king
    };

    // note: consider testing for king + two knights vs king

    // insufficient material if at most one knight or bishop is left per side
    return !(pieceCount[WN] & pieceCount[WB]) & !(pieceCount[BN] & pieceCount[BB]) &
           !(pieceCount[WP] | pieceCount[WR] | pieceCount[WQ] | pieceCount[BP] | pieceCount[BR] |
             pieceCount[BQ]);
}

bool GameState::isTerminal() const {
    const int n = md.history.size();
    return md.movesSinceCapture >= 100 ||
           (n >= 9 && md.history[n - 1] == md.history[n - 5] &&
            md.history[n - 5] == md.history[n - 9]) ||
           insufficientMaterial() || generateMoves().empty();
}

bool GameState::isCheck() const {
    // get position of king
    const uint64_t king = pieceBits[whiteToMove ? WK : BK];

    // check if king is under attack
    return underAttack(king);
}

bool GameState::isMoveLegal(GameState &copy,
                            const uint64_t kingBit,
                            const Move &move,
                            bool white) const {
    // simulate move on copied state
    Metadata md = copy.makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = underAttack(kingBit, !white);

    // unmake move to preserve original state
    copy.unmakeMove(move, md);

    // return true iff the move is not illegal
    return !isOurKingInCheck;
}

std::vector<Move> GameState::generateMoves() const {
    std::vector<Move> moves;

    // stores a copy of the bitboard of a selected piece
    uint64_t pieceBitboard;

    // bit that signifies a piece's initial location
    uint64_t fromBit;

    // bit that signifies a piece's final location
    uint64_t toBit;

    Piece capturedPiece;

    std::vector<PieceType> promotionPieces;
    std::vector<PieceType> oppPieces;
    Piece PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, OPP_PAWN;
    int SIDE;
    int OPP;
    if (whiteToMove) {
        promotionPieces = {WQ, WR, WB, WN};
        oppPieces = {BP, BN, BB, BR, BQ, BK};
        PAWN = WP;
        KNIGHT = WN;
        BISHOP = WB;
        ROOK = WR;
        QUEEN = WQ;
        KING = WK;
        OPP_PAWN = BP;
        SIDE = WHITE;
        OPP = BLACK;
    } else {
        promotionPieces = {BQ, BR, BB, BN};
        oppPieces = {WP, WN, WB, WR, WQ, WK};
        PAWN = BP;
        KNIGHT = BN;
        BISHOP = BB;
        ROOK = BR;
        QUEEN = BQ;
        KING = BK;
        OPP_PAWN = WP;
        SIDE = BLACK;
        OPP = WHITE;
    }

    // used to check if a move is legal
    GameState copy(*this);
    Move candidate;
    const uint64_t kingBit = pieceBits[KING];

    /* PAWN MOVES */
    pieceBitboard = pieceBits[PAWN];
    if (whiteToMove) {
        while (pieceBitboard) {
            fromBit = 1ull << indexOfLs1b(pieceBitboard);
            const sf::Vector2<int> from = bitToCoords(fromBit);

            // single pawn moves
            toBit = fromBit >> 8;
            sf::Vector2<int> to = bitToCoords(toBit);
            candidate = {from, to, PAWN};
            if ((occupancies[NEITHER] & toBit) && isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                // promotion check
                if (toBit & rank8) {
                    for (PieceType pt : promotionPieces) {
                        moves.push_back({from, to, PAWN, None, pt});
                    }
                } else {
                    moves.push_back(candidate);
                }
            }

            // double pawn moves
            uint64_t overBit = fromBit >> 8;
            toBit = fromBit >> 16;
            to = bitToCoords(toBit);
            candidate = {from, to, PAWN};
            if ((rank2 & fromBit) && (occupancies[NEITHER] & overBit) && (occupancies[NEITHER] & toBit) &&
                isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }

            // pawn captures up and right
            toBit = (fromBit & not_file_h) >> 7;
            to = bitToCoords(toBit);
            if (toBit & (occupancies[OPP] | (1 << md.enPassantBitOffset))) {

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank8) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // pawn captures up and left
            toBit = (fromBit & not_file_a) >> 9;
            if (toBit & (occupancies[OPP] | (1 << md.enPassantBitOffset))) {
                to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank8) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            pieceBitboard &= pieceBitboard - 1;
        }
    } else {
        while (pieceBitboard) {
            fromBit = 1ull << indexOfLs1b(pieceBitboard);
            const sf::Vector2<int> from = bitToCoords(fromBit);

            // single pawn moves
            toBit = fromBit << 8;
            if (occupancies[NEITHER] & toBit) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // promotion check
                if (toBit & rank1) {
                    for (PieceType pt : promotionPieces) {
                        candidate = {from, to, PAWN, None, pt};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    candidate = {from, to, PAWN};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // double pawn moves
            uint64_t overBit = fromBit << 8;
            toBit = fromBit << 16;
            if ((rank7 & fromBit) && (occupancies[NEITHER] & overBit) && (occupancies[NEITHER] & toBit)) {
                sf::Vector2<int> to = bitToCoords(toBit);
                candidate = {from, to, PAWN};
                if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                    moves.push_back(candidate);
                }
            }

            // pawn captures down and right
            toBit = (fromBit & not_file_h) << 9;
            if (toBit & (occupancies[OPP] | (1 << md.enPassantBitOffset))) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank1) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            // pawn captures down and left
            toBit = (fromBit & not_file_a) << 7;
            if (toBit & (occupancies[OPP] | (1 << md.enPassantBitOffset))) {
                sf::Vector2<int> to = bitToCoords(toBit);

                // determine what piece was captured
                Piece capturedPiece = getCapturedPiece(toBit, oppPieces);

                if (capturedPiece != NO_PIECE) {
                    // promotion check
                    if (toBit & rank1) {
                        for (PieceType pt : promotionPieces) {
                            candidate = {from, to, PAWN, capturedPiece, pt};
                            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                                moves.push_back(candidate);
                            }
                        }
                    } else {
                        candidate = {from, to, PAWN, capturedPiece};
                        if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                            moves.push_back(candidate);
                        }
                    }
                } else {
                    // must be en passant
                    candidate = {from, to, PAWN, OPP_PAWN, NO_PIECE, false, false, true};
                    if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                        moves.push_back(candidate);
                    }
                }
            }

            pieceBitboard &= pieceBitboard - 1;
        }
    }

    /* KNIGHT MOVES */
    pieceBitboard = pieceBits[KNIGHT];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        // down 2, left 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_a) << 15)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 2, right 1
        if ((toBit = ((fromBit & not_rank_12 & not_file_h) << 17)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 1, right 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_gh) << 10)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // down 1, left 2
        if ((toBit = ((fromBit & not_rank_1 & not_file_ab) << 6)) && (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 2, right 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_h) >> 15)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 2, left 1
        if ((toBit = ((fromBit & not_rank_78 & not_file_a) >> 17)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 1, left 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_ab) >> 10)) &&
            (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        // up 1, right 2
        if ((toBit = ((fromBit & not_rank_8 & not_file_gh) >> 6)) && (toBit & ~occupancies[SIDE])) {
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), KNIGHT, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* BISHOP MOVES */
    pieceBitboard = pieceBits[BISHOP];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = occupancies[ALL] & ~fromBit;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~otherPieces)) {
            toBit = filteredMask >> 7;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 9;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 9;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~otherPieces)) {
            toBit = filteredMask << 7;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), BISHOP, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* ROOK MOVES */
    pieceBitboard = pieceBits[ROOK];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = occupancies[ALL] & ~fromBit;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~otherPieces)) {
            toBit = filteredMask >> 8;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down
        while (filteredMask = (toBit & not_rank_1 & ~otherPieces)) {
            toBit = filteredMask << 8;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // left
        while (filteredMask = (toBit & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 1;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // right
        while (filteredMask = (toBit & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 1;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), ROOK, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* QUEEN MOVES */
    pieceBitboard = pieceBits[QUEEN];
    while (pieceBitboard) {
        fromBit = 1ull << indexOfLs1b(pieceBitboard);
        const sf::Vector2<int> from = bitToCoords(fromBit);

        toBit = fromBit;
        uint64_t filteredMask;
        const uint64_t otherPieces = occupancies[ALL] & ~fromBit;

        // up
        while (filteredMask = (toBit & not_rank_8 & ~otherPieces)) {
            toBit = filteredMask >> 8;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down
        while (filteredMask = (toBit & not_rank_1 & ~otherPieces)) {
            toBit = filteredMask << 8;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // left
        while (filteredMask = (toBit & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 1;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // right
        while (filteredMask = (toBit & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 1;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up right
        while (filteredMask = (toBit & not_rank_8 & not_file_h & ~otherPieces)) {
            toBit = filteredMask >> 7;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // up left
        while (filteredMask = (toBit & not_rank_8 & not_file_a & ~otherPieces)) {
            toBit = filteredMask >> 9;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down right
        while (filteredMask = (toBit & not_rank_1 & not_file_h & ~otherPieces)) {
            toBit = filteredMask << 9;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        toBit = fromBit;

        // down left
        while (filteredMask = (toBit & not_rank_1 & not_file_a & ~otherPieces)) {
            toBit = filteredMask << 7;
            if (toBit & occupancies[SIDE]) {
                break;
            }
            capturedPiece = getCapturedPiece(toBit, oppPieces);
            candidate = {from, bitToCoords(toBit), QUEEN, capturedPiece};
            if (isMoveLegal(copy, kingBit, candidate, whiteToMove)) {
                moves.push_back(candidate);
            }
        }

        pieceBitboard &= pieceBitboard - 1;
    }

    /* KING MOVES */
    fromBit = pieceBits[KING];
    const sf::Vector2<int> from = bitToCoords(fromBit);

    // standard moves
    if ((toBit = ((fromBit & not_rank_1) << 8)) && (toBit & ~occupancies[SIDE])) {
        // down
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8) >> 8)) && (toBit & ~occupancies[SIDE])) {
        // up
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_file_a) >> 1)) && (toBit & ~occupancies[SIDE])) {
        // left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_file_h) << 1)) && (toBit & ~occupancies[SIDE])) {
        // right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_1 & not_file_a) << 7)) && (toBit & ~occupancies[SIDE])) {
        // down left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_1 & not_file_h) << 9)) && (toBit & ~occupancies[SIDE])) {
        // down right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8 & not_file_a) >> 9)) && (toBit & ~occupancies[SIDE])) {
        // up left
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    if ((toBit = ((fromBit & not_rank_8 & not_file_h) >> 7)) && (toBit & ~occupancies[SIDE])) {
        // up right
        capturedPiece = getCapturedPiece(toBit, oppPieces);
        candidate = {from, bitToCoords(toBit), KING, capturedPiece};
        if (isMoveLegal(copy, toBit, candidate, whiteToMove)) {
            moves.push_back(candidate);
        }
    }

    // castling

    // white kingside castle
    if (whiteToMove && (md.castleRights & 0b0001) && (occupancies[NEITHER] & (1ull << f1)) &&
        (occupancies[NEITHER] & (1ull << g1)) && !underAttack(fromBit) &&  // e1
        !underAttack(1ull << f1) && !underAttack(1ull << g1)) {
        moves.push_back({from, {6, 7}, KING, NO_PIECE, NO_PIECE, true, false});
    }

    // white queenside castle
    if (whiteToMove && (md.castleRights & 0b0010) && (occupancies[NEITHER] & (1ull << d1)) &&
        (occupancies[NEITHER] & (1ull << c1)) && (occupancies[NEITHER] & (1ull << b1)) &&
        !underAttack(fromBit) &&  // e1
        !underAttack(1ull << d1) && !underAttack(1ull << c1)) {
        moves.push_back({from, {2, 7}, KING, NO_PIECE, NO_PIECE, false, true});
    }

    // black kingside castle
    if (!whiteToMove && (md.castleRights & 0b0100) && (occupancies[NEITHER] & (1ull << f8)) &&
        (occupancies[NEITHER] & (1ull << g8)) && !underAttack(fromBit) &&  // e8
        !underAttack(1ull << f8) && !underAttack(1ull << g8)) {
        moves.push_back({from, {6, 0}, KING, NO_PIECE, NO_PIECE, true, false});
    }

    // black queenside castle
    if (!whiteToMove && (md.castleRights & 0b1000) && (occupancies[NEITHER] & (1ull << d8)) &&
        (occupancies[NEITHER] & (1ull << c8)) && (occupancies[NEITHER] & (1ull << b8)) &&
        !underAttack(fromBit) &&  // e8
        !underAttack(1ull << d8) && !underAttack(1ull << c8)) {
        moves.push_back({from, {2, 0}, KING, NO_PIECE, NO_PIECE, false, true});
    }

    return moves;
}

uint64_t GameState::hash() const {
    return pieceBits[0] ^ pieceBits[1] ^ pieceBits[2] ^ pieceBits[3] ^ pieceBits[4] ^ pieceBits[5] ^
           pieceBits[6] ^ pieceBits[7] ^ pieceBits[8] ^ pieceBits[9] ^ pieceBits[10] ^
           pieceBits[11] ^ (whiteToMove << 1) ^ (md.castleRights << 2) ^
           (md.enPassantBitOffset << 3);
    // N.B: we exclude movesSinceCapture so that transpositions hash to the same
    // value
}

void GameState::print() const {
    prettyPrintPosition(pieceBits, true);
    std::cout << "whiteToMove = " << whiteToMove << std::endl;
    std::cout << "castleRights = "
              << (md.castleRights == 0 ? "-"
                                       : (std::string((md.castleRights & 0b0001) ? "K" : "") +
                                          std::string((md.castleRights & 0b0010) ? "Q" : "") +
                                          std::string((md.castleRights & 0b0100) ? "k" : "") +
                                          std::string((md.castleRights & 0b1000) ? "q" : "")))
              << std::endl;
    std::cout << "enPassantSquare = "
              << (md.enPassantBitOffset == 0
                      ? "-"
                      : std::to_string(offsetToCoords(md.enPassantBitOffset).x) + ", " +
                            std::to_string(offsetToCoords(md.enPassantBitOffset).y))
              << std::endl;
    std::cout << "movesSinceCapture = " << md.movesSinceCapture << std::endl;
}
