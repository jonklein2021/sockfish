#include <cstdint>
#include <string>

#include <SFML/Graphics.hpp>

/**
 * Represents each the type of piece;
 * 
 * Useful for simplifying moves
 */
enum PieceType {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

/**
 * Stores the current state of the game in bitboard
 * representation. Each uint64_t represents the
 * instances of a certain piece on the board.
 */
struct BitBoard {
    uint64_t pieceBits[12]; // index into this with PieceType
    BitBoard() : pieceBits{0} {}
};

/**
 * Moves are represented as a pair of two 2D vectors, where the
 * first vector is where the piece starts and the second vector
 * is where the piece ends up after the move.
 */
struct Move {
    sf::Vector2<int> from;
    sf::Vector2<int> to;
    PieceType pieceMoved;
    bool isCapture;
    bool isCastle;
    bool isEnPassant;
    char promotionPiece; // nonempty only for pawn promotion
    Move() : from({0, 0}), to({0, 0}), isCapture(false), isCastle(false), isEnPassant(false),  promotionPiece(' ') {}
    Move(sf::Vector2<int> from, sf::Vector2<int> to, bool isCapture, bool isCastle, bool isEnPassant, char promotionPiece)
        : from(from), to(to), isCapture(isCapture), isCastle(isCastle), isEnPassant(isEnPassant), promotionPiece(promotionPiece) {}
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

    // Constructors
    GameState() : board(), whiteToMove(true), whiteKingMoved(false), blackKingMoved(false),
                  whiteRookAMoved(false), whiteRookHMoved(false), blackRookAMoved(false), blackRookHMoved(false) {}
    GameState(BitBoard board, bool whiteToMove, bool whiteKingMoved, bool blackKingMoved,
              bool whiteRookAMoved, bool whiteRookHMoved, bool blackRookAMoved, bool blackRookHMoved)
        : board(board), whiteToMove(whiteToMove), whiteKingMoved(whiteKingMoved), blackKingMoved(blackKingMoved),
          whiteRookAMoved(whiteRookAMoved), whiteRookHMoved(whiteRookHMoved), blackRookAMoved(blackRookAMoved), blackRookHMoved(blackRookHMoved) {}
    
    // Methods
    void ApplyMove(const Move& move);
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

/**
 * Return a list of all legal moves in some
 * game state
 * 
 * @param state the current game state
 * @return a vector of all possible moves
 */
std::vector<Move> generateMoves(const GameState& state);
