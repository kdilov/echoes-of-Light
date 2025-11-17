#include <SFML/Graphics.hpp>

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

struct Entity {
    std::string name;
    std::vector<eol::ComponentPtr> components;
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

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Echoes of Light");

    Entity player = makePlayerEntity();
    Entity beacon = makeLightBeaconEntity();
    Entity enemy = makeEnemyEntity();

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
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();
        window.display();
    }
    return 0;
}