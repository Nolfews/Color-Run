/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Enemy
*/

#include <memory>
#include "Enemy.hpp"

Enemy::Enemy(std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window) : _x(0), _y(0), _colorState(colorState), _window(window)
{
    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color(34, 241, 53));
    _shape.setOutlineThickness(2);
    _shape.setOutlineColor(sf::Color(0, 0, 0));
    _shape.setPosition(_x, _y);
}

Enemy::Enemy(int x, int y, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window) : _x(x), _y(y), _colorState(colorState), _window(window)
{
    _shape.setSize(sf::Vector2f(64, 64));
    _shape.setFillColor(sf::Color(34, 241, 53));
    _shape.setOutlineThickness(2);
    _shape.setOutlineColor(sf::Color(0, 0, 0));
    _shape.setPosition(_x, _y);
}

Enemy::~Enemy()
{
}

sf::Vector2f Enemy::getPosition() const
{
    sf::Vector2f position(_x, _y);

    return position;
}

sf::RectangleShape Enemy::getShape() const
{
    return _shape;
}

sf::FloatRect Enemy::getBounds() const
{
    return _shape.getGlobalBounds();
}

void Enemy::setPosition(int x, int y)
{
    _x = x;
    _y = y;
    _shape.setPosition(_x, _y);
}

void Enemy::draw()
{

}

void Enemy::draw(bool enemyMode)
{
    if (enemyMode) {

        _window->draw(_shape);
    } else {
        return;
    }
}
