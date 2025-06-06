/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Platform
*/

#include "Platform.hpp"

Platform::Platform(Color_t color, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window) : _x(0), _y(0), _color(color), _colorState(colorState), _window(window)
{
    _platform.setSize(sf::Vector2f(64, 64));
    _platform.setOutlineThickness(2);
    _platform.setOutlineColor(sf::Color(0, 0, 0));
    _platform.setPosition(_x, _y);
    setColor(color);
}

Platform::Platform(int x, int y, Color_t color, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window) : _x(x), _y(y), _color(color), _colorState(colorState), _window(window)
{
    _platform.setSize(sf::Vector2f(64, 64));
    _platform.setOutlineThickness(2);
    _platform.setOutlineColor(sf::Color(0, 0, 0));
    _platform.setPosition(_x, _y);
    setColor(color);
}

Platform::~Platform()
{
}

sf::Vector2f Platform::getPosition() const
{
    return sf::Vector2f(_x, _y);
}

sf::RectangleShape Platform::getShape() const
{
    return _platform;
}

sf::FloatRect Platform::getBounds() const
{
    return _platform.getGlobalBounds();
}

void Platform::setPosition(int x, int y)
{
    _x = x;
    _y = y;
    _platform.setPosition(_x, _y);
}

void Platform::draw()
{
    if (!(_colorState->getColor() == _color || _color == BLACK || _color == WHITE)) {
        return;
    }
    sf::Color currentColor = _platform.getFillColor();
    currentColor.a = 255;
    _platform.setFillColor(currentColor);
    _window->draw(_platform);
}

bool Platform::shouldCollideWithPlayer() const
{
    if (_color == BLACK || _color == WHITE) {
        return true;
    }
    return _colorState->getColor() == _color;
}

Color_t Platform::getColor() const
{
    return _color;
}

void Platform::setColor(Color_t color)
{
    _color = color;
    switch (color) {
        case RED:
            _platform.setFillColor(sf::Color(255, 0, 0));
            break;
        case GREEN:
            _platform.setFillColor(sf::Color(0, 255, 0));
            break;
        case BLUE:
            _platform.setFillColor(sf::Color(0, 0, 255));
            break;
        case YELLOW:
            _platform.setFillColor(sf::Color(255, 255, 0));
            break;
        case CYAN:
            _platform.setFillColor(sf::Color(0, 255, 255));
            break;
        case MAGENTA:
            _platform.setFillColor(sf::Color(255, 0, 255));
            break;
        case WHITE:
            _platform.setFillColor(sf::Color(255, 255, 255));
            break;
        case BLACK:
            _platform.setFillColor(sf::Color(0, 0, 0));
            break;
        default:
            _platform.setFillColor(sf::Color(128, 128, 128));
    }
}