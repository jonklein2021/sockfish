#include "Position.h"
#include "src/core/MoveList.h"
#include "src/core/types.h"

namespace PositionUtil {

bool insufficientMaterial(Position &pos);

bool is50MoveRuleDraw(const Position &pos);

// N.B: This doesn't assure that legalMoves belongs to any position
bool isStalemate(const MoveList &legalMoves);

bool isStalemate(Position &pos);

bool isCheckmate(Position &pos);

bool isTerminal(Position &pos);

GameStatus getGameStatus(Position &pos);

}  // namespace PositionUtil
