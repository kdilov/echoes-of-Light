#pragma once

#include "components/Component.h"

#include <string>
#include <vector>

namespace eol {

class UpgradeComponent : public Component {
public:
    UpgradeComponent();

    void setAvailableUpgrades(std::vector<std::string> upgrades);
    const std::vector<std::string>& getAvailableUpgrades() const noexcept;

    unsigned int getCost() const noexcept;
    void setCost(unsigned int cost) noexcept;

private:
    std::vector<std::string> m_availableUpgrades;
    unsigned int m_cost{0};
};

} // namespace eol

