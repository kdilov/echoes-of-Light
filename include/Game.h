#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Systems.h"
#include "components/MirrorComponent.h"

class Game {
public:
    Game();

    int run();

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
    void handleEvents();
    void update(float deltaTime);
    void render();
    std::string findResourcePath(const std::string& relativePath) const;

private:
    static constexpr unsigned int windowWidth = 800;
    static constexpr unsigned int windowHeight = 600;
    static constexpr unsigned int framerateLimit = 60;

    sf::RenderWindow window_;
    sf::Clock clock_;

    sf::Texture idleTexture_;
    sf::Texture moveTexture_;
    sf::Texture debugWhiteTexture_;
    sf::Texture lightNodeTexture_;
    bool initialized_;

    Entity player_;
    Entity lightBeacon_;
    Entity enemy_;
    std::vector<Entity*> entities_;
    std::vector<std::unique_ptr<Entity>> worldObjects_;

    InputSystem inputSystem_;
    AnimationSystem animationSystem_;
    RenderSystem renderSystem_;
    LightSystem lightSystem_;
};

