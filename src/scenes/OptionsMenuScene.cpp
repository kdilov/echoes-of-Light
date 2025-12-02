#include "scenes/OptionsMenuScene.h"
#include "Application.h"
#include "GameSettings.h"
#include <iostream>

OptionsMenuScene::OptionsMenuScene(Application& app)
    : app(app)
{
    if (!font.openFromFile("resources/fonts/ScienceGothic.ttf"))
        std::cout << "Failed to load font!\n";

    buttons.clear();
    buttons.reserve(3);

    const std::vector<std::string> labels = {
        "Resolution",
        "Framerate",
        "Back"
    };

    for (int i = 0; i < labels.size(); ++i)
    {
        sf::Text text(font);          // ✔ SFML 3 compliance
        text.setString(labels[i]);
        text.setCharacterSize(48);
        text.setPosition(GameSettings::relativePos(0.25f, 0.30f + i * 0.12f));
        buttons.push_back(text);
    }

    updateVisuals();
}

void OptionsMenuScene::updateVisuals()
{
    // Update resolution text
    const auto& r = resolutions[resIndex];
    buttons[0].setString("Resolution: " + std::to_string(r.x) + " x " + std::to_string(r.y));

    // Update FPS text
    unsigned int fps = framerates[fpsIndex];
    buttons[1].setString("Framerate: " +
        (fps == 0 ? std::string("Unlimited") : std::to_string(fps)));

    // Highlight selected option
    for (int i = 0; i < buttons.size(); ++i)
        buttons[i].setFillColor(i == selectedIndex ? sf::Color::Yellow
            : sf::Color::White);
}

void OptionsMenuScene::handleEvent(const sf::Event& event)
{
    if (!event.is<sf::Event::KeyPressed>())
        return;

    auto key = event.getIf<sf::Event::KeyPressed>()->code;

    switch (key)
    {
    case sf::Keyboard::Key::Up:
        selectedIndex = (selectedIndex - 1 + buttons.size()) % buttons.size();
        updateVisuals();
        break;

    case sf::Keyboard::Key::Down:
        selectedIndex = (selectedIndex + 1) % buttons.size();
        updateVisuals();
        break;

    case sf::Keyboard::Key::Left:
        applyLeft();
        break;

    case sf::Keyboard::Key::Right:
        applyRight();
        break;

    case sf::Keyboard::Key::Enter:
        activateSelection();
        break;

    case sf::Keyboard::Key::Escape:
        app.popScene();
        break;
    }
}

void OptionsMenuScene::applyLeft()
{
    if (selectedIndex == 0)    // Resolution
        resIndex = (resIndex - 1 + resolutions.size()) % resolutions.size();

    else if (selectedIndex == 1) // FPS
    {
        fpsIndex = (fpsIndex - 1 + framerates.size()) % framerates.size();
        app.setFramerateLimit(framerates[fpsIndex]);
    }

    updateVisuals();
}

void OptionsMenuScene::applyRight()
{
    if (selectedIndex == 0)
        resIndex = (resIndex + 1) % resolutions.size();

    else if (selectedIndex == 1)
    {
        fpsIndex = (fpsIndex + 1) % framerates.size();
        app.setFramerateLimit(framerates[fpsIndex]);
    }

    updateVisuals();
}

void OptionsMenuScene::activateSelection()
{
    if (selectedIndex == 0)
        app.setResolution(resIndex);

    else if (selectedIndex == 2)
        app.popScene();
}

void OptionsMenuScene::update(float) {}

void OptionsMenuScene::render(sf::RenderWindow& window)
{
    for (auto& b : buttons)
        window.draw(b);
}
