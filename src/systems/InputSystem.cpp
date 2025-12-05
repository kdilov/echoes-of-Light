#include "Systems.h"
#include "systems/CollisionSystem.h"
#include "components/AnimationComponent.h"
#include "components/LightEmitterComponent.h"
#include "components/PlayerComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "components/CollisionComponent.h"
#include "components/MirrorComponent.h"
#include "GameSettings.h"

#include <cmath>

namespace {
    sf::Vector2f normalizeVector(const sf::Vector2f& value) {
        const float length = std::sqrt(value.x * value.x + value.y * value.y);
        if (length <= 0.0001f) {
            return sf::Vector2f{ 1.f, 0.f };
        }
        return sf::Vector2f{ value.x / length, value.y / length };
    }

    // Rotate a vector by a given angle in degrees (clockwise)
    sf::Vector2f rotateVector(const sf::Vector2f& v, float degrees) {
        float radians = degrees * 3.14159265f / 180.f;
        float cos = std::cos(radians);
        float sin = std::sin(radians);
        return sf::Vector2f{
            v.x * cos + v.y * sin,
            -v.x * sin + v.y * cos
        };
    }
}

void InputSystem::update(Entity& player, float deltaTime, const sf::RenderWindow& window) {
    auto* transform = player.getComponent<eol::TransformComponent>();
    auto* playerComp = player.getComponent<eol::PlayerComponent>();
    auto* animation = player.getComponent<eol::AnimationComponent>();

    if (!transform || !playerComp) {
        return;
    }

    playerComp->tickInvulnerability(deltaTime);

    sf::Vector2f movement = getMovementInput();
    const bool isMoving = (movement.x != 0.f || movement.y != 0.f);

    if (animation) {
        animation->setAnimation(isMoving ? "walk" : "idle");
    }

    if (isMoving) {
        movement = normalizeVector(movement);

        const float speed = playerComp->getMovementSpeed();
        sf::Vector2f pos = transform->getPosition();
        pos.x += movement.x * speed * deltaTime;
        pos.y += movement.y * speed * deltaTime;

        // Clamp to world bounds using GameSettings
        pos = GameSettings::clampToWorld(pos, 0.025f);

        transform->setPosition(pos);
    }

    updatePlayerEmitter(player, window);
}

// update with collision checking to test
void InputSystem::updateWithCollision(Entity& player,
    float deltaTime,
    const sf::RenderWindow& window,
    std::vector<Entity*>& entities) {
    auto* transform = player.getComponent<eol::TransformComponent>();
    auto* playerComp = player.getComponent<eol::PlayerComponent>();
    auto* collision = player.getComponent<eol::CollisionComponent>();
    auto* animation = player.getComponent<eol::AnimationComponent>();

    if (!transform || !playerComp) {
        return;
    }

    handlePickupDrop(player, entities);
    playerComp->tickInvulnerability(deltaTime);
    handleMirrorRotation(player);

    sf::Vector2f movement = getMovementInput();
    const bool isMoving = (movement.x != 0.f || movement.y != 0.f);

    if (animation) {
        animation->setAnimation(isMoving ? "walk" : "idle");
    }

    if (isMoving) {
        movement = normalizeVector(movement);

        const float speed = playerComp->getMovementSpeed();
        sf::Vector2f currentPos = transform->getPosition();
        sf::Vector2f newPos;
        newPos.x = currentPos.x + movement.x * speed * deltaTime;
        newPos.y = currentPos.y + movement.y * speed * deltaTime;

        // Clamp to world bounds using GameSettings
        newPos = GameSettings::clampToWorld(newPos, 0.025f);

        // If player has collision component, check before moving
        if (collision) {
            sf::Vector2f size = collision->getBoundingBox();

            // Try full movement first
            if (!CollisionSystem::wouldCollide(newPos, size, entities, &player)) {
                transform->setPosition(newPos);
            }
            // Try horizontal only (wall slide)
            else if (!CollisionSystem::wouldCollide(
                sf::Vector2f(newPos.x, currentPos.y), size, entities, &player)) {
                transform->setPosition(sf::Vector2f(newPos.x, currentPos.y));
            }
            // Try vertical only (wall slide)
            else if (!CollisionSystem::wouldCollide(
                sf::Vector2f(currentPos.x, newPos.y), size, entities, &player)) {
                transform->setPosition(sf::Vector2f(currentPos.x, newPos.y));
            }
            // Blocked completely - don't move
        }
        else {
            // No collision component, just move
            transform->setPosition(newPos);
        }
    }

    updatePlayerEmitter(player, window);

    if (playerComp->isCarrying()) {
        Entity* carried = playerComp->getCarriedEntity();
        if (carried) {
            if (auto* carriedTransform = carried->getComponent<eol::TransformComponent>()) {
                // Position mirror slightly in front of player
                carriedTransform->setPosition(transform->getPosition());
            }
        }
    }
}

sf::Vector2f InputSystem::getMovementInput() const {
    sf::Vector2f movement(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        movement.y -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        movement.y += 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        movement.x -= 1.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        movement.x += 1.f;
    }

    return movement;
}

void InputSystem::updatePlayerEmitter(Entity& player, const sf::RenderWindow& window) {
    auto* emitter = player.getComponent<eol::LightEmitterComponent>();
    auto* transform = player.getComponent<eol::TransformComponent>();
    if (!emitter || !transform) {
        return;
    }

    sf::Vector2f origin = transform->getPosition();
    if (auto* render = player.getComponent<eol::RenderComponent>()) {
        const sf::Sprite& sprite = render->getSprite();
        origin = sprite.getTransform().transformPoint(sprite.getOrigin());
    }
    const sf::Vector2f cursor = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f aimDir = cursor - origin;
    aimDir = normalizeVector(aimDir);
    emitter->setDirection(aimDir);

    const bool triggerHeld =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
        sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    emitter->setTriggerHeld(triggerHeld);
}

void InputSystem::handlePickupDrop(Entity& player, std::vector<Entity*>& entities) {
    bool ePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);

    // Only trigger on key press, not hold
    if (ePressed && !m_pickupKeyWasPressed) {
        auto* playerComp = player.getComponent<eol::PlayerComponent>();
        auto* playerTransform = player.getComponent<eol::TransformComponent>();
        if (!playerComp || !playerTransform) return;

        if (playerComp->isCarrying()) {
            // Drop the mirror at current position (it's already there)
            playerComp->setCarriedEntity(nullptr);
        }
        else {
            // Try to pick up nearby mirror
            sf::Vector2f playerPos = playerTransform->getPosition();
            const float pickupRange = GameSettings::relativeMin(0.083f);

            for (Entity* entity : entities) {
                if (!entity) continue;
                auto* mirror = entity->getComponent<eol::MirrorComponent>();
                auto* transform = entity->getComponent<eol::TransformComponent>();
                if (!mirror || !transform || !mirror->isPickable()) continue;

                sf::Vector2f diff = transform->getPosition() - playerPos;
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                if (dist < pickupRange) {
                    playerComp->setCarriedEntity(entity);
                    break;
                }
            }
        }
    }
    m_pickupKeyWasPressed = ePressed;
}

// Rotate the carried mirror by 90 degrees when R is pressed
void InputSystem::handleMirrorRotation(Entity& player) {
    bool rPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);

    // Only trigger on key press, not hold
    if (rPressed && !m_rotateKeyWasPressed) {
        auto* playerComp = player.getComponent<eol::PlayerComponent>();
        if (!playerComp || !playerComp->isCarrying()) {
            m_rotateKeyWasPressed = rPressed;
            return;
        }

        Entity* carried = playerComp->getCarriedEntity();
        if (!carried) {
            m_rotateKeyWasPressed = rPressed;
            return;
        }

        // Get mirror and transform components
        auto* mirror = carried->getComponent<eol::MirrorComponent>();
        auto* transform = carried->getComponent<eol::TransformComponent>();

        if (mirror && transform) {
            // Rotate the mirror's normal by 45 degrees (affects light reflection)
            sf::Vector2f currentNormal = mirror->getNormal();
            sf::Vector2f newNormal = rotateVector(currentNormal, 45.f);
            mirror->setNormal(newNormal);

            // Rotate the visual transform by 45 degrees (so sprite looks correct)
            float currentRotation = transform->getRotation();
            transform->setRotation(currentRotation + 45.f);
        }
    }
    m_rotateKeyWasPressed = rPressed;
}

