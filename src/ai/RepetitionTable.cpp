#include "RepetitionTable.h"

#include <cassert>

void RepetitionTable::push(uint64_t posHash) {
    assert(index < TABLE_SIZE);
    table[index++] = posHash;
}

void RepetitionTable::pop() {
    assert(index > 0);
    index--;
}

bool RepetitionTable::contains(uint64_t posHash) const {
    for (int i = 0; i < index; i++) {
        if (table[i] == posHash) {
            return true;
        }
    }
    return false;
}

void RepetitionTable::clear() {
    index = 0;
}
