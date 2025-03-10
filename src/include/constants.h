#include <string>
#include <vector>

const int BOARD_PIXEL_SIZE = 512; // board is 512x512 pixels
const int TILE_PIXEL_SIZE = 64; // 512/8 = 64

const char* PIECES_TEXTURE_DIR = "assets/pieces/";
const char* BOARD_TEXTURE_PATH = "assets/board.png";

const std::vector<std::string> pieceNames = {"wK", "wQ", "wR", "wB", "wN", "wP", "bK", "bQ", "bR", "bB", "bN", "bP"};
const std::string startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";