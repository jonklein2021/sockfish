
#include "Notation.h"

#include "src/core/PositionUtil.h"
#include "src/core/types.h"
#include "src/movegen/MoveGenerator.h"

#include <sstream>

namespace Notation {

bool validateCoords(const std::string &input) {
    if (input.size() != 4) {
        return false;
    }

    return (input[0] >= 'a' && input[0] <= 'h') && (input[1] >= '1' && input[1] <= '8') &&
           (input[2] >= 'a' && input[2] <= 'h') && (input[3] >= '1' && input[3] <= '8');
}

Move coordsToMove(const Position &pos, const std::string &coords) {
    const Square from = coordinateStringToSquare(coords.substr(0, 2));
    const Square to = coordinateStringToSquare(coords.substr(2));

    // check for castle moves
    if (pieceToPT(pos.pieceAt(from)) == KING && MoveGenerator::isCastleMovement(from, to)) {
        return Move::create<Move::CASTLING>(from, to);
    }

    return Move(from, to);
}

std::string moveToCoords(const Move &move) {
    return squareToCoordinateString(move.getFromSquare()) +
           squareToCoordinateString(move.getToSquare());
}

Move uciToMove(const Position &pos, const std::string &uciString) {
    Move m = coordsToMove(pos, uciString);

    if (uciString.length() == 5) {
        m.setPromotedPieceType(pieceToPT(fenCharToPiece(uciString[4])));
    }

    if (pieceToPT(pos.pieceAt(m.getFromSquare())) == PAWN && m.getToSquare() == pos.getEpSquare()) {
        m.setFlag(Move::EN_PASSANT);
    }

    return m;
}

std::string moveToUci(const Move &move) {
    std::string uci = moveToCoords(move);
    if (move.isPromotion()) {
        uci += PIECE_FEN_CHARS[move.getPromotedPieceType() + 6];
    }
    return uci;
}

std::string getSANDisambiguation(const Move &move, Position &pos) {
    const Square from = move.getFromSquare();
    const Square to = move.getToSquare();
    const Piece p = pos.pieceAt(from);

    std::vector<Square> attackers;

    // find moves of this piece that can move to the same square
    std::vector<Move> legalMoves;
    MoveGenerator::generatePtMoves(legalMoves, pos, pieceToPT(p));
    for (const Move &legalMove : legalMoves) {
        const Square src = legalMove.getFromSquare();
        const Square dst = legalMove.getToSquare();

        if (src == from) {
            continue;
        }

        if (dst != to) {
            continue;
        }

        attackers.push_back(src);
    }

    if (attackers.empty()) {
        return "";
    }

    bool sameFile = false;
    bool sameRank = false;

    for (Square s : attackers) {
        if (fileOf(s) == fileOf(from)) {
            sameFile = true;
        }
        if (rankOf(s) == rankOf(from)) {
            sameRank = true;
        }
    }

    if (!sameFile) {
        return std::string(1, 'a' + fileOf(from));
    }

    if (!sameRank) {
        return std::string(1, '1' + rankOf(from));
    }

    return squareToCoordinateString(from);
}

std::string moveToSAN(const Move &move, Position &pos) {
    std::ostringstream san;

    const Square from = move.getFromSquare();
    const Square to = move.getToSquare();
    const PieceType pt = pieceToPT(pos.pieceAt(from));

    // ---- CASTLING ----
    if (move.isCastles()) {
        san << (fileOf(to) == FILE_G ? "O-O" : "O-O-O");
    } else {
        // ---- PIECE LETTER + DISAMBIGUATION ----
        if (pt != PAWN) {
            san << SAN_PTS[pt];
            san << getSANDisambiguation(move, pos);
        }

        // ---- CAPTURE ----
        if (pos.pieceAt(to) != NO_PIECE || move.isEnPassant()) {
            if (pt == PAWN) {
                san << char('a' + fileOf(from));
            }
            san << 'x';
        }

        // ---- DESTINATION ----
        san << squareToCoordinateString(to);

        // ---- PROMOTION ----
        if (move.isPromotion()) {
            san << '=';
            san << SAN_PTS[move.getPromotedPieceType()];
        }
    }

    // ---- CHECK + MATE ----
    const Position::Metadata md = pos.makeMove(move);

    if (PositionUtil::isCheck(pos)) {
        // can't use legalMoves var from earlier because the position has changed
        san << (PositionUtil::isCheckmate(pos) ? "#" : "+");
    }

    // preserve original state
    pos.unmakeMove(move, md);

    return san.str();
}
};  // namespace Notation
