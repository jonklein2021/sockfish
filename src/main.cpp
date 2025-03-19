#include <iostream>

#include "constants.h"
#include "game/cli/Cli.h"
#include "game/gui/Gui.h"

void usage() {
    std::ostringstream ss;
    
    ss << "Usage: ./build/main [options]\n";
    ss << "Options:\n";
    ss << "  -h, --help      Display this message\n";
    ss << "  -c, --cli       Run the game in CLI mode (default)\n";
    ss << "  -g, --gui       Run the game in GUI mode\n";
    ss << "  -f  <string>    Specify a custom FEN string for the starting position\n";
    ss << "  -d  <depth>     Specify the depth of the minimax search\n";
    
    std::cout << ss.str() << std::endl;
}

int main(int argc, char** argv) {
    // parse command line arguments
    bool useCli = true;
    std::string fen = defaultFEN;
    int depth = 4;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            usage();
            return 0;
        } else if (arg == "-c" || arg == "--cli") {
            useCli = true;
        } else if (arg == "-g" || arg == "--gui") {
            useCli = false;
        } else if (arg == "-f") {
            if (i + 1 < argc) {
                fen = argv[i + 1];
                i++;
            } else {
                std::cerr << "Error: -f option requires a FEN string" << std::endl;
                usage();
                return 1;
            }
        } else if (arg == "-d") {
            if (i + 1 < argc) {
                depth = std::stoi(argv[i + 1]);
                i++;
            } else {
                std::cerr << "Error: -d option requires a depth" << std::endl;
                usage();
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            usage();
            return 1;
        }
    }

    std::cout << "Initializing the " << (useCli ? "CLI" : "GUI") << " with FEN string " << fen << std::endl;

    // run the CLI or GUI
    Game *game;
    if (useCli) {
        game = new Cli(fen, depth);
    } else {
        game = new Gui(fen, depth);
    }
    game->run();

    return 0;
}