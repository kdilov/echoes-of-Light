#pragma once

#include "components/Component.h"

#include <SFML/System/Vector2.hpp>

namespace eol {

class HitboxComponent : public Component {
public:
    HitboxComponent();

    void setSize(const sf::Vector2f& size) noexcept;
    const sf::Vector2f& getSize() const noexcept;

private:
    sf::Vector2f m_size{32.f, 32.f};
};

} // namespace eol



