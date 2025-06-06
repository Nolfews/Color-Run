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
    _enemyMode(false)
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
    _enemy = std::make_unique<Enemy>(100, 100, _colorState, _window->getWindow());
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
    if (_player) {
        _player->handleInput();
        _player->update(deltaTime);
        _player->checkPlatformCollisions(_platforms, _enemyMode);
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
    if (_enemy) {
        _enemy->draw(_enemyMode);
    }
    _window->getWindow()->setView(_window->getWindow()->getDefaultView());
    renderColorIndicators();
    renderColorOverlay();
    _window->getWindow()->draw(_modeIcon);
    if (_font.getInfo().family != "") {
        _window->getWindow()->draw(_modeText);
        _window->getWindow()->draw(_livesText);
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
}

Game::~Game()
{
    _platforms.clear();
    _player.reset();
    _enemy.reset();
    _map.reset();
    _window.reset();
}

