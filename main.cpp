/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include "src/window/window.hpp"

int main()
{
    try {
        Window window(1920, 1080, "Color Run");
        sf::RectangleShape cube(sf::Vector2f(64.0f, 64.0f));
        cube.setFillColor(sf::Color::Red);
        cube.setPosition(
            (1920.0f / 2) - (32.0f / 2),
            (1080.0f / 2) - (32.0f / 2)
        );

        while (window.isOpen()) {
            window.handleEvents();
            window.clear(sf::Color(50, 50, 50));

            window.getWindow().draw(cube);
            window.display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

