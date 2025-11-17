#include "components/PuzzleComponent.h"

namespace eol {

PuzzleComponent::PuzzleComponent()
    : Component("Puzzle") {}

unsigned int PuzzleComponent::getRequiredLight() const noexcept {
    return m_requiredLight;
}

void PuzzleComponent::setRequiredLight(unsigned int requiredLight) noexcept {
    m_requiredLight = requiredLight;
}

bool PuzzleComponent::isSolved() const noexcept {
    return m_solved;
}

void PuzzleComponent::setSolved(bool solved) noexcept {
    m_solved = solved;
}

} // namespace eol

