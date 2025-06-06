/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Enemy
*/

#include <memory>
#include <SFML/Graphics.hpp>
#include "../Color/Color.hpp"

#ifndef ENEMY_HPP_
    #define ENEMY_HPP_

class Enemy {
    public:
        Enemy(std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window);
        Enemy(int x, int y, std::shared_ptr<Color> colorState, std::shared_ptr<sf::RenderWindow> window);
        ~Enemy();

        sf::Vector2f getPosition() const;
        sf::RectangleShape getShape() const;
        void setPosition(int x, int y);
        void draw();
        void draw(bool enemyMode);

    private:
        int _x;
        int _y;
        sf::RectangleShape _shape;
        std::shared_ptr<Color> _colorState;
        std::shared_ptr<sf::RenderWindow> _window;
};

#endif /* !ENEMY_HPP_ */
