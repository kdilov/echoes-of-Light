#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

enum class TileType {
    EMPTY,
    WALL,
    LIGHT_SOURCE,
    MIRROR,
    BEACON,
    START,
    END,
    SPAWNER
};

class Map {
public:
    Map() = default;
    bool loadFromFile(const std::string& filename);
    TileType getTile(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Optional rendering support (you can ignore if not rendering)
    void setTextures(
        const sf::Texture& wallTex,
        const sf::Texture& lightTex,
        const sf::Texture& mirrorTex,
        const sf::Texture& startTex,
        const sf::Texture& endTex,
        const sf::Texture& emptyTex);
    void draw(sf::RenderWindow& window, float tileSize, sf::Vector2f offset = { 0.f, 0.f }) const;

    void setWallTexture(const sf::Texture& tex) { wallTexture = &tex; }

    // Collision helpers
    bool isWalkableTile(TileType t) const;
    bool isWalkableTileCoord(int tx, int ty) const;
    bool isWalkableWorld(float worldx, float worldy, float tileSize = 32) const;

private:
    TileType charToTile(char c) const;

    
    std::vector<std::vector<TileType>> grid;
    int width = 0;
    int height = 0;

    // optional textures (can be nullptr)
    const sf::Texture* wallTexture = nullptr;
    const sf::Texture* lightTexture = nullptr;
    const sf::Texture* mirrorTexture = nullptr;
    const sf::Texture* startTexture = nullptr;
    const sf::Texture* exitTexture = nullptr;
    const sf::Texture* emptyTexture = nullptr;
};
