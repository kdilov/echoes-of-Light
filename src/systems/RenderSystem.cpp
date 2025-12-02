#include "Systems.h"

#include "components/AnimationComponent.h"
#include "components/EnemyComponent.h"
#include "components/PlayerComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>

void RenderSystem::render(sf::RenderWindow& window, std::vector<Entity*>& entities) {
    Entity* player = nullptr;
    for (Entity* entity : entities) {
        if (!entity) continue;

        auto* render = entity->getComponent<eol::RenderComponent>();
        if (!render || !render->isEnabled()) {
            continue;
        }

        sf::Sprite& sprite = render->getSprite();
        updateSpriteFromComponents(sprite, *entity);
        window.draw(sprite);
        drawEnemyHealthBar(window, *entity);

        if (entity->name == "Player") {
            player = entity;
        }
    }

    if (player) {
        drawPlayerHealthBar(window, *player);
    }
}

void RenderSystem::updateSpriteFromComponents(sf::Sprite& sprite, Entity& entity) {
    if (auto* transform = entity.getComponent<eol::TransformComponent>()) {
        sprite.setPosition(transform->getPosition());
        sprite.setScale(transform->getScale());
        sprite.setRotation(sf::degrees(transform->getRotation()));
    }

    if (auto* animation = entity.getComponent<eol::AnimationComponent>()) {
        if (const sf::Texture* texture = animation->getCurrentTexture()) {
            sprite.setTexture(*texture, false);
            sprite.setTextureRect(animation->getCurrentFrameRect());

            sf::IntRect frameRect = animation->getCurrentFrameRect();
            sprite.setOrigin(sf::Vector2f(frameRect.size.x / 2.f, frameRect.size.y / 2.f));
        }
    }

    if (auto* render = entity.getComponent<eol::RenderComponent>()) {
        sprite.setColor(render->getTint());
    }
}

void RenderSystem::drawEnemyHealthBar(sf::RenderWindow& window, Entity& entity) {
    auto* enemy = entity.getComponent<eol::EnemyComponent>();
    auto* transform = entity.getComponent<eol::TransformComponent>();
    if (!enemy || !transform || !enemy->isAlive()) {
        return;
    }

    const float healthRatio = std::clamp(
        enemy->getHealth() / std::max(1.f, enemy->getMaxHealth()),
        0.f,
        1.f);

    const float barWidth = 60.f;
    const float barHeight = 6.f;
    const float verticalOffset = -40.f;

    sf::Vector2f barPosition = transform->getPosition();
    barPosition.y += verticalOffset;

    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setOrigin(sf::Vector2f(barWidth * 0.5f, barHeight * 0.5f));
    background.setPosition(barPosition);
    background.setFillColor(sf::Color(20, 20, 25, 220));
    background.setOutlineColor(sf::Color(10, 10, 10, 240));
    background.setOutlineThickness(1.f);
    window.draw(background);

    sf::RectangleShape health(sf::Vector2f(barWidth * healthRatio, barHeight - 2.f));
    health.setOrigin(sf::Vector2f(barWidth * 0.5f, (barHeight - 2.f) * 0.5f));
    health.setPosition(barPosition);
    health.setFillColor(sf::Color(255, 90, 90, 240));
    window.draw(health);
}

void RenderSystem::drawPlayerHealthBar(sf::RenderWindow& window, Entity& player) {
    auto* stats = player.getComponent<eol::PlayerComponent>();
    if (!stats) {
        return;
    }

    const float healthRatio = std::clamp(
        stats->getHealth() / std::max(1.f, stats->getMaxHealth()),
        0.f,
        1.f);

    const float barWidth = 260.f;
    const float barHeight = 18.f;
    const float margin = 20.f;

    sf::Vector2f origin{ margin, margin };

    sf::RectangleShape background(sf::Vector2f(barWidth, barHeight));
    background.setPosition(origin);
    background.setFillColor(sf::Color(10, 10, 20, 220));
    background.setOutlineColor(sf::Color(0, 0, 0, 240));
    background.setOutlineThickness(2.f);
    window.draw(background);

    sf::RectangleShape health(sf::Vector2f(barWidth * healthRatio, barHeight - 4.f));
    health.setPosition(origin + sf::Vector2f(2.f, 2.f));
    health.setFillColor(sf::Color(120, 255, 160, 240));
    window.draw(health);
}

