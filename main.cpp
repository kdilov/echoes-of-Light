#include <exception>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Game.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "GameSettings.h"


int main() {

   
    Game game;

    if (!game.initialize())
        return -1;

    int menuResult = 2;

    while (menuResult != 0) {   
        Menu menu(game);
        menuResult = menu.run();

        if (menuResult == 1) {  
            game.run();
            menuResult = 2;     
        }
    }

    return 0;

}