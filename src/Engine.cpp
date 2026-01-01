#include "Engine.h"

#include "MoveGenerator.h"

#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>

Engine::Engine(int depth)
    : maxDepth(depth) {}

void Engine::countPositionsBuildup(const std::shared_ptr<Position> pos, int maxDepth) const {
    struct Node {
        Position pos;
        int depth;
    };

    std::queue<Node> q;
    q.push(Node{*pos, 0});

    std::vector<uint64_t> depthCounts(maxDepth + 1, 0);

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        // stop once we reach maxDepth
        if (current.depth >= maxDepth) {
            break;
        }

        std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

        for (const Move &move : legalMoves) {
            const Position::Metadata md = current.pos.makeMove(move);
            depthCounts[current.depth + 1]++;
            q.push({current.pos, current.depth + 1});
            current.pos.unmakeMove(move, md);
        }
    }

    for (int d = 1; d <= maxDepth; d++) {
        std::cout << ";D" << d << " " << depthCounts[d] << " ";
    }

    std::cout << std::endl;
}

void Engine::countPositions(std::shared_ptr<Position> pos, int depth) const {
    uint64_t total = 0;
    uint64_t captures = 0;
    uint64_t eps = 0;
    uint64_t castles = 0;
    uint64_t promotions = 0;
    uint64_t checks = 0;
    uint64_t checkmates = 0;

    std::function<uint64_t(Position &, int)> countPositionsHelper = [&](Position &state,
                                                                        int depth) -> uint64_t {
        // TODO: Also check if this state is terminal
        if (depth == 0) {
            return 1;  // Base case: count this position
        }

        uint64_t count = 0;
        std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

        std::cout << legalMoves.size() << " legal moves\n";
        for (const Move &m : legalMoves) {
            std::cout << m.toString() << "\n";
        }

        for (const Move &move : legalMoves) {
            const Piece capturedPiece = state.pieceAt(move.getToSquare());
            const Position::Metadata md = state.makeMove(move);

            count += countPositionsHelper(state, depth - 1);
            total += count;  // Accumulate total count

            if (capturedPiece != NO_PIECE) {
                captures++;
            }
            if (move.isCastles()) {
                castles++;
            }
            if (move.isEnPassant()) {
                eps++;
            }
            if (move.isPromotion()) {
                promotions++;
            }
            // TODO:
            // if (state.isCheck()) {
            //     checks++;
            //     if (state.isTerminal()) {
            //         checkmates++;
            //     }
            // }

            state.unmakeMove(move, md);
        }

        return count;
    };

    total = countPositionsHelper(*pos, depth);  // Start recursion

    std::cout << "Total positions: " << total << "\n";
    std::cout << "Total captures: " << captures << "\n";
    std::cout << "Total en passants: " << eps << "\n";
    std::cout << "Total castles: " << castles << "\n";
    std::cout << "Total promotions: " << promotions << "\n";
    std::cout << "Total checks: " << checks << "\n";
    std::cout << "Total checkmates: " << checkmates << "\n" << std::endl;
}

eval_t Engine::rateMove(std::shared_ptr<Position> pos, const Move &move) {
    Piece movedPiece = pos->pieceAt(move.getFromSquare());
    Piece capturedPiece = pos->pieceAt(move.getToSquare());

    // capture moves are promising
    eval_t rating = 0;
    if (capturedPiece != NO_PIECE) {
        rating += pieceValues[capturedPiece] - pieceValues[movedPiece];
    }

    // pawn promotion moves are likely to be good
    if (move.isPromotion()) {
        rating += pieceValues[move.getPromotedPieceType()] - pieceValues[movedPiece];
    }

    // moves that put the opponent in check should be checked early
    /*
    pos->makeMove(move);
    if (pos->isCheck()) {
        rating += 10;
    }
    */

    return rating;
}

eval_t Engine::evaluate(const std::shared_ptr<Position> pos) {
    return evaluate(pos, MoveGenerator::generateLegal(pos));
}

eval_t Engine::evaluate(const std::shared_ptr<Position> pos, const std::vector<Move> &&legalMoves) {
    // TODO: Check if checkmate
    if (legalMoves.empty()) {
        return pos->getSideToMove() == WHITE ? 1738 : -1738;
    }

    const eval_t piecePositionWeight = 0.5;

    eval_t score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Piece p : WHITE_PIECES) {
        uint64_t pieces = pos->getPieceBB(p);
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

    for (Piece p : BLACK_PIECES) {
        uint64_t pieces = pos->getPieceBB(p);
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
    return pos->getSideToMove() == WHITE ? score : -score;
}

eval_t Engine::negamax(std::shared_ptr<Position> pos, eval_t alpha, eval_t beta, int depth) {
    uint64_t h = pos->hash();
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
    if (depth >= maxDepth /* || pos->isTerminal() */) {
        return evaluate(pos);
    }

    std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

    eval_t bestEval = std::numeric_limits<eval_t>::lowest();

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos->makeMove(move);

        eval_t eval = -negamax(pos, -beta, -alpha, depth + 1);

        // save result in transposition table
        TTEntry newEntry{eval, depth, TTEntry::EXACT};
        if (eval <= alpha) {
            newEntry.flag = TTEntry::UPPERBOUND;
        }
        if (eval >= beta) {
            newEntry.flag = TTEntry::LOWERBOUND;
        }
        transpositionTable[h] = newEntry;

        pos->unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    return bestEval;
}

eval_t Engine::iterativeDeepening(std::shared_ptr<Position> pos) {
    eval_t bestEval = std::numeric_limits<eval_t>::lowest();

    struct Node {
        Position pos;
        int depth;
    };

    std::queue<Node> q;
    q.push(Node{*pos, 0});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

        // evaluate terminal nodes
        if (current.depth >= maxDepth /* || current.state.isTerminal() */) {
            eval_t evalScore = evaluate(std::make_shared<Position>(current.pos), {});
            if (evalScore > bestEval) {
                bestEval = evalScore;
            }
        }

        // evaluate non-terminal nodes
        for (const Move &move : legalMoves) {
            Position nextState = current.pos;
            nextState.makeMove(move);
            q.push({nextState, current.depth + 1});
        }
    }

    return bestEval;
}

Move Engine::getMove(std::shared_ptr<Position> pos, std::vector<Move> &&legalMoves) {
    if (legalMoves.empty()) {
        return Move();
    }

    // sort moves by how promising they seem
    auto cmp = [&](const Move &a, const Move &b) { return rateMove(pos, a) > rateMove(pos, b); };

    std::sort(legalMoves.begin(), legalMoves.end(), cmp);

    std::cout << "Analyzing moves..." << std::endl;

    eval_t bestEval = std::numeric_limits<eval_t>::lowest();
    Move bestMove;

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos->makeMove(move);

        // state.board.prettyPrint();
        std::cout << "  " << move.toString() << std::endl;

        eval_t eval = -negamax(pos, std::numeric_limits<eval_t>::lowest(),
                               std::numeric_limits<eval_t>::max(), 0);

        std::cout << "\teval = " << eval << std::endl;
        pos->unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
