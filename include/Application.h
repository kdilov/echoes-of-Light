#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "SceneStack.h"
#include "GameSettings.h"


class Application
{
public:
    Application();

    void run();

    void pushScene(std::shared_ptr<Scene> scene);
    void popScene();
    void replaceScene(std::shared_ptr<Scene> scene);

    // Access to window for scenes
    sf::RenderWindow& getWindow() { return window; }

    // Resolution / framerate control
    void setResolution(unsigned int index);
    void setFramerateLimit(unsigned int limit);

private:
    // Main loop helpers
    void processEvents();
    void update(float dt);
    void render();

private:
    sf::RenderWindow window;
    sf::View scaledView;

    SceneStack sceneStack;

    sf::Clock clock;

    // Current settings
    unsigned int currentResolutionIndex = 0;
    unsigned int currentFramerate = 60;
};
