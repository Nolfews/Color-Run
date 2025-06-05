/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** player
*/

#pragma once

#include "../Color/Color.hpp"
#include <memory>
#include <SFML/Graphics.hpp>

class Player {
    public:
        Player();
        ~Player();
        std::shared_ptr<Color> getColor() const;
        void setColor(std::shared_ptr<Color> newColor);
        int getLife() const;
        void setLife(int newLife);
        int getScore() const;
        void setScore(int newScore);

        void applyGravity(float deltaTime);
        void move(int dx, int dy);
        void draw(sf::RenderWindow &window);
        void jump();
        void teleport(int x, int y);
        void update(float deltaTime);
        void handleInput();
    private:
        int _x;
        int _y;
        sf::Vector2f _velocity;
        sf::Vector2f _acceleration;
        sf::Vector2f _gravity;
        sf::Vector2f _maxVelocity;
        sf::RectangleShape _shape;
        sf::Texture _texture;
        int _life;
        std::shared_ptr<Color> _color;
        int _score;
        bool _isOnGround;
        bool _spacePressed;
};