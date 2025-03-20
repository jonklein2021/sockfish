#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "PieceType.h"

#define PIECE_TEXTURE_PATH "assets/pieces/"
#define BOARD_TEXTURE_PATH "assets/board.png"

#define BOARD_PIXEL_SIZE 512
#define TILE_PIXEL_SIZE 64

/**
 * List of all piece filenames, mainly used for loading textures
 * but also helpful for nice debug output
 */
const std::vector<std::string> pieceFilenames = {
    "wP", "wN", "wB", "wR", "wQ", "wK",
    "bP", "bN", "bB", "bR", "bQ", "bK"
};


/**
 * Map from FEN character to PieceType, used in FEN
 * conversion methods
 */
const std::unordered_map<char, PieceType> fenPieceMap = {
    {'P', WP}, {'N', WN}, {'B', WB}, {'R', WR}, {'Q', WQ}, {'K', WK},
    {'p', BP}, {'n', BN}, {'b', BB}, {'r', BR}, {'q', BQ}, {'k', BK}
};

/**
 * List of all possible promotion pieces
 */
const std::vector<PieceType> promotionPiecesWhite = {WQ, WR, WB, WN};
const std::vector<PieceType> promotionPiecesBlack = {BQ, BR, BB, BN};

/**
 * FEN string for the starting position
 */
const std::string defaultFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

/**
 * List of all possible piece moves
 */
const std::vector<std::pair<int, int>> pawnCaptures = {{1, 1}, {-1, 1}};
const std::vector<std::pair<int, int>> knightMoves = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
const std::vector<std::pair<int, int>> bishopMoves = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> rookMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const std::vector<std::pair<int, int>> queenMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

/**
 * Piece values for material evaluation
 */
const std::vector<double> pieceValues = {1.0, 3.0, 3.0, 5.0, 9.0, 1000.0, 1.0, 3.0, 3.0, 5.0, 9.0, 1000.0};
