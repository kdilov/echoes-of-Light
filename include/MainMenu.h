#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>


class MainMenu  {
private:
    sf::Event event;
    sf::Font font;
    std::vector<sf::Text> buttons;
    int selectedIndex = 0;

    void updateVisuals();

public:
    MainMenu();
    int run(sf::RenderWindow& window);
    void update(float dt)  {}
    void draw(sf::RenderWindow& window);
};
