/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include <string>
#include "src/game/game.hpp"

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

