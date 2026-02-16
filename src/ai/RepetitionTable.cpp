#include "RepetitionTable.h"

#include <cassert>

void RepetitionTable::push(uint64_t posHash) {
    table[index++] = posHash;
}

void RepetitionTable::pop() {
    assert(index > 0);
    index--;
}

bool RepetitionTable::contains(uint64_t posHash) {
    for (int i = 0; i < index; i++) {
        if (table[i] == posHash) {
            return true;
        }
    }
    return false;
}
