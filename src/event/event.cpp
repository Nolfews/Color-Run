/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** event
*/

#include "event.hpp"

EventManager::EventManager() : _windowCloseCallback(nullptr)
{
}

void EventManager::processEvents(sf::RenderWindow &window)
{
    while (window.pollEvent(_event)) {
        if (_event.type == sf::Event::Closed) {
            if (_windowCloseCallback) {
                _windowCloseCallback();
            } else {
                window.close();
            }
        }

        if (_event.type == sf::Event::KeyPressed) {
            auto it = _keyPressedCallbacks.find(_event.key.code);
            if (it != _keyPressedCallbacks.end()) {
                it->second();
            }
        }

        if (_event.type == sf::Event::KeyReleased) {
            auto it = _keyReleasedCallbacks.find(_event.key.code);
            if (it != _keyReleasedCallbacks.end()) {
                it->second();
            }
        }

        if (_event.type == sf::Event::MouseButtonPressed) {
            auto it = _mousePressedCallbacks.find(_event.mouseButton.button);
            if (it != _mousePressedCallbacks.end()) {
                it->second(_event.mouseButton.x, _event.mouseButton.y);
            }
        }

        if (_event.type == sf::Event::MouseButtonReleased) {
            auto it = _mouseReleasedCallbacks.find(_event.mouseButton.button);
            if (it != _mouseReleasedCallbacks.end()) {
                it->second(_event.mouseButton.x, _event.mouseButton.y);
            }
        }
    }
}

void EventManager::addKeyPressedCallback(sf::Keyboard::Key key, std::function<void()> callback)
{
    _keyPressedCallbacks[key] = callback;
}

void EventManager::addKeyReleasedCallback(sf::Keyboard::Key key, std::function<void()> callback)
{
    _keyReleasedCallbacks[key] = callback;
}

void EventManager::addMousePressedCallback(sf::Mouse::Button button, std::function<void(int, int)> callback)
{
    _mousePressedCallbacks[button] = callback;
}

void EventManager::addMouseReleasedCallback(sf::Mouse::Button button, std::function<void(int, int)> callback)
{
    _mouseReleasedCallbacks[button] = callback;
}

void EventManager::addWindowCloseCallback(std::function<void()> callback)
{
    _windowCloseCallback = callback;
}
