#include <exception>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "Game.h"
#include "Menu.h"
#include "OptionsMenu.h"



int main() {

    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Echoes of Wisdom");

    Game game;
    game.createWindow();
    Menu menu(game);
    menu.run();
    return 0;


}