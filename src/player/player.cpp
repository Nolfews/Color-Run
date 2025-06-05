/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** player
*/

#include "player.hpp"

Player::Player()
{
    _x = 0;
    _y = 0;
    _life = 3;
    _score = 0;
    _color = std::make_shared<Color>();
    _isOnGround = false;
    _spacePressed = false;

    _velocity = sf::Vector2f(0, 0);
    _acceleration = sf::Vector2f(0, 0);
    _gravity = sf::Vector2f(0, 500.0f);
    _maxVelocity = sf::Vector2f(300.0f, 600.0f);

    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color::White);
    _shape.setPosition(static_cast<float>(_x), static_cast<float>(_y));
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

void Player::applyGravity(float deltaTime)
{
    _velocity += _gravity * deltaTime;
}

void Player::move(int dx, int dy)
{
    _x += dx;
    _y += dy;
    _shape.setPosition(static_cast<float>(_x), static_cast<float>(_y));
    // texture
}

void Player::draw(sf::RenderWindow &window)
{
    window.draw(_shape);
}

void Player::jump()
{
    if (_isOnGround) {
        _velocity.y = -350.0f;
        _isOnGround = false;
    }
}

void Player::teleport(int x, int y)
{
    _x = x;
    _y = y;
    _shape.setPosition(static_cast<float>(_x), static_cast<float>(_y));
    // texture
}

void Player::handleInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        _velocity.x = -200.0f;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        _velocity.x = 200.0f;
    } else {
        _velocity.x *= 0.9f;
    }

    bool spaceCurrentlyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceCurrentlyPressed && !_spacePressed) {
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

    _x += static_cast<int>(_velocity.x * deltaTime);
    _y += static_cast<int>(_velocity.y * deltaTime);

    // je laisse les bordures de l'écran pour le player
    if (_y >= 1016) {
        _y = 1016;
        _velocity.y = 0;
        _isOnGround = true;
    } else if (_y < 1016 && _velocity.y > 0) {
        _isOnGround = false;
    }

    if (_x < 0) {
        _x = 0;
        _velocity.x = 0;
    } else if (_x > 1856) {
        _x = 1856;
        _velocity.x = 0;
    }

    _shape.setPosition(static_cast<float>(_x), static_cast<float>(_y));
}
