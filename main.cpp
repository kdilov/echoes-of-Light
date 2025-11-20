/*
 * Echoes of Light - Main Game File
 * Entity-Component-System architecture
 * Controls: WASD to move, ESC to exit
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <filesystem>

#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/PlayerComponent.h"
#include "components/AnimationComponent.h"
#include "components/LightComponent.h"
#include "components/UpgradeComponent.h"
#include "components/EnemyComponent.h"
#include "components/LightSourceComponent.h"
#include "components/PuzzleComponent.h"

 // TODO: Uncomment when LevelManager is fully implemented
 // #include "LevelManager.hpp"
 // #include "components/Map.hpp"

#include "Systems.h"

constexpr int TILE_SIZE = 32;

// Helper to find resource files from different run locations
std::string findResourcePath(const std::string& relativePath) {
    std::vector<std::string> possiblePaths = {
        relativePath,
        "../../../" + relativePath,
        "../../../../" + relativePath,
        "D:/code/echoes/echoes-of-Light/" + relativePath
    };

    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    return relativePath;
}

// NOTE: Entity struct is defined in Systems.h - don't redefine it here!

// Create player entity with animations
Entity makePlayerEntity(const sf::Texture& idleTexture, const sf::Texture& moveTexture) {
    Entity entity;
    entity.name = "Player";

    // Transform: position, scale, rotation
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        sf::Vector2f{ 400.f, 300.f },  // Center of screen
        sf::Vector2f{ 0.5f, 0.5f },    // Scale down to 64x64 pixels
        0.f
    ));

    // Render component for drawing
    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());

    // Player component with speed and stats
    entity.components.emplace_back(std::make_unique<eol::PlayerComponent>());

    // Animation component with idle and walk animations
    auto animComp = std::make_unique<eol::AnimationComponent>();

    // Idle animation - 4 frames
    eol::Animation idleAnim;
    idleAnim.name = "idle";
    idleAnim.texture = &idleTexture;
    idleAnim.frameCount = 4;
    idleAnim.frameWidth = 128;       // Sprite sheet: 512 / 4 = 128
    idleAnim.frameHeight = 128;
    idleAnim.frameDuration = 0.15f;
    idleAnim.loop = true;
    animComp->addAnimation("idle", idleAnim);

    // Walk animation - 6 frames
    eol::Animation walkAnim;
    walkAnim.name = "walk";
    walkAnim.texture = &moveTexture;
    walkAnim.frameCount = 6;
    walkAnim.frameWidth = 128;       // Sprite sheet: 768 / 6 = 128
    walkAnim.frameHeight = 128;
    walkAnim.frameDuration = 0.1f;
    walkAnim.loop = true;
    animComp->addAnimation("walk", walkAnim);

    animComp->setAnimation("idle");
    entity.components.emplace_back(std::move(animComp));

    // Components for future features
    entity.components.emplace_back(std::make_unique<eol::LightComponent>());
    entity.components.emplace_back(std::make_unique<eol::UpgradeComponent>());

    return entity;
}

Entity makeLightBeaconEntity() {
    Entity entity;
    entity.name = "LightBeacon";
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>());
    entity.components.emplace_back(std::make_unique<eol::LightSourceComponent>());
    entity.components.emplace_back(std::make_unique<eol::PuzzleComponent>());
    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());
    return entity;
}

Entity makeEnemyEntity() {
    Entity entity;
    entity.name = "Enemy";
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>());
    entity.components.emplace_back(std::make_unique<eol::EnemyComponent>());
    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());
    entity.components.emplace_back(std::make_unique<eol::LightComponent>());
    return entity;
}

int main() {

    // Create window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Echoes of Light");
    window.setFramerateLimit(60);

    std::cout << "=== ECHOES OF LIGHT ===" << std::endl;

    // Load sprite sheet textures
    std::string idlePath = findResourcePath("resources/sprites/Character_Idle.png");
    std::string movePath = findResourcePath("resources/sprites/Character_Move.png");

    sf::Texture idleTexture;
    sf::Texture moveTexture;

    if (!idleTexture.loadFromFile(idlePath)) {
        std::cerr << "ERROR: Failed to load idle texture" << std::endl;
        return -1;
    }

    if (!moveTexture.loadFromFile(movePath)) {
        std::cerr << "ERROR: Failed to load move texture" << std::endl;
        return -1;
    }

    std::cout << "Textures loaded successfully" << std::endl;

    // Create entities
    Entity player = makePlayerEntity(idleTexture, moveTexture);
    Entity beacon = makeLightBeaconEntity();
    Entity enemy = makeEnemyEntity();

    // Entity list for systems
    std::vector<Entity*> entities;
    entities.push_back(&player);

    // Create systems
    InputSystem inputSystem;
    AnimationSystem animationSystem;
    RenderSystem renderSystem;

    // TODO: Uncomment when LevelManager is fully implemented
    /*
    // Load levels + first map
    LevelManager levels;
    levels.loadCurrentLevel();

    // Scan map objects (exit tile, mirrors, light sources)
    LevelObjects objects = levels.scanObjects();

    // Convert exit tile to world position
    sf::Vector2f exitPos(
        objects.exitTile.x * TILE_SIZE + TILE_SIZE / 2.0f,
        objects.exitTile.y * TILE_SIZE + TILE_SIZE / 2.0f
    );

    std::cout << "Exit Beacon placed at: " << exitPos.x << ", " << exitPos.y << std::endl;
    */

    // Delta time clock
    sf::Clock clock;

    std::cout << "Controls: WASD to move, ESC to exit" << std::endl;
    std::cout << "Animation system active!" << std::endl;

    // Main game loop
    while (window.isOpen()) {

        float deltaTime = clock.restart().asSeconds();

        // Handle events
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // Update systems
        inputSystem.update(player, deltaTime);
        animationSystem.update(entities, deltaTime);

        // TODO: Uncomment when level progression is finished
        /*
        // Get player position for win condition check
        auto* transform = player.getComponent<eol::TransformComponent>();
        sf::Vector2f pos = transform->getPosition();

        // Win condition check
        float dx = pos.x - exitPos.x;
        float dy = pos.y - exitPos.y;
        float distanceSquared = (dx * dx + dy * dy);

        if (distanceSquared < (TILE_SIZE * TILE_SIZE * 0.5f)) {
            std::cout << "Level Completed!" << std::endl;

            if (!levels.isFinished()) {
                std::cout << "Loading Next Level..." << std::endl;

                levels.nextLevel();
                objects = levels.scanObjects();

                exitPos = sf::Vector2f(
                    objects.exitTile.x * TILE_SIZE + TILE_SIZE / 2.0f,
                    objects.exitTile.y * TILE_SIZE + TILE_SIZE / 2.0f
                );

                // Reset player position for new level
                transform->setPosition(sf::Vector2f{64.f, 64.f});
            }
            else {
                std::cout << "GAME COMPLETE — YOU WON!" << std::endl;
                window.close();
            }
        }
        */

        // Render
        window.clear(sf::Color(20, 20, 30));

        // TODO: Uncomment when map rendering is ready
        // levels.getMap().draw(window);


        // Draw entities (player, enemies, etc)
        renderSystem.render(window, entities);

        window.display();
    }

    return 0;
}