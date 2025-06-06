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
    _window(std::make_unique<Window>(1280, 720, "Color Run")),
    _currentLevel(1),
    _maxLevel(0),
    _levelBasePath("assets/levels/level_"),
    _enemyMode(false),
    _gameOver(false),
    _gameState(GameState::MENU),
    _currentMenuOption(MenuOption::PLAY),
    _currentSettingsOption(SettingsOption::CONTROLS),
    _currentKeyBindingOption(KeyBindingOption::LEFT),
    _currentScreenSizeOption(ScreenSizeOption::SIZE_1280x720),
    _isInKeyBinding(false),
    _isInScreenSize(false),
    _fullscreen(false),
    _windowedSize(1280, 720),
    _leftKey(sf::Keyboard::Left),
    _rightKey(sf::Keyboard::Right),
    _upKey(sf::Keyboard::Up),
    _downKey(sf::Keyboard::Down),
    _tabKey(sf::Keyboard::Tab),
    _restartKey(sf::Keyboard::R)
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
    initGameEntities();
    initMenu();
    initSettings();
    _cameraView.setSize(1280, 720);
    _cameraView.setCenter(640, 360);
    setupEventHandlers();
    _backgroundMusic.openFromFile("assets/sounds/Hajar.ogg");
    _backgroundMusic.setLoop(true);
    _backgroundMusic.setVolume(50);

    _gameOverText.setCharacterSize(48);
    _gameOverText.setFillColor(sf::Color::Red);
    _gameOverText.setString("GAME OVER");
    if (_font.getInfo().family != "") {
        _gameOverText.setFont(_font);
    }

    _finalScoreText.setCharacterSize(32);
    _finalScoreText.setFillColor(sf::Color::White);
    if (_font.getInfo().family != "") {
        _finalScoreText.setFont(_font);
    }

    _scoreText.setCharacterSize(24);
    _scoreText.setFillColor(sf::Color::White);
    if (_font.getInfo().family != "") {
        _scoreText.setFont(_font);
    }
}

void Game::run()
{
    while (_window->isOpen()) {
        float deltaTime = _gameClock.restart().asSeconds();

        _window->handleEvents();

        if (_gameState == GameState::MENU) {
            renderMenu();
        } else if (_gameState == GameState::SETTINGS) {
            renderSettings();
        } else if (_gameState == GameState::PLAYING) {
            if (_backgroundMusic.getStatus() != sf::Music::Playing) {
                _backgroundMusic.play();
            }
            updateGame(deltaTime);
            renderGame();
        }
    }
}

void Game::loadLevel(int levelNumber)
{
    std::string levelPath = _levelBasePath + std::to_string(levelNumber) + ".txt";

    try {
        _map = std::make_unique<Map>(levelPath);
        _currentLevel = levelNumber;
        createEnemiesFromMap();
        if (_player && _map) {
            sf::Vector2f spawnPos = _map->getSpawnPosition();
            _player->teleport(spawnPos.x, spawnPos.y);
        }
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

bool Game::isEnemyMode() const
{
    return _enemyMode;
}

void Game::setupEventHandlers()
{
    _window->setKeyPressedCallback([this](sf::Keyboard::Key key) {
        handleKeyPressed(key);
    });
    _window->setResizeCallback([this](unsigned int width, unsigned int height) {
        updateColorCirclesPositions();
        updateLivesDisplay();
    });
}

void Game::handleKeyPressed(sf::Keyboard::Key key)
{
    if (_gameState == GameState::MENU) {
        handleMenuKeyPressed(key);
        return;
    }
    if (_gameState == GameState::SETTINGS) {
        handleSettingsKeyPressed(key);
        return;
    }
    if (_gameOver) {
        if (key == _restartKey) {
            _gameOver = false;
            if (_player) {
                _player->setLife(3);
                _player->setScore(0);
                resetPlayerToSpawn();
            }
        }
        return;
    }
    
    if (key == _leftKey) {
        cycleColor(-1);
    } else if (key == _rightKey) {
        cycleColor(1);
    } else if (key == _upKey) {
        nextLevel();
    } else if (key == _downKey) {
        previousLevel();
    } else if (key == _tabKey) {
        _enemyMode = !_enemyMode;
        if (_enemyMode) {
            _modeIcon.setTexture(_enemyModeTexture);
        } else {
            _modeIcon.setTexture(_colorModeTexture);
        }
        checkPlayerPlatformValidity();
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

void Game::initGameEntities()
{
    if (!_font.loadFromFile("goffy.TTF")) {
        std::cout << "Warning: Could not load goffy.TTF font, text may not display properly" << std::endl;
    }
    _availableColors = {Color_t::RED, Color_t::GREEN, Color_t::BLUE, Color_t::YELLOW, Color_t::CYAN, Color_t::MAGENTA};
    _currentColorIndex = 0;
    _colorState = std::make_shared<Color>(_availableColors[_currentColorIndex]);
    createTestPlatforms();
    _player = std::make_shared<Player>(100.0f, 550.0f);
    _player->setColor(_colorState);
    _colorText.setCharacterSize(24);
    _colorText.setFillColor(sf::Color::White);
    _colorText.setPosition(10, 10);
    if (_font.getInfo().family != "") {
        _colorText.setFont(_font);
    }
    if (!_colorModeTexture.loadFromFile("assets/img/color.png")) {
        std::cout << "Warning: Could not load color.png texture" << std::endl;
    }
    if (!_enemyModeTexture.loadFromFile("assets/img/loupe.png")) {
        std::cout << "Warning: Could not load loupe.png texture" << std::endl;
    }
    _modeIcon.setTexture(_colorModeTexture);
    _modeIcon.setPosition(10, 40);
    _modeIcon.setScale(0.5f, 0.5f);
    _modeText.setCharacterSize(20);
    _modeText.setFillColor(sf::Color::Yellow);
    _modeText.setPosition(60, 45);
    if (_font.getInfo().family != "") {
        _modeText.setFont(_font);
    }

    _livesText.setCharacterSize(24);
    _livesText.setFillColor(sf::Color::Red);
    if (_font.getInfo().family != "") {
        _livesText.setFont(_font);
    }
    _colorCircles.clear();
    float windowWidth = _window->getWindow()->getSize().x;
    float totalWidth = (_availableColors.size() - 1) * CIRCLE_SPACING;
    float startX = (windowWidth - totalWidth) / 2.0f;
    for (size_t i = 0; i < _availableColors.size(); ++i) {
        sf::CircleShape circle(CIRCLE_RADIUS);
        circle.setFillColor(getColorFromEnum(_availableColors[i]));
        circle.setOutlineThickness(2.0f);
        circle.setOutlineColor(sf::Color::White);
        circle.setPosition(startX + i * CIRCLE_SPACING - CIRCLE_RADIUS, INDICATOR_Y_POSITION - CIRCLE_RADIUS);
        _colorCircles.push_back(circle);
    }
    _currentColorIndicator.setRadius(CIRCLE_RADIUS + 5.0f);
    _currentColorIndicator.setFillColor(sf::Color::Transparent);
    _currentColorIndicator.setOutlineThickness(3.0f);
    _currentColorIndicator.setOutlineColor(sf::Color::Yellow);
    float indicatorX = startX + _currentColorIndex * CIRCLE_SPACING - (_currentColorIndicator.getRadius());
    _currentColorIndicator.setPosition(indicatorX, INDICATOR_Y_POSITION - _currentColorIndicator.getRadius());

    _colorOverlay.setSize(sf::Vector2f(_window->getWindow()->getSize().x, _window->getWindow()->getSize().y));
    _colorOverlay.setPosition(0, 0);
    _colorOverlay.setFillColor(sf::Color::Transparent);
}

void Game::createTestPlatforms()
{
    _platforms.clear();
    _platforms.push_back(std::make_unique<Platform>(50, 600, Color_t::BLACK, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(114, 600, Color_t::BLACK, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(178, 600, Color_t::BLACK, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(400, 500, Color_t::RED, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(600, 400, Color_t::GREEN, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(800, 300, Color_t::BLUE, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(1000, 200, Color_t::WHITE, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(300, 350, Color_t::YELLOW, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(500, 250, Color_t::CYAN, _colorState, _window->getWindow()));
    _platforms.push_back(std::make_unique<Platform>(700, 150, Color_t::MAGENTA, _colorState, _window->getWindow()));
}

void Game::updateGame(float deltaTime)
{
    if (_gameOver) {
        return;
    }
    if (_player) {
        _player->handleInput();
        _player->update(deltaTime);
        _player->checkPlatformCollisions(_platforms, _enemyMode);
        checkPlayerEnemyCollision();
        checkPlayerSpecialTileCollisions();
    }
    updateCamera();
    updateLivesDisplay();
}

void Game::renderGame()
{
    if (!_window || !_window->getWindow())
        return;
    _window->clear(sf::Color(50, 50, 50));
    _window->getWindow()->setView(_cameraView);
    for (const auto& platform : _platforms) {
        if (platform) {
            platform->draw(_enemyMode);
        }
    }
    for (const auto& coin : _coins) {
        if (coin) {
            coin->draw();
        }
    }
    if (_player) {
        _player->draw(*_window->getWindow());
    }
    for (const auto& enemy : _enemies) {
        if (enemy) {
            enemy->draw(_enemyMode);
        }
    }
    _window->getWindow()->setView(_window->getWindow()->getDefaultView());
    renderColorIndicators();
    renderColorOverlay();
    _window->getWindow()->draw(_modeIcon);
    if (_font.getInfo().family != "") {
        _window->getWindow()->draw(_modeText);
        _window->getWindow()->draw(_livesText);
        _window->getWindow()->draw(_scoreText);
    }

    if (_gameOver) {
        renderGameOver();
    }

    _window->display();
}

void Game::cycleColor(int direction)
{
    _currentColorIndex += direction;
    if (_currentColorIndex < 0) {
        _currentColorIndex = _availableColors.size() - 1;
    } else if (_currentColorIndex >= static_cast<int>(_availableColors.size())) {
        _currentColorIndex = 0;
    }
    _colorState->setColor(_availableColors[_currentColorIndex]);
    sf::Color overlayColor = getColorFromEnum(_availableColors[_currentColorIndex]);
    overlayColor.a = 30;
    _colorOverlay.setFillColor(overlayColor);
    updateColorCirclesPositions();
    checkPlayerPlatformValidity();
}

sf::Color Game::getColorFromEnum(Color_t colorEnum)
{
    switch (colorEnum) {
        case Color_t::RED:     return sf::Color::Red;
        case Color_t::GREEN:   return sf::Color::Green;
        case Color_t::BLUE:    return sf::Color::Blue;
        case Color_t::YELLOW:  return sf::Color::Yellow;
        case Color_t::CYAN:    return sf::Color::Cyan;
        case Color_t::MAGENTA: return sf::Color::Magenta;
        case Color_t::WHITE:   return sf::Color::White;
        case Color_t::BLACK:   return sf::Color::Black;
        default:               return sf::Color::White;
    }
}

void Game::renderColorIndicators()
{
    if (!_window || !_window->getWindow())
        return;
    for (const auto& circle : _colorCircles) {
        _window->getWindow()->draw(circle);
    }
    _window->getWindow()->draw(_currentColorIndicator);
}

void Game::updateColorCirclesPositions()
{
    if (!_window || !_window->getWindow())
        return;
    float windowWidth = _window->getWindow()->getSize().x;
    float totalWidth = (_availableColors.size() - 1) * CIRCLE_SPACING;
    float startX = (windowWidth - totalWidth) / 2.0f;
    _colorOverlay.setSize(sf::Vector2f(windowWidth, _window->getWindow()->getSize().y));
    for (size_t i = 0; i < _colorCircles.size(); ++i) {
        _colorCircles[i].setPosition(startX + i * CIRCLE_SPACING - CIRCLE_RADIUS, INDICATOR_Y_POSITION - CIRCLE_RADIUS);
    }
    float indicatorX = startX + _currentColorIndex * CIRCLE_SPACING - (_currentColorIndicator.getRadius());
    _currentColorIndicator.setPosition(indicatorX, INDICATOR_Y_POSITION - _currentColorIndicator.getRadius());
}

void Game::renderColorOverlay()
{
    if (!_window || !_window->getWindow())
        return;
    _window->getWindow()->draw(_colorOverlay);
}

void Game::checkPlayerPlatformValidity()
{
    if (!_player)
        return;
    sf::FloatRect playerBounds = _player->getBounds();
    bool isOnValidPlatform = false;
    for (const auto& platform : _platforms) {
        if (!platform)
            continue;
        sf::FloatRect platformBounds = platform->getBounds();
        if (playerBounds.left < platformBounds.left + platformBounds.width &&
            playerBounds.left + playerBounds.width > platformBounds.left &&
            playerBounds.top + playerBounds.height >= platformBounds.top &&
            playerBounds.top + playerBounds.height <= platformBounds.top + 10.0f) {
            if (platform->shouldCollideWithPlayer(_enemyMode)) {
                isOnValidPlatform = true;
                break;
            }
        }
    }
    if (!isOnValidPlatform && _player->isOnGround()) {
        _player->setGroundState(false);
    }
}

void Game::updateCamera()
{
    if (!_player || !_window || !_window->getWindow())
        return;
    sf::Vector2f playerPos = _player->getPosition();
    sf::Vector2f currentCenter = _cameraView.getCenter();
    float lerp = 0.1f;
    sf::Vector2f targetCenter = playerPos + sf::Vector2f(PLAYER_SIZE / 2, PLAYER_SIZE / 2);
    sf::Vector2f newCenter;
    newCenter.x = currentCenter.x + (targetCenter.x - currentCenter.x) * lerp;
    newCenter.y = currentCenter.y + (targetCenter.y - currentCenter.y) * lerp;
    float cameraHalfWidth = _cameraView.getSize().x / 2.0f;
    float cameraHalfHeight = _cameraView.getSize().y / 2.0f;
    if (newCenter.x - cameraHalfWidth < 0) {
        newCenter.x = cameraHalfWidth;
    }
    if (newCenter.y - cameraHalfHeight < 0) {
        newCenter.y = cameraHalfHeight;
    }
    _cameraView.setCenter(newCenter);
    _window->getWindow()->setView(_cameraView);
}

void Game::updateLivesDisplay()
{
    if (!_player || !_window || !_window->getWindow())
        return;
    std::string livesStr = "Vies: " + std::to_string(_player->getLife());
    _livesText.setString(livesStr);
    float windowWidth = _window->getWindow()->getSize().x;
    if (_font.getInfo().family != "") {
        sf::FloatRect textBounds = _livesText.getLocalBounds();
        _livesText.setPosition(windowWidth - textBounds.width - 20, 10);
    } else {
        _livesText.setPosition(windowWidth - 150, 10);
    }
    std::string scoreStr = "Score: " + std::to_string(_player->getScore());
    _scoreText.setString(scoreStr);
    if (_font.getInfo().family != "") {
        sf::FloatRect scoreBounds = _scoreText.getLocalBounds();
        _scoreText.setPosition(windowWidth - scoreBounds.width - 20, 40);
    } else {
        _scoreText.setPosition(windowWidth - 150, 40);
    }
}

void Game::checkPlayerEnemyCollision()
{
    if (!_player) {
        return;
    }
    sf::FloatRect playerBounds = _player->getBounds();
    for (const auto& enemy : _enemies) {
        if (!enemy) continue;
        sf::FloatRect enemyBounds = enemy->getBounds();
        if (playerBounds.intersects(enemyBounds)) {
            handlePlayerDeath();
            return;
        }
    }
}

void Game::resetPlayerToSpawn()
{
    if (!_player || !_map) {
        return;
    }
    sf::Vector2f spawnPos = _map->getSpawnPosition();
    _player->teleport(spawnPos.x, spawnPos.y);
}

void Game::handlePlayerDeath()
{
    if (!_player) {
        return;
    }
    _player->takeDamage(1);
    if (_player->getLife() <= 0) {
        _gameOver = true;
    } else {
        resetPlayerToSpawn();
    }
}

void Game::renderGameOver()
{
    if (!_window || !_window->getWindow()) {
        return;
    }
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(_window->getWindow()->getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    _window->getWindow()->draw(overlay);
    sf::Vector2u windowSize = _window->getWindow()->getSize();
    sf::FloatRect gameOverBounds = _gameOverText.getLocalBounds();
    _gameOverText.setPosition(
        (windowSize.x - gameOverBounds.width) / 2.0f,
        (windowSize.y - gameOverBounds.height) / 2.0f - 80.0f
    );
    _window->getWindow()->draw(_gameOverText);
    std::string scoreText = "Score Final: " + std::to_string(_player ? _player->getScore() : 0);
    _finalScoreText.setString(scoreText);
    sf::FloatRect scoreBounds = _finalScoreText.getLocalBounds();
    _finalScoreText.setPosition(
        (windowSize.x - scoreBounds.width) / 2.0f,
        (windowSize.y - scoreBounds.height) / 2.0f - 20.0f
    );
    _window->getWindow()->draw(_finalScoreText);
    sf::Text restartText;
    restartText.setString("Appuyez sur R pour recommencer");
    restartText.setCharacterSize(24);
    restartText.setFillColor(sf::Color::Yellow);
    if (_font.getInfo().family != "") {
        restartText.setFont(_font);
    }
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition(
        (windowSize.x - restartBounds.width) / 2.0f,
        (windowSize.y - restartBounds.height) / 2.0f + 40.0f
    );
    _window->getWindow()->draw(restartText);
}

void Game::checkPlayerSpecialTileCollisions()
{
    if (!_player || !_map) {
        return;
    }
    sf::FloatRect playerBounds = _player->getBounds();
    std::vector<std::vector<Tile>>& tiles = _map->getTiles();
    int leftTile = static_cast<int>(playerBounds.left / 64);
    int rightTile = static_cast<int>((playerBounds.left + playerBounds.width) / 64);
    int topTile = static_cast<int>(playerBounds.top / 64);
    int bottomTile = static_cast<int>((playerBounds.top + playerBounds.height) / 64);
    leftTile = std::max(0, leftTile);
    topTile = std::max(0, topTile);
    if (topTile < static_cast<int>(tiles.size())) {
        rightTile = std::min(rightTile, static_cast<int>(tiles[topTile].size()) - 1);
        bottomTile = std::min(bottomTile, static_cast<int>(tiles.size()) - 1);
        for (int y = topTile; y <= bottomTile; ++y) {
            for (int x = leftTile; x <= rightTile; ++x) {
                if (y >= 0 && y < static_cast<int>(tiles.size()) &&
                    x >= 0 && x < static_cast<int>(tiles[y].size())) {
                    TileType tileType = tiles[y][x].type;
                    if (tileType == FINISH) {
                        _player->addScore(100);
                        std::cout << "Niveau terminé! +100 points" << std::endl;
                        nextLevel();
                    } else if (tileType == TRAP) {
                        _player->addScore(50);
                        std::cout << "Piège évité! +50 points" << std::endl;
                        tiles[y][x].type = EMPTY;
                    }
                }
            }
        }
    }
}

void Game::createEnemiesFromMap()
{
    if (!_map) {
        return;
    }
    _enemies.clear();
    std::vector<sf::Vector2f> enemyPositions = _map->getEnemyPositions();
    for (const sf::Vector2f& pos : enemyPositions) {
        auto enemy = std::make_unique<Enemy>(static_cast<int>(pos.x), static_cast<int>(pos.y),
                                           _colorState, _window->getWindow());
        _enemies.push_back(std::move(enemy));
    }
    std::cout << "Created " << _enemies.size() << " enemies from map" << std::endl;
}

void Game::initMenu()
{
    if (!_titleTexture.loadFromFile("assets/color-run-title.png")) {
        std::cerr << "Error loading title texture!" << std::endl;
    }
    _titleSprite.setTexture(_titleTexture);
    sf::FloatRect titleBounds = _titleSprite.getLocalBounds();
    _titleSprite.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    _titleSprite.setPosition(640, 150);
    if (!_font.loadFromFile("goffy.TTF")) {
        std::cerr << "Error loading font for menu!" << std::endl;
    }
    _menuOptions.resize(3);
    std::vector<std::string> optionTexts = {"JOUER", "PARAMETRES", "QUITTER"};
    for (size_t i = 0; i < _menuOptions.size(); ++i) {
        if (_font.getInfo().family != "") {
            _menuOptions[i].setFont(_font);
        }
        _menuOptions[i].setString(optionTexts[i]);
        _menuOptions[i].setCharacterSize(36);
        _menuOptions[i].setFillColor(sf::Color::White);
        sf::FloatRect textBounds = _menuOptions[i].getLocalBounds();
        _menuOptions[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        _menuOptions[i].setPosition(640, 300 + i * 80);
    }
    _menuSelector.setSize(sf::Vector2f(300, 50));
    _menuSelector.setFillColor(sf::Color(255, 255, 255, 50));
    _menuSelector.setOutlineColor(sf::Color::Yellow);
    _menuSelector.setOutlineThickness(2);
    _menuSelector.setOrigin(150, 25);
    _menuSelector.setPosition(640, 300);
}

void Game::handleMenuKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Up) {
        navigateMenu(-1);
    } else if (key == sf::Keyboard::Down) {
        navigateMenu(1);
    } else if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
        selectMenuOption();
    }
}

void Game::navigateMenu(int direction)
{
    int currentOption = static_cast<int>(_currentMenuOption);
    currentOption += direction;
    if (currentOption < 0) {
        currentOption = 2;
    } else if (currentOption > 2) {
        currentOption = 0;
    }
    _currentMenuOption = static_cast<MenuOption>(currentOption);
    _menuSelector.setPosition(640, 300 + currentOption * 80);
}

void Game::selectMenuOption()
{
    switch (_currentMenuOption) {
        case MenuOption::PLAY:
            startGame();
            break;
        case MenuOption::SETTINGS:
            openSettings();
            break;
        case MenuOption::QUIT:
            quitGame();
            break;
    }
}

void Game::startGame()
{
    _gameState = GameState::PLAYING;
    loadLevel(_currentLevel);
    _backgroundMusic.play();
}

void Game::openSettings()
{
    _gameState = GameState::SETTINGS;
    _currentSettingsOption = SettingsOption::CONTROLS;
    _isInKeyBinding = false;
    _isInScreenSize = false;
}

void Game::quitGame()
{
    _window->getWindow()->close();
}

void Game::renderMenu()
{
    _window->clear(sf::Color::Black);
    _window->getWindow()->draw(_titleSprite);
    for (const auto& option : _menuOptions) {
        _window->getWindow()->draw(option);
    }
    _window->getWindow()->draw(_menuSelector);
    _window->display();
}

void Game::initSettings()
{
    _settingsOptions.resize(3);
    std::vector<std::string> settingsTexts = {"CONTROLES", "TAILLE ECRAN", "RETOUR"};
    for (size_t i = 0; i < _settingsOptions.size(); ++i) {
        if (_font.getInfo().family != "") {
            _settingsOptions[i].setFont(_font);
        }
        _settingsOptions[i].setString(settingsTexts[i]);
        _settingsOptions[i].setCharacterSize(36);
        _settingsOptions[i].setFillColor(sf::Color::White);
        sf::FloatRect textBounds = _settingsOptions[i].getLocalBounds();
        _settingsOptions[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        _settingsOptions[i].setPosition(640, 300 + i * 80);
    }
    _keyBindingOptions.resize(7);
    std::vector<std::string> keyTexts = {"GAUCHE: ", "DROITE: ", "HAUT: ", "BAS: ", "MODE: ", "RESTART: ", "RETOUR"};
    for (size_t i = 0; i < _keyBindingOptions.size(); ++i) {
        if (_font.getInfo().family != "") {
            _keyBindingOptions[i].setFont(_font);
        }
        _keyBindingOptions[i].setCharacterSize(28);
        _keyBindingOptions[i].setFillColor(sf::Color::White);
        _keyBindingOptions[i].setPosition(350, 250 + i * 50);
    }
    _screenSizeOptions.resize(5);
    std::vector<std::string> sizeTexts = {"1280x720", "1920x1080", "2560x1440", "PLEIN ECRAN", "RETOUR"};
    for (size_t i = 0; i < _screenSizeOptions.size(); ++i) {
        if (_font.getInfo().family != "") {
            _screenSizeOptions[i].setFont(_font);
        }
        _screenSizeOptions[i].setString(sizeTexts[i]);
        _screenSizeOptions[i].setCharacterSize(36);
        _screenSizeOptions[i].setFillColor(sf::Color::White);
        sf::FloatRect textBounds = _screenSizeOptions[i].getLocalBounds();
        _screenSizeOptions[i].setOrigin(textBounds.width / 2, textBounds.height / 2);
        _screenSizeOptions[i].setPosition(640, 300 + i * 80);
    }
    updateKeyBindingDisplay();
}

void Game::handleSettingsKeyPressed(sf::Keyboard::Key key)
{
    if (_isInKeyBinding) {
        handleKeyBindingKeyPressed(key);
    } else if (_isInScreenSize) {
        handleScreenSizeKeyPressed(key);
    } else {
        if (key == sf::Keyboard::Up) {
            navigateSettings(-1);
        } else if (key == sf::Keyboard::Down) {
            navigateSettings(1);
        } else if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
            selectSettingsOption();
        } else if (key == sf::Keyboard::Escape) {
            backToMainMenu();
        }
    }
}

void Game::renderSettings()
{
    _window->clear(sf::Color::Black);
    if (_isInKeyBinding) {
        renderKeyBindingMenu();
    } else if (_isInScreenSize) {
        renderScreenSizeMenu();
    } else {
        sf::Text title;
        title.setString("PARAMETRES");
        title.setCharacterSize(48);
        title.setFillColor(sf::Color::Yellow);
        if (_font.getInfo().family != "") {
            title.setFont(_font);
        }
        sf::FloatRect titleBounds = title.getLocalBounds();
        title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
        title.setPosition(640, 150);
        _window->getWindow()->draw(title);
        for (const auto& option : _settingsOptions) {
            _window->getWindow()->draw(option);
        }
        _menuSelector.setPosition(640, 300 + static_cast<int>(_currentSettingsOption) * 80);
        _window->getWindow()->draw(_menuSelector);
    }
    _window->display();
}

void Game::navigateSettings(int direction)
{
    int currentOption = static_cast<int>(_currentSettingsOption);
    currentOption += direction;
    if (currentOption < 0) {
        currentOption = 2;
    } else if (currentOption > 2) {
        currentOption = 0;
    }
    _currentSettingsOption = static_cast<SettingsOption>(currentOption);
}

void Game::selectSettingsOption()
{
    switch (_currentSettingsOption) {
        case SettingsOption::CONTROLS:
            _isInKeyBinding = true;
            _currentKeyBindingOption = KeyBindingOption::LEFT;
            break;
        case SettingsOption::SCREEN_SIZE:
            _isInScreenSize = true;
            _currentScreenSizeOption = ScreenSizeOption::SIZE_1280x720;
            break;
        case SettingsOption::BACK:
            backToMainMenu();
            break;
    }
}

void Game::renderKeyBindingMenu()
{
    sf::Text title;
    title.setString("CONFIGURATION TOUCHES");
    title.setCharacterSize(42);
    title.setFillColor(sf::Color::Yellow);
    if (_font.getInfo().family != "") {
        title.setFont(_font);
    }
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    title.setPosition(640, 150);
    _window->getWindow()->draw(title);
    for (const auto& option : _keyBindingOptions) {
        _window->getWindow()->draw(option);
    }
    sf::RectangleShape selector;
    selector.setSize(sf::Vector2f(500, 40));
    selector.setFillColor(sf::Color(255, 255, 255, 50));
    selector.setOutlineColor(sf::Color::Yellow);
    selector.setOutlineThickness(2);
    selector.setPosition(320, 235 + static_cast<int>(_currentKeyBindingOption) * 50);
    _window->getWindow()->draw(selector);
}

void Game::handleKeyBindingKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Up) {
        navigateKeyBinding(-1);
    } else if (key == sf::Keyboard::Down) {
        navigateKeyBinding(1);
    } else if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
        selectKeyBindingOption();
    } else if (key == sf::Keyboard::Escape) {
        backToSettings();
    } else if (key == sf::Keyboard::F1) {
        resetToDefaultKeys();
    }
}

void Game::navigateKeyBinding(int direction)
{
    int currentOption = static_cast<int>(_currentKeyBindingOption);
    currentOption += direction;
    if (currentOption < 0) {
        currentOption = 6;
    } else if (currentOption > 6) {
        currentOption = 0;
    }
    _currentKeyBindingOption = static_cast<KeyBindingOption>(currentOption);
}

void Game::selectKeyBindingOption()
{
    if (_currentKeyBindingOption == KeyBindingOption::BACK) {
        backToSettings();
        return;
    }
    sf::Text waitText;
    waitText.setString("Appuyez sur une touche...");
    waitText.setCharacterSize(24);
    waitText.setFillColor(sf::Color::Red);
    if (_font.getInfo().family != "") {
        waitText.setFont(_font);
    }
    waitText.setPosition(350, 600);
    _window->clear(sf::Color::Black);
    renderKeyBindingMenu();
    _window->getWindow()->draw(waitText);
    _window->display();
    sf::Event event;
    while (_window->getWindow()->waitEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
            sf::Keyboard::Key newKey = event.key.code;
            if (newKey == sf::Keyboard::Escape) {
                break;
            }
            switch (_currentKeyBindingOption) {
                case KeyBindingOption::LEFT:
                    _leftKey = newKey;
                    break;
                case KeyBindingOption::RIGHT:
                    _rightKey = newKey;
                    break;
                case KeyBindingOption::UP:
                    _upKey = newKey;
                    break;
                case KeyBindingOption::DOWN:
                    _downKey = newKey;
                    break;
                case KeyBindingOption::TAB:
                    _tabKey = newKey;
                    break;
                case KeyBindingOption::RESTART:
                    _restartKey = newKey;
                    break;
                case KeyBindingOption::BACK:
                    break;
            }
            updateKeyBindingDisplay();
            break;
        }
    }
}

void Game::renderScreenSizeMenu()
{
    // Render title
    sf::Text title;
    title.setString("TAILLE ECRAN");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color::Yellow);
    if (_font.getInfo().family != "") {
        title.setFont(_font);
    }
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.width / 2, titleBounds.height / 2);
    title.setPosition(640, 150);
    _window->getWindow()->draw(title);
    for (const auto& option : _screenSizeOptions) {
        _window->getWindow()->draw(option);
    }
    _menuSelector.setPosition(640, 300 + static_cast<int>(_currentScreenSizeOption) * 80);
    _window->getWindow()->draw(_menuSelector);
}

void Game::handleScreenSizeKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::Up) {
        navigateScreenSize(-1);
    } else if (key == sf::Keyboard::Down) {
        navigateScreenSize(1);
    } else if (key == sf::Keyboard::Enter || key == sf::Keyboard::Space) {
        selectScreenSizeOption();
    } else if (key == sf::Keyboard::Escape) {
        backToSettings();
    }
}

void Game::navigateScreenSize(int direction)
{
    int currentOption = static_cast<int>(_currentScreenSizeOption);
    currentOption += direction;
    if (currentOption < 0) {
        currentOption = 4;
    } else if (currentOption > 4) {
        currentOption = 0;
    }
    _currentScreenSizeOption = static_cast<ScreenSizeOption>(currentOption);
}

void Game::selectScreenSizeOption()
{
    switch (_currentScreenSizeOption) {
        case ScreenSizeOption::SIZE_1280x720:
            changeWindowSize(1280, 720, false);
            break;
        case ScreenSizeOption::SIZE_1920x1080:
            changeWindowSize(1920, 1080, false);
            break;
        case ScreenSizeOption::SIZE_2560x1440:
            changeWindowSize(2560, 1440, false);
            break;
        case ScreenSizeOption::FULLSCREEN:
            changeWindowSize(0, 0, true);
            break;
        case ScreenSizeOption::BACK:
            backToSettings();
            break;
    }
}

void Game::changeWindowSize(int width, int height, bool fullscreen)
{
    if (fullscreen) {
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        _window->getWindow()->create(desktopMode, "Color Run", sf::Style::Fullscreen);
        _fullscreen = true;
        width = desktopMode.width;
        height = desktopMode.height;
    } else {
        if (_fullscreen) {
            _fullscreen = false;
        }
        _windowedSize = sf::Vector2u(width, height);
        _window->getWindow()->create(sf::VideoMode(width, height), "Color Run", sf::Style::Default);
    }
    _window->getWindow()->setFramerateLimit(60);
    setupEventHandlers();
    _cameraView.setSize(width, height);
    _cameraView.setCenter(width / 2.0f, height / 2.0f);
    recreateGameEntities();
    updateColorCirclesPositions();
    updateLivesDisplay();
}

void Game::resetToDefaultKeys()
{
    _leftKey = sf::Keyboard::Left;
    _rightKey = sf::Keyboard::Right;
    _upKey = sf::Keyboard::Up;
    _downKey = sf::Keyboard::Down;
    _tabKey = sf::Keyboard::Tab;
    _restartKey = sf::Keyboard::R;
    updateKeyBindingDisplay();
}

void Game::backToMainMenu()
{
    _gameState = GameState::MENU;
    _currentMenuOption = MenuOption::PLAY;
}

void Game::backToSettings()
{
    _isInKeyBinding = false;
    _isInScreenSize = false;
    _currentSettingsOption = SettingsOption::CONTROLS;
}

void Game::updateKeyBindingDisplay()
{
    std::vector<sf::Keyboard::Key> keys = {_leftKey, _rightKey, _upKey, _downKey, _tabKey, _restartKey};
    std::vector<std::string> keyNames = {"GAUCHE: ", "DROITE: ", "HAUT: ", "BAS: ", "MODE: ", "RESTART: ", "RETOUR"};
    for (size_t i = 0; i < keys.size(); ++i) {
        std::string keyStr = getKeyName(keys[i]);
        _keyBindingOptions[i].setString(keyNames[i] + keyStr);
    }
    _keyBindingOptions[6].setString("RETOUR (F1 = defaut)");
}

std::string Game::getKeyName(sf::Keyboard::Key key)
{
    switch (key) {
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::B: return "B";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::G: return "G";
        case sf::Keyboard::H: return "H";
        case sf::Keyboard::I: return "I";
        case sf::Keyboard::J: return "J";
        case sf::Keyboard::K: return "K";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::M: return "M";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::O: return "O";
        case sf::Keyboard::P: return "P";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::U: return "U";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::Y: return "Y";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::Left: return "FLECHE GAUCHE";
        case sf::Keyboard::Right: return "FLECHE DROITE";
        case sf::Keyboard::Up: return "FLECHE HAUT";
        case sf::Keyboard::Down: return "FLECHE BAS";
        case sf::Keyboard::Space: return "ESPACE";
        case sf::Keyboard::Tab: return "TAB";
        case sf::Keyboard::Enter: return "ENTREE";
        case sf::Keyboard::Escape: return "ECHAP";
        case sf::Keyboard::LShift: return "SHIFT GAUCHE";
        case sf::Keyboard::RShift: return "SHIFT DROITE";
        case sf::Keyboard::LControl: return "CTRL GAUCHE";
        case sf::Keyboard::RControl: return "CTRL DROITE";
        case sf::Keyboard::LAlt: return "ALT GAUCHE";
        case sf::Keyboard::RAlt: return "ALT DROITE";
        default: return "INCONNU";
    }
}

void Game::recreateGameEntities()
{
    if (_map) {
        _platforms.clear();
    } else {
        createTestPlatforms();
    }
    if (_map) {
        createEnemiesFromMap();
    }
}

Game::~Game()
{
    if (_backgroundMusic.getStatus() == sf::Music::Playing) {
        _backgroundMusic.stop();
    }
    _platforms.clear();  // Les plateformes utilisent _window->getWindow()
    _enemies.clear();    // Les ennemis utilisent _window->getWindow()
    _player.reset();     // Le joueur pourrait utiliser la fenêtre
    _map.reset();        // La carte pourrait utiliser la fenêtre
    _window.reset();
}

