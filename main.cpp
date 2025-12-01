#include <exception>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "Game.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "GameSettings.h"


int main() {

    // Create window at Full HD resolution by default
    sf::RenderWindow window(sf::VideoMode({ GameSettings::refWidth, GameSettings::refHeight }), "Echoes of Light");

    Game game;
    game.createWindow();
    Menu menu(game);
    menu.run();
    return 0;


}