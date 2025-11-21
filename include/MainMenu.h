#pragma once
#include "State.h"
#include <vector>
#include <SFML/Graphics.hpp>

class MainMenu : public State {
private:
    sf::Font font;
    std::vector<sf::Text> buttons;
    int selectedIndex = 0;

    void updateVisuals();

public:
    MainMenu();

    void handleInput(sf::RenderWindow& window) override;
    void update(float dt) override {}
    void draw(sf::RenderWindow& window) override;
};
