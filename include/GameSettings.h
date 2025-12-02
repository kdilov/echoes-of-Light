#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>


// Stores resolution width, height and a display label
struct Resolution {
    unsigned int width;
    unsigned int height;
    std::string label;
};

// Handles resolution settings and view scaling for the game
// All game logic uses the reference resolution (1920x1080)
// The view automatically scales to fit any window size
class GameSettings {
public:
    // Reference resolution - design all game elements for this size
    static constexpr unsigned int refWidth = 1920;
    static constexpr unsigned int refHeight = 1080;

    // Returns the list of available resolutions
    static const std::vector<Resolution>& getAvailableResolutions() {
        static std::vector<Resolution> resolutions = {
            {1920, 1080, "1920x1080 (Full HD)"},
            {1280, 720,  "1280x720 (HD)"},
            {800,  600,  "800x600"}
        };
        return resolutions;
    }

    // Creates a view that scales the reference resolution to fit the window
    // Maintains aspect ratio with letterboxing if needed
    static sf::View getScaledView(const sf::Vector2u& windowSize) {
        // View covers the entire reference resolution
        sf::View view(sf::FloatRect(
            sf::Vector2f{ 0.f, 0.f },
            sf::Vector2f{ static_cast<float>(refWidth), static_cast<float>(refHeight) }
        ));

        // Calculate aspect ratios
        float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        float viewRatio = static_cast<float>(refWidth) / static_cast<float>(refHeight);

        // Viewport size and position (normalized 0-1)
        float sizeX = 1.f;
        float sizeY = 1.f;
        float posX = 0.f;
        float posY = 0.f;

        if (windowRatio > viewRatio) {
            // Window is wider than view - add letterbox on sides
            sizeX = viewRatio / windowRatio;
            posX = (1.f - sizeX) / 2.f;
        }
        else if (windowRatio < viewRatio) {
            // Window is taller than view - add letterbox on top/bottom
            sizeY = windowRatio / viewRatio;
            posY = (1.f - sizeY) / 2.f;
        }

        view.setViewport(sf::FloatRect(sf::Vector2f{ posX, posY }, sf::Vector2f{ sizeX, sizeY }));
        return view;
    }

    // ========== RELATIVE POSITIONING HELPERS ==========
    // These convert percentages (0.0 - 1.0) to actual pixel positions
    // This avoids hardcoded values and makes layouts resolution-independent

    // Get reference width as float
    static constexpr float width() {
        return static_cast<float>(refWidth);
    }

    // Get reference height as float
    static constexpr float height() {
        return static_cast<float>(refHeight);
    }

    // Center of the screen
    static sf::Vector2f center() {
        return sf::Vector2f{ width() * 0.5f, height() * 0.5f };
    }

    // Convert percentage position to actual coordinates
    // Example: relativePos(0.5f, 0.5f) returns center of screen
    static sf::Vector2f relativePos(float xPercent, float yPercent) {
        return sf::Vector2f{
            xPercent * width(),
            yPercent * height()
        };
    }

    // Convert percentage to size
    // Example: relativeSize(0.1f, 0.05f) returns 10% of width, 5% of height
    static sf::Vector2f relativeSize(float widthPercent, float heightPercent) {
        return sf::Vector2f{
            widthPercent * width(),
            heightPercent * height()
        };
    }

    // Get X position as percentage of width
    static float relativeX(float percent) {
        return percent * width();
    }

    // Get Y position as percentage of height
    static float relativeY(float percent) {
        return percent * height();
    }

    // Get a size value relative to the smaller dimension (useful for squares)
    static float relativeMin(float percent) {
        return percent * (width() < height() ? width() : height());
    }

    // World bounds with margin (for keeping entities on screen)
    static sf::FloatRect getWorldBounds(float marginPercent = 0.025f) {
        float margin = relativeMin(marginPercent);
        return sf::FloatRect(
            sf::Vector2f{ margin, margin },
            sf::Vector2f{ width() - margin * 2.f, height() - margin * 2.f }
        );
    }

    // Clamp a position to stay within world bounds
    static sf::Vector2f clampToWorld(const sf::Vector2f& pos, float marginPercent = 0.025f) {
        float margin = relativeMin(marginPercent);
        return sf::Vector2f{
            std::max(margin, std::min(pos.x, width() - margin)),
            std::max(margin, std::min(pos.y, height() - margin))
        };
    }
};