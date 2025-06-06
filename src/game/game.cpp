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
    _gameOver(false)
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
    _cameraView.setSize(1280, 720);
    _cameraView.setCenter(640, 360);
    setupEventHandlers();
    loadLevel(_currentLevel);
    
    // Initialize Game Over text
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

        updateGame(deltaTime);

        renderGame();
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
    if (_gameOver) {
        if (key == sf::Keyboard::R) {
            // Restart the game
            _gameOver = false;
            if (_player) {
                _player->setLife(3);
                _player->setScore(0);
                resetPlayerToSpawn();
            }
        }
        return;
    }
    
    if (key == sf::Keyboard::Left) {
        cycleColor(-1);
    } else if (key == sf::Keyboard::Right) {
        cycleColor(1);
    } else if (key == sf::Keyboard::Up) {
        nextLevel();
    } else if (key == sf::Keyboard::Down) {
        previousLevel();
    } else if (key == sf::Keyboard::Tab) {
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
    _player = std::make_unique<Player>(100.0f, 550.0f);
    _player->setColor(_colorState);
    // Les ennemis seront créés par createEnemiesFromMap() après le chargement de la carte
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
        return; // Don't update game if it's over
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
        
    // Update lives display
    std::string livesStr = "Vies: " + std::to_string(_player->getLife());
    _livesText.setString(livesStr);
    float windowWidth = _window->getWindow()->getSize().x;
    if (_font.getInfo().family != "") {
        sf::FloatRect textBounds = _livesText.getLocalBounds();
        _livesText.setPosition(windowWidth - textBounds.width - 20, 10);
    } else {
        _livesText.setPosition(windowWidth - 150, 10);
    }
    
    // Update score display
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
        
        // Check collision between player and enemy
        if (playerBounds.intersects(enemyBounds)) {
            handlePlayerDeath();
            return; // Exit after first collision
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
    
    // Create semi-transparent overlay
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(_window->getWindow()->getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    _window->getWindow()->draw(overlay);
    
    // Position Game Over text
    sf::Vector2u windowSize = _window->getWindow()->getSize();
    sf::FloatRect gameOverBounds = _gameOverText.getLocalBounds();
    _gameOverText.setPosition(
        (windowSize.x - gameOverBounds.width) / 2.0f,
        (windowSize.y - gameOverBounds.height) / 2.0f - 80.0f
    );
    _window->getWindow()->draw(_gameOverText);
    
    // Position and display final score
    std::string scoreText = "Score Final: " + std::to_string(_player ? _player->getScore() : 0);
    _finalScoreText.setString(scoreText);
    sf::FloatRect scoreBounds = _finalScoreText.getLocalBounds();
    _finalScoreText.setPosition(
        (windowSize.x - scoreBounds.width) / 2.0f,
        (windowSize.y - scoreBounds.height) / 2.0f - 20.0f
    );
    _window->getWindow()->draw(_finalScoreText);
    
    // Add restart instruction
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
    
    // Calculate which tiles the player is overlapping
    int leftTile = static_cast<int>(playerBounds.left / 64);
    int rightTile = static_cast<int>((playerBounds.left + playerBounds.width) / 64);
    int topTile = static_cast<int>(playerBounds.top / 64);
    int bottomTile = static_cast<int>((playerBounds.top + playerBounds.height) / 64);
    
    // Ensure bounds are within map limits
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
                        // Mark tile as collected so it doesn't give points again
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
    
    // Clear existing enemies
    _enemies.clear();
    
    // Get enemy positions from map
    std::vector<sf::Vector2f> enemyPositions = _map->getEnemyPositions();
    
    // Create an enemy for each position
    for (const sf::Vector2f& pos : enemyPositions) {
        auto enemy = std::make_unique<Enemy>(static_cast<int>(pos.x), static_cast<int>(pos.y), 
                                           _colorState, _window->getWindow());
        _enemies.push_back(std::move(enemy));
    }
    
    std::cout << "Created " << _enemies.size() << " enemies from map" << std::endl;
}

Game::~Game()
{
    _platforms.clear();
    _enemies.clear();
    _player.reset();
    _map.reset();
    _window.reset();
}

