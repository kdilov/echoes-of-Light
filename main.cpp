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

#include "Systems.h"

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

// Create player entity with all components
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

    // Create player and entity list
    Entity player = makePlayerEntity(idleTexture, moveTexture);
    std::vector<Entity*> entities;
    entities.push_back(&player);

    // Create systems
    InputSystem inputSystem;
    AnimationSystem animationSystem;
    RenderSystem renderSystem;

    // Delta time clock
    sf::Clock clock;

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

        // Render
        window.clear(sf::Color(20, 20, 30));
        renderSystem.render(window, entities);
        window.display();
    }

    return 0;
}