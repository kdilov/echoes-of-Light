#include "components/TransformComponent.h"

namespace eol {

TransformComponent::TransformComponent()
    : Component("Transform") {}

TransformComponent::TransformComponent(const sf::Vector2f& position,
                                       const sf::Vector2f& scale,
                                       float rotation)
    : Component("Transform"),
      m_position(position),
      m_scale(scale),
      m_rotation(rotation) {}

const sf::Vector2f& TransformComponent::getPosition() const noexcept {
    return m_position;
}

void TransformComponent::setPosition(const sf::Vector2f& position) noexcept {
    m_position = position;
}

const sf::Vector2f& TransformComponent::getScale() const noexcept {
    return m_scale;
}

void TransformComponent::setScale(const sf::Vector2f& scale) noexcept {
    m_scale = scale;
}

float TransformComponent::getRotation() const noexcept {
    return m_rotation;
}

void TransformComponent::setRotation(float rotation) noexcept {
    m_rotation = rotation;
}

} // namespace eol

