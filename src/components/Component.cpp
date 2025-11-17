#include "components/Component.h"

#include <utility>

namespace eol {

Component::Component(std::string name)
    : m_name(std::move(name)) {}

const std::string& Component::getName() const noexcept {
    return m_name;
}

bool Component::isEnabled() const noexcept {
    return m_enabled;
}

void Component::setEnabled(bool enabled) noexcept {
    m_enabled = enabled;
}

} // namespace eol

