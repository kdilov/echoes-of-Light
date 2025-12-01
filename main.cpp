#include <exception>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "Game.h"
#include "MainMenu.h"
#include "GameSettings.h"



int main() {

    // Create window at Full HD resolution by default
    sf::RenderWindow window(sf::VideoMode({ GameSettings::refWidth, GameSettings::refHeight }), "Echoes of Light");

    
    MainMenu menu;

    int choice = menu.run(window);

    if (choice == 0) {
        Game game;
        return game.run();
    }
    else if(choice == 1){
        // Options menu to be implemented
        return 0;
    }
    else {
        return 0;
        // Quit the game
    }
}