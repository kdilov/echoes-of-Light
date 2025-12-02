#include "components/LevelManager.h"
#include <iostream>

LevelManager::LevelManager()
    : currentLevelIndex(0)
{
    // 12-level default list (you can replace these names)
    levelFiles = {
        "resources/levels/past_1.txt",
        "resources/levels/past_2.txt",
        "resources/levels/past_3.txt",
        "resources/levels/past_final.txt",
        "resources/levels/present_1.txt",
        "resources/levels/present_2.txt",
        "resources/levels/present_3.txt",
        "resources/levels/present_final.txt",
        "resources/levels/future_1.txt",
        "resources/levels/future_2.txt",
        "resources/levels/future_3.txt",
        "resources/levels/future_final.txt"
    };

    // load first by default
    if (!levelFiles.empty()) map.loadFromFile(levelFiles[currentLevelIndex]);
}

bool LevelManager::loadLevel(const std::string& levelName) {
    std::cout << "LevelManager: Loading " << levelName << std::endl;
    bool ok = map.loadFromFile(levelName);
    if (ok) {
        // update current index to match if the file is in the list
        for (size_t i = 0; i < levelFiles.size(); ++i) {
            if (levelFiles[i] == levelName) { currentLevelIndex = static_cast<int>(i); break; }
        }
    }
    return ok;
}

bool LevelManager::loadCurrentLevel() {
    if (currentLevelIndex < 0 || currentLevelIndex >= static_cast<int>(levelFiles.size())) {
        std::cout << "LevelManager: current index out of range\n";
        return false;
    }
    return loadLevel(levelFiles[currentLevelIndex]);
}

void LevelManager::nextLevel() {
    if (currentLevelIndex < static_cast<int>(levelFiles.size())) ++currentLevelIndex;
    if (!isLevelComplete()) loadCurrentLevel();
}

bool LevelManager::isLevelComplete() const {
    return currentLevelIndex >= static_cast<int>(levelFiles.size());
}

const Map& LevelManager::getCurrentMap() const {
    return map;
}

void LevelManager::setCurrentIndex(int idx) {
    if (idx >= 0 && idx < static_cast<int>(levelFiles.size())) currentLevelIndex = idx;
}

LevelObjects LevelManager::scanObjects() const {
    LevelObjects objs;
    const Map& m = map;
    for (int y = 0; y < m.getHeight(); ++y) {
        for (int x = 0; x < m.getWidth(); ++x) {
            TileType t = m.getTile(x, y);
            if (t == TileType::LIGHT_SOURCE) objs.lightTiles.emplace_back(x, y);
            else if (t == TileType::MIRROR) objs.mirrorTiles.emplace_back(x, y);
            else if (t == TileType::END) objs.exitTile = {x, y};
        }
    }
    return objs;
}
