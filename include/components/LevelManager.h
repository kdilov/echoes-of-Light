#pragma once
#include <string>
#include <vector>
#include "Map.h"

class LevelManager {
public:
    LevelManager();
    
    // Load current level from list
    bool loadCurrentLevel();

    // Move to next level (Past -> Present -> Future)
    void nextLevel();

    // Returns true if all levels are finished
    bool isFinished() const;

    // Called from game loop when player reaches the exit tile
    void markLevelComplete();

    // Access the map for rendering, collision, etc.
    const Map& getMap() const { return map; }

    // Gets exit tile position for win detection
    sf::Vector2i getExitTile() const { return exitTile; }

private:
    std::vector<std::string> levelFiles;

    int currentLevelIndex = 0;
    bool levelCompleted = false;

    Map map;

    // Store exit position for level logic
    sf::Vector2i exitTile { -1, -1 };

    // Helper to find exit tile after loading
    void findExitTile();
};
