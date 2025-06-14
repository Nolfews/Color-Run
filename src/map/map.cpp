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
    
    // Charger la texture des traps
    if (!_tileTextures[TRAP].loadFromFile("assets/img/trap.png")) {
        std::cerr << "Warning: Could not load trap texture from assets/img/trap.png" << std::endl;
    }
    
    // Charger la texture des plateformes colorées (sauf noir et blanc)
    sf::Texture platformTexture;
    if (!platformTexture.loadFromFile("assets/img/platforme.png")) {
        std::cerr << "Warning: Could not load platform texture from assets/img/platforme.png" << std::endl;
    } else {
        std::cout << "Platform texture loaded successfully!" << std::endl;
        // Charger la même texture pour toutes les plateformes colorées (sauf noir et blanc)
        _tileTextures[RED_TILE].loadFromFile("assets/img/platforme.png");
        _tileTextures[GREEN_TILE].loadFromFile("assets/img/platforme.png");
        _tileTextures[BLUE_TILE].loadFromFile("assets/img/platforme.png");
        _tileTextures[YELLOW_TILE].loadFromFile("assets/img/platforme.png");
        _tileTextures[CYAN_TILE].loadFromFile("assets/img/platforme.png");
        _tileTextures[MAGENTA_TILE].loadFromFile("assets/img/platforme.png");
        std::cout << "All platform textures loaded!" << std::endl;
    }

    if (!loadFromFile(levelPath)) {
        std::cerr << "Error loading level file: " << levelPath << std::endl;
    }
}

bool Map::loadFromFile(const std::string &levelPath)
{
    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file " << levelPath << std::endl;
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
                default:           tile.color = TRANSPARENT; break;
            }

            tile.shape.setSize(sf::Vector2f(_tileSize, _tileSize));
            tile.shape.setPosition(x * _tileSize, y * _tileSize);
            tile.shape.setFillColor(getTileColor(tile.type));

            tile.sprite.setPosition(x * _tileSize, y * _tileSize);
            tile.hasTexture = false;

            // Modified condition to exclude SPAWN from having an outline
            if (tile.type != EMPTY && tile.type != INVISIBLE_BOUNDARY && tile.type != SPAWN) {
                tile.shape.setOutlineThickness(1.0f);
                tile.shape.setOutlineColor(sf::Color(30, 30, 30));
            }

            if (tile.type == SPAWN) {
                _spawnPosition = sf::Vector2f(x * _tileSize, y * _tileSize);
                // Make sure spawn tiles are completely transparent
                tile.shape.setFillColor(sf::Color::Transparent);
                tile.shape.setOutlineThickness(0.0f);
            }

            if (tile.type == ENEMY) {
                _enemyPositions.push_back(sf::Vector2f(x * _tileSize, y * _tileSize));
            }

            if (tile.type == COIN) {
                _coinPositions.push_back(sf::Vector2f(x * _tileSize, y * _tileSize));
            }

            // Appliquer la texture aux traps si elle est chargée
            if (tile.type == TRAP && _tileTextures.find(TRAP) != _tileTextures.end()) {
                setTileTexture(tile, _tileTextures[TRAP]);
            }

            // Appliquer la texture aux plateformes colorées (sauf noir et blanc) si elle est chargée
            if ((tile.type == RED_TILE || tile.type == GREEN_TILE || tile.type == BLUE_TILE ||
                 tile.type == YELLOW_TILE || tile.type == CYAN_TILE || tile.type == MAGENTA_TILE) && 
                 _tileTextures.find(tile.type) != _tileTextures.end()) {
                setTileTexture(tile, _tileTextures[tile.type]);
            }

            row.push_back(tile);
        }
        _tiles.push_back(row);
        y++;
    }

    file.close();
    return true;
}

void Map::draw(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<Color> colorState, bool enemyMode)
{
    for (const auto &row : _tiles) {
        for (const auto &tile : row) {
            bool shouldDraw = false;

            if (tile.type != EMPTY && tile.type != INVISIBLE_BOUNDARY && tile.type != COIN) {
                if (tile.type == SPAWN || tile.type == FINISH) {
                    shouldDraw = true;
                } else if (tile.type == TRAP) {
                    shouldDraw = enemyMode;
                } else if (enemyMode) {
                    shouldDraw = (tile.color == BLACK || tile.color == WHITE);
                } else if (colorState) {
                    shouldDraw = (tile.color == colorState->getColor() || tile.color == BLACK || tile.color == WHITE);
                } else {
                    shouldDraw = true;
                }
                if (shouldDraw) {
                    window->draw(tile.shape);
                    // Dessiner la texture par-dessus si elle existe
                    if (tile.hasTexture) {
                        window->draw(tile.sprite);
                    }
                }
            }
        }
    }
}

sf::Vector2f Map::getSpawnPosition() const
{
    return _spawnPosition;
}

std::vector<sf::Vector2f> Map::getEnemyPositions() const
{
    return _enemyPositions;
}

std::vector<sf::Vector2f> Map::getCoinPositions() const
{
    return _coinPositions;
}

std::vector<std::vector<Tile>> &Map::getTiles()
{
    return _tiles;
}

void Map::initializeTileColors()
{
    _tileColors[EMPTY]       = sf::Color::Transparent;
    _tileColors[SPAWN]       = sf::Color::Transparent;
    _tileColors[FINISH]      = sf::Color(255, 215, 0);    // Gold
    _tileColors[TRAP]        = sf::Color(139, 0, 0);      // Dark Red
    _tileColors[ENEMY]       = sf::Color(34, 241, 53);    // Green (like Enemy)
    _tileColors[COIN]        = sf::Color(255, 215, 0);    // Gold (like coins)
    _tileColors[RED_TILE]    = sf::Color(255, 0, 0);     // Red
    _tileColors[GREEN_TILE]  = sf::Color(0, 255, 0);     // Green
    _tileColors[BLUE_TILE]   = sf::Color(0, 0, 255);     // Blue
    _tileColors[YELLOW_TILE] = sf::Color(255, 255, 0);   // Yellow
    _tileColors[CYAN_TILE]   = sf::Color(0, 255, 255);   // Cyan
    _tileColors[MAGENTA_TILE]= sf::Color(255, 0, 255);   // Magenta
    _tileColors[WHITE_TILE]  = sf::Color(255, 255, 255); // White
    _tileColors[BLACK_TILE]  = sf::Color(0, 0, 0);       // Black
    _tileColors[INVISIBLE_BOUNDARY] = sf::Color::Transparent; // Invisible comme EMPTY
}

TileType Map::charToTileType(char c)
{
    switch (c) {
        case '_': return EMPTY;
        case 'P': return SPAWN;
        case 'F': return FINISH;
        case 'Q': return TRAP;
        case 'E': return ENEMY;
        case 'C': return COIN;         // COIN
        case '1': return RED_TILE;     // RED
        case '2': return GREEN_TILE;   // GREEN
        case '3': return BLUE_TILE;    // BLUE
        case '4': return YELLOW_TILE;  // YELLOW
        case '5': return CYAN_TILE;    // CYAN
        case '6': return MAGENTA_TILE; // MAGENTA
        case '7': return WHITE_TILE;   // WHITE
        case '8': return BLACK_TILE;   // BLACK
        case '#': return INVISIBLE_BOUNDARY; // Limite invisible
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
        default:      return sf::Color::Transparent;
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

void Map::setTileTexture(Tile& tile, const sf::Texture& texture)
{
    tile.sprite.setTexture(texture);
    
    // Adapter la taille du sprite à la taille de la tile
    sf::Vector2u textureSize = texture.getSize();
    float scaleX = static_cast<float>(_tileSize) / textureSize.x;
    float scaleY = static_cast<float>(_tileSize) / textureSize.y;
    tile.sprite.setScale(scaleX, scaleY);
    
    tile.hasTexture = true;
}

void Map::setTileTextureAt(int x, int y, const sf::Texture& texture)
{
    if (y >= 0 && y < static_cast<int>(_tiles.size())) {
        if (x >= 0 && x < static_cast<int>(_tiles[y].size())) {
            setTileTexture(_tiles[y][x], texture);
        }
    }
}

void Map::loadTileTextures(const std::string& texturesPath)
{
    // Exemple : charger des textures pour certains types de tiles
    // Vous pouvez adapter selon vos besoins
    
    // Exemple pour COIN
    if (_tileTextures.find(COIN) == _tileTextures.end()) {
        if (_tileTextures[COIN].loadFromFile(texturesPath + "/coins.png")) {
            // Appliquer la texture à toutes les tiles COIN existantes
            for (auto& row : _tiles) {
                for (auto& tile : row) {
                    if (tile.type == COIN) {
                        setTileTexture(tile, _tileTextures[COIN]);
                    }
                }
            }
        }
    }
}

unsigned int Map::getWidth() const
{
    if (_tiles.empty()) {
        return 0;
    }
    unsigned int maxWidth = 0;
    for (const auto& row : _tiles) {
        if (row.size() > maxWidth) {
            maxWidth = row.size();
        }
    }
    return maxWidth * _tileSize;
}

unsigned int Map::getHeight() const
{
    return _tiles.size() * _tileSize;
}

unsigned int Map::getTileSize() const
{
    return _tileSize;
}
