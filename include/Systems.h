#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <memory>

// Forward declarations
namespace eol {
    class Component;
    using ComponentPtr = std::unique_ptr<Component>;
}

// Simple Entity structure (same as before)
struct Entity {
    std::string name;
    std::vector<eol::ComponentPtr> components;

    template<typename T>
    T* getComponent() {
        for (auto& comp : components) {
            if (T* result = dynamic_cast<T*>(comp.get())) {
                return result;
            }
        }
        return nullptr;
    }

    template<typename T>
    bool hasComponent() {
        return getComponent<T>() != nullptr;
    }
};


// INPUT SYSTEM - Handles player keyboard input
class InputSystem {
public:
    void update(Entity& player, float deltaTime);

private:
    sf::Vector2f getMovementInput() const;
};

// ANIMATION SYSTEM - Updates all entity animations
class AnimationSystem {
public:
    void update(std::vector<Entity*>& entities, float deltaTime);
};

// RENDER SYSTEM - Draws all entities to the screen
class RenderSystem {
public:
    void render(sf::RenderWindow& window, std::vector<Entity*>& entities);

private:
    void updateSpriteFromComponents(sf::Sprite& sprite, Entity& entity);
};