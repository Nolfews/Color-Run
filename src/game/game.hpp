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

enum class GameState {
    MENU,
    PLAYING,
    SETTINGS,
    GAME_OVER
};

enum class MenuOption {
    PLAY = 0,
    SETTINGS = 1,
    QUIT = 2
};

enum class SettingsOption {
    CONTROLS = 0,
    SCREEN_SIZE = 1,
    BACK = 2
};

enum class KeyBindingOption {
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3,
    TAB = 4,
    RESTART = 5,
    BACK = 6
};

enum class ScreenSizeOption {
    SIZE_1280x720 = 0,
    SIZE_1920x1080 = 1,
    SIZE_2560x1440 = 2,
    FULLSCREEN = 3,
    BACK = 4
};

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

    void handleMenuKeyPressed(sf::Keyboard::Key key);
    void renderMenu();
    void initMenu();
    void navigateMenu(int direction);
    void selectMenuOption();
    void startGame();
    void openSettings();
    void quitGame();
    void handleSettingsKeyPressed(sf::Keyboard::Key key);
    void renderSettings();
    void initSettings();
    void navigateSettings(int direction);
    void selectSettingsOption();
    void renderKeyBindingMenu();
    void handleKeyBindingKeyPressed(sf::Keyboard::Key key);
    void navigateKeyBinding(int direction);
    void selectKeyBindingOption();
    void renderScreenSizeMenu();
    void handleScreenSizeKeyPressed(sf::Keyboard::Key key);
    void navigateScreenSize(int direction);
    void selectScreenSizeOption();
    void changeWindowSize(int width, int height, bool fullscreen = false);
    void resetToDefaultKeys();
    void backToMainMenu();
    void backToSettings();
    void updateKeyBindingDisplay();
    std::string getKeyName(sf::Keyboard::Key key);
    void recreateGameEntities();
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

    GameState _gameState;
    MenuOption _currentMenuOption;
    sf::Texture _titleTexture;
    sf::Sprite _titleSprite;
    std::vector<sf::Text> _menuOptions;
    sf::RectangleShape _menuSelector;

    SettingsOption _currentSettingsOption;
    KeyBindingOption _currentKeyBindingOption;
    ScreenSizeOption _currentScreenSizeOption;
    std::vector<sf::Text> _settingsOptions;
    std::vector<sf::Text> _keyBindingOptions;
    std::vector<sf::Text> _screenSizeOptions;
    bool _isInKeyBinding;
    bool _isInScreenSize;
    bool _fullscreen;
    sf::Vector2u _windowedSize;
    sf::Keyboard::Key _leftKey;
    sf::Keyboard::Key _rightKey;
    sf::Keyboard::Key _upKey;
    sf::Keyboard::Key _downKey;
    sf::Keyboard::Key _tabKey;
    sf::Keyboard::Key _restartKey;
};


