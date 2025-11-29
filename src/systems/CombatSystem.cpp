#include "Systems.h"

#include "components/EnemyComponent.h"
#include "components/LightComponent.h"
#include "components/PlayerComponent.h"
#include "components/RenderComponent.h"

#include <algorithm>

namespace {

float clampf(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(maxValue, value));
}

} // namespace

void CombatSystem::applyBeamHit(Entity& attacker,
                                Entity& target,
                                float intensity,
                                const sf::Vector2f& /*hitPoint*/) {
    if (applyEnemyHit(target, intensity)) {
        return;
    }

    applyPlayerHit(attacker, target, intensity);
}

bool CombatSystem::applyEnemyHit(Entity& target, float intensity) {
    auto* enemy = target.getComponent<eol::EnemyComponent>();
    if (!enemy) {
        return false;
    }

    const float resistanceLoss = clampf(intensity * 0.04f, 0.f, 5.f);
    enemy->setResistance(std::max(0.f, enemy->getResistance() - resistanceLoss));

    const float mitigation = clampf(enemy->getResistance(), 0.f, 1.f);
    const float damage = intensity * (0.35f + (1.f - mitigation) * 0.5f);
    enemy->applyDamage(damage);

    if (!enemy->isAlive()) {
        enemy->setBlocksLight(false);
        if (auto* render = target.getComponent<eol::RenderComponent>()) {
            render->setTint(sf::Color(30, 30, 30, 160));
        }
        if (auto* light = target.getComponent<eol::LightComponent>()) {
            light->setIntensity(light->getBaseIntensity());
        }
        return true;
    }

    if (auto* render = target.getComponent<eol::RenderComponent>()) {
        const float healthRatio = clampf(
            enemy->getHealth() / std::max(1.f, enemy->getMaxHealth()),
            0.f,
            1.f);
        const std::uint8_t green = static_cast<std::uint8_t>(100.f + 155.f * healthRatio);
        const std::uint8_t blue = static_cast<std::uint8_t>(120.f + 110.f * healthRatio);
        render->setTint(sf::Color(255, green, blue, 200));
    }

    return true;
}

void CombatSystem::applyPlayerHit(Entity& attacker, Entity& target, float intensity) {
    auto* player = target.getComponent<eol::PlayerComponent>();
    if (!player) {
        return;
    }

    if (player->isInvulnerable()) {
        return;
    }

    constexpr float baseDamage = 8.f;
    float damage = baseDamage + intensity * 0.15f;
    if (attacker.getComponent<eol::EnemyComponent>()) {
        damage *= 1.25f;
    }

    player->applyDamage(damage);

    if (auto* render = target.getComponent<eol::RenderComponent>()) {
        render->setTint(sf::Color(255, 160, 160, 220));
    }
}

