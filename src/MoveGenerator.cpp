#include "MoveGenerator.h"

#include "Move.h"
#include "MoveComputers.h"
#include "Position.h"
#include "bit_tools.h"
#include "src/types.h"

#include <algorithm>

bool MoveGenerator::isMoveLegal(std::shared_ptr<Position> pos, Move &move) {
    Color moveMaker = pos->getSideToMove();

    // simulate move on copied state
    Position::Metadata md = pos->makeMove(move);

    // test if king is in check after move
    bool isOurKingInCheck = PositionUtil::isCheck(pos, moveMaker);

    // unmake move to preserve original state
    pos->unmakeMove(move, md);

    // return true iff the move hasn't put its own king in check
    return !isOurKingInCheck;
}

template<Move::Type moveType>
void MoveGenerator::appendMovesFromBitboard(std::vector<Move> &moveList,
                                            Bitboard moves,
                                            Square srcSq) {
    while (moves) {
        const Bitboard destSqBB = moves & -moves;
        const Square destSq = Square(getLsbIndex(destSqBB));

        if (moveType == Move::PROMOTION) {
            for (PieceType promotionPiece : PROMOTION_PIECE_TYPES) {
                Move m = Move::create<Move::PROMOTION>(srcSq, destSq, promotionPiece);
                moveList.push_back(std::move(m));
            }
        } else {
            Move m = Move::create<moveType>(srcSq, destSq);
            moveList.push_back(std::move(m));
        }

        moves ^= destSqBB;
    }
}

template<Move::Type moveType, PieceType pt, typename MoveComputer>
void MoveGenerator::appendMovesFromPiece(std::shared_ptr<Position> pos,
                                         std::vector<Move> &moveList,
                                         MoveComputer moveComputer) {
    const Color toMove = pos->getSideToMove();
    const Piece piece = ptToPiece(pt, toMove);
    auto bb = pos->getPieceBB(piece);  // N.B: this needs to make a copy
    while (bb) {
        const Bitboard srcSqBB = bb & -bb;  // isolate the LSB
        const Square srcSq = Square(getLsbIndex(srcSqBB));

        // compute a bitboard of all destination squares that this piece can go to,
        // according to the specified moveComputer
        Bitboard computedDstBB = moveComputer(pos, srcSq);

        // create and append moves to the moveList
        appendMovesFromBitboard<moveType>(moveList, computedDstBB, srcSq);

        // pop this bit
        bb ^= srcSqBB;
    }
}

void MoveGenerator::appendCastlingMoves(std::vector<Move> &moveList,
                                        std::shared_ptr<Position> pos) {
    static constexpr std::array<CastleRights, 2> KINGSIDE = {WHITE_OO, BLACK_OO};
    static constexpr std::array<CastleRights, 2> QUEENSIDE = {WHITE_OOO, BLACK_OOO};

    static constexpr Square KING_FROM[2] = {e1, e8};

    static constexpr Square KING_TO_K[2] = {g1, g8};
    static constexpr Square KING_TO_Q[2] = {c1, c8};

    static constexpr Bitboard EMPTY_K[2] = {(1ull << f1) | (1ull << g1),
                                            (1ull << f8) | (1ull << g8)};

    static constexpr Bitboard EMPTY_Q[2] = {(1ull << d1) | (1ull << c1) | (1ull << b1),
                                            (1ull << d8) | (1ull << c8) | (1ull << b8)};

    static constexpr Square PASS_K[2][2] = {
        {f1, g1},  // white
        {f8, g8}   // black
    };

    static constexpr Square PASS_Q[2][2] = {{d1, c1}, {d8, c8}};

    const Color side = pos->getSideToMove();
    const CastleRights cr = pos->getMetadata().castleRights;
    const Bitboard empty = pos->getBoard().getOccupancy(EMPTY_OCCUPANCY);

    // King must not currently be in check
    if (PositionUtil::isCheck(pos, side)) {
        return;
    }

    // --- Kingside ---
    if (cr & KINGSIDE[side]) {
        if ((empty & EMPTY_K[side]) == EMPTY_K[side] &&
            !PositionUtil::isUnderAttack(pos, PASS_K[side][0], otherColor(side)) &&
            !PositionUtil::isUnderAttack(pos, PASS_K[side][1], otherColor(side))) {

            moveList.push_back(Move::create<Move::CASTLING>(KING_FROM[side], KING_TO_K[side]));
        }
    }

    // --- Queenside ---
    if (cr & QUEENSIDE[side]) {
        if ((empty & EMPTY_Q[side]) == EMPTY_Q[side] &&
            !PositionUtil::isUnderAttack(pos, PASS_Q[side][0], otherColor(side)) &&
            !PositionUtil::isUnderAttack(pos, PASS_Q[side][1], otherColor(side))) {

            moveList.push_back(Move::create<Move::CASTLING>(KING_FROM[side], KING_TO_Q[side]));
        }
    }
}

// TODO: Finish this
std::vector<Move> MoveGenerator::generatePseudolegal(std::shared_ptr<Position> pos) {
    // cannot do 218 max move space optimization since these are pseudolegal moves
    std::vector<Move> moveList;

    /* PAWNS */
    // TODO: Handle pawn promotions
    appendMovesFromPiece<Move::NORMAL, PAWN>(pos, moveList, MoveComputers::computePawnMoves);
    appendMovesFromPiece<Move::NORMAL, PAWN>(pos, moveList, MoveComputers::computePawnCaptures);
    appendMovesFromPiece<Move::EN_PASSANT, PAWN>(pos, moveList,
                                                 MoveComputers::computePawnEnPassantCaptures);

    /* KNIGHTS */
    appendMovesFromPiece<Move::NORMAL, KNIGHT>(pos, moveList, MoveComputers::computeKnightMoves);

    /* BISHOPS */
    appendMovesFromPiece<Move::NORMAL, BISHOP>(pos, moveList, MoveComputers::computeBishopMoves);

    /* ROOKS */
    appendMovesFromPiece<Move::NORMAL, ROOK>(pos, moveList, MoveComputers::computeRookMoves);

    /* QUEENS */
    appendMovesFromPiece<Move::NORMAL, QUEEN>(pos, moveList, MoveComputers::computeQueenMoves);

    /* KING */
    appendMovesFromPiece<Move::NORMAL, KING>(pos, moveList, MoveComputers::computeKingMoves);

    /* CASTLING */
    appendCastlingMoves(moveList, pos);

    return moveList;
}

std::vector<Move> MoveGenerator::generateLegal(std::shared_ptr<Position> pos) {
    std::vector<Move> pseudolegal = generatePseudolegal(pos);
    std::vector<Move> legal;
    legal.reserve(218);

    // only copy non-violating moves to legal vector
    std::copy_if(pseudolegal.begin(), pseudolegal.end(), std::back_inserter(legal),
                 [&](Move m) { return isMoveLegal(pos, m); });

    return legal;
}
