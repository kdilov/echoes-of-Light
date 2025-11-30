#pragma once

#include "components/Component.h"

#include <SFML/System/Vector2.hpp>
#include <vector>

namespace eol {

class EnemyAIComponent : public Component {
public:
    enum class BehaviorState {
        Patrol,
        Chase,
        Attack
    };

    EnemyAIComponent();

    void setPatrolPoints(std::vector<sf::Vector2f> points);
    const std::vector<sf::Vector2f>& getPatrolPoints() const noexcept;
    std::size_t getCurrentPatrolIndex() const noexcept;
    void advancePatrolPoint() noexcept;

    void setDetectionRange(float range) noexcept;
    float getDetectionRange() const noexcept;

    void setAttackRange(float range) noexcept;
    float getAttackRange() const noexcept;

    void setMoveSpeed(float speed) noexcept;
    float getMoveSpeed() const noexcept;

    void setState(BehaviorState state) noexcept;
    BehaviorState getState() const noexcept;

private:
    std::vector<sf::Vector2f> m_patrolPoints;
    std::size_t m_currentPatrolIndex{0};
    float m_detectionRange{220.f};
    float m_attackRange{60.f};
    float m_moveSpeed{85.f};
    BehaviorState m_state{BehaviorState::Patrol};
};

} // namespace eol


