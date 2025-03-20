#include <functional>
#include <queue>
#include "Engine.h"

Engine::Engine() : Engine(8) {}

Engine::Engine(int depth) : maxDepth(depth) {}

void Engine::countPositionsBuildup(GameState& state, int maxDepth) const {
    struct Node {
        GameState state;
        int depth;
    };

    std::queue<Node> q;
    q.push({state, 0});

    std::vector<uint64_t> depthCounts(maxDepth + 1, 0);

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        // as soon as we reach maxDepth we can stop
        if (current.depth >= maxDepth) break;

        std::vector<Move> legalMoves = current.state.generateMoves();

        // current.state.board.prettyPrint();

        // std::cout << legalMoves.size() << " legal moves" << std::endl;
        for (const Move& move : legalMoves) {
            // std::cout << moveToCoords(move) << std::endl;
            const Metadata md = current.state.makeMove(move);
            depthCounts[current.depth+1]++;
            q.push({current.state, current.depth + 1});
            current.state.unmakeMove(move, md);
        }
    }

    for (int d = 1; d <= maxDepth; d++) {
        std::cout << ";D" << d << " " << depthCounts[d] << " ";
    }

    std::cout << std::endl;
}

void Engine::countPositions(GameState& state, int depth) const {
    uint64_t total = 0;
    uint64_t captures = 0;
    uint64_t eps = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t checkmates = 0;

    std::function<uint64_t(GameState&, int)> countPositionsHelper = [&](GameState& state, int depth) -> uint64_t {
        if (depth == 0 || state.isTerminal()) {
            return 1; // Base case: count this position
        }

        uint64_t count = 0;
        std::vector<Move> legalMoves = state.generateMoves();
        
        for (const Move& move : legalMoves) {            
            const Metadata md = state.makeMove(move);
            
            count += countPositionsHelper(state, depth - 1);
            total += count;  // Accumulate total count
            
            if (move.isCapture) captures++;
            if (abs(move.from.x - move.to.x) >= 2 && (move.piece == WK || move.piece == BK)) castles++;
            if (move.isEnPassant) eps++;
            if (move.promotionPiece != None) promotions++;
            if (state.isCheck()) {
                checks++;
                if (state.isTerminal()) {
                    // std::cout << moveToCoords(move) << " is checkmate\n";
                    // state.board.prettyPrint();
                    checkmates++;
                }
            }

            state.unmakeMove(move, md);
        }

        return count;
    };

    total = countPositionsHelper(state, depth); // Start recursion

    std::cout << "Total positions: " << total << "\n";
    std::cout << "Total captures: " << captures << "\n";
    std::cout << "Total en passants: " << eps << "\n";
    std::cout << "Total castles: " << castles << "\n";
    std::cout << "Total promotions: " << promotions << "\n";
    std::cout << "Total checks: " << checks << "\n";
    std::cout << "Total checkmates: " << checkmates << "\n" << std::endl;
}

eval_t Engine::evaluate(const GameState& state, const std::vector<Move>& legalMoves) {
    if (legalMoves.empty() && state.isCheck()) {
        return state.whiteToMove ? 1738 : -1738;
    }
    
    // piece values
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
    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    std::vector<Move> legalMoves = state.generateMoves();

    if (depth >= maxDepth || state.isTerminal()) {
        eval_t eval = evaluate(state, legalMoves);
        // std::cout << std::string(depth * 2, ' ') << "Eval: " << eval << "\n";
        return eval;
    }

    for (const Move& move : legalMoves) {
        const Metadata md = state.makeMove(move);

        // Print move with indentation based on depth
        // std::cout << std::string(depth * 2, ' ') << "Depth " << depth << ": " << move.to_string() << "\n";
        // state.board.prettyPrint();

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

    std::cout << "Analyzing moves..." << std::endl;

    Move bestMove;
    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    for (const Move& move : legalMoves) {
        const Metadata md = state.makeMove(move);
        
        std::cout << "  " << move.to_string() << std::endl;
        // state.board.prettyPrint();
        
        eval_t eval = -negamax(state, std::numeric_limits<eval_t>::lowest(), std::numeric_limits<eval_t>::max(), 1);
        std::cout << "    eval = " << eval << std::endl;
        
        state.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
