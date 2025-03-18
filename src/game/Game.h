#pragma once

#include <string>

/**
 * Abstract class for a game extended
 * by classes in Cli.h and Gui.h
 */

class Game {
public:
    /**
     * Runs the game loop
     */
    virtual void run() = 0;
};