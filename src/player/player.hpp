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
#include <vector>

// Forward declaration
class Platform;

#define GROUND_Y 950.0f
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f
#define PLAYER_SIZE 64.0f

#define GRAVITY 800.0f
#define JUMP_FORCE -400.0f
#define MOVE_SPEED 250.0f
#define FRICTION 0.85f

class Player {
    public:
        // Constructeurs et destructeur
        Player(float startX = 100.0f, float startY = 900.0f);
        ~Player();
        
        // Getters et setters
        std::shared_ptr<Color> getColor() const;
        void setColor(std::shared_ptr<Color> newColor);
        int getLife() const;
        void setLife(int newLife);
        int getScore() const;
        void setScore(int newScore);
        sf::Vector2f getPosition() const;
        sf::FloatRect getBounds() const;
        bool isOnGround() const;

        // Méthodes principales
        void handleInput();
        void update(float deltaTime);
        void draw(sf::RenderWindow &window);
        void checkPlatformCollisions(const std::vector<std::unique_ptr<Platform>>& platforms);
        
        // Actions du joueur
        void jump();
        void takeDamage(int damage = 1);
        void addScore(int points);
        void teleport(float x, float y);
        void reset();

    private:
        // Position et mouvement
        sf::Vector2f _position;
        sf::Vector2f _velocity;
        sf::Vector2f _maxVelocity;
        
        // État du joueur
        bool _isOnGround;
        bool _spacePressed;
        int _life;
        int _score;
        std::shared_ptr<Color> _color;
        
        // Rendu
        sf::RectangleShape _shape;
        sf::Texture _texture;
        
        // Méthodes privées
        void applyGravity(float deltaTime);
        void applyFriction(float deltaTime);
        void updatePosition(float deltaTime);
        void checkBounds();
        void checkGroundCollision();
        void updateVisuals();
        void handlePlatformCollision(Platform* platform, const sf::FloatRect& platformBounds);
};