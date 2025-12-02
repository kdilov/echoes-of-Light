#pragma once
#include "Menu.h"
#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>




class OptionsMenu : public Menu {
private:
    
    void updateVisuals();

    // list of selectable resolutions
    std::vector<sf::Vector2u> resolutions{
        {1920,1080},
        {1280,720},
        {800,640}
    };
    // Selectable framerates, 0 being uncapped
    std::vector<int> framerates{ 60,120,0 };

    int resIndex = 0;
    int fpsIndex = 0;

public:
    OptionsMenu(Game& game);
    int run() override;
    
    void draw() override;
};
