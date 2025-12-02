#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Systems.h"
#include "components/MirrorComponent.h"
#include "GameSettings.h"

/*
    Game
    ----
    This class now ONLY holds gameplay logic:

      ✔ Entity creation
      ✔ ECS Systems (animation, input, render, AI, light, combat)
      ✔ Resource loading
      ✔ Gameplay update() logic
      ✔ Rendering logic

    Removed:
      ✘ Window creation
      ✘ Event polling
      ✘ Game::run()
      ✘ Resolution control

    The Application + Scene system now owns the window and loop.
*/

class Game
{
public:
    Game();

    // Initialize gameplay systems, load textures, build level.
    bool initialize();

    // Update game state using ECS, requires Application's window
    void update(float deltaTime, sf::RenderWindow& window);

    // Draw everything to the provided window
    void render(sf::RenderWindow& window);

    // Framerate limit is now stored here but applied externally by Application
    void setFramerateLimit(unsigned int limit);
    unsigned int getFramerateLimit() const;

private:
    // Resource loading
    bool loadResources();
    std::string findResourcePath(const std::string& relativePath) const;

    // World building
    void createEntities();

    // Entity creation helpers (unchanged)
    Entity createPlayerEntity();
    Entity createLightBeaconEntity();
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

private:
    bool initialized_;

    // Textures
    sf::Texture idleTexture_;
    sf::Texture moveTexture_;
    sf::Texture debugWhiteTexture_;
    sf::Texture lightNodeTexture_;

    // ECS: Main gameplay entities
    Entity player_;
    Entity lightBeacon_;
    Entity enemy_;
    std::vector<Entity*> entities_;

    // Dynamic world objects (e.g., mirrors, walls)
    std::vector<std::unique_ptr<Entity>> worldObjects_;

    // ECS systems
    InputSystem inputSystem_;
    AnimationSystem animationSystem_;
    RenderSystem renderSystem_;
    CombatSystem combatSystem_;
    EnemyAISystem enemyAISystem_;
    LightSystem lightSystem_;

    // Game settings
    unsigned int currentFramerate;
};


