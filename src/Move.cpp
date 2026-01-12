#include "Move.h"

#include "types.h"

#include <sstream>

void Move::setFlag(Type t) {
    data |= t;
}

void Move::setPromotedPieceType(PieceType promotedPieceType) {
    data |= ((promotedPieceType - KNIGHT) << 12);  // set piece
    data |= PROMOTION;                             // set flag
}

Move Move::fromCoordinateString(const std::string &coords) {
    const Square from = coordinateStringToSquare(coords.substr(0, 2));
    const Square to = coordinateStringToSquare(coords.substr(2));
    return Move(from, to);
}

std::string Move::toCoordinateString() const {
    return squareToCoordinateString(getFromSquare()) + squareToCoordinateString(getToSquare());
}

std::string Move::toString() const {
    std::stringstream ss;
    if (data != 0) {
        ss << toCoordinateString() << ", ";
    }
    if (isPromotion()) {
        ss << "Promote to " << PIECE_TYPE_NAMES[getPromotedPieceType()].data() << ", ";
    }
    if (isEnPassant()) {
        ss << "En Passant, ";
    }
    if (isCastles()) {
        ss << "Castles, ";
    }
    ss << "data=" << data;
    return ss.str();
}

bool validateCoords(const std::string &input) {
    if (input.size() != 4) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') && (input[1] >= '1' && input[1] <= '8') &&
           (input[2] >= 'a' && input[2] <= 'h') && (input[3] >= '1' && input[3] <= '8');
}
