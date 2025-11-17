#pragma once

#include "components/Component.h"

namespace eol {

class EnemyComponent : public Component {
public:
    EnemyComponent();

    float getResistance() const noexcept;
    void setResistance(float resistance) noexcept;

    float getAwarenessRadius() const noexcept;
    void setAwarenessRadius(float radius) noexcept;

    bool blocksLight() const noexcept;
    void setBlocksLight(bool blocksLight) noexcept;

private:
    float m_resistance{1.f};
    float m_awarenessRadius{120.f};
    bool m_blocksLight{true};
};

} // namespace eol

