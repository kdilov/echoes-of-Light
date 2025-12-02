#pragma once
#include <SFML/Graphics.hpp>

class Scene
{
public:
    virtual ~Scene() = default;
    // Called when first on stack
    virtual void onEnter() {}
    // Called when removed
    virtual void onExit() {}
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    // Handle events 
    virtual void handleEvent(const sf::Event& event) = 0;
    //Block game when paused
    virtual bool blocksUpdate() const { return true; }
    virtual bool isTransparent() const { return false; }
};
