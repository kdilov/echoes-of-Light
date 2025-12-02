#pragma once
#include <memory>
#include "Scene.h"
#include "Application.h"
#include "Game.h"   
#include "GameSettings.h"



class GameplayScene : public Scene
{
public:
    GameplayScene(Application& app);

    // Lifecycle
    void onEnter() override;
    void onExit() override {}

    // Scene interface
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // Gameplay blocks update of scenes below (always)
    bool blocksUpdate() const override { return true; }
    bool isTransparent() const override { return false; }

private:
    Application& app;

    // Your existing ECS-heavy game logic class
    Game game;

    bool initialized = false;
};
