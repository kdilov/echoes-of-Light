#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include "Systems.h"
#include "components/MirrorComponent.h"
#include "GameSettings.h"

class Game {
public:
    Game();

    int run();
    void createWindow();

    // Resolution control
    void setResolution(const sf::Vector2u& res);
    sf::Vector2u getResolution() const;

    // Frame rate control
    void setFramerateLimit(unsigned int limit);
    unsigned int getFramerateLimit() const;

    // Access window for menus (optional)
    sf::RenderWindow& getWindow();

    // Resolution management
    void setResolution(unsigned int index);
    unsigned int getCurrentResolutionIndex() const noexcept;

private:
    bool initialize();
    bool loadResources();
    void createEntities();
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
    void handleEvents();
    void update(float deltaTime);
    void render();
    std::string findResourcePath(const std::string& relativePath) const;

private:
    // Window defaults to Full HD, uses reference resolution via view scaling
    static constexpr unsigned int defaultWindowWidth = GameSettings::refWidth;
    static constexpr unsigned int defaultWindowHeight = GameSettings::refHeight;
    static constexpr unsigned int framerateLimit = 60;

    sf::Vector2u currentResolution{ windowWidth, windowHeight };
    unsigned int currentFramerate = framerateLimit;

    sf::RenderWindow window_;
    sf::View gameView_;
    sf::Clock clock_;
    sf::Texture idleTexture_;
    sf::Texture moveTexture_;
    sf::Texture debugWhiteTexture_;
    sf::Texture lightNodeTexture_;
    bool initialized_;

    unsigned int currentResolutionIndex_{ 0 };

    Entity player_;
    Entity lightBeacon_;
    Entity enemy_;
    std::vector<Entity*> entities_;
    std::vector<std::unique_ptr<Entity>> worldObjects_;

    InputSystem inputSystem_;
    AnimationSystem animationSystem_;
    RenderSystem renderSystem_;
    CombatSystem combatSystem_;
    EnemyAISystem enemyAISystem_;
    LightSystem lightSystem_;
};

