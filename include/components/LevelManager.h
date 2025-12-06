#pragma once
#include <vector>
#include <string>
#include <SFML/System/Vector2.hpp>
#include "components/Map.h"

// Struct storing special tile positions for a level
struct LevelObjects {
    std::vector<sf::Vector2i> lightTiles;
    std::vector<sf::Vector2i> mirrorTiles;
    std::vector<sf::Vector2i> spawnerTiles;
    sf::Vector2i exitTile { -1, -1 };
};

class LevelManager {
public:
    LevelManager();
    ~LevelManager() = default;

    bool loadLevel(const std::string& levelName);
    bool loadCurrentLevel();
    void nextLevel();

    bool isLevelComplete() const;
    const Map& getCurrentMap() const;
    Map& getCurrentMapMutable() { return map; }

    LevelObjects scanObjects() const;
    const std::vector<std::string>& getLevelFiles() const { return levelFiles; }
    int getCurrentIndex() const { return currentLevelIndex; }
    void setCurrentIndex(int idx);

private:
    std::vector<std::string> levelFiles;
    int currentLevelIndex = 0;
    Map map;
};
