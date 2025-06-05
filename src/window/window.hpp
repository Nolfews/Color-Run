/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** window
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Window {
public:
    Window(unsigned int width = 1920, unsigned int height = 1080, const std::string& title = "Color Run");
    ~Window();

    void clear(sf::Color color = sf::Color::Black);
    void display();
    bool isOpen() const;
    void handleEvents();
    sf::RenderWindow& getWindow();

private:
    sf::RenderWindow _window;
    sf::Event _event;
};

