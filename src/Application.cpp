#include "Application.h"
#include <iostream>

Application::Application()
{
    // Start at default (1920x1080)
    const auto& resolutions = GameSettings::getAvailableResolutions();
    const auto& res = resolutions[currentResolutionIndex];

    window.create(
        sf::VideoMode({ res.width, res.height }),
        "Echoes of Light",
        sf::Style::Close
    );

    setFramerateLimit(currentFramerate);

    // Apply scaled reference view
    scaledView = GameSettings::getScaledView(window.getSize());
    window.setView(scaledView);
}

void Application::run()
{
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        processEvents();
        update(dt);
        render();
    }
}

// --------------------------------------------------------
// Scene Management
// --------------------------------------------------------

void Application::pushScene(std::shared_ptr<Scene> scene)
{
    sceneStack.pushScene(scene);
}

void Application::popScene()
{
    sceneStack.popScene();
}

void Application::replaceScene(std::shared_ptr<Scene> scene)
{
    sceneStack.replaceScene(scene);
}

// --------------------------------------------------------
// Window Settings
// --------------------------------------------------------

void Application::setResolution(unsigned int index)
{
    const auto& resolutions = GameSettings::getAvailableResolutions();

    if (index >= resolutions.size())
        return;

    currentResolutionIndex = index;
    const auto& res = resolutions[index];

    window.create(
        sf::VideoMode({ res.width, res.height }),
        "Echoes of Light",
        sf::Style::Close
    );

    // Recalculate scaling
    scaledView = GameSettings::getScaledView(window.getSize());
    window.setView(scaledView);

    setFramerateLimit(currentFramerate);

    std::cout << "Resolution changed to: " << res.label << "\n";
}

void Application::setFramerateLimit(unsigned int limit)
{
    currentFramerate = limit;
    window.setFramerateLimit(limit);

    if (limit == 0)
        std::cout << "Framerate: Unlimited\n";
    else
        std::cout << "Framerate set to: " << limit << "\n";
}

// --------------------------------------------------------
// Main Loop Internals
// --------------------------------------------------------

void Application::processEvents()
{
    while (auto event = window.pollEvent())
    {
        // Global close
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            return;
        }

        // Forward to current scene
        sceneStack.handleEvent(*event);
    }
}

void Application::update(float dt)
{
    sceneStack.update(dt);
}

void Application::render()
{
    window.clear(sf::Color(20, 20, 30));

    window.setView(scaledView);
    sceneStack.render(window);

    window.display();
}
