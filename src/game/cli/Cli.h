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