#include "CliFrontend.h"
#include "Config.h"
#include "Engine.h"
#include "GameController.h"
#include "GuiFrontend.h"
#include "Magic.h"
#include "Zobrist.h"

#include <cstdlib>
#include <ctime>
#include <memory>

int main(int argc, char **argv) {
    // initialize magic and zobrist tables
    Magic::init();
    Zobrist::init();

    // parse command line arguments
    Config cfg;
    cfg.parseArgs(argc, argv);

    // randomly assign colors if no override
    std::srand(std::time(nullptr));
    if (cfg.randomColor) {
        cfg.randomizeSides();
    }

    cfg.print();

    // create base objects
    Position pos(cfg.fen);
    std::unique_ptr<Engine> engine = std::make_unique<Engine>(cfg.searchDepth);
    GameController gameController(pos, std::move(engine), cfg.humanSide);

    // create and launch game
    if (cfg.useGui) {
        GuiFrontend gui(gameController);
        gui.run();
    } else {
        CliFrontend cli(gameController);
        cli.run();
    }

    return 0;
}
