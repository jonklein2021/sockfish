#pragma once

#include "Move.h"

#include <algorithm>
#include <cassert>

class MoveList {
   private:
    static constexpr int MOVE_LIST_CAPACITY = 256;

    std::array<Move, MOVE_LIST_CAPACITY> moves;
    int length = 0;

   public:
    void add(const Move &m) {
        assert(length < MOVE_LIST_CAPACITY);
        moves[length++] = m;
    }

    auto begin() {
        return moves.begin();
    }

    auto end() {
        return moves.begin() + length;
    }

    auto begin() const {
        return moves.begin();
    }

    auto end() const {
        return moves.begin() + length;
    }

    bool has(const Move m) const {
        return std::find(begin(), end(), m) != end();
    }

    void clear() {
        length = 0;
    }

    size_t size() const {
        return length;
    }

    size_t empty() const {
        return length == 0;
    }

    // read and write friendly subscript operator
    Move &operator[](int index) {
        assert(index >= 0 && index < MOVE_LIST_CAPACITY);
        return moves.at(index);
    }

    // read-only subscript operator
    const Move &operator[](int index) const {
        assert(index >= 0 && index < MOVE_LIST_CAPACITY);
        return moves[index];
    }
};
