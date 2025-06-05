/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** event
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <map>

class EventManager {
public:
    EventManager();
    ~EventManager() = default;

    void processEvents(sf::RenderWindow &window);

    void addKeyPressedCallback(sf::Keyboard::Key key, std::function<void()> callback);
    void addKeyReleasedCallback(sf::Keyboard::Key key, std::function<void()> callback);
    void addMousePressedCallback(sf::Mouse::Button button, std::function<void(int, int)> callback);
    void addMouseReleasedCallback(sf::Mouse::Button button, std::function<void(int, int)> callback);
    void addWindowCloseCallback(std::function<void()> callback);

private:
    sf::Event _event;
    std::map<sf::Keyboard::Key, std::function<void()>> _keyPressedCallbacks;
    std::map<sf::Keyboard::Key, std::function<void()>> _keyReleasedCallbacks;
    std::map<sf::Mouse::Button, std::function<void(int, int)>> _mousePressedCallbacks;
    std::map<sf::Mouse::Button, std::function<void(int, int)>> _mouseReleasedCallbacks;
    std::function<void()> _windowCloseCallback;
};
