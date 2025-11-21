#include "MainMenu.h"
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
        text.setCharacterSize(40);
        text.setPosition(sf::Vector2f(200.f, 200.f + i * 60.f));
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
    for (auto& b : buttons)
        window.draw(b);
}

