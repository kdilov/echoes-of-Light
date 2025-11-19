#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "components/EnemyComponent.h"
#include "components/LightComponent.h"
#include "components/LightSourceComponent.h"
#include "components/PlayerComponent.h"
#include "components/PuzzleComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "components/UpgradeComponent.h"

#include "components/Map.hpp"
#include "LevelManager.hpp"

constexpr int TILE_SIZE = 32;

// ===== ENTITY FACTORIES ===== //

struct Entity {
    std::string name;
    std::vector<eol::ComponentPtr> components;
};

Entity makePlayerEntity() {
    Entity entity;
    entity.name = "Player";
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(sf::Vector2f{64.f, 64.f}, sf::Vector2f{1.f, 1.f}, 0.f));
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

// ===== MAIN GAME ENTRY ===== //

int main() {

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Echoes of Light");

    // Create main player and objects
    Entity player = makePlayerEntity();
    Entity beacon = makeLightBeaconEntity();
    Entity enemy = makeEnemyEntity();

    // Load levels + first map
    LevelManager levels;
    levels.loadCurrentLevel();
    
    // Scan map objects (exit tile, mirrors, light sources)
    LevelObjects objects = levels.scanObjects();

    // Convert exit tile to world position once
    sf::Vector2f exitPos(
        objects.exitTile.x * TILE_SIZE + TILE_SIZE / 2.0f,
        objects.exitTile.y * TILE_SIZE + TILE_SIZE / 2.0f
    );

    std::cout << "Exit Beacon placed at: " << exitPos.x << ", " << exitPos.y << std::endl;

    // MAIN LOOP
    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // ===== PLAYER UPDATE (placeholder) ===== //
        // This should be replaced when your movement system is implemented:
        auto transform = dynamic_cast<eol::TransformComponent*>(player.components[0].get());
        sf::Vector2f pos = transform->getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) pos.y -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) pos.y += 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) pos.x -= 2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) pos.x += 2;

        transform->setPosition(pos);


        // ===== WIN CONDITION CHECK ===== //

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
            }
            else {
                std::cout << "GAME COMPLETE — YOU WON!" << std::endl;
                window.close();
            }
        }


        // ===== RENDER ===== //
        window.clear();

        // draw full map (later add player + UI + light effects)
        levels.getMap().draw(window);

        window.display();
    }

    return 0;
}
