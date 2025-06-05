/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** game
*/

#include "game.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>

Game::Game() :
    _window(std::make_unique<Window>(1920, 1080, "Color Run")),
    _currentLevel(1),
    _maxLevel(0),
    _levelBasePath("assets/levels/level_")
{
    for (const auto& entry : std::filesystem::directory_iterator("assets/levels/")) {
        const std::string& path = entry.path().string();
        if (path.find("level_") != std::string::npos && path.find(".txt") != std::string::npos) {
            std::string levelStr = path.substr(path.find("level_") + 6);
            levelStr = levelStr.substr(0, levelStr.find(".txt"));
            try {
                int levelNum = std::stoi(levelStr);
                _maxLevel = std::max(_maxLevel, levelNum);
            } catch (const std::exception& e) {
            }
        }
    }
    setupEventHandlers();
    loadLevel(_currentLevel);
}

void Game::run()
{
    while (_window->isOpen()) {
        _window->handleEvents();
        _window->clear(sf::Color(50, 50, 50));

        if (_map) {
            _map->draw(_window->getWindow());
        }

        _window->display();
    }
}

void Game::loadLevel(int levelNumber)
{
    std::string levelPath = _levelBasePath + std::to_string(levelNumber) + ".txt";

    try {
        _map = std::make_unique<Map>(levelPath);
        _currentLevel = levelNumber;
    } catch (const std::exception& e) {
        std::cerr << "Error loading level " << levelNumber << ": " << e.what() << std::endl;
    }
}

bool Game::nextLevel()
{
    if (_currentLevel < _maxLevel) {
        loadLevel(_currentLevel + 1);
        return true;
    }
    std::cout << "You've completed all available levels!" << std::endl;
    return false;
}

int Game::getCurrentLevel() const
{
    return _currentLevel;
}

void Game::setupEventHandlers()
{
    _window->setKeyPressedCallback([this](sf::Keyboard::Key key) {
        handleKeyPressed(key);
    });
}

void Game::handleKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Left) {
        previousLevel();
    } else if (key == sf::Keyboard::Right) {
        nextLevel();
    }
}

bool Game::previousLevel()
{
    if (_currentLevel > 1) {
        std::cout << "Loading previous level..." << std::endl;
        loadLevel(_currentLevel - 1);
        return true;
    }
    std::cout << "You are already at the first level!" << std::endl;
    return false;
}
