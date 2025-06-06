/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include "src/game/game.hpp"
#include "src/window/window.hpp"
#include "src/player/player.hpp"
#include "src/Color/Color.hpp"
#include "src/enemy/Enemy.hpp"
#include "src/platform/Platform.hpp"

int main()
{
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

