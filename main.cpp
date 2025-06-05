/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include "src/window/window.hpp"
#include "src/player/player.hpp"

int main()
{
    try {
        Window window(1920, 1080, "Color Run");
        Player player;

        while (window.isOpen()) {
            window.handleEvents();
            player.handleInput();
            
            window.clear(sf::Color(50, 50, 50));
            player.update(1.0f / 60.0f);
            player.draw(window.getWindow());
            window.display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

