#include "Engine.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>

Engine::Engine()
    : Engine(4) {}

Engine::Engine(int depth)
    : maxDepth(depth) {}

void Engine::countPositionsBuildup(const Position &state, int maxDepth) const {
    struct Node {
        Position state;
        int depth;
    };

    std::queue<Node> q;
    q.push({state, 0});

    std::vector<uint64_t> depthCounts(maxDepth + 1, 0);

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        // as soon as we reach maxDepth we can stop
        if (current.depth >= maxDepth) {
            break;
        }

        std::vector<Move> legalMoves = current.state.generateMoves();

        for (const Move &move : legalMoves) {
            const Metadata md = current.state.makeMove(move);
            depthCounts[current.depth + 1]++;
            q.push({current.state, current.depth + 1});
            current.state.unmakeMove(move, md);
        }
    }

    for (int d = 1; d <= maxDepth; d++) {
        std::cout << ";D" << d << " " << depthCounts[d] << " ";
    }

    std::cout << std::endl;
}

void Engine::countPositions(Position &state, int depth) const {
    uint64_t total = 0;
    uint64_t captures = 0;
    uint64_t eps = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t checkmates = 0;

    std::function<uint64_t(Position &, int)> countPositionsHelper = [&](Position &state,
                                                                         int depth) -> uint64_t {
        if (depth == 0 || state.isTerminal()) {
            return 1;  // Base case: count this position
        }

        uint64_t count = 0;
        std::vector<Move> legalMoves = state.generateMoves();

        std::cout << legalMoves.size() << " legal moves\n";
        for (const Move &m : legalMoves) {
            std::cout << m.toString() << "\n";
        }

        for (const Move &move : legalMoves) {
            const Metadata md = state.makeMove(move);

            state.print();

            count += countPositionsHelper(state, depth - 1);
            total += count;  // Accumulate total count

            if (move.capturedPiece != NO_PIECE) {
                captures++;
            }
            if (abs(move.from.x - move.to.x) >= 2 && (move.piece == WK || move.piece == BK)) {
                castles++;
            }
            if (move.isEnPassant) {
                eps++;
            }
            if (move.promotionPiece != NO_PIECE) {
                promotions++;
            }
            if (state.isCheck()) {
                checks++;
                if (state.isTerminal()) {
                    // std::cout << moveToCoords(move) << " is checkmate\n";
                    // state.print();
                    checkmates++;
                }
            }

            state.unmakeMove(move, md);
        }

        return count;
    };

    total = countPositionsHelper(state, depth);  // Start recursion

    std::cout << "Total positions: " << total << "\n";
    std::cout << "Total captures: " << captures << "\n";
    std::cout << "Total en passants: " << eps << "\n";
    std::cout << "Total castles: " << castles << "\n";
    std::cout << "Total promotions: " << promotions << "\n";
    std::cout << "Total checks: " << checks << "\n";
    std::cout << "Total checkmates: " << checkmates << "\n" << std::endl;
}

eval_t Engine::rateMove(const Position &state, const Move &move) {
    // capture moves are promising
    eval_t rating = 0;
    if (move.capturedPiece != NO_PIECE) {
        rating += pieceValues[move.capturedPiece] - pieceValues[move.piece];
    }

    // pawn promotion moves are likely to be good
    if (move.promotionPiece != NO_PIECE) {
        rating += pieceValues[move.promotionPiece] - pieceValues[move.piece];
    }

    // moves that put the opponent in check should be checked early
    Position temp(state);
    temp.makeMove(move);
    temp.whiteToMove = !temp.whiteToMove;
    if (temp.isCheck()) {
        rating += 10;
    }

    return rating;
}

eval_t Engine::evaluate(const Position &state) {
    return evaluate(state, state.generateMoves());
}

eval_t Engine::evaluate(const Position &state, const std::vector<Move> &legalMoves) {
    if (legalMoves.empty() && state.isCheck()) {
        return state.whiteToMove ? 1738 : -1738;
    }

    const eval_t piecePositionWeight = 0.5;

    eval_t score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Piece p : {WP, WN, WB, WR, WQ, WK}) {
        uint64_t pieces = state.pieceBit(p);
        while (pieces) {
            int trailingZeros = __builtin_ctzll(pieces);
            int x = trailingZeros % 8;
            int y = trailingZeros / 8;
            // std::cout << pieceFilenames[p] << " at " << x << ", " << y << ":
            // " << pieceValues[p] << " + " << piecePositionWeight << "*" <<
            // pieceSquareTables[p][y][x] << std::endl;
            score += (pieceValues[p] + piecePositionWeight * pieceSquareTables[p][y][x]);
            pieces &= pieces - 1;
        }
    }

    for (Piece p : {BP, BN, BB, BR, BQ, BK}) {
        uint64_t pieces = state.pieceBit(p);
        while (pieces) {
            int trailingZeros = __builtin_ctzll(pieces);
            int x = trailingZeros % 8;
            int y = trailingZeros / 8;
            // std::cout << pieceFilenames[p] << " at " << x << ", " << y << ":
            // " << pieceValues[p] << " + " << piecePositionWeight << "*" <<
            // pieceSquareTables[p][y][x] << std::endl;
            score -= (pieceValues[p] + piecePositionWeight * pieceSquareTables[p][y][x]);
            pieces &= pieces - 1;
        }
    }

    // return score relative to the current player for negamax
    return state.whiteToMove ? score : -score;
}

eval_t Engine::negamax(Position &state, eval_t alpha, eval_t beta, int depth) {
    uint64_t h = state.hash();
    if (transpositionTable.find(h) != transpositionTable.end()) {
        const TTEntry &entry = transpositionTable[h];
        if (entry.depth >= depth) {
            if (entry.flag == TTEntry::EXACT) {
                return entry.eval;
            }
            if (entry.flag == TTEntry::LOWERBOUND && entry.eval >= beta) {
                return entry.eval;
            }
            if (entry.flag == TTEntry::UPPERBOUND && entry.eval <= alpha) {
                return entry.eval;
            }
        }
    }

    // base case
    if (depth >= maxDepth || state.isTerminal()) {
        return evaluate(state);
    }

    std::vector<Move> legalMoves = state.generateMoves();

    eval_t bestEval = std::numeric_limits<eval_t>::lowest();

    for (const Move &move : legalMoves) {
        const Metadata md = state.makeMove(move);

        eval_t eval = -negamax(state, -beta, -alpha, depth + 1);

        // save result in transposition table
        TTEntry newEntry{eval, depth, TTEntry::EXACT};
        if (eval <= alpha) {
            newEntry.flag = TTEntry::UPPERBOUND;
        }
        if (eval >= beta) {
            newEntry.flag = TTEntry::LOWERBOUND;
        }
        transpositionTable[h] = newEntry;

        state.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    return bestEval;
}

eval_t Engine::iterativeDeepening(const Position &state) {
    eval_t bestEval = std::numeric_limits<eval_t>::lowest();

    struct Node {
        Position state;
        int depth;
    };

    std::queue<Node> q;
    q.push({state, 0});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        std::vector<Move> legalMoves = current.state.generateMoves();

        // evaluate terminal nodes
        if (current.depth >= maxDepth || current.state.isTerminal()) {
            eval_t evalScore = evaluate(current.state, {});
            if (evalScore > bestEval) {
                bestEval = evalScore;
            }
        }

        // evaluate non-terminal nodes
        for (const Move &move : legalMoves) {
            Position nextState = current.state;
            nextState.makeMove(move);
            q.push({nextState, current.depth + 1});
        }
    }

    return bestEval;
}

Move Engine::getMove(Position &state, std::vector<Move> &legalMoves) {
    if (legalMoves.empty()) {
        return Move();
    }

    // sort moves by how promising they seem
    auto cmp = [&](const Move &a, const Move &b) {
        return rateMove(state, a) > rateMove(state, b);
    };

    std::sort(legalMoves.begin(), legalMoves.end(), cmp);

    std::cout << "Analyzing moves..." << std::endl;

    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    Move bestMove;

    for (const Move &move : legalMoves) {
        const Metadata md = state.makeMove(move);

        // state.board.prettyPrint();
        std::cout << "  " << move.toString() << std::endl;

        eval_t eval = -negamax(state, std::numeric_limits<eval_t>::lowest(),
                               std::numeric_limits<eval_t>::max(), 0);

        std::cout << "\teval = " << eval << std::endl;
        state.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
