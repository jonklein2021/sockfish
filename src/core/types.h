#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>

/*

    -- Bitboard Cheatsheet --

    y increasing
    |
    v   8   R N B Q K B N R
    |   7   P P P P P P P P
    v   6   . . . . . . . .
    |   5   . . . . . . . .
    v   4   . . . . . . . .
    |   3   . . . . . . . .
    v   2   p p p p p p p p
    |   1   r n b q k b n r
    v
            a b c d e f g h

    x increasing ->->->->->

    a8 is the 0th bit, h8 is the 7th bit
    a1 is the 56th bit, h1 is the 63rd bit

    Move one square up: x >> 8
    Move one square down: x << 8
    Move one square left: (x & not_file_a) >> 1
    Move one square right: (x & not_file_h) << 1

*/
using Bitboard = uint64_t;

using Eval = int;

/**
 * Conveys the player's pieces during game initialization
 */
enum Color { WHITE, BLACK };

constexpr std::array<Color, 2> COLORS = {WHITE, BLACK};

constexpr std::array<std::string_view, 2> COLOR_NAMES = {"White", "Black"};

constexpr std::array<int, 2> SIGN = {1, -1};

inline static constexpr Color otherColor(Color c) {
    return Color(c ^ 1);
}

/**
 * Represents each type of piece without a color
 */
enum PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PT };

constexpr std::array<PieceType, 6> PIECE_TYPES = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

constexpr std::array<PieceType, 4> PROMOTION_PIECE_TYPES = {KNIGHT, BISHOP, ROOK, QUEEN};

/**
 * Represents each possible piece, including color
 */
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK, NO_PIECE };

inline static constexpr Piece ptToPiece(PieceType pt, Color side) {
    return Piece(pt + 6 * side);
}

inline static constexpr PieceType pieceToPT(Piece p) {
    assert(p != NO_PIECE);
    return PieceType(p % 6);
}

inline static constexpr Color pieceColor(Piece p) {
    assert(p != NO_PIECE);
    return Color(p / 6);
}

constexpr std::array<Piece, 6> WHITE_PIECES = {WP, WN, WB, WR, WQ, WK};

constexpr std::array<Piece, 6> BLACK_PIECES = {BP, BN, BB, BR, BQ, BK};

constexpr std::array<Piece, 12> ALL_PIECES = {WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK};

constexpr std::array<std::array<Piece, 6>, 2> COLOR_TO_PIECES = {WHITE_PIECES, BLACK_PIECES};

/**
 * List of all piece filenames, mainly used for loading textures
 */
constexpr std::array<std::string_view, 12> PIECE_FILENAMES = {"wP", "wN", "wB", "wR", "wQ", "wK",
                                                              "bP", "bN", "bB", "bR", "bQ", "bK"};

/**
 * Unicode pieces for pretty printing
 */
constexpr std::array<std::string_view, 13> PIECE_UNICODE_SYMBOLS = {
    "♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚", "."};

/**
 * Mainly used for debug output
 */
constexpr std::array<std::string_view, 13> PIECE_NAMES = {
    "White Pawn",  "White Knight", "White Bishop", "White Rook",   "White Queen",
    "White King",  "Black Pawn",   "Black Knight", "Black Bishop", "Black Rook",
    "Black Queen", "Black King",   "None"};

/**
 * Mainly used for debug output
 */
constexpr std::array<std::string_view, 7> PIECE_TYPE_NAMES = {"Pawn",  "Knight", "Bishop", "Rook",
                                                              "Queen", "King",   "None"};

constexpr std::array<char, NO_PIECE> pieceToFenChar = {'P', 'N', 'B', 'R', 'Q', 'K',
                                                       'p', 'n', 'b', 'r', 'q', 'k'};

/**
 * Maps FEN character to PieceType enum
 */
constexpr Piece fenCharToPiece(char c) {
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
        default: return NO_PIECE;
    }
}

/**
 * List of all possible promotion pieces
 */
constexpr std::array<Piece, 4> WHITE_PROMOTION_PIECES = {WQ, WR, WB, WN};
constexpr std::array<Piece, 4> BLACK_PROMOTION_PIECES = {BQ, BR, BB, BN};

/**
 * FEN string for the starting position
 */
constexpr std::string_view STARTING_POSITION_FEN =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * Piece values for material evaluation
 * PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE
 */
constexpr std::array<Eval, 7> pieceTypeValues = {100, 300, 320, 500, 900, 500000, 0};

// Bitboard Constants
constexpr Bitboard NOT_FILE_A = 18374403900871474942ull;
constexpr Bitboard NOT_FILE_H = 9187201950435737471ull;
constexpr Bitboard NOT_FILE_GH = 4557430888798830399ull;
constexpr Bitboard NOT_FILE_AB = 18229723555195321596ull;
constexpr Bitboard NOT_RANK_1 = 72057594037927935ull;
constexpr Bitboard NOT_RANK_1_NOR_2 = 281474976710655ull;
constexpr Bitboard NOT_RANK_8 = 18446744073709551360ull;
constexpr Bitboard NOT_RANK_7_NOR_8 = 18446744073709486080ull;
constexpr Bitboard RANK_1 = 18374686479671623680ull;
constexpr Bitboard RANK_2 = 71776119061217280ull;
constexpr Bitboard RANK_4 = 1095216660480ull;
constexpr Bitboard RANK_5 = 4278190080ull;
constexpr Bitboard RANK_7 = 65280ull;
constexpr Bitboard RANK_8 = 255ull;

// clang-format off
enum Square : uint8_t {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,

    NO_SQ
};

inline static constexpr int fileOf(Square sq) {
    return sq & 7;
}

inline static constexpr int rankOf(Square sq) {
    return sq >> 3;
}

constexpr std::array<Square, 64> ALL_SQUARES = {
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
};
// clang-format on

// x and y are 0-indexed, where (0, 0) is the top-left square (a8)
inline static constexpr Square xyToSquare(int x, int y) {
    return Square(y * 8 + x);
}

// input is assumed to be in a2a4 format
static inline Square coordinateStringToSquare(const std::string &str) {
    const int fileIndex = str[0] - 'a';
    const int rankIndex = '8' - str[1];
    return xyToSquare(fileIndex, rankIndex);
}

static inline std::string squareToCoordinateString(const Square &sq) {
    const char file = 'a' + (sq % 8);
    const char rank = '8' - (sq / 8);
    return std::string{file, rank};
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

inline constexpr Bitboard shift(Bitboard bb, Direction dir) {
    switch (dir) {
        case NORTH: return bb << 8;
        case SOUTH: return bb >> 8;
        case EAST: return (bb & NOT_FILE_H) << 1;
        case WEST: return (bb & NOT_FILE_A) >> 1;
        case NORTH_EAST: return (bb & NOT_FILE_H) << 9;
        case NORTH_WEST: return (bb & NOT_FILE_A) << 7;
        case SOUTH_EAST: return (bb & NOT_FILE_H) >> 7;
        case SOUTH_WEST: return (bb & NOT_FILE_A) >> 9;
    }
}

// Credit:
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
    CASTLING_SZ                                      // 0b00010000
};

inline static constexpr bool hasCastleRights(const CastleRights &current, CastleRights query) {
    return current & query;
}

// only used in FEN parsing
inline static constexpr void addCastleRights(CastleRights &current, CastleRights toAdd) {
    current = CastleRights(current | toAdd);
}

inline static constexpr void removeCastleRights(CastleRights &current, CastleRights toRemove) {
    current = CastleRights(current & ~toRemove);
}

// file paths and GUI Constants
constexpr std::string_view PGN_OUTPUT_PATH = "../games/";
constexpr std::string_view DEFAULT_OUT_FILE = "../games/recent.txt";
constexpr std::string_view PIECE_TEXTURE_PATH = "../src/assets/pieces/";
constexpr std::string_view BOARD_TEXTURE_PATH = "../src/assets/board.png";
constexpr std::string_view DEFAULT_PIECE_THEME = "horsey";
constexpr int BOARD_SIZE = 512;
constexpr int TILE_SIZE = 64;
