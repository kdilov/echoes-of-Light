#include <iostream>
#include "Application.h"
#include "scenes/MainMenuScene.h"

int main()
{
    try
    {
        Application app;

        // Start at the main menu
        app.pushScene(std::make_shared<MainMenuScene>(app));

        // Run the application loop
        app.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << "\n";
        return -1;
    }

    return 0;
}
