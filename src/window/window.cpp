/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** window
*/

#include "window.hpp"

Window::Window(unsigned int width, unsigned int height, const std::string& title)
{
    _window.create(sf::VideoMode(width, height), title, sf::Style::Fullscreen);
    _window.setFramerateLimit(60);
}

Window::~Window()
{
    if (_window.isOpen())
        _window.close();
}

void Window::clear(sf::Color color)
{
    _window.clear(color);
}

void Window::display()
{
    _window.display();
}

bool Window::isOpen() const
{
    return _window.isOpen();
}

void Window::handleEvents()
{
    while (_window.pollEvent(_event)) {
        if (_event.type == sf::Event::Closed)
            _window.close();
        if (_event.type == sf::Event::KeyPressed && _event.key.code == sf::Keyboard::Escape)
            _window.close();
    }
}

sf::RenderWindow& Window::getWindow()
{
    return _window;
}

