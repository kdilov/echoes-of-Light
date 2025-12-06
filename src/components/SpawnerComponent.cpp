#include "components/SpawnerComponent.h"
#include <algorithm>

namespace eol {

    SpawnerComponent::SpawnerComponent()
        : Component("Spawner") {
    }

    void SpawnerComponent::setSpawnInterval(float interval) noexcept {
        m_spawnInterval = std::max(0.1f, interval);  // Minimum 0.1 seconds
    }

    float SpawnerComponent::getSpawnInterval() const noexcept {
        return m_spawnInterval;
    }

    void SpawnerComponent::setMaxEnemies(int max) noexcept {
        m_maxEnemies = std::max(1, max);
    }

    int SpawnerComponent::getMaxEnemies() const noexcept {
        return m_maxEnemies;
    }

    void SpawnerComponent::setCurrentEnemies(int count) noexcept {
        m_currentEnemies = std::max(0, count);
    }

    int SpawnerComponent::getCurrentEnemies() const noexcept {
        return m_currentEnemies;
    }

    void SpawnerComponent::incrementEnemies() noexcept {
        m_currentEnemies++;
    }

    void SpawnerComponent::decrementEnemies() noexcept {
        m_currentEnemies = std::max(0, m_currentEnemies - 1);
    }

    void SpawnerComponent::updateTimer(float deltaTime) noexcept {
        m_spawnTimer += deltaTime;
    }

    bool SpawnerComponent::isReadyToSpawn() const noexcept {
        return m_active &&
            m_spawnTimer >= m_spawnInterval &&
            m_currentEnemies < m_maxEnemies;
    }

    void SpawnerComponent::resetTimer() noexcept {
        m_spawnTimer = 0.f;
    }

    void SpawnerComponent::setActive(bool active) noexcept {
        m_active = active;
    }

    bool SpawnerComponent::isActive() const noexcept {
        return m_active;
    }

} // namespace eol