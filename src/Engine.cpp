#include "Engine.h"

#include "MoveGenerator.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>
#include <queue>

Engine::Engine(int depth)
    : maxDepth(depth) {}

Eval Engine::rateMove(Position &pos, const Move &move) {
    Eval rating = 0;
    const PieceType movedPT = pieceToPT(pos.pieceAt(move.getFromSquare()));
    const PieceType capturedPT = pieceToPT(pos.pieceAt(move.getToSquare()));

    // MVV-LVA
    rating += pieceValues[capturedPT] - pieceValues[movedPT];

    // pawn promotion moves are likely to be good
    if (move.isPromotion()) {
        rating += 50 + pieceValues[move.getPromotedPieceType()] - pieceValues[movedPT];
    }

    // moves that put the opponent in check should also be checked early
    Position::Metadata prevMD = pos.makeMove(move);
    if (PositionUtil::isCheck(pos)) {
        rating += 10;
    }
    pos.unmakeMove(move, prevMD);

    return rating;
}

Eval Engine::evaluate(Position &pos) {
    return evaluate(pos, MoveGenerator::generateLegal(pos));
}

Eval Engine::evaluate(Position &pos, const std::vector<Move> &&legalMoves) {
    if (PositionUtil::isCheckmate(pos)) {
        return pos.getSideToMove() == WHITE ? 1738 : -1738;
    }

    const float piecePositionWeight = 0.5;

    Eval score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Piece p : WHITE_PIECES) {
        uint64_t pieces = pos.getPieceBB(p);
        while (pieces) {
            Square sq = Square(getLsbIndex(pieces));
            int file = fileOf(sq), rank = rankOf(sq);
            score += (pieceValues[p] + piecePositionWeight * pieceSquareTables[p][rank][file]);
            pieces &= pieces - 1;
        }
    }

    for (Piece p : BLACK_PIECES) {
        uint64_t pieces = pos.getPieceBB(p);
        while (pieces) {
            Square sq = Square(getLsbIndex(pieces));
            int file = fileOf(sq), rank = rankOf(sq);
            score -= (pieceValues[p] + piecePositionWeight * pieceSquareTables[p][rank][file]);
            pieces &= pieces - 1;
        }
    }

    // mobility bonus
    score += 0.1 * legalMoves.size();

    // return score relative to the current player for negamax
    return pos.getSideToMove() == WHITE ? score : -score;
}

Eval Engine::negamax(Position &pos, Eval alpha, Eval beta, int depth) {
    Eval alphaOrig = alpha;
    uint64_t h = pos.getHash();
    int remainingDepth = maxDepth - depth;
    if (transpositionTable.find(h) != transpositionTable.end()) {
        const TTEntry &e = transpositionTable[h];
        if (e.depth >= remainingDepth) {
            if (e.flag == TTEntry::EXACT) {
                return e.eval;
            }
            if (e.flag == TTEntry::LOWERBOUND) {
                alpha = std::max(alpha, e.eval);
            }
            if (e.flag == TTEntry::UPPERBOUND) {
                beta = std::max(beta, e.eval);
            }
            if (alpha >= beta) {
                return e.eval;
            }
        }
    }

    // base case
    if (depth >= maxDepth || PositionUtil::isTerminal(pos)) {
        return evaluate(pos);
    }

    std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

    Eval bestEval = std::numeric_limits<Eval>::lowest();

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        Eval eval = -negamax(pos, -beta, -alpha, depth + 1);

        pos.unmakeMove(move, md);

        bestEval = std::max(bestEval, eval);
        alpha = std::max(alpha, eval);

        if (alpha >= beta) {
            break;
        }
    }

    // save result in transposition table
    TTEntry entry;
    entry.eval = bestEval;
    entry.depth = remainingDepth;
    if (bestEval <= alphaOrig) {
        entry.flag = TTEntry::UPPERBOUND;
    } else if (bestEval >= beta) {
        entry.flag = TTEntry::LOWERBOUND;
    } else {
        entry.flag = TTEntry::EXACT;
    }
    transpositionTable[h] = entry;

    return bestEval;
}

Eval Engine::iterativeDeepening(Position &pos) {
    Eval bestEval = std::numeric_limits<Eval>::lowest();

    struct Node {
        Position pos;
        int depth;
    };

    std::queue<Node> q;
    q.push(Node{pos, 0});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        std::vector<Move> legalMoves = MoveGenerator::generateLegal(pos);

        // evaluate terminal nodes
        if (current.depth >= maxDepth || PositionUtil::isTerminal(current.pos)) {
            Eval evalScore = evaluate(current.pos, {});
            if (evalScore > bestEval) {
                bestEval = evalScore;
            }
        }

        // evaluate non-terminal nodes
        for (const Move &move : legalMoves) {
            Position nextPos = current.pos;
            nextPos.makeMove(move);
            q.push({nextPos, current.depth + 1});
        }
    }

    return bestEval;
}

Move Engine::getMove(Position &pos, std::vector<Move> &legalMoves) {
    if (legalMoves.empty()) {
        return Move::none();
    }

    // sort moves by how promising they seem
    auto cmp = [&](const Move &a, const Move &b) { return rateMove(pos, a) > rateMove(pos, b); };

    std::sort(legalMoves.begin(), legalMoves.end(), cmp);

    std::cout << "Analyzing moves..." << std::endl;

    Eval bestEval = std::numeric_limits<Eval>::lowest();
    Move bestMove;

    for (const Move &move : legalMoves) {
        const Position::Metadata md = pos.makeMove(move);

        std::cout << "  " << move.toString() << std::endl;

        Eval eval =
            -negamax(pos, std::numeric_limits<Eval>::lowest(), std::numeric_limits<Eval>::max(), 0);

        std::cout << "\teval = " << eval << std::endl;
        pos.unmakeMove(move, md);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }
    return bestMove;
}
