#include <exception>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Game.h"
#include "Menu.h"
#include "OptionsMenu.h"
#include "GameSettings.h"


int main() {

   

    Game game;

    if (!game.initialize()) {
        return -1;
    }

    Menu menu(game);
    menu.run();
    return 0;



}