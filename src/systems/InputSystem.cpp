#include "Systems.h"

#include "components/AnimationComponent.h"
#include "components/PlayerComponent.h"
#include "components/TransformComponent.h"

#include <cmath>

void InputSystem::update(Entity& player, float deltaTime) {
    auto* transform = player.getComponent<eol::TransformComponent>();
    auto* playerComp = player.getComponent<eol::PlayerComponent>();
    auto* animation = player.getComponent<eol::AnimationComponent>();

    if (!transform || !playerComp) {
        return;
    }

    sf::Vector2f movement = getMovementInput();
    const bool isMoving = (movement.x != 0.f || movement.y != 0.f);

    if (animation) {
        animation->setAnimation(isMoving ? "walk" : "idle");
    }

    if (!isMoving) {
        return;
    }

    const float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length > 0.f) {
        movement.x /= length;
        movement.y /= length;
    }

    const float speed = playerComp->getMovementSpeed();
    sf::Vector2f pos = transform->getPosition();
    pos.x += movement.x * speed * deltaTime;
    pos.y += movement.y * speed * deltaTime;

    if (pos.x < 0.f) pos.x = 0.f;
    if (pos.y < 0.f) pos.y = 0.f;
    if (pos.x > 736.f) pos.x = 736.f;
    if (pos.y > 536.f) pos.y = 536.f;

    transform->setPosition(pos);
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

