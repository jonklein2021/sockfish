#include "Position.h"
#include "types.h"

namespace PositionUtil {

/**
 * Returns true if the given square is under attack by the given side
 */
bool isUnderAttack(Position &pos, Square sq, Color attacker);

bool isCheck(Position &pos);

bool isCheck(Position &pos, Color defender);

bool insufficientMaterial(Position &pos);

bool isCheckmate(Position &pos);

bool isTerminal(Position &pos);

}  // namespace PositionUtil
