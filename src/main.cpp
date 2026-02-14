#include "src/ai/Engine.h"
#include "src/ai/search/SearchStopwatch.h"
#include "src/config/Config.h"
#include "src/core/GameController.h"
#include "src/frontend/cli/CliFrontend.h"
#include "src/frontend/gui/GuiFrontend.h"

int main(int argc, char **argv) {
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
    SearchStopwatch stopper(5000);
    Engine engine(&stopper);
    GameController gameController(pos, engine, cfg.humanSide);

    // create and launch game
    if (cfg.useGui) {
        GuiFrontend gui(gameController, cfg.pieceTheme);
        gui.run();
    } else {
        CliFrontend cli(gameController);
        cli.run();
    }

    return 0;
}
