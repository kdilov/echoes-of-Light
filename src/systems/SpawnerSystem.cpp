#include "systems/SpawnerSystem.h"
#include "Systems.h"
#include "components/SpawnerComponent.h"
#include "components/TransformComponent.h"

void SpawnerSystem::setEnemyFactory(EnemyFactory factory) {
    m_enemyFactory = factory;
}

std::vector<Entity> SpawnerSystem::update(std::vector<Entity*>& entities, float deltaTime) {
    std::vector<Entity> newEnemies;

    if (!m_enemyFactory) {
        return newEnemies;
    }

    for (Entity* entity : entities) {
        if (!entity) continue;

        auto* spawner = entity->getComponent<eol::SpawnerComponent>();
        auto* transform = entity->getComponent<eol::TransformComponent>();

        if (!spawner || !transform || !spawner->isActive()) {
            continue;
        }

        // Update the spawn timer
        spawner->updateTimer(deltaTime);

        // Check if ready to spawn
        if (spawner->isReadyToSpawn()) {
            // Create enemy at spawner position
            sf::Vector2f spawnPos = transform->getPosition();
            Entity newEnemy = m_enemyFactory(spawnPos);
            newEnemies.push_back(std::move(newEnemy));

            // Update spawner state
            spawner->incrementEnemies();
            spawner->resetTimer();
        }
    }

    return newEnemies;
}