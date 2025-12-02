#include "scenes/MainMenuScene.h"
#include "scenes/OptionsMenuScene.h"
#include "scenes/GameplayScene.h"
#include "Application.h"
#include "GameSettings.h"
#include <iostream>

MainMenuScene::MainMenuScene(Application& app)
    : app(app)
{
    if (!font.openFromFile("resources/fonts/ScienceGothic.ttf"))
        std::cout << "Failed to load font!\n";

    const std::vector<std::string> labels = {
        "Start Game",
        "Options",
        "Quit"
    };

    buttons.clear();
    buttons.reserve(labels.size());

    for (int i = 0; i < labels.size(); ++i)
    {
        sf::Text text(font);             // ✔ SFML 3-compliant
        text.setString(labels[i]);
        text.setCharacterSize(64);
        text.setPosition(GameSettings::relativePos(0.30f, 0.40f + i * 0.12f));

        buttons.push_back(text);
    }

    buttons[0].setFillColor(sf::Color::Yellow);
}

void MainMenuScene::updateVisuals()
{
    for (int i = 0; i < buttons.size(); ++i)
        buttons[i].setFillColor(i == selectedIndex ? sf::Color::Yellow
            : sf::Color::White);
}

void MainMenuScene::handleEvent(const sf::Event& event)
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
        if (selectedIndex == 0)
            app.pushScene(std::make_shared<GameplayScene>(app));

        else if (selectedIndex == 1)
            app.pushScene(std::make_shared<OptionsMenuScene>(app));

        else if (selectedIndex == 2)
            app.getWindow().close();
    }
}

void MainMenuScene::update(float) {}

void MainMenuScene::render(sf::RenderWindow& window)
{
    sf::Text title(font);
    title.setString("ECHOES OF LIGHT");
    title.setCharacterSize(96);
    title.setFillColor(sf::Color(255, 230, 160));
    title.setPosition(GameSettings::relativePos(0.20f, 0.15f));

    window.draw(title);

    for (auto& b : buttons)
        window.draw(b);
}
