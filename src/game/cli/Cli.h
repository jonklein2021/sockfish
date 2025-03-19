#include "../Game.h"

class Cli : public Game {
private:
    /**
     * Check if the input matches "a-h1-8 a-h1-8",
     * the format that the user's input should be in
     * 
     * @param input The input string
     * @return True if the input is valid, false otherwise
     */
    bool validInput(const std::string& input);

    /**
     * Convert a string in the format "a-h1-8 a-h1-8"
     * to a Move object
     * 
     * @param input The input string
     * @return The Move object
     */
    Move coordsToMove(const std::string& input);

    /**
     * Convert a Move object to a string in the format
     * "a-h1-8 a-h1-8"
     * 
     * @param move The Move object
     * @return The string representation of the move 
     */
    std::string moveToCoords(const Move& move);

    /**
     * Get a move from stdin, looping until
     * a valid legal move is entered
     */
    Move getMoveFromStdin();
public:
    Cli();
    Cli(const std::string &fen, int depth);
    Cli(const std::string &fen, int depth, bool playerIsWhite);

    /**
     * Run the game loop
     */
    void run();
};