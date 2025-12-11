#pragma once

#include <memory>
#include <unordered_map>

#include "GameState.h"

typedef double eval_t;

class Engine {
  private:
    /**
     * Entry in transposition tables
     *
     * Stores the evaluation of a given state at some depth,
     * as well as whether that evaluation is an upper/lower
     * bound or an exact evaluation
     */
    struct TTEntry {
        eval_t eval;
        int depth;
        enum Flag { EXACT, LOWERBOUND, UPPERBOUND } flag;
    };

    /**
     * Stores previously evaluated states
     */
    std::unordered_map<uint64_t, TTEntry> transpositionTable;

    const eval_t pieceSquareTables[12][8][8] = {
        {// WP
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
         {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
         {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
         {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
         {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
         {0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
         {0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}},
        {// WN
         {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
         {-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0},
         {-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0},
         {-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0},
         {-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0},
         {-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0},
         {-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0},
         {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}},
        {// WB
         {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
         {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0},
         {-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0},
         {-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0},
         {-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0},
         {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0},
         {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}},
        {// WR
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
         {0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0}},
        {// WQ
         {-1.5, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -1.5},
         {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
         {0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
         {-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
         {-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
         {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-1.5, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -1.5}},
        {// WK
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
         {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
         {2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
         {2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0}},
        {// BP
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
         {0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5},
         {0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5},
         {0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0},
         {0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5},
         {1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0},
         {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0},
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}},
        {// BN
         {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
         {-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0},
         {-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0},
         {-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0},
         {-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0},
         {-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0},
         {-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0},
         {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}},
        {// BB
         {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
         {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0},
         {-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0},
         {-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0},
         {-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0},
         {-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0},
         {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}},
        {// BR
         {0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5},
         {0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5},
         {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}},
        {// BQ
         {-1.5, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -1.5},
         {-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
         {0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
         {-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5},
         {-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0},
         {-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0},
         {-1.5, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -1.5}},
        {// BK
         {2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0},
         {2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0},
         {-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
         {-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}}};

    /**
     * The maximum depth to search
     */
    int maxDepth;

    /**
     * Assigns a move a value based on how promising
     * it looks in that state; useful for move ordering
     * before alpha-beta search
     *
     * @param state The state in which that move should be judged
     * @param move The move to rate
     * @return A value representing how promising the move is
     */
    eval_t rateMove(const std::unique_ptr<GameState> state, const Move &move);

    /**
     * Calculates heuristic value of state
     *
     * @param state The state to evaluate
     * @return The heuristic value of the state
     */
    eval_t evaluate(const std::unique_ptr<GameState> state);

    /**
     * Calculates heuristic value of state
     *
     * @param state The state to evaluate
     * @param legalMoves The possible legal moves from that state
     * @return The heuristic value of the state
     */
    eval_t evaluate(const std::unique_ptr<GameState> state,
                    const std::vector<Move> &legalMoves);

    /**
     * Minimax algorithm with alpha-beta pruning
     *
     * @param state The state to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the state to move to
     */
    eval_t negamax(std::unique_ptr<GameState> state, eval_t alpha, eval_t beta,
                   int depth);

    /**
     * Iterative deepening search (basically BFS)
     *
     * @param state The state to evaluate
     * @return The heuristic value of the state to move to
     */
    eval_t iterativeDeepening(std::unique_ptr<GameState> state);

  public:
    Engine();
    Engine(int depth);

    /**
     * for testing
     */
    eval_t get_eval(std::unique_ptr<GameState> state) {
        return evaluate(std::move(state));
    }

    /**
     * Counts the number of positions that can be reached from the current state
     *
     * @param state The state to evaluate
     * @param depth The depth to search
     * @return The number of positions that can be reached
     */
    void countPositions(std::unique_ptr<GameState> state, int depth) const;

    /**
     * Counts the number of positions with depth 1, 2, ..., maxDepth
     * usng iterative deepening and prints the results in perft format
     *
     * @param state The state to evaluate
     * @param maxDepth The maximum depth to search
     */
    void countPositionsBuildup(std::unique_ptr<GameState> state,
                               int maxDepth) const;

    /**
     * Gets the best move for the current state
     *
     * @param state The state to evaluate
     * @param legalMoves The possible legal moves from that state
     * @return The best move to make
     */
    Move getMove(std::unique_ptr<GameState> state,
                 std::vector<Move> &legalMoves);
};
