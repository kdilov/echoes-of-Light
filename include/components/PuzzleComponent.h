#pragma once

#include "components/Component.h"

#include <unordered_set>

struct Entity;

namespace eol {

class PuzzleComponent : public Component {
public:
    enum class LightRequirement {
        Any,
        PlayerOnly,
        BeaconOnly
    };

    PuzzleComponent();

    unsigned int getRequiredLight() const noexcept;
    void setRequiredLight(unsigned int requiredLight) noexcept;

    float getAccumulatedLight() const noexcept;
    void setAccumulatedLight(float value) noexcept;
    void addReceivedLight(float value) noexcept;

    bool isSolved() const noexcept;
    void setSolved(bool solved) noexcept;

    void setLightRequirement(LightRequirement requirement) noexcept;
    LightRequirement getLightRequirement() const noexcept;

    void setRequiredUniqueSources(unsigned int count) noexcept;
    unsigned int getRequiredUniqueSources() const noexcept;
    void registerSourceContribution(const Entity* source);
    bool hasRequiredUniqueSources() const noexcept;
    void resetSources() noexcept;

private:
    unsigned int m_requiredLight{1};
    float m_receivedLight{0.f};
    bool m_solved{false};
    LightRequirement m_requirement{LightRequirement::Any};
    unsigned int m_requiredUniqueSources{1};
    std::unordered_set<const Entity*> m_uniqueSources;
};

} // namespace eol

