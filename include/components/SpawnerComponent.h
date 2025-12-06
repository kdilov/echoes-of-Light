#pragma once

#include "components/Component.h"
#include <SFML/System/Vector2.hpp>

namespace eol {

    class SpawnerComponent : public Component {
    public:
        SpawnerComponent();

        // Spawn interval in seconds
        void setSpawnInterval(float interval) noexcept;
        float getSpawnInterval() const noexcept;

        // Maximum enemies this spawner can have alive at once
        void setMaxEnemies(int max) noexcept;
        int getMaxEnemies() const noexcept;

        // Current count of enemies spawned by this spawner
        void setCurrentEnemies(int count) noexcept;
        int getCurrentEnemies() const noexcept;
        void incrementEnemies() noexcept;
        void decrementEnemies() noexcept;

        // Timer management
        void updateTimer(float deltaTime) noexcept;
        bool isReadyToSpawn() const noexcept;
        void resetTimer() noexcept;

        // Enable/disable spawning
        void setActive(bool active) noexcept;
        bool isActive() const noexcept;

    private:
        float m_spawnInterval{ 5.f };    // Spawn every 5 seconds by default
        float m_spawnTimer{ 0.f };       // Current timer
        int m_maxEnemies{ 3 };           // Max 3 enemies from this spawner
        int m_currentEnemies{ 0 };       // Currently alive enemies
        bool m_active{ true };           // Is spawner active?
    };

} // namespace eol