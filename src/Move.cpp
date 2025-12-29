#include "Move.h"

#include "types.h"

#include <sstream>

Move::Move() = default;

// constexpr Move::Move(uint16_t _data)
//     : data(_data) {}
//
// constexpr Move::Move(Square from, Square to)
//     : data(from | (to << 6)) {}
//
// constexpr Move::Move(Square from, Square to, Type moveType, PieceType promotedPieceType)
//     : data(from | (to << 6) | moveType | ((promotedPieceType - KNIGHT) << 11)) {}

void Move::setFlag(Type t) {
    data |= t;
}

void Move::setPromotedPiece(PieceType promotedPieceType) {
    data |= ((promotedPieceType - KNIGHT) << 11);  // set piece
    data |= PROMOTION;                             // set flag
}

Move Move::fromCoordinateString(const std::string &coords) {
    const Square from = coordinateStringToSquare(coords.substr(0, 2));
    const Square to = coordinateStringToSquare(coords.substr(2));
    return Move(from, to);
}

std::string Move::toCoordinateString() const {
    return squareToCoordinateString(fromSquare()) + " " + squareToCoordinateString(toSquare());
}

std::string Move::toString() const {
    std::stringstream ss(toCoordinateString());
    if (isPromotion()) {
        ss << ", Promote to " << pieceTypeNames[promotedPieceType()].data();
    }
    if (isEnPassant()) {
        ss << ", En Passant";
    }
    if (isCastles()) {
        ss << ", Castles";
    }
    ss << ", data=" << data;
    return ss.str();
}

bool Move::operator==(const Move &other) const {
    return data == other.raw();
}

bool Move::operator!=(const Move &other) const {
    return data != other.raw();
}

bool validateCoords(const std::string &input) {
    if (input.size() != 4) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') && (input[1] >= '1' && input[1] <= '8') &&
           (input[2] >= 'a' && input[2] <= 'h') && (input[3] >= '1' && input[3] <= '8');
}
