#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Scene.h"
#include "Application.h"
#include "GameSettings.h"



class OptionsMenuScene : public Scene
{
public:
    OptionsMenuScene(Application& app);

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
    void applyLeft();
    void applyRight();
    void activateSelection();

private:
    Application& app;

    sf::Font font;
    std::vector<sf::Text> buttons;

    int selectedIndex = 0;

    // Resolution list (width/height pairs)
    std::vector<sf::Vector2u> resolutions{
        {1920,1080},
        {1280,720},
        {800,600}
    };

    // Framerate list (0 = unlimited)
    std::vector<unsigned int> framerates{ 60, 120, 0 };

    int resIndex = 0;
    int fpsIndex = 0;
};
