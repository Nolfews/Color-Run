/*
** EPITECH PROJECT, 2025
** Grrrrr
** File description:
** Coins
*/

#include <iostream>
#include <SFML/Audio.hpp>
#include "Coins.hpp"

Coins::Coins(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player) : _x(0), _y(0), _value(1), _isCollected(false), _window(window), _player(player)
{
    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color(255, 215, 0));
    _texture.loadFromFile("assets/img/coins.png", sf::IntRect(0, 0, 64, 64));
    _shape.setTexture(&_texture);
}

Coins::Coins(int x, int y, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player) : _x(x), _y(y), _value(1), _isCollected(false), _window(window), _player(player)
{
    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color::Yellow);
    _texture.loadFromFile("assets/img/coins.png", sf::IntRect(0, 0, 64, 64));
    _shape.setTexture(&_texture);
    _shape.setPosition(static_cast<float>(x), static_cast<float>(y));
}

Coins::Coins(int x, int y, int value, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player) : _x(x), _y(y), _value(value), _isCollected(false), _window(window), _player(player)
{
    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color::Yellow);
    _texture.loadFromFile("assets/img/coins.png", sf::IntRect(0, 0, 64, 64));
    _shape.setTexture(&_texture);
    _shape.setPosition(static_cast<float>(x), static_cast<float>(y));
}

Coins::~Coins()
{
}

sf::Vector2f Coins::getPosition() const
{
    sf::Vector2f position(static_cast<float>(_x), static_cast<float>(_y));

    return position;
}

sf::RectangleShape Coins::getShape() const
{
    return _shape;
}

int Coins::getValue() const
{
    return _value;
}

bool Coins::isCollected() const
{
    return _isCollected;
}

void Coins::setPosition(int x, int y)
{
    _x = x;
    _y = y;
    _shape.setPosition(static_cast<float>(x), static_cast<float>(y));
}

void Coins::setValue(int value)
{
    _value = value;
}

void Coins::setCollected(bool collected)
{
    _isCollected = collected;

    if (collected) {
        static sf::SoundBuffer buffer;
        static sf::Sound sound;
        static bool loaded = false;
        
        if (!loaded) {
            if (buffer.loadFromFile("assets/sounds/coin.mp3")) {
                sound.setBuffer(buffer);
                loaded = true;
            }
        }
        
        if (loaded) {
            sound.setVolume(100);
            sound.play();
        }
        _player->addScore(_value);
    }
}

void Coins::draw()
{
    if (_window && !_isCollected) {
        checkPlayerCollision();
        anim_coins();
        _window->draw(_shape);
    }
}

void Coins::anim_coins()
{
    sf::Time elapsed = _clock.getElapsedTime();
    float f = elapsed.asSeconds();

    if (f < 0.2f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(0, 0, 64, 64));
    } else if (f < 0.4f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(64, 0, 64, 64));
    } else if (f < 0.6f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(128, 0, 64, 64));
    } else if (f < 0.8f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(192, 0, 64, 64));
    } else if (f < 1.0f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(256, 0, 64, 64));
    } else if (f < 1.2f) {
        _texture.loadFromFile("assets/img/coins.png", sf::IntRect(320, 0, 64, 64));
    } else {
        _clock.restart();
    }
}

void Coins::checkPlayerCollision()
{
    if (!_window || !_window->getView().getCenter().x || !_window->getView().getCenter().y)
        return;

    if (_player) {
        sf::FloatRect playerBounds = _player->getBounds();
        sf::FloatRect coinBounds = _shape.getGlobalBounds();

        if (coinBounds.intersects(playerBounds)) {
            setCollected(true);
            // _player->addScore(_value);
            _clock.restart();
        }
    }
}
