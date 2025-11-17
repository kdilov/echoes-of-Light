#include "components/LightComponent.h"

namespace eol {

LightComponent::LightComponent()
    : Component("Light") {}

void LightComponent::setRadius(float radius) noexcept {
    m_radius = radius;
}

float LightComponent::getRadius() const noexcept {
    return m_radius;
}

void LightComponent::setIntensity(float intensity) noexcept {
    m_intensity = intensity;
}

float LightComponent::getIntensity() const noexcept {
    return m_intensity;
}

void LightComponent::setWeaponized(bool weaponized) noexcept {
    m_weaponized = weaponized;
}

bool LightComponent::isWeaponized() const noexcept {
    return m_weaponized;
}

} // namespace eol

