#pragma once

#include "components/Component.h"

namespace eol {

class LightComponent : public Component {
public:
    LightComponent();

    void setRadius(float radius) noexcept;
    float getRadius() const noexcept;

    void setIntensity(float intensity) noexcept;
    float getIntensity() const noexcept;

    void setWeaponized(bool weaponized) noexcept;
    bool isWeaponized() const noexcept;

private:
    float m_radius{160.f};
    float m_intensity{1.f};
    bool m_weaponized{false};
};

} // namespace eol

