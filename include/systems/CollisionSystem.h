#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

struct Entity;

// Simple collision helper - all static methods, no state
class CollisionSystem {
public:
    // Check if two boxes overlap
    static bool checkOverlap(const sf::FloatRect& a, const sf::FloatRect& b);

    // Get bounding box for an entity in world space
    static sf::FloatRect getBounds(Entity& entity);

    // Check if moving to a position would hit any solid entity
    static bool wouldCollide(const sf::Vector2f& position,
                             const sf::Vector2f& size,
                             std::vector<Entity*>& entities,
                             Entity* ignore = nullptr);
};
