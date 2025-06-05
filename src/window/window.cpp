/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** window
*/

#include "window.hpp"

Window::Window(unsigned int width, unsigned int height, const std::string& title)
{
    _window = std::make_shared<sf::RenderWindow>(sf::VideoMode(width, height), title, sf::Style::Fullscreen);
    _window->setFramerateLimit(60);
    _keyPressedCallback = nullptr;
    _keyReleasedCallback = nullptr;
}

Window::~Window()
{
    if (_window && _window->isOpen())
        _window->close();
}

void Window::clear(sf::Color color)
{
    _window->clear(color);
}

void Window::display()
{
    _window->display();
}

bool Window::isOpen() const
{
    return _window && _window->isOpen();
}

void Window::handleEvents()
{
    while (_window->pollEvent(_event)) {
        if (_event.type == sf::Event::Closed)
            _window->close();
        if (_event.type == sf::Event::KeyPressed && _event.key.code == sf::Keyboard::Escape)
            _window->close();
        if (_event.type == sf::Event::KeyPressed && _keyPressedCallback) {
            _keyPressedCallback(_event.key.code);
        }
        if (_event.type == sf::Event::KeyReleased && _keyReleasedCallback) {
            _keyReleasedCallback(_event.key.code);
        }
    }
}

std::shared_ptr<sf::RenderWindow> Window::getWindow()
{
    return _window;
}

void Window::setKeyPressedCallback(std::function<void(sf::Keyboard::Key)> callback)
{
    _keyPressedCallback = callback;
}

void Window::setKeyReleasedCallback(std::function<void(sf::Keyboard::Key)> callback)
{
    _keyReleasedCallback = callback;
}
