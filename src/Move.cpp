#include "Move.h"

Move::Move() = default;

constexpr Move::Move(uint16_t _data)
    : data(_data) {}

constexpr Move::Move(Square from, Square to)
    : data(from | (to << 6)) {}

constexpr Move::Move(Square from, Square to, Type moveType, PieceType promotedPieceType)
    : data(from | (to << 6) | moveType | ((promotedPieceType - KNIGHT) << 11)) {}

constexpr uint16_t Move::raw() const {
    return data;
}

constexpr Square Move::fromSquare() const {
    return static_cast<Square>(data & 0x3F);
}

constexpr Square Move::toSquare() const {
    return static_cast<Square>((data >> 6) & 0x3F);
}

constexpr Piece Move::promotedPieceType() const {
    return static_cast<Piece>(((data >> 11) & 0x3) + KNIGHT);
}

constexpr bool Move::isPromotion() const {
    return (data & (3 << 14)) == PROMOTION;
}

constexpr bool Move::isEnPassant() const {
    return (data & (3 << 14)) == EN_PASSANT;
}

// Note: When true, the type of castle can be determined by which rook moves, which is
// represented in the from/to squares
constexpr bool Move::isCastles() const {
    return (data & (3 << 14)) == CASTLING;
}

/**
 * Convert the move to a readable string representation,
 * mainly used for debugging
 *
 * @return string representation of the move
 */
std::string Move::toString() const {
    return squareToString(fromSquare()) + " " + squareToString(toSquare());
}

bool Move::operator==(const Move &other) const {
    return data == other.raw();
}

bool Move::operator!=(const Move &other) const {
    return data != other.raw();
}

bool validateCoords(const std::string &input) {
    if (input.size() != 5) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') && (input[1] >= '1' && input[1] <= '8') &&
           (input[2] == ' ') && (input[3] >= 'a' && input[3] <= 'h') &&
           (input[4] >= '1' && input[4] <= '8');
}
