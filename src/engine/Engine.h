#pragma once

#include <limits>
#include <iostream>

#include "GameState.h"

typedef double eval_t;

class Engine {
private:
    int maxDepth;

    /**
     * Calculates heuristic value of state
     * 
     * @param state The state to evaluate
     * @param legalMoves The possible legal moves from that state
     * @return The heuristic value of the state
     */
    eval_t evaluate(const GameState& state, const std::vector<Move>& legalMoves);

    /**
     * Minimax algorithm with alpha-beta pruning
     * 
     * @param state The state to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the state to move to
     */
    eval_t negamax(GameState& state, eval_t alpha, eval_t beta, int depth);
    
public:
    Engine();
    Engine(int depth);
    Move getMove(GameState& state, const std::vector<Move>& legalMoves);
};