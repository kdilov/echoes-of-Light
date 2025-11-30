#pragma once

#include "components/Component.h"
#include <SFML/System/Vector2.hpp>

namespace eol {

// Simple collision component - just a bounding box
class CollisionComponent : public Component {
public:
    CollisionComponent();

    void setBoundingBox(const sf::Vector2f& size) noexcept;
    const sf::Vector2f& getBoundingBox() const noexcept;

    void setSolid(bool solid) noexcept;
    bool isSolid() const noexcept;

private:
    sf::Vector2f m_boundingBox{32.f, 32.f};
    bool m_solid{true};
};

} // namespace eol
