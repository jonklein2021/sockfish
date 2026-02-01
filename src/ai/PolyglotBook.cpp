#include "PolyglotBook.h"

#include "src/bitboard/Magic.h"
#include "src/bitboard/bit_tools.h"
#include "src/core/types.h"

#include <cstdint>
#include <iostream>
#include <vector>

struct PgEntry;

PolyglotBook::PolyglotBook() {
    std::ifstream file(FILE_PATH, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open book: " << FILE_PATH << "\n";
        return;
    }

    PgEntry raw;

    while (file.read(reinterpret_cast<char *>(&raw), sizeof(raw))) {
        PgEntry e {};

        // swap bytes on little endian machines
        e.key = swap64(raw.key);
        e.move = swap16(raw.move);
        e.weight = swap16(raw.weight);
        e.learn = swap32(raw.learn);

        bookEntries.push_back(e);
    }

    std::cout << "Loaded " << bookEntries.size() << " polyglot entries\n";
}

Move PolyglotBook::decodePgMove(const Position &pos, uint16_t pgMove) {
    // need to flip rank because PG orders ranks in reverse of sockfish
    const Square to = flipRank(Square(pgMove & 0x3F));
    const Square from = flipRank(Square((pgMove >> 6) & 0x3F));
    const Piece p = pos.pieceAt(from);

    // 0 = none, 1=n,2=b,3=r,4=q
    // N.B: only difference with our PieceType enum is that 0=p
    const PieceType promo = PieceType((pgMove >> 12) & 0x7);

    // check for castling
    if (p != NO_PIECE && pieceToPT(p) == KING) {
        if (from == e1 && to == h1) {
            return Move::create<Move::CASTLING>(from, g1);
        } else if (from == e1 && to == a1) {
            return Move::create<Move::CASTLING>(from, c1);
        } else if (from == e8 && to == h8) {
            return Move::create<Move::CASTLING>(from, g8);
        } else if (from == e8 && to == a8) {
            return Move::create<Move::CASTLING>(from, c8);
        }
    }

    if (promo == 0) {
        return Move::create<Move::NORMAL>(from, to);
    }

    return Move::create<Move::PROMOTION>(from, to, promo);
}

uint64_t PolyglotBook::getPgHash(const Position &pos) {
    uint64_t hash = 0;

    // ---- Pieces ----
    for (Square sq : ALL_SQUARES) {
        Piece p = pos.pieceAt(sq);
        if (p != NO_PIECE) {
            int offsetPiece = 64 * pgKindOfPiece(p) + 8 * pgRowOf(sq) + pgFileOf(sq);
            hash ^= POLYGLOT_RANDOM[offsetPiece];
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
    Color side = pos.getSideToMove();

    if (ep != NO_SQ) {
        int file = pgFileOf(ep);

        // Only include EP if a pawn can capture it
        Bitboard stmPawnBB = pos.getPieceBB(ptToPiece(PAWN, side));
        Bitboard reverseEpPawnAttacks = PAWN_ATTACK_MASKS[otherColor(side)][ep];
        if (stmPawnBB & reverseEpPawnAttacks) {
            hash ^= POLYGLOT_RANDOM[772 + file];
        }
    }

    // ---- Side to move ----
    if (side == WHITE) {
        hash ^= POLYGLOT_RANDOM[780];
    }

    return hash;
}

std::vector<PolyglotBook::PgEntry> PolyglotBook::getPgEntries(const Position &pos) {
    uint64_t key = getPgHash(pos);

    std::vector<PgEntry> bookMoves;

    // use binary search?
    for (const PgEntry &e : bookEntries) {
        if (e.key == key) {
            bookMoves.push_back(e);
        }
    }

    return bookMoves;
}

std::vector<Move> PolyglotBook::getMoves(const Position &pos) {
    std::vector<PgEntry> pgEntries = getPgEntries(pos);
    std::vector<Move> bookMoves;
    bookMoves.reserve(pgEntries.size());

    // map decoded moves to result
    for (const PgEntry &e : pgEntries) {
        bookMoves.emplace_back(decodePgMove(pos, e.move));
    }

    return bookMoves;
}

Move PolyglotBook::getMove(const Position &pos) {
    std::vector<PgEntry> pgEntries = getPgEntries(pos);
    if (pgEntries.empty()) {
        return Move::none();
    }

    PgEntry result = pgEntries[0];

    // sum up weights
    int totalWeight = 0;
    for (const PgEntry &e : pgEntries) {
        totalWeight += e.weight;
    }

    // pick a move at random, where entries with higher weights have a higher chance of their move
    // being selected
    int r = rng.next() % totalWeight;
    for (const PgEntry &e : pgEntries) {
        r -= e.weight;
        if (r < 0) {
            result = e;
            break;
        }
    }

    return decodePgMove(pos, result.move);
}
