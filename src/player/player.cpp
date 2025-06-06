/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** player
*/

#include "player.hpp"
#include "../platform/Platform.hpp"
#include <iostream>
#include <algorithm>
#include <SFML/Audio.hpp>

Player::Player(float startX, float startY)
    : _position(startX, startY)
    , _velocity(0.0f, 0.0f)
    , _maxVelocity(MOVE_SPEED, 600.0f)
    , _isOnGround(false)
    , _spacePressed(false)
    , _life(3)
    , _score(0)
    , _color(std::make_shared<Color>())
{
    _texture.loadFromFile("assets/img/stand.png");
    _shape.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    _shape.setFillColor(sf::Color::White);
    _shape.setTexture(&_texture);
    _shape.setPosition(_position);
    _soundBuffer.loadFromFile("assets/sounds/jump.ogg");
    _jumpSound.setBuffer(_soundBuffer);
}

Player::~Player()
{
}

std::shared_ptr<Color> Player::getColor() const
{
    return _color;
}

void Player::setColor(std::shared_ptr<Color> newColor)
{
    _color = newColor;
}

int Player::getLife() const
{
    return _life;
}

void Player::setLife(int newLife)
{
    _life = newLife;
}

int Player::getScore() const
{
    return _score;
}

void Player::setScore(int newScore)
{
    _score = newScore;
}

sf::Vector2f Player::getPosition() const
{
    return _position;
}

sf::FloatRect Player::getBounds() const
{
    return _shape.getGlobalBounds();
}

bool Player::isOnGround() const
{
    return _isOnGround;
}

void Player::handleInput()
{
    bool movingLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool movingRight = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool facingRight = _shape.getScale().x > 0;

    if (movingLeft) {
        _velocity.x = -MOVE_SPEED;
        if (facingRight) {
            _position.x += PLAYER_SIZE;
            _shape.setScale(-1.0f, 1.0f);
        }
        if (_isOnGround) {
            handleMovement();
        } else {
            handleAerialMovement();
        }
    } else if (movingRight) {
        _velocity.x = MOVE_SPEED;
        if (!facingRight) {
            _position.x -= PLAYER_SIZE;
            _shape.setScale(1.0f, 1.0f);
        }
        if (_isOnGround) {
            handleMovement();
        } else {
            handleAerialMovement();
        }
    } else {
        applyFriction(1.0f / 60.0f);
        if (_isOnGround) {
            _texture.loadFromFile("assets/img/stand.png");
            _clock.restart();
        } else {
            handleAerialMovement();
        }
    }

    bool spaceCurrentlyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceCurrentlyPressed && !_spacePressed && _isOnGround) {
        _jumpSound.play();
        jump();
    }
    _spacePressed = spaceCurrentlyPressed;
}

void Player::update(float deltaTime)
{
    if (!_isOnGround) {
        applyGravity(deltaTime);
    }

    if (_velocity.x > _maxVelocity.x) {
        _velocity.x = _maxVelocity.x;
    } else if (_velocity.x < -_maxVelocity.x) {
        _velocity.x = -_maxVelocity.x;
    }
    if (_velocity.y > _maxVelocity.y) {
        _velocity.y = _maxVelocity.y;
    } else if (_velocity.y < -_maxVelocity.y) {
        _velocity.y = -_maxVelocity.y;
    }

    updatePosition(deltaTime);
    checkBounds();
    checkGroundCollision();
    updateVisuals();
}

void Player::draw(sf::RenderWindow &window)
{
    window.draw(_shape);
}

void Player::jump()
{
    if (_isOnGround) {
        _velocity.y = JUMP_FORCE;
        _isOnGround = false;
    }
}

void Player::takeDamage(int damage)
{
    _life -= damage;
    if (_life < 0) {
        _life = 0;
    }
}

void Player::addScore(int points)
{
    _score += points;
}

void Player::teleport(float x, float y)
{
    _position.x = x;
    _position.y = y;
    updateVisuals();
}

void Player::reset()
{
    _position = sf::Vector2f(100.0f, 900.0f);
    _velocity = sf::Vector2f(0.0f, 0.0f);
    _life = 3;
    _score = 0;
    _isOnGround = false;
    _spacePressed = false;
    updateVisuals();
}

void Player::setGroundState(bool onGround)
{
    _isOnGround = onGround;
}

void Player::applyGravity(float deltaTime)
{
    _velocity.y += GRAVITY * deltaTime;
}

void Player::applyFriction(float deltaTime)
{
    _velocity.x *= FRICTION;

    if (std::abs(_velocity.x) < 1.0f) {
        _velocity.x = 0.0f;
    }
}

void Player::updatePosition(float deltaTime)
{
    _position += _velocity * deltaTime;
}

void Player::checkBounds()
{
    if (_position.x < 0) {
        _position.x = 0;
        _velocity.x = 0;
    } else if (_position.x > SCREEN_WIDTH - PLAYER_SIZE) {
        _position.x = SCREEN_WIDTH - PLAYER_SIZE;
        _velocity.x = 0;
    }
}

void Player::checkGroundCollision()
{
    if (_position.y >= GROUND_Y) {
        _position.y = GROUND_Y;
        _velocity.y = 0;
        _isOnGround = true;
    } else if (_position.y < GROUND_Y && _velocity.y > 0) {
        _isOnGround = false;
    }
}

void Player::updateVisuals()
{
    _shape.setPosition(_position);
    if (_color) {
        switch (_color->getColor()) {
            case Color_t::RED:
                _shape.setFillColor(sf::Color::Red);
                break;
            case Color_t::GREEN:
                _shape.setFillColor(sf::Color::Green);
                break;
            case Color_t::BLUE:
                _shape.setFillColor(sf::Color::Blue);
                break;
            case Color_t::YELLOW:
                _shape.setFillColor(sf::Color::Yellow);
                break;
            case Color_t::CYAN:
                _shape.setFillColor(sf::Color::Cyan);
                break;
            case Color_t::MAGENTA:
                _shape.setFillColor(sf::Color::Magenta);
                break;
            case Color_t::WHITE:
                _shape.setFillColor(sf::Color::White);
                break;
            case Color_t::BLACK:
                _shape.setFillColor(sf::Color::Black);
                break;
            default:
                _shape.setFillColor(sf::Color::White);
                break;
        }
    }
}

void Player::checkPlatformCollisions(const std::vector<std::unique_ptr<Platform>>& platforms)
{
    sf::FloatRect playerBounds = getBounds();

    float searchRadius = PLAYER_SIZE * 2.0f;
    sf::FloatRect searchArea(
        _position.x - searchRadius,
        _position.y - searchRadius,
        PLAYER_SIZE + 2 * searchRadius,
        PLAYER_SIZE + 2 * searchRadius
    );
    for (const auto& platform : platforms) {
        if (!platform) continue;
        sf::FloatRect platformBounds = platform->getBounds();
        if (!searchArea.intersects(platformBounds)) {
            continue;
        }
        if (!platform->shouldCollideWithPlayer()) {
            continue;
        }
        if (playerBounds.intersects(platformBounds)) {
            handlePlatformCollision(platform.get(), platformBounds);
        }
    }
}

void Player::checkPlatformCollisions(const std::vector<std::unique_ptr<Platform>>& platforms, bool enemyMode)
{
    sf::FloatRect playerBounds = getBounds();

    float searchRadius = PLAYER_SIZE * 2.0f;
    sf::FloatRect searchArea(
        _position.x - searchRadius,
        _position.y - searchRadius,
        PLAYER_SIZE + 2 * searchRadius,
        PLAYER_SIZE + 2 * searchRadius
    );
    for (const auto& platform : platforms) {
        if (!platform) continue;
        sf::FloatRect platformBounds = platform->getBounds();
        if (!searchArea.intersects(platformBounds)) {
            continue;
        }
        if (!platform->shouldCollideWithPlayer(enemyMode)) {
            continue;
        }
        if (playerBounds.intersects(platformBounds)) {
            handlePlatformCollision(platform.get(), platformBounds);
        }
    }
}

void Player::handlePlatformCollision(Platform* platform, const sf::FloatRect& platformBounds)
{
    sf::FloatRect playerBounds = getBounds();
    float overlapLeft = (playerBounds.left + playerBounds.width) - platformBounds.left;
    float overlapRight = (platformBounds.left + platformBounds.width) - playerBounds.left;
    float overlapTop = (playerBounds.top + playerBounds.height) - platformBounds.top;
    float overlapBottom = (platformBounds.top + platformBounds.height) - playerBounds.top;
    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
    if (minOverlap == overlapTop && _velocity.y > 0) {
        _position.y = platformBounds.top - PLAYER_SIZE;
        _velocity.y = 0;
        _isOnGround = true;
    } else if (minOverlap == overlapBottom && _velocity.y < 0) {
        _position.y = platformBounds.top + platformBounds.height;
        _velocity.y = 0;
    } else if (minOverlap == overlapLeft && _velocity.x > 0) {
        _position.x = platformBounds.left - PLAYER_SIZE;
        _velocity.x = 0;
    } else if (minOverlap == overlapRight && _velocity.x < 0) {
        _position.x = platformBounds.left + platformBounds.width;
        _velocity.x = 0;
    }
}

void Player::handleMovement()
{
    sf::Time elapsed = _clock.getElapsedTime();
    float f = elapsed.asSeconds();

    if (f < 0.2f) {
        _texture.loadFromFile("assets/img/walk.png", sf::IntRect(0, 0, 64, 64));
    } else if (f < 0.4f) {
        _texture.loadFromFile("assets/img/walk.png", sf::IntRect(64, 0, 64, 64));
    } else if (f < 0.6f) {
        _texture.loadFromFile("assets/img/walk.png", sf::IntRect(128, 0, 64, 64));
    } else if (f < 0.8f) {
        _texture.loadFromFile("assets/walk.png", sf::IntRect(192, 0, 64, 64));
    } else if (f < 1.0f) {
        _texture.loadFromFile("assets/img/walk.png", sf::IntRect(256, 0, 64, 64));
    } else if (f < 1.2f) {
        _texture.loadFromFile("assets/img/walk.png", sf::IntRect(320, 0, 64, 64));
    } else {
        _clock.restart();
    }
}

void Player::handleAerialMovement()
{
    if (_velocity.y < -200.0f) {
        _texture.loadFromFile("assets/img/jump.png", sf::IntRect(0, 0, 64, 64));
    } else if (_velocity.y < -50.0f) {
        _texture.loadFromFile("assets/img/jump.png", sf::IntRect(64, 0, 64, 64));
    } else if (std::abs(_velocity.y) < 50.0f) {
        _texture.loadFromFile("assets/img/jump.png", sf::IntRect(128, 0, 64, 64));
    } else if (_velocity.y > 50.0f && _velocity.y < 300.0f) {
        _texture.loadFromFile("assets/img/jump.png", sf::IntRect(192, 0, 64, 64));
    } else if (_velocity.y >= 300.0f) {
        _texture.loadFromFile("assets/img/jump.png", sf::IntRect(256, 0, 64, 64));
    }
}
