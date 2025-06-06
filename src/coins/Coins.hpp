/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** Coins
*/

#include <memory>
#include <SFML/Graphics.hpp>
#include "../player/player.hpp"

#ifndef COINS_HPP_
    #define COINS_HPP_

class Coins {
    public:
        Coins(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player);
        Coins(int x, int y, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player);
        Coins(int x, int y, int value, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Player> player);
        ~Coins();

        sf::Vector2f getPosition() const;
        sf::RectangleShape getShape() const;
        int getValue() const;
        bool isCollected() const;
        void setPosition(int x, int y);
        void setValue(int value);
        void setCollected(bool collected);
        void draw();

    private:
        int _x;
        int _y;
        int _value;
        bool _isCollected;
        sf::Clock _clock;
        sf::RectangleShape _shape;
        sf::Texture _texture;
        std::shared_ptr<sf::RenderWindow> _window;
        std::shared_ptr<Player> _player;
        void anim_coins();
        void checkPlayerCollision();
};

#endif /* !COINS_HPP_ */
