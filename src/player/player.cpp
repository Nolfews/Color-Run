/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** player
*/

#include "player.hpp"
#include <iostream>

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
    _shape.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    _shape.setFillColor(sf::Color::White);
    _shape.setPosition(_position);
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        _velocity.x = -MOVE_SPEED;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        _velocity.x = MOVE_SPEED;
    } else {
        applyFriction(1.0f / 60.0f);
    }

    bool spaceCurrentlyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceCurrentlyPressed && !_spacePressed && _isOnGround) {
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
            case Color_t::WHITE:
            default:
                _shape.setFillColor(sf::Color::White);
                break;
        }
    }
}
