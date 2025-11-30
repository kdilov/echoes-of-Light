#include "components/EnemyAIComponent.h"

#include <algorithm>

namespace eol {

EnemyAIComponent::EnemyAIComponent()
    : Component("EnemyAI") {}

void EnemyAIComponent::setPatrolPoints(std::vector<sf::Vector2f> points) {
    m_patrolPoints = std::move(points);
    if (m_patrolPoints.empty()) {
        m_currentPatrolIndex = 0;
    }
    else {
        m_currentPatrolIndex %= m_patrolPoints.size();
    }
}

const std::vector<sf::Vector2f>& EnemyAIComponent::getPatrolPoints() const noexcept {
    return m_patrolPoints;
}

std::size_t EnemyAIComponent::getCurrentPatrolIndex() const noexcept {
    return m_currentPatrolIndex;
}

void EnemyAIComponent::advancePatrolPoint() noexcept {
    if (m_patrolPoints.empty()) {
        return;
    }
    m_currentPatrolIndex = (m_currentPatrolIndex + 1) % m_patrolPoints.size();
}

void EnemyAIComponent::setDetectionRange(float range) noexcept {
    m_detectionRange = std::max(0.f, range);
}

float EnemyAIComponent::getDetectionRange() const noexcept {
    return m_detectionRange;
}

void EnemyAIComponent::setAttackRange(float range) noexcept {
    m_attackRange = std::max(0.f, range);
}

float EnemyAIComponent::getAttackRange() const noexcept {
    return m_attackRange;
}

void EnemyAIComponent::setMoveSpeed(float speed) noexcept {
    m_moveSpeed = std::max(0.f, speed);
}

float EnemyAIComponent::getMoveSpeed() const noexcept {
    return m_moveSpeed;
}

void EnemyAIComponent::setState(BehaviorState state) noexcept {
    m_state = state;
}

EnemyAIComponent::BehaviorState EnemyAIComponent::getState() const noexcept {
    return m_state;
}

} // namespace eol


