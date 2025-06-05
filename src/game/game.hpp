/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** game
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "window/window.hpp"
#include "map/map.hpp"

class Game {
public:
    Game();
    ~Game() = default;

    void run();
    void loadLevel(int levelNumber);
    bool nextLevel();
    bool previousLevel();
    int getCurrentLevel() const;

private:
    void setupEventHandlers();
    void handleKeyPressed(sf::Keyboard::Key key);
    std::unique_ptr<Window> _window;
    std::unique_ptr<Map> _map;
    int _currentLevel;
    int _maxLevel;
    std::string _levelBasePath;
};
