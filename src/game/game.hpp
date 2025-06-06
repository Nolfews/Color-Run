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
#include <SFML/System/Clock.hpp>
#include "window/window.hpp"
#include "map/map.hpp"
#include "player/player.hpp"
#include "enemy/Enemy.hpp"
#include "Color/Color.hpp"

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
    void updateGame(float deltaTime);
    void renderGame();
    void initGameEntities();

private:
    std::unique_ptr<Window> _window;
    std::unique_ptr<Map> _map;
    std::unique_ptr<Player> _player;
    std::unique_ptr<Enemy> _enemy;
    std::shared_ptr<Color> _colorState;
    sf::Clock _gameClock;
    int _currentLevel;
    int _maxLevel;
    std::string _levelBasePath;
};
