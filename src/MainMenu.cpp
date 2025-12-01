#include "SFML/Graphics.hpp"
#include "MainMenu.h"
#include "GameSettings.h"
#include <iostream>

MainMenu::MainMenu() {

    if (!font.openFromFile("resources/fonts/ScienceGothic.ttf")) {
        std::cout << "Failed to load font!\n";
    }

    const std::vector<std::string> labels = {
        "Start",
        "Options",
        "Quit"
    };

    for (int i = 0; i < labels.size(); i++) {
        sf::Text text(font);
        text.setString(labels[i]);
        text.setCharacterSize(72);
        text.setPosition(GameSettings::relativePos(0.25f, 0.33f + static_cast<float>(i) * 0.11f));
        buttons.push_back(text);
    }

    buttons[0].setFillColor(sf::Color::Yellow);
}

void MainMenu::updateVisuals() {
    for (int i = 0; i < buttons.size(); i++) {
        buttons[i].setFillColor(i == selectedIndex ? sf::Color::Yellow : sf::Color::White);
    }
}

int MainMenu::run(sf::RenderWindow& window) {
    const auto buttonCount = static_cast<int>(buttons.size());
    if (buttonCount == 0) {
        return 2;
    }

    // Set up scaled view for the menu
    sf::View menuView = GameSettings::getScaledView(window.getSize());
    window.setView(menuView);

    while (window.isOpen()) {

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 2;
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
                case sf::Keyboard::Key::Enter:
                    return selectedIndex;
                case sf::Keyboard::Key::Escape:
                    return 2;
                default:
                    break;
                }
            }
        }

        
    
        window.clear();
        draw(window);
        window.display();
    }

    return 2; // Quit by default
}


void MainMenu::draw(sf::RenderWindow& window) {
    // Draw title
    sf::Text title(font);
    title.setString("ECHOES OF LIGHT");
    title.setCharacterSize(96);
    title.setFillColor(sf::Color(255, 230, 160));
    title.setPosition(GameSettings::relativePos(0.25f, 0.14f));
    window.draw(title);

    // Draw menu buttons
    for (auto& b : buttons)
        window.draw(b);

    // Draw resolution hint
    sf::Text hint(font);
    hint.setString("Press F1/F2/F3 in game to change resolution");
    hint.setCharacterSize(28);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setPosition(GameSettings::relativePos(0.25f, 0.83f));
    window.draw(hint);
}
