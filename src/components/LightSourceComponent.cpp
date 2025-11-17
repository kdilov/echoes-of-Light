#include "components/LightSourceComponent.h"

namespace eol {

LightSourceComponent::LightSourceComponent()
    : Component("LightSource") {}

bool LightSourceComponent::isMovable() const noexcept {
    return m_movable;
}

void LightSourceComponent::setMovable(bool movable) noexcept {
    m_movable = movable;
}

bool LightSourceComponent::isActive() const noexcept {
    return m_active;
}

void LightSourceComponent::setActive(bool active) noexcept {
    m_active = active;
}

float LightSourceComponent::getFuel() const noexcept {
    return m_fuel;
}

void LightSourceComponent::setFuel(float fuel) noexcept {
    m_fuel = fuel;
}

} // namespace eol

