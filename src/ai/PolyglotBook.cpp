#include "PolyglotBook.h"

#include "src/bitboard/Magic.h"
#include "src/bitboard/bit_tools.h"

#include <iostream>
#include <vector>

PolyglotBook::PolyglotBook(const std::string &path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open book: " << path << "\n";
        return;
    }

    Entry raw;

    while (file.read(reinterpret_cast<char *>(&raw), sizeof(raw))) {
        Entry e {};

        // swap bytes on little endian machines
        e.key = swap64(raw.key);
        e.move = swap16(raw.move);
        e.weight = swap16(raw.weight);
        e.learn = swap32(raw.learn);

        book.push_back(e);
    }

    std::cout << "Loaded " << book.size() << " polyglot entries\n";
}

Move PolyglotBook::decodePgMove(const Position &pos, uint16_t pgMove) {
    const Square to = Square(pgMove & 0x3F);
    const Square from = Square((pgMove >> 6) & 0x3F);

    // 0 = none, 1=n,2=b,3=r,4=q
    // N.B: only difference with our PieceType enum is that 0=p
    const PieceType promo = PieceType((pgMove >> 12) & 0x7);

    // check for castling
    if (pieceToPT(pos.pieceAt(from)) == KING) {
        if (from == e1 && to == h1) {
            return Move::create<Move::NORMAL>(from, g1);
        } else if (from == e1 && to == a1) {
            return Move::create<Move::NORMAL>(from, c1);
        } else if (from == e8 && to == h8) {
            return Move::create<Move::NORMAL>(from, g8);
        } else if (from == e8 && to == a8) {
            return Move::create<Move::NORMAL>(from, c8);
        }
    }

    if (promo == 0) {
        return Move::create<Move::NORMAL>(from, to);
    }

    return Move::create<Move::PROMOTION>(from, to, promo);
}

uint64_t PolyglotBook::getHash(const Position &pos) {
    uint64_t hash = 0;

    // ---- Pieces ----
    for (Square sq : ALL_SQUARES) {
        Piece p = pos.pieceAt(sq);
        if (p != NO_PIECE) {
            hash ^= POLYGLOT_RANDOM[64 * pgPieceIdx(p) + 8 * pgRankOf(sq) * pgFileOf(sq)];
        }
    }

    // ---- Castling ----
    CastleRights cr = pos.getCastleRights();

    if (hasCastleRights(cr, WHITE_OO)) {
        hash ^= POLYGLOT_RANDOM[768];
    }
    if (hasCastleRights(cr, WHITE_OOO)) {
        hash ^= POLYGLOT_RANDOM[769];
    }
    if (hasCastleRights(cr, BLACK_OO)) {
        hash ^= POLYGLOT_RANDOM[770];
    }
    if (hasCastleRights(cr, BLACK_OOO)) {
        hash ^= POLYGLOT_RANDOM[771];
    }

    // ---- En Passant ----
    Square ep = pos.getEpSquare();

    if (ep != NO_SQ) {
        int file = pgFileOf(ep);
        Color stm = pos.getSideToMove();

        // Only include EP if a pawn can capture it
        if (stm == WHITE) {
            if (pos.getPieceBB(WP) & PAWN_ATTACK_MASKS[BLACK][ep]) {
                hash ^= POLYGLOT_RANDOM[772 + file];
            }
        } else {
            if (pos.getPieceBB(BP) & PAWN_ATTACK_MASKS[WHITE][ep]) {
                hash ^= POLYGLOT_RANDOM[772 + file];
            }
        }
    }

    // ---- Side to move ----
    if (pos.getSideToMove() == BLACK) {
        hash ^= POLYGLOT_RANDOM[780];
    }

    return hash;
}

Move PolyglotBook::getMove(const Position &pos) {
    uint64_t key = getHash(pos);

    std::vector<uint16_t> viablePgMoves;

    for (Entry &e : book) {
        if (e.key == key) {
            viablePgMoves.push_back(e.move);
        }
    }

    // TODO: make use of the weights
    if (!viablePgMoves.empty()) {
        int randomIndex = rng.next() % viablePgMoves.size();
        return decodePgMove(pos, viablePgMoves[randomIndex]);
    }

    return Move::none();
}
