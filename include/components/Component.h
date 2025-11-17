#pragma once

#include <memory>
#include <string>

namespace eol {

class Component {
public:
    explicit Component(std::string name);
    virtual ~Component() = default;

    const std::string& getName() const noexcept;
    bool isEnabled() const noexcept;
    void setEnabled(bool enabled) noexcept;

private:
    std::string m_name;
    bool m_enabled{true};
};

using ComponentPtr = std::unique_ptr<Component>;

} // namespace eol

