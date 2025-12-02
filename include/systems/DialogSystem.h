#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Represents a single line of dialog
struct DialogLine {
    std::string speaker;    // Name of the speaker (e.g., "King", "Hero")
    std::string text;       // The dialog text
    
    DialogLine(const std::string& spk, const std::string& txt)
        : speaker(spk), text(txt) {}
};

// Stores visual settings for a speaker
struct SpeakerStyle {
    sf::Color nameColor;    // Color for the speaker's name
    sf::Color textColor;    // Color for their dialog text
    
    SpeakerStyle(sf::Color name = sf::Color::White, sf::Color text = sf::Color::White)
        : nameColor(name), textColor(text) {}
};

// Dialog system that displays text with typewriter effect
// Supports multiple speakers with different colors
class DialogSystem {
public:
    DialogSystem();
    
    // Initialize with font - must be called before use
    bool initialize(const sf::Font& font);
    
    // Start a new dialog sequence
    void startDialog(const std::vector<DialogLine>& lines);
    
    // Add a single line to the queue
    void queueLine(const std::string& speaker, const std::string& text);
    
    // Clear all queued dialog
    void clear();
    
    // Update typewriter effect and handle input
    void update(float deltaTime);
    
    // Render the dialog box
    void render(sf::RenderWindow& window);
    
    // Check if dialog is currently showing
    bool isActive() const noexcept;
    
    // Check if current line is fully displayed
    bool isLineComplete() const noexcept;
    
    // Skip to end of current line or advance to next
    void advance();
    
    // Register a speaker with custom colors
    void registerSpeaker(const std::string& name, const SpeakerStyle& style);
    
    // Settings
    void setTypewriterSpeed(float charsPerSecond) noexcept;
    void setBoxOpacity(float opacity) noexcept;
    
private:
    void advanceToNextLine();
    void setupBoxGeometry();
    const SpeakerStyle& getSpeakerStyle(const std::string& speaker) const;
    
private:
    // Font reference
    const sf::Font* m_font;
    bool m_initialized;
    
    // Dialog queue
    std::vector<DialogLine> m_dialogQueue;
    std::size_t m_currentLineIndex;
    
    // Typewriter effect
    std::string m_fullText;           // Complete text of current line
    std::string m_displayedText;      // Text shown so far
    float m_typewriterTimer;
    float m_charsPerSecond;
    std::size_t m_currentCharIndex;
    
    // Speaker styles
    std::map<std::string, SpeakerStyle> m_speakerStyles;
    SpeakerStyle m_defaultStyle;
    
    // Visual elements
    sf::RectangleShape m_boxBackground;
    sf::RectangleShape m_nameBackground;
    
    // Text objects need font at construction, so we use unique_ptr
    // These are created in initialize() once we have the font
    std::unique_ptr<sf::Text> m_nameText;
    std::unique_ptr<sf::Text> m_dialogText;
    std::unique_ptr<sf::Text> m_continueIndicator;
    
    // Settings
    float m_boxOpacity;
    
    // Input tracking
    bool m_advanceKeyWasPressed;
    
    // Animation for continue indicator
    float m_indicatorTimer;
};
