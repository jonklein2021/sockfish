#pragma once

#include <array>
#include <cstdint>
#include <cwctype>
#include <string>
#include <string_view>

using Bitboard = uint64_t;

using Eval = int32_t;

/**
 * Conveys the player's pieces during game initialization
 */
enum Color { WHITE, BLACK };

/**
 * Used to index into the occupancies array
 */
enum OccupancyType { WHITE_OCCUPANCY, BLACK_OCCUPANCY, BOTH_OCCUPANCY, EMPTY_OCCUPANCY };

/**
 * Represents each type of piece without a color
 */
enum PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE };

/**
 * Represents each possible piece, including color
 */
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK, NONE };

/**
 * List of all piece filenames, mainly used for loading textures
 */
constexpr std::array<std::string_view, 12> pieceFilenames = {"wP", "wN", "wB", "wR", "wQ", "wK",
                                                             "bP", "bN", "bB", "bR", "bQ", "bK"};

/**
 * Unicode pieces for pretty printing
 */
constexpr std::array<std::string_view, 13> unicode_pieces = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎",
                                                             "♞", "♝", "♜", "♛", "♚", "."};

/**
 * Mainly used for debug output
 */
constexpr std::array<std::string_view, 13> pieceNames = {
    "White Pawn",  "White Knight", "White Bishop", "White Rook",   "White Queen",
    "White King",  "Black Pawn",   "Black Knight", "Black Bishop", "Black Rook",
    "Black Queen", "Black King",   "None"};

/**
 * Mainly used for debug output
 */
constexpr std::array<std::string_view, 7> pieceTypeNames = {"Pawn",  "Knight", "Bishop", "Rook",
                                                            "Queen", "King",   "None"};

/**
 * Maps FEN character to PieceType enum
 */
constexpr Piece fenToPiece(char c) {
    switch (c) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;
        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
        default: return NONE;
    }
}

/**
 * List of all possible promotion pieces
 */
constexpr std::array<Piece, 4> promotionPiecesWhite = {WQ, WR, WB, WN};
constexpr std::array<Piece, 4> promotionPiecesBlack = {BQ, BR, BB, BN};

/**
 * FEN string for the starting position
 */
constexpr std::string_view defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * Piece move constants
 */
struct Offset {
    int dx, dy;
};

constexpr std::array<Offset, 2> pawnCaptures = {{{1, 1}, {-1, 1}}};
constexpr std::array<Offset, 8> knightMoves = {
    {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}}};
constexpr std::array<Offset, 4> bishopMoves = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
constexpr std::array<Offset, 4> rookMoves = {{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
constexpr std::array<Offset, 8> queenMoves = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};
constexpr std::array<Offset, 8> kingMoves = {
    {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

/**
 * Piece values for material evaluation
 */
constexpr std::array<Eval, 12> pieceValues = {100, 300, 300, 500, 900, 100000,
                                              100, 300, 300, 500, 900, 100000};

// Bitboard Constants
constexpr Bitboard not_file_a = 18374403900871474942ull;
constexpr Bitboard not_file_h = 9187201950435737471ull;
constexpr Bitboard not_file_gh = 4557430888798830399ull;
constexpr Bitboard not_file_ab = 18229723555195321596ull;
constexpr Bitboard not_rank_1 = 72057594037927935ull;
constexpr Bitboard not_rank_12 = 281474976710655ull;
constexpr Bitboard not_rank_8 = 18446744073709551360ull;
constexpr Bitboard not_rank_78 = 18446744073709486080ull;
constexpr Bitboard rank1 = 18374686479671623680ull;
constexpr Bitboard rank2 = 71776119061217280ull;
constexpr Bitboard rank7 = 65280ull;
constexpr Bitboard rank8 = 255ull;

// clang-format off
enum Square : uint8_t {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1
};

// clang-format on

// (0, 0) corresponds to a8, the top-left square
static inline Square coordsToSquare(int x, int y) {
    return Square(y * 8 + x);
}

static inline std::string squareToString(Square sq) {
    return std::to_string('a' + sq % 8) + std::to_string(8 - sq / 8);
}

enum Direction : int8_t {
    NORTH = -8,
    EAST = 1,
    SOUTH = -NORTH,
    WEST = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

// https://github.com/official-stockfish/Stockfish/blob/c109a88ebe93ab7652c7cb4694cfc405568e5e50/src/types.h#L126
enum CastleRights : int8_t {
    NO_CASTLING,                                     // 0b00000000
    WHITE_OO,                                        // 0b00000001
    WHITE_OOO = WHITE_OO << 1,                       // 0b00000010
    BLACK_OO = WHITE_OO << 2,                        // 0b00000100
    BLACK_OOO = WHITE_OO << 3,                       // 0b00001000
    KING_SIDE = WHITE_OO | BLACK_OO,                 // 0b00000101
    QUEEN_SIDE = WHITE_OOO | BLACK_OOO,              // 0b00001010
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,           // 0b00000011
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,           // 0b00001100
    ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING,  // 0b00001111
    CASTLING_RIGHT_NB = 16                           // 0b00010000
};

// GUI Constants
constexpr std::string_view PIECE_TEXTURE_PATH = "../assets/pieces/";
constexpr std::string_view BOARD_TEXTURE_PATH = "../assets/board.png";
constexpr int32_t BOARD_PIXEL_SIZE = 512;
constexpr int32_t TILE_PIXEL_SIZE = 64;
