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

enum GameStatus {
    IN_PROGRESS,
    CHECKMATE,
    DRAW_BY_REPETITION,
    DRAW_BY_STALEMATE,
    DRAW_BY_INSUFFICIENT_MATERIAL,
    DRAW_BY_50_MOVE_RULE
};

/**
 * Conveys the player's pieces during game initialization
 */
enum Color { WHITE, BLACK };

constexpr std::array<Color, 2> COLORS = {WHITE, BLACK};

constexpr std::array<std::string_view, 2> COLOR_NAMES = {"White", "Black"};

constexpr std::array<int, 2> SIGN = {1, -1};

inline constexpr Color otherColor(Color c) {
    return Color(c ^ 1);
}

/**
 * Represents each type of piece without a color
 */
enum PieceType { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PT };

constexpr std::array<PieceType, 6> PIECE_TYPES = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

constexpr std::array<PieceType, 4> PROMOTION_PIECE_TYPES = {KNIGHT, BISHOP, ROOK, QUEEN};

constexpr std::array<std::string_view, 6> SAN_PTS = {"P", "N", "B", "R", "Q", "K"};

/**
 * Represents each possible piece, including color
 */
enum Piece { WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK, NO_PIECE };

inline constexpr Piece ptToPiece(PieceType pt, Color side) {
    return Piece(pt + 6 * side);
}

inline constexpr PieceType pieceToPT(Piece p) {
    assert(p != NO_PIECE);
    return PieceType(p % 6);
}

inline constexpr Color pieceColor(Piece p) {
    assert(p != NO_PIECE);
    return Color(p >= 6);
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

constexpr std::array<char, NO_PIECE> PIECE_FEN_CHARS = {'P', 'N', 'B', 'R', 'Q', 'K',
                                                        'p', 'n', 'b', 'r', 'q', 'k'};

/**
 * Maps FEN character to PieceType enum
 */
inline constexpr Piece fenCharToPiece(char c) {
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

constexpr Eval INFINITY = pieceTypeValues[KING];

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

constexpr std::array<Square, NO_SQ> ALL_SQUARES = {
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

inline constexpr Bitboard ALL_SQUARES_BB = 0xFFFFFFFFFFFFFFFF;

// x and y are 0-indexed, where (0, 0) is the top-left square (a8)
inline constexpr Square xyToSquare(int x, int y) {
    return Square((y << 3) + x);
}

// File Constants
enum File : int { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };

constexpr Bitboard FILE_MASKS[8] = {
    72340172838076673ull,    // A
    144680345676153346ull,   // B
    289360691352306692ull,   // C
    578721382704613384ull,   // D
    1157442765409226768ull,  // E
    2314885530818453536ull,  // F
    4629771061636907072ull,  // G
    9259542123273814144ull   // H
};

constexpr Bitboard notA = ~FILE_MASKS[FILE_A];
constexpr Bitboard notAB = ~(FILE_MASKS[FILE_A] | FILE_MASKS[FILE_B]);
constexpr Bitboard notH = ~FILE_MASKS[FILE_H];
constexpr Bitboard notGH = ~(FILE_MASKS[FILE_G] | FILE_MASKS[FILE_H]);

// Rank Constants
enum Rank : int { RANK_8, RANK_7, RANK_6, RANK_5, RANK_4, RANK_3, RANK_2, RANK_1 };

constexpr Bitboard RANK_MASKS[8] = {
    255ull,                  // 8
    65280ull,                // 7
    16711680ull,             // 6
    4278190080ull,           // 5
    1095216660480ull,        // 4
    280375465082880ull,      // 3
    71776119061217280ull,    // 2
    18374686479671623680ull  // 1
};

constexpr Bitboard not1 = ~(RANK_MASKS[RANK_1]);
constexpr Bitboard not12 = ~(RANK_MASKS[RANK_1] | RANK_MASKS[RANK_2]);
constexpr Bitboard not8 = ~(RANK_MASKS[RANK_8]);
constexpr Bitboard not78 = ~(RANK_MASKS[RANK_7] | RANK_MASKS[RANK_8]);

inline constexpr File fileOf(Square sq) {
    return File(sq & 7);  // same as sq % 8
}

inline constexpr Rank rankOf(Square sq) {
    return Rank(sq >> 3);  // same as sq / 8
}

// input is assumed to be in a2a4 format
inline Square coordinateStringToSquare(const std::string &str) {
    const int fileIndex = str[0] - 'a';
    const int rankIndex = '8' - str[1];
    return xyToSquare(fileIndex, rankIndex);
}

inline std::string squareToCoordinateString(const Square &sq) {
    const char fileSymbol = 'a' + fileOf(sq);
    const char rankSymbol = '8' - rankOf(sq);
    return std::string {fileSymbol, rankSymbol};
}

// Diagonal Masks

/* 0 0 0 0 1
 * 0 0 0 1 0
 * 0 0 1 0 0
 * 0 1 0 0 0
 * 1 0 0 0 0
 */
constexpr Bitboard SLASH_DIAGONAL_MASKS[15] = {
    1ull,                    // a8
    258ull,                  // a7, b8
    66052ull,                // a6...c8
    16909320ull,             // a5...d8
    4328785936ull,           // a4...e8
    1108169199648ull,        // a3...f8
    283691315109952ull,      // a2...g8
    72624976668147840ull,    // a1...h8
    145249953336295424ull,   // b1...h7
    290499906672525312ull,   // c1...h6
    580999813328273408ull,   // d1...h5
    1161999622361579520ull,  // e1...h4
    2323998145211531264ull,  // f1...h3
    4647714815446351872ull,  // g1, h2
    9223372036854775808ull   // h1
};

// N.B: Consider precomputing these indices for each square
inline constexpr int getSlashDiagonalIndex(Square sq) {
    return rankOf(sq) + fileOf(sq);
}

/* 1 0 0 0 0
 * 0 1 0 0 0
 * 0 0 1 0 0
 * 0 0 0 1 0
 * 0 0 0 0 1
 */
constexpr Bitboard BACKSLASH_DIAGONAL_MASKS[15] = {
    72057594037927936ull,    // a1
    144396663052566528ull,   // a2, b1
    288794425616760832ull,   // a3...c1
    577588855528488960ull,   // a4...d1
    1155177711073755136ull,  // a5...e1
    2310355422147575808ull,  // a6...f1
    4620710844295151872ull,  // a7...g1
    9241421688590303745ull,  // a8...h1
    36099303471055874ull,    // b8...h2
    141012904183812ull,      // c8...h3
    550831656968ull,         // d8...h4
    2151686160ull,           // e8...h5
    8405024ull,              // f8...h6
    32832ull,                // g8, h7
    128ull,                  // h8
};

// N.B: Consider precomputing these indices for each square
inline constexpr int getBackslashDiagonalIndex(Square sq) {
    return 7 - rankOf(sq) + fileOf(sq);
}

// Returns a "ray", i.e. a line that overlaps both squares
// NTS: Is this better than simply trying all 4 rays?
[[maybe_unused]]
inline constexpr Bitboard findOverlapRay(Square sq1, Square sq2) {
    const Rank r1 = rankOf(sq1), r2 = rankOf(sq2);
    const File f1 = fileOf(sq1), f2 = fileOf(sq2);
    if (r1 == r2) {
        return RANK_MASKS[r1];
    } else if (f1 == f2) {
        return FILE_MASKS[f1];
    } else if (r2 - r1 == f2 - f1) {
        return BACKSLASH_DIAGONAL_MASKS[getBackslashDiagonalIndex(sq1)];
    } else {
        return SLASH_DIAGONAL_MASKS[getSlashDiagonalIndex(sq1)];
    }
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

inline constexpr Direction otherDir(Direction dir) {
    return Direction(-dir);
}

inline constexpr Bitboard shift(Bitboard bb, Direction dir) {
    switch (dir) {
        case NORTH: return bb >> 8;
        case SOUTH: return bb << 8;
        case EAST: return (bb & notH) << 1;
        case WEST: return (bb & notA) >> 1;
        case NORTH_EAST: return (bb & notH) >> 7;
        case NORTH_WEST: return (bb & notA) >> 9;
        case SOUTH_EAST: return (bb & notH) << 9;
        case SOUTH_WEST: return (bb & notA) << 7;
        default: return 0ull;  // unreachable
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

inline constexpr bool hasCastleRights(const CastleRights &current, CastleRights query) {
    return current & query;
}

// only used in FEN parsing
inline constexpr void addCastleRights(CastleRights &current, CastleRights toAdd) {
    current = CastleRights(current | toAdd);
}

inline constexpr void removeCastleRights(CastleRights &current, CastleRights toRemove) {
    current = CastleRights(current & ~toRemove);
}

// file paths and GUI Constants
constexpr std::string_view PGN_OUTPUT_PATH = "../games/";
constexpr std::string_view DEFAULT_OUT_FILE = "../games/recent.txt";
constexpr std::string_view SFX_PATH = "../src/assets/sfx/";
constexpr std::string_view PIECE_TEXTURE_PATH = "../src/assets/pieces/";
constexpr std::string_view BOARD_TEXTURE_FILE = "../src/assets/board.png";
constexpr std::string_view DEFAULT_PIECE_THEME = "horsey";
constexpr int BOARD_SIZE = 512;
constexpr int TILE_SIZE = 64;
