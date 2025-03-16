#include "gui/Game.h"

int main() {
    const std::string initFen = "6k1/1P3pp1/7p/8/8/6P1/1p3P1P/6K1";

    Game *game = new Game(initFen);
    game->run();
}