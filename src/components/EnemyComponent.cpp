#include "components/EnemyComponent.h"

namespace eol {

EnemyComponent::EnemyComponent()
    : Component("Enemy") {}

float EnemyComponent::getResistance() const noexcept {
    return m_resistance;
}

void EnemyComponent::setResistance(float resistance) noexcept {
    m_resistance = resistance;
}

float EnemyComponent::getAwarenessRadius() const noexcept {
    return m_awarenessRadius;
}

void EnemyComponent::setAwarenessRadius(float radius) noexcept {
    m_awarenessRadius = radius;
}

bool EnemyComponent::blocksLight() const noexcept {
    return m_blocksLight;
}

void EnemyComponent::setBlocksLight(bool blocksLight) noexcept {
    m_blocksLight = blocksLight;
}

} // namespace eol

