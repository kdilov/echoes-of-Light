#include "SFML/Graphics.hpp"
#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu() {

    if (!font.loadFromFile("resources/fonts/ScienceGothic.ttf")) {
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
    while (window.isOpen()) {
        

        while (window.pollEvent()) {
            if (event.type == sf::Event::Closed)
                return 2; // Quit

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) {
                    selectedIndex = (selectedIndex - 1 + buttons.size()) % buttons.size();
                    updateVisuals();
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    selectedIndex = (selectedIndex + 1) % buttons.size();
                    updateVisuals();
                }
                else if (event.key.code == sf::Keyboard::Enter) {
                    return selectedIndex;
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

