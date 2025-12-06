#include "components/PuzzleComponent.h"

#include <algorithm>

namespace eol {

PuzzleComponent::PuzzleComponent()
    : Component("Puzzle") {}

unsigned int PuzzleComponent::getRequiredLight() const noexcept {
    return m_requiredLight;
}

void PuzzleComponent::setRequiredLight(unsigned int requiredLight) noexcept {
    m_requiredLight = requiredLight;
}

float PuzzleComponent::getAccumulatedLight() const noexcept {
    return m_receivedLight;
}

void PuzzleComponent::setAccumulatedLight(float value) noexcept {
    m_receivedLight = value;
}

void PuzzleComponent::addReceivedLight(float value) noexcept {
    m_receivedLight += value;
}

bool PuzzleComponent::isSolved() const noexcept {
    return m_solved;
}

void PuzzleComponent::setSolved(bool solved) noexcept {
    m_solved = solved;
}

void PuzzleComponent::setLightRequirement(LightRequirement requirement) noexcept {
    m_requirement = requirement;
}

PuzzleComponent::LightRequirement PuzzleComponent::getLightRequirement() const noexcept {
    return m_requirement;
}

void PuzzleComponent::setRequiredUniqueSources(unsigned int count) noexcept {
    m_requiredUniqueSources = std::max(1u, count);
    if (m_requiredUniqueSources <= 1) {
        m_uniqueSources.clear();
    }
}

unsigned int PuzzleComponent::getRequiredUniqueSources() const noexcept {
    return m_requiredUniqueSources;
}

void PuzzleComponent::registerSourceContribution(const Entity* source) {
    if (m_requiredUniqueSources <= 1 || !source) {
        return;
    }
    m_uniqueSources.insert(source);
}

bool PuzzleComponent::hasRequiredUniqueSources() const noexcept {
    if (m_requiredUniqueSources <= 1) {
        return true;
    }
    return m_uniqueSources.size() >= m_requiredUniqueSources;
}

void PuzzleComponent::resetSources() noexcept {
    m_uniqueSources.clear();
}

} // namespace eol

