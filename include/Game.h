#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Systems.h"
#include "components/MirrorComponent.h"
#include "GameSettings.h"
#include "systems/DialogSystem.h"


class Game
{
public:
    Game();

    bool initialize();
    
    void update(float deltaTime, sf::RenderWindow& window);
    
    void render(sf::RenderWindow& window);
    
    // Frame rate control
    void setFramerateLimit(unsigned int limit);
    unsigned int getFramerateLimit() const;

private:
    
    bool loadResources();
    std::string findResourcePath(const std::string& relativePath) const;

    
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

private:
    bool initialized_;

    sf::Texture idleTexture_;
    sf::Texture moveTexture_;
    sf::Texture debugWhiteTexture_;
    sf::Texture lightNodeTexture_;

   
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

    
    unsigned int currentFramerate;

    // Dialog system
    sf::Font gameFont_;
    DialogSystem dialogSystem_;
};


