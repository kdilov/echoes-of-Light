#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include <functional>

struct Entity;

class SpawnerSystem {
public:
    // Callback type for creating enemies
    using EnemyFactory = std::function<Entity(const sf::Vector2f& position)>;

    void setEnemyFactory(EnemyFactory factory);

    // Update all spawners, returns list of newly spawned enemies
    std::vector<Entity> update(std::vector<Entity*>& entities, float deltaTime);

private:
    EnemyFactory m_enemyFactory;
};