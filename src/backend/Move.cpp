#include <sstream>
#include "Move.h"
#include "constants.h"

/**
 * N.B: This method ONLY compares the `from` and `to` members of the Move struct.
 * It does not compare `pieceMoved` or `isCapture`
 * 
 * @param other move to compare to
 */
bool Move::equals(const Move &other) {
    return (
        from.x == other.from.x &&
        from.y == other.from.y &&
        to.x == other.to.x &&
        to.y == other.to.y &&
        promotionPiece == other.promotionPiece
    );
}

std::string Move::to_string() const {
    std::ostringstream ss;
    
    ss << (pieceMoved == None ? "None" : pieceFilenames[pieceMoved]) << ": (" 
    << from.x << ", " << from.y << ") -> ("
    << to.x << ", " << to.y << ")" << (isCapture ? " (capture)" : "")
    << (promotionPiece == None ? "" : " (promotion: " + pieceFilenames[promotionPiece] + ")");

    return ss.str();
}