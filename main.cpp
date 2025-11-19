#include <SFML/Graphics.hpp>

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "components/EnemyComponent.h"
#include "components/LightComponent.h"
#include "components/LightSourceComponent.h"
#include "components/PlayerComponent.h"
#include "components/PuzzleComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "components/UpgradeComponent.h"

struct Entity {
    std::string name;
    std::vector<eol::ComponentPtr> components;

    // Helper function to get a component by type
    template<typename T>
    T* getComponent() {
        for (auto& comp : components) {
            if (T* result = dynamic_cast<T*>(comp.get())) {
                return result;
            }
        }
        return nullptr;
    }
};

Entity makePlayerEntity() {
    Entity entity;
    entity.name = "Player";
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        sf::Vector2f{64.f, 64.f}, sf::Vector2f{1.f, 1.f}, 0.f));
    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());
    entity.components.emplace_back(std::make_unique<eol::PlayerComponent>());
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

// Helper function to draw an entity as a colored rectangle
void drawEntity(sf::RenderWindow& window, Entity& entity, sf::Color color) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    if (transform) {
        sf::RectangleShape shape(sf::Vector2f(32.f, 32.f)); // 32x32 pixel square
        shape.setPosition(transform->getPosition());
        shape.setFillColor(color);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(2.f);
        window.draw(shape);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Echoes of Light");

    Entity player = makePlayerEntity();
    Entity beacon = makeLightBeaconEntity();
    Entity enemy = makeEnemyEntity();

    // Get player's speed from PlayerComponent
    auto* playerComp = player.getComponent<eol::PlayerComponent>();
    float moveSpeed = playerComp ? playerComp->getMovementSpeed() : 120.f;

    // Clock for delta time (frame-rate independent movement)
    sf::Clock clock;

    // Simple placeholder interaction between components to avoid unused warnings.
    if (!player.components.empty()) {
        player.components.front()->setEnabled(true);
    }
    if (!beacon.components.empty()) {
        beacon.components.front()->setEnabled(true);
    }
    if (!enemy.components.empty()) {
        enemy.components.front()->setEnabled(true);
    }

    while (window.isOpen()) {

        // Calculate delta time
        float deltaTime = clock.restart().asSeconds();

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        
        // ===== PLAYER MOVEMENT ===== //
        auto* transform = player.getComponent<eol::TransformComponent>();
        if (transform) {
            sf::Vector2f pos = transform->getPosition();
            sf::Vector2f movement(0.f, 0.f);

            // Calculate movement based on input
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                movement.y -= 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                movement.y += 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                movement.x -= 1.f;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                movement.x += 1.f;
            }

            // Apply movement with delta time (frame-rate independent)
            if (movement.x != 0.f || movement.y != 0.f) {
                // Normalize diagonal movement
                float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
                movement.x /= length;
                movement.y /= length;

                // Apply speed and delta time
                pos.x += movement.x * moveSpeed * deltaTime;
                pos.y += movement.y * moveSpeed * deltaTime;

                // Keep player on screen
                if (pos.x < 0) pos.x = 0;
                if (pos.y < 0) pos.y = 0;
                if (pos.x > 800 - 32) pos.x = 800 - 32;
                if (pos.y > 600 - 32) pos.y = 600 - 32;

                transform->setPosition(pos);
            }
        }

        // ===== RENDERING ===== //
        window.clear(sf::Color(20, 20, 30)); // Dark blue-gray background

        // Draw entities as colored rectangles
        drawEntity(window, player, sf::Color::Green);      // Player = Green
        drawEntity(window, beacon, sf::Color::Yellow);     // Beacon = Yellow
        

        window.display();
    }
    return 0;
}