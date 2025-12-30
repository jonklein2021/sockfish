#include "Position.h"
#include "types.h"

#include <memory>

namespace PositionUtil {

/**
 * Returns true if the given square is under attack by the given side
 */
bool isUnderAttack(std::shared_ptr<Position> pos, Square sq, Color attacker);

bool isCheck(std::shared_ptr<Position> pos);

bool isCheck(std::shared_ptr<Position> pos, Color defender);

bool insufficientMaterial(std::shared_ptr<Position> pos);

bool isCheckmate(std::shared_ptr<Position> pos);

bool isTerminal(std::shared_ptr<Position> pos);

}  // namespace PositionUtil
