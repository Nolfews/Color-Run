/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** main
*/

#include <iostream>
#include "src/window/window.hpp"
#include "src/player/player.hpp"

#include "src/Color/Color.hpp"
#include "src/enemy/Enemy.hpp"

int main()
{
    try {
        std::shared_ptr<sf::RenderWindow> window = std::make_shared<sf::RenderWindow>(sf::VideoMode(1920, 1080), "Color Run");
        std::shared_ptr<Color> colorState = std::make_shared<Color>(Color_t::WHITE);
        sf::Event event;

        Enemy enemy(100, 100, colorState, window);
        Player player(100.0f, 900.0f);
        player.setColor(colorState);

        sf::Clock clock;

        while (window->isOpen()) {
            float deltaTime = clock.restart().asSeconds();
            while (window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window->close();
            }

            player.handleInput();
            player.update(deltaTime);
            window->clear(sf::Color(50, 50, 50));
            player.draw(*window);
            enemy.draw();
            window->display();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}

