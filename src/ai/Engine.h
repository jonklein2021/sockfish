#pragma once

#include "src/core/Position.h"

#include <unordered_map>
#include <vector>

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
        Eval eval;
        int depth;

        enum Flag { EXACT, LOWERBOUND, UPPERBOUND } flag;
    };

    /**
     * Stores previously evaluated positions
     */
    std::unordered_map<uint64_t, TTEntry> transpositionTable;

    // clang-format off
    static constexpr Eval pieceSquareTables[12][64] = {
        {// WP
         +00, +00, +00, +00, +00, +00, +00, +00,
         +50, +50, +50, +50, +50, +50, +50, +50,
         +10, +10, +20, +30, +30, +20, +10, +10,
         +05, +05, +10, +25, +25, +10, +05, +05,
         +00, +00, +00, +20, +20, +00, +00, +00,
         +05, -05, -10, +00, +00, -10, -05, +05,
         +05, +10, +10, -20, -20, +10, +10, +05,
         +00, +00, +00, +00, +00, +00, +00, +00},
        {// WN
         -50, -40, -30, -30, -30, -30, -40, -50,
         -40, -20, +00, +00, +00, +00, -20, -40,
         -30, +00, +10, +15, +15, +10, +00, -30,
         -30, +05, +15, +20, +20, +15, +05, -30,
         -30, +00, +15, +20, +20, +15, +00, -30,
         -30, +05, +10, +15, +15, +10, +05, -30,
         -40, -20, +00, +05, +05, +00, -20, -40,
         -50, -40, -30, -30, -30, -30, -40, -50},
        {// WB
         -20, -10, -10, -10, -10, -10, -10, -20,
         -10, +00, +00, +00, +00, +00, +00, -10,
         -10, +00, +05, +10, +10, +05, +00, -10,
         -10, +05, +05, +10, +10, +05, +05, -10,
         -10, +00, +10, +10, +10, +10, +00, -10,
         -10, +10, +10, +10, +10, +10, +10, -10,
         -10, +05, +00, +00, +00, +00, +05, -10,
         -20, -10, -10, -10, -10, -10, -10, -20},
        {// WR
         +00, +00, +00, +00, +00, +00, +00, +00,
         +05, +10, +10, +10, +10, +10, +10, +05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         +00, +00, +00, +05, +05, +00, +00, +00},
        {// WQ
         -15, -10, -10, -05, -05, -10, -10, -15,
         -10, +05, +00, +00, +00, +00, +00, -10,
         -10, +05, +05, +05, +05, +05, +00, -10,
         +00, +00, +05, +05, +05, +05, +00, -05,
         -05, +00, +05, +05, +05, +05, +00, -05,
         -10, +00, +05, +05, +05, +05, +00, -10,
         -10, +00, +00, +00, +00, +00, +00, -10,
         -15, -10, -10, -05, -05, -10, -10, -15},
        {// WK
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -20, -30, -30, -40, -40, -30, -30, -20,
         -10, -20, -20, -20, -20, -20, -20, -10,
         +20, +20, +00, +00, +00, +00, +20, +20,
         +20, +30, +10, +00, +00, +10, +30, +20},
        {// BP
         +00, +00, +00, +00, +00, +00, +00, +00,
         +05, +10, +10, -20, -20, +10, +10, +05,
         +05, -05, -10, +00, +00, -10, -05, +05,
         +00, +00, +00, +20, +20, +00, +00, +00,
         +05, +05, +10, +25, +25, +10, +05, +05,
         +10, +10, +20, +30, +30, +20, +10, +10,
         +50, +50, +50, +50, +50, +50, +50, +50,
         +00, +00, +00, +00, +00, +00, +00, +00},
        {// BN
         -50, -40, -30, -30, -30, -30, -40, -50,
         -40, -20, +00, +05, +05, +00, -20, -40,
         -30, +05, +10, +15, +15, +10, +05, -30,
         -30, +00, +15, +20, +20, +15, +00, -30,
         -30, +05, +15, +20, +20, +15, +05, -30,
         -30, +00, +10, +15, +15, +10, +00, -30,
         -40, -20, +00, +00, +00, +00, -20, -40,
         -50, -40, -30, -30, -30, -30, -40, -50},
        {// BB
         -20, -10, -10, -10, -10, -10, -10, -20,
         -10, +05, +00, +00, +00, +00, +05, -10,
         -10, +10, +10, +10, +10, +10, +10, -10,
         -10, +00, +10, +10, +10, +10, +00, -10,
         -10, +05, +05, +10, +10, +05, +05, -10,
         -10, +00, +05, +10, +10, +05, +00, -10,
         -10, +00, +00, +00, +00, +00, +00, -10,
         -20, -10, -10, -10, -10, -10, -10, -20},
        {// BR
         +00, +00, +00, +05, +05, +00, +00, +00,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         -05, +00, +00, +00, +00, +00, +00, -05,
         +05, +10, +10, +10, +10, +10, +10, +05,
         +00, +00, +00, +00, +00, +00, +00, +00},
        {// BQ
         -15, -10, -10, -05, -05, -10, -10, -15,
         -10, +05, +00, +00, +00, +00, +00, -10,
         -10, +05, +05, +05, +05, +05, +00, -10,
         +00, +00, +05, +05, +05, +05, +00, -05,
         -05, +00, +05, +05, +05, +05, +00, -05,
         -10, +00, +05, +05, +05, +05, +00, -10,
         -10, +00, +00, +00, +00, +00, +00, -10,
         -15, -10, -10, -05, -05, -10, -10, -15},
        {// BK
         +20, +30, +10, +00, +00, +10, +30, +20,
         +20, +20, +00, +00, +00, +00, +20, +20,
         -10, -20, -20, -20, -20, -20, -20, -10,
         -20, -30, -30, -40, -40, -30, -30, -20,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30,
         -30, -40, -40, -50, -50, -40, -40, -30}
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
    Eval rateMove(Position &pos, const Move &move);

    /**
     * Calculates heuristic value of a position
     *
     * @param pos to evaluate
     * @return The heuristic value of the position
     */
    Eval evaluate(Position &pos);

    /**
     * Calculates heuristic value of a position
     *
     * @param pos to evaluate
     * @param legalMoves The possible legal moves from that position
     * @return The heuristic value of the position
     */
    Eval evaluate(Position &pos, const std::vector<Move> &&legalMoves);

    /**
     * Minimax algorithm with alpha-beta pruning
     *
     * @param pos to evaluate
     * @param alpha The alpha value
     * @param beta The beta value
     * @return The heuristic value of the position to move to
     */
    Eval negamax(Position &pos, Eval alpha, Eval beta, int depth);

    /**
     * Iterative deepening search (basically BFS)
     *
     * @param pos to evaluate
     * @return The heuristic value of the position to move to
     */
    Eval iterativeDeepening(Position &pos);

   public:
    Engine(int depth);

    // for testing
    Eval getEval(Position &pos);

    /**
     * Gets the best move in a certain position
     *
     * @param pos to evaluate
     * @param legalMoves The possible legal moves from that position
     * @return The best move to make
     */
    Move getMove(Position &pos, std::vector<Move> &legalMoves);
};
