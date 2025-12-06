#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Systems.h"
#include "components/MirrorComponent.h"
#include "systems/DialogSystem.h"
#include "components/LevelManager.h"
#include "systems/SpawnerSystem.h"

class Game
{
public:
    Game();
    explicit Game(int startLevel);
    int run();

    bool initialize();
    
    void update(float deltaTime, sf::RenderWindow& window);
    
    void render(sf::RenderWindow& window);
    
    // Frame rate control
    void setFramerateLimit(unsigned int limit);
    unsigned int getFramerateLimit() const;

private:
    
    bool loadResources();
    std::string findResourcePath(const std::string& relativePath) const;

    LevelManager levels_;
    int startLevelIndex_ = 0;
    bool gameComplete_ = false;
    void recalculateTileSize();
    void applyWallTextureForCurrentLevel();
    void createEntities();


    
    Entity createPlayerEntity();
    Entity createLightBeaconEntity(const sf::Vector2f& worldPosition, int beaconNumber);
    Entity createEnemyEntity();
    Entity createMirrorEntity(const sf::Vector2f& position,
        const sf::Vector2f& normal,
        const sf::Vector2f& size,
        eol::MirrorComponent::MirrorType type);
    Entity createLightSourceNode(const std::string& name,
        const sf::Vector2f& position,
        bool movable);
    Entity createWallEntity(const sf::Vector2f& position,
        const sf::Vector2f& size);
    Entity createSpawnerEntity(const sf::Vector2f& position,
        float interval,
        int maxEnemies);
    Entity createEnemyAtPosition(const sf::Vector2f& position);


    // Tile size for current level
    float tileSize_ = 0.f;
    sf::Vector2f mapOffset_ = { 0.f, 0.f };


    // Helper to convert tile coords to world coords
    sf::Vector2f tileToWorld(int tileX, int tileY) const;

private:
    bool initialized_;
    bool playerReachedExit();
    bool isBeaconPuzzleSolved();

    sf::Texture idleTexture_;
    sf::Texture moveTexture_;
    sf::Texture debugWhiteTexture_;
    sf::Texture lightNodeTexture_;
    sf::Texture wallTexturePast_;
    sf::Texture wallTexturePresent_;
    sf::Texture wallTextureFuture_;

   
    Entity player_;
    std::vector<Entity*> beacons_;
    Entity enemy_;
    std::vector<Entity*> entities_;

    
    std::vector<std::unique_ptr<Entity>> worldObjects_;


    
    InputSystem inputSystem_;
    AnimationSystem animationSystem_;
    RenderSystem renderSystem_;
    CombatSystem combatSystem_;
    EnemyAISystem enemyAISystem_;
    LightSystem lightSystem_;
    SpawnerSystem spawnerSystem_;


    
    unsigned int currentFramerate;

    // Dialog system
    sf::Font gameFont_;
    DialogSystem dialogSystem_;
};


