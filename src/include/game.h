#include <cstdint>
#include <string>

#include <SFML/Graphics.hpp>

/**
 * Stores the current state of the game in bitboard
 * representation. Each uint64_t represents the
 * instances of a certain piece on the board.
 */
struct BitBoard {
    uint64_t wp, wn, wb, wr, wq, wk; // positions of white pieces
    uint64_t bp, bn, bb, br, bq, bk; // positions of black pieces
    BitBoard() : wp(0), wn(0), wb(0), wr(0), wq(0), wk(0),
                 bp(0), bn(0), bb(0), br(0), bq(0), bk(0) {}
};

/**
 * Moves are represented as a pair of two 2D vectors, where the
 * first vector is where the piece starts and the second vector
 * is where the piece ends up after the move.
 */
struct Move {
    sf::Vector2<int> from;
    sf::Vector2<int> to;
    bool isCapture;
    bool isCastle;
    char promotionPiece;
    bool isEnPassant;
    Move() : from({0, 0}), to({0, 0}), isCapture(false), isCastle(false), promotionPiece(' '), isEnPassant(false) {}
    Move(sf::Vector2<int> from, sf::Vector2<int> to, bool isCapture, bool isCastle, char promotionPiece, bool isEnPassant)
        : from(from), to(to), isCapture(isCapture), isCastle(isCastle), promotionPiece(promotionPiece), isEnPassant(isEnPassant) {}
};

/**
 * The game state includes the current position
 * whose turn it is, if each side can castle, etc.
 */
struct GameState {
    BitBoard board;
    bool whiteToMove;
    bool whiteKingMoved;
    bool blackKingMoved;
    bool whiteRookAMoved;
    bool whiteRookHMoved;
    bool blackRookAMoved;
    bool blackRookHMoved;
};

/**
 * Converts a FEN string to a BitBoard
 * 
 * Only used in the initialization of the game
 * 
 * @param fen the FEN string to convert
 * @return the BitBoard representation of the FEN string
 */
BitBoard fenToBitBoard(const std::string& fen);

/**
 * Converts a BitBoard to a FEN string
 * 
 * Can be used to export the game state
 * 
 * @param board the BitBoard to convert
 * @return the FEN string representation of the BitBoard
 */
std::string bitBoardToFen(const BitBoard& board);
