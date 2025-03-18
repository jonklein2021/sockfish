#pragma once

#include <string>
#include <vector>

#define PIECE_TEXTURE_PATH "assets/pieces/"
#define BOARD_TEXTURE_PATH "assets/board.png"

#define BOARD_PIXEL_SIZE 512 // board is 512x512 pixels
#define TILE_PIXEL_SIZE 64 // 512/8 = 64

// piece & position details
const std::vector<std::string> pieceFilenames = {
    "wP", "wN", "wB", "wR", "wQ", "wK",
    "bP", "bN", "bB", "bR", "bQ", "bK"
};

const std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

const std::vector<std::pair<int, int>> pawnCaptures = {{1, 1}, {-1, 1}};
const std::vector<std::pair<int, int>> knightMoves = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
const std::vector<std::pair<int, int>> bishopMoves = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> rookMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
const std::vector<std::pair<int, int>> queenMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
const std::vector<std::pair<int, int>> kingMoves = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

