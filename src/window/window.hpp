/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** window
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <memory>

class Window {
public:
    Window(unsigned int width = 1920, unsigned int height = 1080, const std::string& title = "Color Run");
    ~Window();

    void clear(sf::Color color = sf::Color::Black);
    void display();
    bool isOpen() const;
    void handleEvents();
    std::shared_ptr<sf::RenderWindow> getWindow();

    void setKeyPressedCallback(std::function<void(sf::Keyboard::Key)> callback);
    void setKeyReleasedCallback(std::function<void(sf::Keyboard::Key)> callback);

private:
    std::shared_ptr<sf::RenderWindow> _window;
    sf::Event _event;

    std::function<void(sf::Keyboard::Key)> _keyPressedCallback;
    std::function<void(sf::Keyboard::Key)> _keyReleasedCallback;
};

