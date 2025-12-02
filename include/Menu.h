#pragma once
#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>



class Menu  {
protected:
    sf::Font font;
    std::vector<sf::Text> buttons;
    int selectedIndex = 0;
    Game& game;
    void updateVisuals();

public:
    Menu(Game& game);
    virtual int run();
    virtual void draw();
    int choice(int selectedIndex);
  
};
