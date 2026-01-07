#pragma once

#include "Position.h"

#include <unordered_map>
#include <vector>

typedef double eval_t;

class Engine {
   private:
    /**
     * Transposition table entry
     *
     * Stores the evaluation of a given position at some depth,
     * as well as whether that evaluation is an upper/lower
     * bound or an exact evaluation
     */
    struct TTEntry {
        eval_t eval;
        int depth;

        enum Flag { EXACT, LOWERBOUND, UPPERBOUND } flag;
    };

    /**
     * Stores previously evaluated positions
     */
    std::unordered_map<uint64_t, TTEntry> transpositionTable;

    // clang-format off
    static constexpr eval_t pieceSquareTables[12][8][8] = {
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
         {-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}}
    };
    // clang-format on

    /**
     * The maximum depth to search
     */
    int maxDepth;

    /**
     * Assigns a move a value based on how promising
     * it looks in that position; useful for move ordering
     * before alpha-beta search
     *
     * @param pos in which a move should be judged
     * @param move The move to rate
     * @return A value representing how promising the move is
     */
    eval_t rateMove(Position &pos, const Move &move);

    /**
     * Calculates heuristic value of a position
     *
     * @param pos to evaluate
     * @return The heuristic value of the position
     */
    eval_t evaluate(Position &pos);

    /**
     * Calculates heuristic value of a position
     *
     * @param pos to evaluate
     * @param legalMoves The possible legal moves from that position
     * @return The heuristic value of the position
     */
    eval_t evaluate(Position &pos, const std::vector<Move> &&legalMoves);

    /**
     * Minimax algorithm with alpha-beta pruning
     *
     * @param pos to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the position to move to
     */
    eval_t negamax(Position &pos, eval_t alpha, eval_t beta, int depth);

    /**
     * Iterative deepening search (basically BFS)
     *
     * @param pos to evaluate
     * @return The heuristic value of the position to move to
     */
    eval_t iterativeDeepening(Position &pos);

   public:
    Engine(int depth);

    /**
     * for testing
     */
    // eval_t get_eval(Position& pos) {
    //     return evaluate(pos);
    // }

    /**
     * Counts the number of positions that can be reached from the current position
     *
     * @param pos to evaluate
     * @param depth The depth to search
     * @return The number of positions that can be reached
     */
    void countPositions(Position &pos, int depth) const;

    /**
     * Counts the number of positions with depth 1, 2, ..., maxDepth
     * usng iterative deepening and prints the results in perft format
     *
     * @param pos to evaluate
     * @param maxDepth The maximum depth to search
     */
    void countPositionsBuildup(Position &pos, int maxDepth) const;

    /**
     * Gets the best move in a certain position
     *
     * @param pos to evaluate
     * @param legalMoves The possible legal moves from that position
     * @return The best move to make
     */
    Move getMove(Position &pos, std::vector<Move> &&legalMoves);
};
