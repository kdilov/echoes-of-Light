#include "components/PlayerComponent.h"

namespace eol {

PlayerComponent::PlayerComponent()
    : Component("Player") {}

float PlayerComponent::getMovementSpeed() const noexcept {
    return m_movementSpeed;
}

void PlayerComponent::setMovementSpeed(float speed) noexcept {
    m_movementSpeed = speed;
}

float PlayerComponent::getLightCapacity() const noexcept {
    return m_lightCapacity;
}

void PlayerComponent::setLightCapacity(float capacity) noexcept {
    m_lightCapacity = capacity;
}

std::uint32_t PlayerComponent::getFragments() const noexcept {
    return m_lightFragments;
}

void PlayerComponent::setFragments(std::uint32_t count) noexcept {
    m_lightFragments = count;
}

std::uint32_t PlayerComponent::getUpgradePoints() const noexcept {
    return m_upgradePoints;
}

void PlayerComponent::setUpgradePoints(std::uint32_t points) noexcept {
    m_upgradePoints = points;
}

} // namespace eol

