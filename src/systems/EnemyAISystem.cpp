#include "Systems.h"

#include "components/CollisionComponent.h"
#include "components/EnemyAIComponent.h"
#include "components/MeleeAttackComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "systems/CollisionSystem.h"
#include "GameSettings.h"

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
        const bool hasLos = hasLineOfSight(enemyPos, playerPos, entities, entity, &player);

        eol::EnemyAIComponent::BehaviorState nextState = eol::EnemyAIComponent::BehaviorState::Patrol;
        if (hasLos && distanceSq <= attackRange * attackRange) {
            nextState = eol::EnemyAIComponent::BehaviorState::Attack;
        }
        else if (hasLos && distanceSq <= detectionRange * detectionRange) {
            nextState = eol::EnemyAIComponent::BehaviorState::Chase;
        }

        ai->setState(nextState);
        if (ai->isEnabled()) {
            driveBehavior(*entity, *ai, playerPos, deltaTime, entities);
        }
    }
}

void EnemyAISystem::driveBehavior(Entity& entity,
                                  eol::EnemyAIComponent& ai,
                                  const sf::Vector2f& playerPos,
                                  float deltaTime,
                                  std::vector<Entity*>& entities) {
    switch (ai.getState()) {
        case eol::EnemyAIComponent::BehaviorState::Attack:
            executeAttack(entity);
            break;
        case eol::EnemyAIComponent::BehaviorState::Chase:
            executeChase(entity, ai, playerPos, deltaTime, entities);
            break;
        case eol::EnemyAIComponent::BehaviorState::Patrol:
        default:
            executePatrol(entity, ai, deltaTime, entities);
            break;
    }
}

void EnemyAISystem::executePatrol(Entity& entity,
                                  eol::EnemyAIComponent& ai,
                                  float deltaTime,
                                  std::vector<Entity*>& entities) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    auto* collision = entity.getComponent<eol::CollisionComponent>();
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
    const float speed = ai.getMoveSpeed();
    const sf::Vector2f currentPos = transform->getPosition();
    sf::Vector2f desiredPos = currentPos + direction * speed * deltaTime;
    desiredPos = GameSettings::clampToWorld(desiredPos, 0.02f);

    if (!collision || !CollisionSystem::wouldCollide(desiredPos, collision->getBoundingBox(), entities, &entity)) {
        transform->setPosition(desiredPos);
        return;
    }

    // Slide attempt along axes
    sf::Vector2f tryX{ desiredPos.x, currentPos.y };
    if (!collision || !CollisionSystem::wouldCollide(tryX, collision->getBoundingBox(), entities, &entity)) {
        transform->setPosition(tryX);
        return;
    }

    sf::Vector2f tryY{ currentPos.x, desiredPos.y };
    if (!collision || !CollisionSystem::wouldCollide(tryY, collision->getBoundingBox(), entities, &entity)) {
        transform->setPosition(tryY);
    }
}

void EnemyAISystem::executeChase(Entity& entity,
                                 const eol::EnemyAIComponent& ai,
                                 const sf::Vector2f& playerPos,
                                 float deltaTime,
                                 std::vector<Entity*>& entities) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    auto* collision = entity.getComponent<eol::CollisionComponent>();
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
    const float speed = ai.getMoveSpeed();
    const sf::Vector2f currentPos = transform->getPosition();
    sf::Vector2f desiredPos = currentPos + direction * speed * deltaTime;
    desiredPos = GameSettings::clampToWorld(desiredPos, 0.02f);

    auto tryMove = [&](const sf::Vector2f& pos) -> bool {
        if (!collision || !CollisionSystem::wouldCollide(pos, collision->getBoundingBox(), entities, &entity)) {
            transform->setPosition(pos);
            return true;
        }
        return false;
    };

    if (tryMove(desiredPos)) return;
    if (tryMove({ desiredPos.x, currentPos.y })) return;
    tryMove({ currentPos.x, desiredPos.y });
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

bool EnemyAISystem::hasLineOfSight(const sf::Vector2f& origin,
                                   const sf::Vector2f& target,
                                   const std::vector<Entity*>& entities,
                                   Entity* self,
                                   Entity* targetEntity) const {
    for (Entity* entity : entities) {
        if (!entity || entity == self || entity == targetEntity) {
            continue;
        }

        auto* collision = entity->getComponent<eol::CollisionComponent>();
        if (!collision || !collision->isSolid()) {
            continue;
        }

        sf::FloatRect bounds = CollisionSystem::getBounds(*entity);
        if (lineIntersectsRect(origin, target, bounds)) {
            return false;
        }
    }

    // Include player's own size when checking immediate obstruction (optional)
    return true;
}

bool EnemyAISystem::lineIntersectsRect(const sf::Vector2f& a,
                                       const sf::Vector2f& b,
                                       const sf::FloatRect& rect) const {
    const float xMin = rect.position.x;
    const float xMax = rect.position.x + rect.size.x;
    const float yMin = rect.position.y;
    const float yMax = rect.position.y + rect.size.y;

    float t0 = 0.f;
    float t1 = 1.f;
    const sf::Vector2f d = b - a;

    auto clip = [&](float p, float q) -> bool {
        if (std::abs(p) < 1e-6f) {
            return q >= 0.f;
        }
        const float r = q / p;
        if (p < 0.f) {
            if (r > t1) return false;
            if (r > t0) t0 = r;
        }
        else {
            if (r < t0) return false;
            if (r < t1) t1 = r;
        }
        return true;
    };

    if (clip(-d.x, a.x - xMin) &&
        clip(d.x, xMax - a.x) &&
        clip(-d.y, a.y - yMin) &&
        clip(d.y, yMax - a.y)) {
        return t0 <= t1;
    }

    return false;
}

