#include "Engine.h"

#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

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
    const Piece capturedPiece = pos.pieceAt(move.getToSquare());
    const PieceType capturedPT = capturedPiece == NO_PIECE ? NO_PT : pieceToPT(capturedPiece);

    // MVV-LVA
    rating += pieceTypeValues[capturedPT] - pieceTypeValues[movedPT];

    // pawn promotion moves are likely to be good
    if (move.isPromotion()) {
        rating += 50 + pieceTypeValues[move.getPromotedPieceType()] - pieceTypeValues[movedPT];
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
    const int sign = SIGN[pos.getSideToMove()];
    if (PositionUtil::isCheckmate(pos)) {
        return -sign * pieceTypeValues[KING];
    }

    // need to fine-tune this
    const float piecePositionWeight = 0.05;

    Eval score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Square sq : ALL_SQUARES) {
        const Piece p = pos.pieceAt(sq);
        if (p == NO_PIECE) {
            continue;
        }
        const Eval value =
            pieceTypeValues[pieceToPT(p)] + piecePositionWeight * pieceSquareTables[p][sq];
        const Color c = pieceColor(p);
        score += SIGN[c] * value;
    }

    // mobility bonus
    score += 0.1 * legalMoves.size();

    // return score relative to the current player for negamax
    return sign * score;
}

Eval Engine::getEval(Position &pos) {
    const int sign = SIGN[pos.getSideToMove()];
    if (PositionUtil::isCheckmate(pos)) {
        return -sign * pieceTypeValues[KING];
    }

    // need to fine-tune this
    const float piecePositionWeight = 0.05;

    Eval score = 0;

    // total up pieces, scaling by piece value and position in piece-square
    // board
    for (Square sq : ALL_SQUARES) {
        const Piece p = pos.pieceAt(sq);
        if (p == NO_PIECE) {
            continue;
        }
        const Color c = pieceColor(p);
        Eval value = pieceTypeValues[pieceToPT(p)] + piecePositionWeight * pieceSquareTables[p][sq];
        score += (c == WHITE ? value : -value);

        printf("\npieceValues[%s] = %d\n", PIECE_FILENAMES[p].data(), pieceTypeValues[p]);
        printf("piecePositionWeight = %.1f\n", piecePositionWeight);
        printf("pieceSquareTables[%s][%s] = %d\n", PIECE_FILENAMES[p].data(),
               squareToCoordinateString(sq).c_str(), pieceSquareTables[p][sq]);
        printf("value = %d\n", value);
        printf("score = %d\n", score);
    }

    // mobility bonus
    // score += 0.1 * legalMoves.size();

    // return score relative to the current player for negamax
    return sign * score;
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
