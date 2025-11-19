#include "LevelManager.hpp"
#include <iostream>

LevelManager::LevelManager() {
    levelFiles = {
        "resources/levels/past.txt",
        "resources/levels/present.txt",
        "resources/levels/future.txt"
    };
}

bool LevelManager::loadCurrentLevel() {
    if(currentIndex >= levelFiles.size()) return false;

    std::cout << "Loading level " << currentIndex + 1 
              << ": " << levelFiles[currentIndex] << std::endl;

    return map.loadFromFile(levelFiles[currentIndex]);
}

void LevelManager::nextLevel() {
    currentIndex++;
    if (!isFinished()) {
        loadCurrentLevel();
    }
}

bool LevelManager::isFinished() const {
    return currentIndex >= levelFiles.size();
}

const Map& LevelManager::getMap() const {
    return map;
}

struct LevelObjects{
    std::vector<sf::Vector2i> lightTiles;
    std::vector<sf::Vector2i> mirrorTiles;
    sf::Vector2i exitTile {-1, -1};
};

LevelObjects LevelManager::scanObjects() const {
    LevelObjects objs;

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            TileType t = map.getTile(x, y);

            if (t == TileType::LIGHT_SOURCE) {
                objs.lightTiles.emplace_back(x, y);
            } else if (t == TileType::MIRROR) {
                objs.mirrorTiles.emplace_back(x, y);
            } else if (t == TileType::EXIT) {
                objs.exitTile = { x, y };
            }
        }
    }

    return objs;
}

levels.loadCurrentLevel();
levels.getMap().setTextures(&wallTex, &floorTex, &lightTex, &mirrorTex, &exitTex);

LevelObjects objects = levels.scanObjects();

// Example: convert tile coords to world coords
auto tileToWorld = [](sf::Vector2i t) {
    return sf::Vector2f(t.x * TILE_SIZE + TILE_SIZE / 2.0f,
                        t.y * TILE_SIZE + TILE_SIZE / 2.0f);
};

// Clear old entities
lights.clear();
mirrors.clear();

// Spawn light sources
for (auto& t : objects.lightTiles) {
    sf::Vector2f pos = tileToWorld(t);
    // however you construct a light in your engine:
    // lights.emplace_back(Light(pos));
}

// Spawn mirrors
for (auto& t : objects.mirrorTiles) {
    sf::Vector2f pos = tileToWorld(t);
    // mirrors.emplace_back(Mirror(pos));
}

// Set exit position for win check
sf::Vector2f exitPos = tileToWorld(objects.exitTile);
