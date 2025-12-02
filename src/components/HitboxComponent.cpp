#include "components/HitboxComponent.h"

namespace eol {

HitboxComponent::HitboxComponent()
    : Component("Hitbox") {}

void HitboxComponent::setSize(const sf::Vector2f& size) noexcept {
    m_size = size;
}

const sf::Vector2f& HitboxComponent::getSize() const noexcept {
    return m_size;
}

} // namespace eol


