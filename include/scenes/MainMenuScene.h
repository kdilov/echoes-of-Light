#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Scene.h"
#include "Application.h" 
#include "GameSettings.h"

class MainMenuScene : public Scene
{
public:
    MainMenuScene(Application& app);

    // Lifecycle
    void onEnter() override {}
    void onExit() override {}

    // Scene interface
    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;

    bool blocksUpdate() const override { return true; }
    bool isTransparent() const override { return false; }


private:
    void updateVisuals();
    void activateSelection();

private:
    Application& app;
    sf::Font font;
    std::vector<sf::Text> buttons;
    int selectedIndex = 0;
};
