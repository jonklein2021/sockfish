#include <cstdint>
#include "Engine.h"

Engine::Engine() : Engine(8) {}

Engine::Engine(int depth) : maxDepth(depth) {}

eval_t Engine::evaluate(const GameState& state) {
    eval_t score = 0;

    // total up white piece values
    for (int i = 0; i < 6; i++) {
        score += __builtin_popcountll(state.board.pieceBits[i]) * pieceValues[i];
    }

    // total up black piece values
    for (int i = 6; i < 12; i++) {
        score -= __builtin_popcountll(state.board.pieceBits[i]) * pieceValues[i];
    }

    // return score relative to the current player for negamax
    return state.whiteToMove ? score : -score;
}

eval_t Engine::negamax(GameState& state, eval_t alpha, eval_t beta, int depth) {
    if (depth >= maxDepth || state.isTerminal()) {
        eval_t eval = evaluate(state);
        // std::cout << std::string(depth * 2, ' ') << "Eval: " << eval << "\n";
        return eval;
    }

    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    std::vector<Move> legalMoves = state.generateMoves();

    for (const Move& move : legalMoves) {
        const Metadata md = state.makeMove(move);

        // Print move with indentation based on depth
        // std::cout << std::string(depth * 2, ' ') << "Depth " << depth << ": " << move.to_string() << "\n";

        eval_t eval = -negamax(state, -beta, -alpha, depth + 1);
        state.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            // std::cout << std::string(depth * 2, ' ') << "Pruned at depth " << depth << "\n";
            break;
        }
    }

    return bestEval;
}

Move Engine::getMove(GameState& state, const std::vector<Move>& legalMoves) {
    if (legalMoves.empty()) {
        return Move();
    }

    Move bestMove;
    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    for (const Move& move : legalMoves) {
        const Metadata md = state.makeMove(move);
        
        // std::cout << std::string(0 * 2, ' ') << "Depth " << 0 << ": " << move.to_string() << "\n";
        
        eval_t eval = -negamax(state, std::numeric_limits<eval_t>::lowest(), std::numeric_limits<eval_t>::max(), 1);
        state.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
