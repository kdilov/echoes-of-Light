#include "components/CollisionComponent.h"

namespace eol {

CollisionComponent::CollisionComponent()
    : Component("Collision") {}

void CollisionComponent::setBoundingBox(const sf::Vector2f& size) noexcept {
    m_boundingBox = size;
}

const sf::Vector2f& CollisionComponent::getBoundingBox() const noexcept {
    return m_boundingBox;
}

void CollisionComponent::setSolid(bool solid) noexcept {
    m_solid = solid;
}

bool CollisionComponent::isSolid() const noexcept {
    return m_solid;
}

} // namespace eol
