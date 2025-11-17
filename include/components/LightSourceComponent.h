#pragma once

#include "components/Component.h"

namespace eol {

class LightSourceComponent : public Component {
public:
    LightSourceComponent();

    bool isMovable() const noexcept;
    void setMovable(bool movable) noexcept;

    bool isActive() const noexcept;
    void setActive(bool active) noexcept;

    float getFuel() const noexcept;
    void setFuel(float fuel) noexcept;

private:
    bool m_movable{false};
    bool m_active{true};
    float m_fuel{100.f};
};

} // namespace eol

