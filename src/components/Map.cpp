#include "components/Map.h"
#include <fstream>
#include <iostream>

TileType Map::charToTile(char c) const {
    switch (c) {
        case '#': return TileType::WALL;
        case 'L': return TileType::LIGHT_SOURCE;
        case '/': return TileType::MIRROR;
        case 'S': return TileType::START;
        case 'E': return TileType::END;
        default:  return TileType::EMPTY;
    }
}

bool Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Map::loadFromFile - failed to open: " << filename << std::endl;
        return false;
    }

    grid.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<TileType> row;
        row.reserve(line.size());
        for (char c : line) row.push_back(charToTile(c));
        grid.push_back(std::move(row));
    }

    height = static_cast<int>(grid.size());
    width = height > 0 ? static_cast<int>(grid[0].size()) : 0;

    std::cout << "Map loaded: " << filename << " (" << width << "x" << height << ")\n";
    return true;
}

TileType Map::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return TileType::WALL;
    return grid[y][x];
}

void Map::setTextures(const sf::Texture& wallTex,
                      const sf::Texture& lightTex,
                      const sf::Texture& mirrorTex,
                      const sf::Texture& startTex,
                      const sf::Texture& endTex,
                      const sf::Texture& emptyTex) {
    wallTexture = &wallTex;
    lightTexture = &lightTex;
    mirrorTexture = &mirrorTex;
    startTexture = &startTex;
    exitTexture = &endTex;
    emptyTexture = &emptyTex;
}

void Map::draw(sf::RenderWindow& window, int tileSize) const {
    if (width == 0 || height == 0) return;
    for (int y=0;y<height;++y) {
        for (int x=0;x<width;++x) {
            const sf::Texture* tex = nullptr;
            switch (getTile(x,y)) {
                case TileType::WALL: tex = wallTexture; break;
                case TileType::LIGHT_SOURCE: tex = lightTexture; break;
                case TileType::MIRROR: tex = mirrorTexture; break;
                case TileType::START: tex = startTexture; break;
                case TileType::END: tex = exitTexture; break;
                case TileType::EMPTY: default: tex = emptyTexture; break;
            }
            if (tex) {
                sf::Sprite s(*tex);
                s.setPosition(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize));
                window.draw(s);
            }
        }
    }
}

bool Map::isWalkableTile(TileType t) const {
    switch (t) {
        case TileType::WALL: return false;
        default: return true;
    }
}

bool Map::isWalkableTileCoord(int tx, int ty) const {
    if (tx < 0 || tx >= width || ty < 0 || ty >= height) return false;
    return isWalkableTile(getTile(tx, ty));
}

bool Map::isWalkableWorld(float worldx, float worldy, int tileSize) const {
    int tx = static_cast<int>(worldx) / tileSize;
    int ty = static_cast<int>(worldy) / tileSize;
    return isWalkableTileCoord(tx, ty);
}
