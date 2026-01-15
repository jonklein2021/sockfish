#include "Position.h"
#include "src/core/types.h"

#include <vector>

namespace PositionUtil {

/**
 * Returns true if the given square is under attack by the given side
 */
bool isUnderAttack(Position &pos, Square sq, Color attacker);

bool isCheck(Position &pos);

bool isCheck(Position &pos, Color defender);

bool insufficientMaterial(Position &pos);

bool is50MoveRuleDraw(const Position &pos);

// N.B: This doesn't assure that legalMoves belongs to any position
bool isStalemate(const std::vector<Move> &legalMoves);

bool isStalemate(Position &pos);

bool isCheckmate(Position &pos);

bool isTerminal(Position &pos);

GameStatus getGameStatus(Position &pos);

}  // namespace PositionUtil
