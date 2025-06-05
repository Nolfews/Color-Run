/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include "src/window/window.hpp"
#include "src/Color/Color.hpp"
#include "src/enemy/Enemy.hpp"
#include "src/platform/Platform.hpp"

int main()
{
    try {;
        std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1920, 1080), "Color Run");
        std::shared_ptr<Color> colorState = std::make_shared<Color>(Color_t::GREEN);
        sf::Event event;
        Enemy enemy(100, 100, colorState, window);
        Platform platform(200, 200, Color_t::GREEN, colorState, window);

        while (window->isOpen()) {
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window->close();
            }
            window->clear(sf::Color(50, 50, 50));

            enemy.draw();
            platform.draw();
            window->display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

