#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>d
#include "Scene.h"
#include "Application.h"
#include "GameSettings.h"

class PauseMenuScene : public Scene
{
public:
    PauseMenuScene(Application& app);

    // Lifecycle
    void onEnter() override {}
    void onExit() override {}

    // Scene interface
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    // Pause menu should NOT block render of scenes under it,
    // but it SHOULD block update.
    bool blocksUpdate() const override { return false; }
    bool isTransparent() const override { return true; }

private:
    void updateVisuals();
    void activateSelection();

private:
    Application& app;

    sf::Font font;
    std::vector<sf::Text> buttons;
    int selectedIndex = 0;
};
