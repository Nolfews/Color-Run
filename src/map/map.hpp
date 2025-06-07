/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** map
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include "../Color/Color.hpp"

enum TileType {
    EMPTY,      // '_'
    SPAWN,      // 'P'
    FINISH,     // 'F'
    TRAP,       // 'Q'
    ENEMY,      // 'E'
    COIN,       // 'C'
    RED_TILE,   // '1' - RED
    GREEN_TILE, // '2' - GREEN
    BLUE_TILE,  // '3' - BLUE
    YELLOW_TILE,// '4' - YELLOW
    CYAN_TILE,  // '5' - CYAN
    MAGENTA_TILE,// '6' - MAGENTA
    WHITE_TILE, // '7' - WHITE
    BLACK_TILE, // '8' - BLACK
    INVISIBLE_BOUNDARY // '#' - Invisible boundary that acts like a trap
};

struct Tile {
    TileType type;
    Color_t color;
    sf::RectangleShape shape;
    sf::Sprite sprite;
    bool hasTexture = false;
};

class Map {
public:
    Map(const std::string &levelPath, unsigned int tileSize = 64);
    ~Map() = default;

    bool loadFromFile(const std::string &levelPath);
    void draw(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Color> colorState = nullptr, bool enemyMode = false);
    sf::Vector2f getSpawnPosition() const;
    std::vector<sf::Vector2f> getEnemyPositions() const;
    std::vector<sf::Vector2f> getCoinPositions() const;
    std::vector<std::vector<Tile>> &getTiles();
    Color_t getTileColorEnum(int x, int y) const;

    unsigned int getWidth() const;
    unsigned int getHeight() const;
    unsigned int getTileSize() const;
    
    // Méthodes pour les textures
    void setTileTextureAt(int x, int y, const sf::Texture& texture);
    void loadTileTextures(const std::string& texturesPath); // Pour charger des textures prédéfinies

private:
    void initializeTileColors();
    TileType charToTileType(char c);
    sf::Color getTileColor(TileType type);
    sf::Color convertColor(Color_t color);
    void setTileTexture(Tile& tile, const sf::Texture& texture); // Nouvelle méthode

    std::vector<std::vector<Tile>> _tiles;
    std::map<TileType, sf::Color> _tileColors;
    std::map<TileType, sf::Texture> _tileTextures; // Stockage des textures
    sf::Vector2f _spawnPosition;
    std::vector<sf::Vector2f> _enemyPositions;
    std::vector<sf::Vector2f> _coinPositions;
    unsigned int _tileSize;
};
