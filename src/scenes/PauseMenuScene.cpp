#include "scenes/PauseMenuScene.h"
#include "scenes/OptionsMenuScene.h"
#include "scenes/MainMenuScene.h"
#include "Application.h"
#include "GameSettings.h"
#include <iostream>

PauseMenuScene::PauseMenuScene(Application& app)
    : app(app)
{
    if (!font.openFromFile("resources/fonts/ScienceGothic.ttf"))
        std::cout << "Failed to load font!\n";

    const std::vector<std::string> labels = {
        "Resume",
        "Options",
        "Quit to Main Menu"
    };

    buttons.clear();
    buttons.reserve(labels.size());

    for (int i = 0; i < labels.size(); ++i)
    {
        sf::Text text(font);        
        text.setString(labels[i]);
        text.setCharacterSize(64);
        text.setPosition(GameSettings::relativePos(0.30f, 0.35f + i * 0.12f));
        buttons.push_back(text);
    }

    buttons[0].setFillColor(sf::Color::Yellow);
}

void PauseMenuScene::updateVisuals()
{
    for (int i = 0; i < buttons.size(); ++i)
        buttons[i].setFillColor(i == selectedIndex ? sf::Color::Yellow
            : sf::Color::White);
}

void PauseMenuScene::handleEvent(const sf::Event& event)
{
    if (!event.is<sf::Event::KeyPressed>())
        return;

    auto key = event.getIf<sf::Event::KeyPressed>()->code;

    if (key == sf::Keyboard::Key::Up)
    {
        selectedIndex = (selectedIndex - 1 + buttons.size()) % buttons.size();
        updateVisuals();
    }
    else if (key == sf::Keyboard::Key::Down)
    {
        selectedIndex = (selectedIndex + 1) % buttons.size();
        updateVisuals();
    }
    else if (key == sf::Keyboard::Key::Enter)
    {
        activateSelection();
    }
    else if (key == sf::Keyboard::Key::Escape)
    {
        app.popScene(); 
    }
}

void PauseMenuScene::activateSelection()
{
    switch (selectedIndex)
    {
    case 0: // Resume
        app.popScene();
        break;

    case 1: // Options
        app.pushScene(std::make_shared<OptionsMenuScene>(app));
        break;

    case 2: // Quit to Main Menu
        app.popScene(); 
        app.popScene(); 
        app.pushScene(std::make_shared<MainMenuScene>(app));
        break;
    }
}

void PauseMenuScene::update(float) {}

void PauseMenuScene::render(sf::RenderWindow& window)
{
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    sf::Text title(font);
    title.setString("Paused");
    title.setCharacterSize(96);
    title.setFillColor(sf::Color(255, 230, 160));
    title.setPosition(GameSettings::relativePos(0.30f, 0.18f));
    window.draw(title);

    for (auto& b : buttons)
        window.draw(b);
}
