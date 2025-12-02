#include "SFML/Graphics.hpp"
#include "OptionsMenu.h"
#include "Menu.h"
#include "Game.h"
#include <iostream>


OptionsMenu::OptionsMenu(Game& game) : Menu(game) {

    if (!font.openFromFile("resources/fonts/ScienceGothic.ttf")) {
        std::cout << "Failed to load font!\n";
    }
    buttons.clear();

    std::vector<sf::Vector2f> positions = {
        {200, 200},
        {200, 260},
        {200, 320}
    };

    sf::Text text(font);
    for (auto pos : positions) {
        text.setFont(font);
        text.setCharacterSize(40);
        text.setPosition(pos);
        buttons.push_back(text);
    }

    updateVisuals();

}

void OptionsMenu::updateVisuals()
{
    // --- RESOLUTION TEXT ---
    const auto& res = resolutions[resIndex];
    buttons[0].setString(
        "Resolution: " + std::to_string(res.x) + " x " + std::to_string(res.y)
    );

    // --- FRAME RATE TEXT ---
    unsigned int fps = framerates[fpsIndex];
    std::string fpsLabel = (fps == 0) ? "Unlimited" : std::to_string(fps);
    buttons[1].setString("Frame Rate: " + fpsLabel);

    // --- BACK BUTTON
    buttons[2].setString("Back");

    // --- Highlight selected option ---
    for (int i = 0; i < buttons.size(); i++)
    {
        buttons[i].setFillColor(
            (i == selectedIndex) ? sf::Color::Yellow : sf::Color::White
        );
    }
}


int OptionsMenu::run() {

    sf::RenderWindow& window = game.getWindow();

    const auto buttonCount = static_cast<int>(buttons.size());
    if (buttonCount == 0) {
        return 2;
    }

    while (window.isOpen()) {

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                
                case sf::Keyboard::Key::Up:
                    selectedIndex = (selectedIndex - 1 + buttonCount) % buttonCount;
                    updateVisuals();
                    break;
                
                case sf::Keyboard::Key::Down:
                    selectedIndex = (selectedIndex + 1) % buttonCount;
                    updateVisuals();
                    break;
                case sf::Keyboard::Key::Left:
                    if (selectedIndex == 0) {
                        resIndex = (resIndex + resolutions.size() - 1) % resolutions.size();
                        updateVisuals();
                    }
                    if (selectedIndex == 1) {
                        fpsIndex = (fpsIndex + framerates.size() - 1) % framerates.size();
                        game.setFramerateLimit(framerates[fpsIndex]);
                        updateVisuals();
                    }
                    break;

                case sf::Keyboard::Key::Right:
                   
                    if (selectedIndex == 0) {
                        resIndex = (resIndex + 1) % resolutions.size();
                        updateVisuals();
                    }
                    if (selectedIndex == 1) {
                        fpsIndex = (fpsIndex + 1) % framerates.size();
                        game.setFramerateLimit(framerates[fpsIndex]);
                        updateVisuals();
                    }
                    break;
                
                case sf::Keyboard::Key::Enter:
                    if (selectedIndex == 0) {
                        game.setResolution(resIndex);
                        break;
                   }
                
                case sf::Keyboard::Key::Escape:
                    return 2;

                default:
                    break;
                }
            }
        }

        window.clear();
        draw();
        window.display();
    }

    return 1; // Quit by default
}


void OptionsMenu::draw() {
    sf::RenderWindow& window = game.getWindow();
    for (auto& b : buttons)
        window.draw(b);
}

