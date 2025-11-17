#pragma once

#include "components/Component.h"

#include <SFML/System/Vector2.hpp>

namespace eol {

class TransformComponent : public Component {
public:
    TransformComponent();
    TransformComponent(const sf::Vector2f& position,
                       const sf::Vector2f& scale,
                       float rotation);

    const sf::Vector2f& getPosition() const noexcept;
    void setPosition(const sf::Vector2f& position) noexcept;

    const sf::Vector2f& getScale() const noexcept;
    void setScale(const sf::Vector2f& scale) noexcept;

    float getRotation() const noexcept;
    void setRotation(float rotation) noexcept;

private:
    sf::Vector2f m_position{};
    sf::Vector2f m_scale{1.f, 1.f};
    float m_rotation{0.f};
};

} // namespace eol

