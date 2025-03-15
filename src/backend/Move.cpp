#include <sstream>
#include "Move.h"
#include "constants.h"

/**
 * N.B: This method IGNORES the isCapture
 * member for convenience
 * 
 * @param other move to compare to
 */
bool Move::equals(const Move &other) {
    return (
        pieceMoved == other.pieceMoved &&
        from.x == other.from.x &&
        from.y == other.from.y &&
        to.x == other.to.x &&
        to.y == other.to.y
    );
}

std::string Move::to_string() {
    std::ostringstream ss;
    
    ss << pieceFilenames[pieceMoved] << ": (" 
    << from.x << ", " << from.y << ") -> ("
    << to.x << ", " << to.y << ")";

    return ss.str();
}