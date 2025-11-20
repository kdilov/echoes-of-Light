#include "Systems.h"
#include "components/TransformComponent.h"
#include "components/RenderComponent.h"
#include "components/PlayerComponent.h"
#include "components/AnimationComponent.h"
#include <cmath>
#include <iostream>


// INPUT SYSTEM


void InputSystem::update(Entity& player, float deltaTime) {
    auto* transform = player.getComponent<eol::TransformComponent>();
    auto* playerComp = player.getComponent<eol::PlayerComponent>();
    auto* animation = player.getComponent<eol::AnimationComponent>();

    if (!transform || !playerComp) {
        return;
    }

    // Get movement input
    sf::Vector2f movement = getMovementInput();
    bool isMoving = (movement.x != 0.f || movement.y != 0.f);

    // Update animation based on movement
    if (animation) {
        if (isMoving) {
            animation->setAnimation("walk");
        }
        else {
            animation->setAnimation("idle");
        }
    }

    // Apply movement
    if (isMoving) {
        // Normalize diagonal movement
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement.x /= length;
        movement.y /= length;

        // Get speed from PlayerComponent
        float speed = playerComp->getMovementSpeed();

        // Update position
        sf::Vector2f pos = transform->getPosition();
        pos.x += movement.x * speed * deltaTime;
        pos.y += movement.y * speed * deltaTime;

        // Keep on screen (sprite is 64x64 at 0.5 scale)
        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x > 800 - 64) pos.x = 800 - 64;
        if (pos.y > 600 - 64) pos.y = 600 - 64;

        transform->setPosition(pos);
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

// ANIMATION SYSTEM

void AnimationSystem::update(std::vector<Entity*>& entities, float deltaTime) {
    for (Entity* entity : entities) {
        if (!entity) continue;

        auto* animation = entity->getComponent<eol::AnimationComponent>();
        if (animation && animation->isEnabled()) {
            animation->update(deltaTime);
        }
    }
}

// RENDER SYSTEM

void RenderSystem::render(sf::RenderWindow& window, std::vector<Entity*>& entities) {
    for (Entity* entity : entities) {
        if (!entity) continue;

        auto* render = entity->getComponent<eol::RenderComponent>();
        if (!render || !render->isEnabled()) {
            continue;
        }

        // Get the sprite from RenderComponent
        sf::Sprite& sprite = render->getSprite();

        // Update sprite from other components
        updateSpriteFromComponents(sprite, *entity);

        // Draw the sprite
        window.draw(sprite);
    }
}

void RenderSystem::updateSpriteFromComponents(sf::Sprite& sprite, Entity& entity) {
    // Update position and scale from TransformComponent
    auto* transform = entity.getComponent<eol::TransformComponent>();
    if (transform) {
        sprite.setPosition(transform->getPosition());
        sprite.setScale(transform->getScale());
        sprite.setRotation(sf::degrees(transform->getRotation()));
    }

    // Update texture and frame from AnimationComponent
    auto* animation = entity.getComponent<eol::AnimationComponent>();
    if (animation) {
        const sf::Texture* texture = animation->getCurrentTexture();
        if (texture) {
            sprite.setTexture(*texture, false); // false = don't reset rect
            sprite.setTextureRect(animation->getCurrentFrameRect());
        }
    }

    // Apply tint from RenderComponent
    auto* render = entity.getComponent<eol::RenderComponent>();
    if (render) {
        sprite.setColor(render->getTint());
    }
}