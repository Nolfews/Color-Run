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
#include "platform/Platform.hpp"
#include "Color/Color.hpp"

#define CIRCLE_RADIUS 20.0f
#define CIRCLE_SPACING 50.0f
#define INDICATOR_Y_POSITION 30.0f

class Game {
public:
    Game();
    ~Game();

    void run();
    void loadLevel(int levelNumber);
    bool nextLevel();
    bool previousLevel();
    int getCurrentLevel() const;
    bool isEnemyMode() const;

private:
    void setupEventHandlers();
    void handleKeyPressed(sf::Keyboard::Key key);
    void updateGame(float deltaTime);
    void renderGame();
    void initGameEntities();
    void createTestPlatforms();
    void cycleColor(int direction);
    void renderColorIndicators();
    void updateColorCirclesPositions();
    void checkPlayerPlatformValidity();
    sf::Color getColorFromEnum(Color_t colorEnum);
    void renderColorOverlay();
    void updateCamera();
    void updateLivesDisplay();
    void checkPlayerEnemyCollision();
    void resetPlayerToSpawn();
    void handlePlayerDeath();
    void renderGameOver();
    void checkPlayerSpecialTileCollisions();
    void createEnemiesFromMap();

private:
    std::unique_ptr<Window> _window;
    std::unique_ptr<Map> _map;
    std::unique_ptr<Player> _player;
    std::vector<std::unique_ptr<Enemy>> _enemies;
    std::shared_ptr<Color> _colorState;
    std::vector<std::unique_ptr<Platform>> _platforms;
    std::vector<Color_t> _availableColors;
    int _currentColorIndex;
    sf::Font _font;
    sf::Text _colorText;
    sf::Text _modeText;
    sf::Text _livesText;
    sf::Texture _colorModeTexture;
    sf::Texture _enemyModeTexture;
    sf::Sprite _modeIcon;
    sf::Clock _gameClock;
    int _currentLevel;
    int _maxLevel;
    std::string _levelBasePath;
    std::vector<sf::CircleShape> _colorCircles;
    sf::CircleShape _currentColorIndicator;
    sf::RectangleShape _colorOverlay;
    sf::View _cameraView;
    sf::Music _backgroundMusic;
    bool _enemyMode;
    bool _gameOver;
    sf::Text _gameOverText;
    sf::Text _finalScoreText;
    sf::Text _scoreText;
};


