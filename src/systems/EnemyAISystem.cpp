#include "Systems.h"

#include "components/EnemyAIComponent.h"
#include "components/MeleeAttackComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"

#include <cmath>

namespace {
float lengthSquared(const sf::Vector2f& value) {
    return value.x * value.x + value.y * value.y;
}

sf::Vector2f normalizeVector(const sf::Vector2f& value) {
    const float lenSq = lengthSquared(value);
    if (lenSq <= 0.0001f) {
        return sf::Vector2f{0.f, 0.f};
    }
    const float invLen = 1.f / std::sqrt(lenSq);
    return sf::Vector2f{value.x * invLen, value.y * invLen};
}
} // namespace

void EnemyAISystem::update(std::vector<Entity*>& entities, float deltaTime, Entity& player) {
    auto* playerTransform = player.getComponent<eol::TransformComponent>();
    if (!playerTransform) {
        return;
    }

    const sf::Vector2f playerPos = playerTransform->getPosition();

    for (Entity* entity : entities) {
        if (!entity) continue;

        auto* ai = entity->getComponent<eol::EnemyAIComponent>();
        auto* transform = entity->getComponent<eol::TransformComponent>();
        if (!ai || !transform || !ai->isEnabled()) {
            continue;
        }

        const sf::Vector2f enemyPos = transform->getPosition();
        const sf::Vector2f toPlayer = playerPos - enemyPos;
        const float distanceSq = lengthSquared(toPlayer);
        const float attackRange = ai->getAttackRange();
        const float detectionRange = ai->getDetectionRange();

        eol::EnemyAIComponent::BehaviorState nextState = eol::EnemyAIComponent::BehaviorState::Patrol;
        if (distanceSq <= attackRange * attackRange) {
            nextState = eol::EnemyAIComponent::BehaviorState::Attack;
        }
        else if (distanceSq <= detectionRange * detectionRange) {
            nextState = eol::EnemyAIComponent::BehaviorState::Chase;
        }

        ai->setState(nextState);
        driveBehavior(*entity, *ai, playerPos, deltaTime);
    }
}

void EnemyAISystem::driveBehavior(Entity& entity,
                                  eol::EnemyAIComponent& ai,
                                  const sf::Vector2f& playerPos,
                                  float deltaTime) {
    switch (ai.getState()) {
        case eol::EnemyAIComponent::BehaviorState::Attack:
            executeAttack(entity);
            break;
        case eol::EnemyAIComponent::BehaviorState::Chase:
            executeChase(entity, ai, playerPos, deltaTime);
            break;
        case eol::EnemyAIComponent::BehaviorState::Patrol:
        default:
            executePatrol(entity, ai, deltaTime);
            break;
    }
}

void EnemyAISystem::executePatrol(Entity& entity,
                                  eol::EnemyAIComponent& ai,
                                  float deltaTime) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    auto* melee = entity.getComponent<eol::MeleeAttackComponent>();
    if (melee) {
        melee->setEnabled(false);
    }
    if (auto* render = entity.getComponent<eol::RenderComponent>()) {
        render->setTint(sf::Color(255, 140, 140, 220));
    }

    const auto& points = ai.getPatrolPoints();
    if (points.empty() || !transform) {
        return;
    }

    const std::size_t targetIndex = ai.getCurrentPatrolIndex();
    if (targetIndex >= points.size()) {
        return;
    }

    const sf::Vector2f target = points[targetIndex];
    sf::Vector2f delta = target - transform->getPosition();

    if (lengthSquared(delta) < 16.f) {
        ai.advancePatrolPoint();
        return;
    }

    const sf::Vector2f direction = normalizeVector(delta);
    transform->setPosition(transform->getPosition() + direction * ai.getMoveSpeed() * deltaTime);
}

void EnemyAISystem::executeChase(Entity& entity,
                                 const eol::EnemyAIComponent& ai,
                                 const sf::Vector2f& playerPos,
                                 float deltaTime) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    auto* melee = entity.getComponent<eol::MeleeAttackComponent>();
    if (melee) {
        melee->setEnabled(false);
    }
    if (auto* render = entity.getComponent<eol::RenderComponent>()) {
        render->setTint(sf::Color(255, 120, 120, 230));
    }

    if (!transform) {
        return;
    }

    sf::Vector2f delta = playerPos - transform->getPosition();
    const sf::Vector2f direction = normalizeVector(delta);
    transform->setPosition(transform->getPosition() + direction * ai.getMoveSpeed() * deltaTime);
}

void EnemyAISystem::executeAttack(Entity& entity) {
    auto* melee = entity.getComponent<eol::MeleeAttackComponent>();
    if (melee) {
        melee->setEnabled(true);
    }

    auto* render = entity.getComponent<eol::RenderComponent>();
    if (render) {
        render->setTint(sf::Color(255, 90, 90, 240));
    }
}

