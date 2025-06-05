/*
** EPITECH PROJECT, 2025
** Color-Run
** File description:
** map
*/

#include "map.hpp"

Map::Map(const std::string &levelPath, unsigned int tileSize) : _tileSize(tileSize)
{
    initializeTileColors();
    loadFromFile(levelPath);
}

bool Map::loadFromFile(const std::string &levelPath)
{
    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << levelPath << std::endl;
        return false;
    }

    std::string line;
    int y = 0;
    _tiles.clear();

    while (std::getline(file, line)) {
        std::vector<Tile> row;
        for (size_t x = 0; x < line.size(); x++) {
            Tile tile;
            tile.type = charToTileType(line[x]);

            switch (tile.type) {
                case RED_TILE:     tile.color = RED; break;
                case GREEN_TILE:   tile.color = GREEN; break;
                case BLUE_TILE:    tile.color = BLUE; break;
                case YELLOW_TILE:  tile.color = YELLOW; break;
                case CYAN_TILE:    tile.color = CYAN; break;
                case MAGENTA_TILE: tile.color = MAGENTA; break;
                case WHITE_TILE:   tile.color = WHITE; break;
                case BLACK_TILE:   tile.color = BLACK; break;
                default:           tile.color = WHITE; break;
            }

            tile.shape.setSize(sf::Vector2f(_tileSize, _tileSize));
            tile.shape.setPosition(x * _tileSize, y * _tileSize);
            tile.shape.setFillColor(getTileColor(tile.type));

            if (tile.type != EMPTY) {
                tile.shape.setOutlineThickness(1.0f);
                tile.shape.setOutlineColor(sf::Color(30, 30, 30));
            }

            if (tile.type == SPAWN) {
                _spawnPosition = sf::Vector2f(x * _tileSize, y * _tileSize);
            }

            row.push_back(tile);
        }
        _tiles.push_back(row);
        y++;
    }

    file.close();
    return true;
}

void Map::draw(std::shared_ptr<sf::RenderWindow> window)
{
    for (const auto &row : _tiles) {
        for (const auto &tile : row) {
            if (tile.type != EMPTY) {
                window->draw(tile.shape);
            }
        }
    }
}

sf::Vector2f Map::getSpawnPosition() const
{
    return _spawnPosition;
}

std::vector<std::vector<Tile>> &Map::getTiles()
{
    return _tiles;
}

void Map::initializeTileColors()
{
    _tileColors[EMPTY]       = sf::Color::Transparent;
    _tileColors[SPAWN]       = sf::Color(50, 205, 50);    // Lime Green
    _tileColors[FINISH]      = sf::Color(255, 215, 0);    // Gold
    _tileColors[TRAP]        = sf::Color(139, 0, 0);      // Dark
    _tileColors[RED_TILE]    = sf::Color(255, 0, 0);     // Red
    _tileColors[GREEN_TILE]  = sf::Color(0, 255, 0);     // Green
    _tileColors[BLUE_TILE]   = sf::Color(0, 0, 255);     // Blue
    _tileColors[YELLOW_TILE] = sf::Color(255, 255, 0);   // Yellow
    _tileColors[CYAN_TILE]   = sf::Color(0, 255, 255);   // Cyan
    _tileColors[MAGENTA_TILE]= sf::Color(255, 0, 255);   // Magenta
    _tileColors[WHITE_TILE]  = sf::Color(255, 255, 255); // White
    _tileColors[BLACK_TILE]  = sf::Color(0, 0, 0);       // Black
}

TileType Map::charToTileType(char c)
{
    switch (c) {
        case '_': return EMPTY;
        case 'P': return SPAWN;
        case 'F': return FINISH;
        case 'Q': return TRAP;
        case '1': return RED_TILE;     // RED
        case '2': return GREEN_TILE;   // GREEN
        case '3': return BLUE_TILE;    // BLUE
        case '4': return YELLOW_TILE;  // YELLOW
        case '5': return CYAN_TILE;    // CYAN
        case '6': return MAGENTA_TILE; // MAGENTA
        case '7': return WHITE_TILE;   // WHITE
        case '8': return BLACK_TILE;   // BLACK
        default:  return EMPTY;
    }
}

sf::Color Map::getTileColor(TileType type)
{
    return _tileColors[type];
}

sf::Color Map::convertColor(Color_t color)
{
    switch (color) {
        case RED:     return sf::Color(255, 0, 0);
        case GREEN:   return sf::Color(0, 255, 0);
        case BLUE:    return sf::Color(0, 0, 255);
        case YELLOW:  return sf::Color(255, 255, 0);
        case CYAN:    return sf::Color(0, 255, 255);
        case MAGENTA: return sf::Color(255, 0, 255);
        case WHITE:   return sf::Color(255, 255, 255);
        case BLACK:   return sf::Color(0, 0, 0);
        default:      return sf::Color::White;
    }
}

Color_t Map::getTileColorEnum(int x, int y) const
{
    if (y >= 0 && y < static_cast<int>(_tiles.size())) {
        if (x >= 0 && x < static_cast<int>(_tiles[y].size())) {
            return _tiles[y][x].color;
        }
    }
    return WHITE;
}
