#pragma once

#include "components/Component.h"

namespace eol {

class PuzzleComponent : public Component {
public:
    PuzzleComponent();

    unsigned int getRequiredLight() const noexcept;
    void setRequiredLight(unsigned int requiredLight) noexcept;

    bool isSolved() const noexcept;
    void setSolved(bool solved) noexcept;

private:
    unsigned int m_requiredLight{1};
    bool m_solved{false};
};

} // namespace eol

