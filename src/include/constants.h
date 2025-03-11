#pragma once

#include <string>
#include <vector>

// size of board and tiles in pixels
const int BOARD_PIXEL_SIZE = 512; // board is 512x512 pixels
const int TILE_PIXEL_SIZE = 64; // 512/8 = 64

// piece & position details
const std::vector<std::string> pieceNames = {
    "wP", "wN", "wB", "wR", "wQ", "wK",
    "bP", "bN", "bB", "bR", "bQ", "bK"
};

const std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
