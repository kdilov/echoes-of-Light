#include "scenes/GameplayScene.h"
#include "scenes/PauseMenuScene.h"
#include "GameSettings.h"
#include <iostream>

GameplayScene::GameplayScene(Application& app)
    : app(app)
{
}

void GameplayScene::onEnter()
{
    if (!initialized)
    {
        if (!game.initialize())
        {
            std::cout << "Failed to initialize Game!" << std::endl;
        }

        initialized = true;
    }
}

void GameplayScene::handleEvent(const sf::Event& event)
{
    // Pause on ESC
    if (event.is<sf::Event::KeyPressed>())
    {
        auto key = event.getIf<sf::Event::KeyPressed>()->code;

        if (key == sf::Keyboard::Key::Escape)
        {
            app.pushScene(std::make_shared<PauseMenuScene>(app));
            return;
        }
    }

    // NOTE:
    // If you decide to move any input-event logic for the player here,
    // you can do that later. Currently, InputSystem handles real-time inputs,
    // so no work is needed.
}

void GameplayScene::update(float dt)
{
    // Get window reference
    auto& window = app.getWindow();

    // IMPORTANT:
    // Ensure gameplay uses the scaled reference view (for correct collision,
    // mouse mapping, raycasts, and light system behaviour)
    sf::View scaledView = GameSettings::getScaledView(window.getSize());
    window.setView(scaledView);

    // Update gameplay
    game.update(dt, window);
}

void GameplayScene::render(sf::RenderWindow& window)
{
    // Use the same scaled view for rendering
    sf::View scaledView = GameSettings::getScaledView(window.getSize());
    window.setView(scaledView);

    // Render the ECS world
    game.render(window);
}

