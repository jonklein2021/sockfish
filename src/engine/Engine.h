#pragma once

#include <limits>
#include <iostream>

#include "GameState.h"

typedef double eval_t;

class Engine {
private:
    std::unordered_map<uint64_t, eval_t> transpositionTable;
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
    
    /**
     * Counts the number of positions that can be reached from the current state
     * 
     * @param state The state to evaluate
     * @param depth The depth to search
     * @return The number of positions that can be reached
     */
    void countPositions(GameState& state, int depth) const;

    /**
     * Counts the number of positions with depth 1, 2, ..., maxDepth
     * usng iterative deepening and prints the results in perft format
     * 
     * @param state The state to evaluate
     * @param maxDepth The maximum depth to search
     */
    void countPositionsBuildup(GameState& state, int maxDepth) const;

    /**
     * Gets the best move for the current state
     * 
     * @param state The state to evaluate
     * @param legalMoves The possible legal moves from that state
     * @return The best move to make
     */
    Move getMove(GameState& state, const std::vector<Move>& legalMoves);
};