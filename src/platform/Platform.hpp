/*
** EPITECH PROJECT, 2025
** Platform
** File description:
** Platform
*/

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "../Color/Color.hpp"

#ifndef PLATFORM_HPP_
    #define PLATFORM_HPP_

class Platform {
    public:
        Platform(Color_t color, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window);
        Platform(int x, int y, Color_t color, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window);
        ~Platform();

        sf::Vector2f getPosition() const;
        sf::RectangleShape getShape() const;
        sf::FloatRect getBounds() const;
        void setPosition(int x, int y);
        void draw();
        void draw(bool enemyMode);
        bool shouldCollideWithPlayer() const;
        bool shouldCollideWithPlayer(bool enemyMode) const;
        Color_t getColor() const;

    private:
        int _x;
        int _y;
        Color_t _color;
        sf::Texture _texture;
        sf::RectangleShape _platform;
        std::shared_ptr<Color> _colorState;
        std::shared_ptr<sf::RenderWindow> _window;
        void setColor(Color_t color);
};

#endif /* !PLATFORM_HPP_ */
