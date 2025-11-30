#include "systems/CollisionSystem.h"
#include "Systems.h"
#include "components/CollisionComponent.h"
#include "components/TransformComponent.h"

bool CollisionSystem::checkOverlap(const sf::FloatRect& a, const sf::FloatRect& b) {
    // Standard AABB overlap check
    return a.position.x < b.position.x + b.size.x &&
           a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y &&
           a.position.y + a.size.y > b.position.y;
}

sf::FloatRect CollisionSystem::getBounds(Entity& entity) {
    auto* transform = entity.getComponent<eol::TransformComponent>();
    auto* collision = entity.getComponent<eol::CollisionComponent>();

    // Need both components to get bounds
    if (!transform || !collision) {
        return sf::FloatRect();
    }

    sf::Vector2f pos = transform->getPosition();
    sf::Vector2f size = collision->getBoundingBox();

    // Bounding box is centered on the entity position
    return sf::FloatRect(
        sf::Vector2f(pos.x - size.x * 0.5f, pos.y - size.y * 0.5f),
        size
    );
}

bool CollisionSystem::wouldCollide(const sf::Vector2f& position,
                                   const sf::Vector2f& size,
                                   std::vector<Entity*>& entities,
                                   Entity* ignore) {
    // Create a test box at the position we want to move to
    sf::FloatRect testBox(
        sf::Vector2f(position.x - size.x * 0.5f, position.y - size.y * 0.5f),
        size
    );

    // Check against all solid entities
    for (Entity* entity : entities) {
        // Skip null, self, or non-collidable entities
        if (!entity || entity == ignore) {
            continue;
        }

        auto* collision = entity->getComponent<eol::CollisionComponent>();
        if (!collision || !collision->isSolid()) {
            continue;
        }

        sf::FloatRect entityBounds = getBounds(*entity);
        if (checkOverlap(testBox, entityBounds)) {
            return true;  // Would hit something
        }
    }

    return false;  // Path is clear
}
