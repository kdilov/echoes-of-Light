#include "components/UpgradeComponent.h"

#include <utility>

namespace eol {

UpgradeComponent::UpgradeComponent()
    : Component("Upgrade") {}

void UpgradeComponent::setAvailableUpgrades(std::vector<std::string> upgrades) {
    m_availableUpgrades = std::move(upgrades);
}

const std::vector<std::string>& UpgradeComponent::getAvailableUpgrades() const noexcept {
    return m_availableUpgrades;
}

unsigned int UpgradeComponent::getCost() const noexcept {
    return m_cost;
}

void UpgradeComponent::setCost(unsigned int cost) noexcept {
    m_cost = cost;
}

} // namespace eol

