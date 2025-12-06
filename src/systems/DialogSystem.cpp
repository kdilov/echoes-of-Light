#include "systems/DialogSystem.h"
#include "GameSettings.h"
#include <algorithm>
#include <cmath>

DialogSystem::DialogSystem()
    : m_font(nullptr)
    , m_initialized(false)
    , m_dialogQueue()
    , m_currentLineIndex(0)
    , m_fullText()
    , m_displayedText()
    , m_typewriterTimer(0.f)
    , m_charsPerSecond(40.f)
    , m_currentCharIndex(0)
    , m_speakerStyles()
    , m_defaultStyle(sf::Color::White, sf::Color::White)
    , m_boxBackground()
    , m_nameBackground()
    , m_nameText(nullptr)
    , m_dialogText(nullptr)
    , m_continueIndicator(nullptr)
    , m_boxOpacity(0.85f)
    , m_advanceKeyWasPressed(false)
    , m_indicatorTimer(0.f)
{
}

bool DialogSystem::initialize(const sf::Font& font) {
    m_font = &font;
    
    // SFML 3.x: Create text objects with font reference
    m_nameText = std::make_unique<sf::Text>(font);
    m_dialogText = std::make_unique<sf::Text>(font);
    m_continueIndicator = std::make_unique<sf::Text>(font);
    
    // Configure text sizes (relative to screen)
    unsigned int nameSize = static_cast<unsigned int>(GameSettings::relativeMin(0.028f));
    unsigned int textSize = static_cast<unsigned int>(GameSettings::relativeMin(0.024f));
    unsigned int indicatorSize = static_cast<unsigned int>(GameSettings::relativeMin(0.018f));
    
    m_nameText->setCharacterSize(nameSize);
    m_dialogText->setCharacterSize(textSize);
    m_continueIndicator->setCharacterSize(indicatorSize);
    
    m_continueIndicator->setString("Press ENTER to continue...");
    m_continueIndicator->setFillColor(sf::Color(200, 200, 200, 180));
    
    // Set up box geometry
    setupBoxGeometry();
    
    // Register default speakers with colors that fit the game's theme
    registerSpeaker("King", SpeakerStyle(sf::Color(255, 215, 0), sf::Color(255, 250, 220)));      // Gold
    registerSpeaker("Hero", SpeakerStyle(sf::Color(100, 200, 255), sf::Color(220, 240, 255)));    // Light blue
    registerSpeaker("Enemy", SpeakerStyle(sf::Color(255, 80, 80), sf::Color(255, 200, 200)));     // Red
    registerSpeaker("Guide", SpeakerStyle(sf::Color(180, 255, 180), sf::Color(220, 255, 220)));   // Green
    registerSpeaker("Narrator", SpeakerStyle(sf::Color(200, 200, 200), sf::Color(240, 240, 240)));// Gray
    registerSpeaker("Shadow", SpeakerStyle(sf::Color(150, 100, 180), sf::Color(200, 180, 220)));  // Purple
    
    m_initialized = true;
    return true;
}

void DialogSystem::setupBoxGeometry() {
    // Dialog box at bottom of screen
    // Height: 20% of screen, Width: 90% of screen, centered
    float boxWidth = GameSettings::relativeX(0.90f);
    float boxHeight = GameSettings::relativeY(0.20f);
    float boxX = GameSettings::relativeX(0.05f);  // 5% margin on each side
    float boxY = GameSettings::relativeY(0.78f);  // 78% down (leaving 2% margin at bottom)
    
    m_boxBackground.setSize(sf::Vector2f(boxWidth, boxHeight));
    m_boxBackground.setPosition(sf::Vector2f(boxX, boxY));
    m_boxBackground.setFillColor(sf::Color(20, 20, 35, static_cast<std::uint8_t>(255 * m_boxOpacity)));
    m_boxBackground.setOutlineThickness(GameSettings::relativeMin(0.003f));
    m_boxBackground.setOutlineColor(sf::Color(80, 80, 120, 200));
    
    // Name background - small box above the main dialog box
    float nameBoxWidth = GameSettings::relativeX(0.15f);
    float nameBoxHeight = GameSettings::relativeY(0.045f);
    float nameBoxX = boxX + GameSettings::relativeX(0.02f);
    float nameBoxY = boxY - nameBoxHeight + GameSettings::relativeY(0.005f);
    
    m_nameBackground.setSize(sf::Vector2f(nameBoxWidth, nameBoxHeight));
    m_nameBackground.setPosition(sf::Vector2f(nameBoxX, nameBoxY));
    m_nameBackground.setFillColor(sf::Color(40, 40, 60, static_cast<std::uint8_t>(255 * m_boxOpacity)));
    m_nameBackground.setOutlineThickness(GameSettings::relativeMin(0.002f));
    m_nameBackground.setOutlineColor(sf::Color(80, 80, 120, 200));
    
    // Position text elements
    float textPadding = GameSettings::relativeMin(0.02f);
    
    if (m_nameText) {
        m_nameText->setPosition(sf::Vector2f(
            nameBoxX + textPadding,
            nameBoxY + (nameBoxHeight * 0.15f)
        ));
    }
    
    if (m_dialogText) {
        m_dialogText->setPosition(sf::Vector2f(
            boxX + textPadding,
            boxY + textPadding
        ));
    }
    
    // Continue indicator in bottom-right of dialog box
    if (m_continueIndicator) {
        m_continueIndicator->setPosition(sf::Vector2f(
            boxX + boxWidth - GameSettings::relativeX(0.22f),
            boxY + boxHeight - GameSettings::relativeY(0.045f)
        ));
    }
}

void DialogSystem::startDialog(const std::vector<DialogLine>& lines) {
    if (!m_initialized || lines.empty() || !m_nameText || !m_dialogText) {
        return;
    }
    
    m_dialogQueue = lines;
    m_currentLineIndex = 0;
    
    // Set up the first line
    const DialogLine& firstLine = m_dialogQueue[0];
    m_fullText = firstLine.text;
    m_displayedText.clear();
    m_currentCharIndex = 0;
    m_typewriterTimer = 0.f;
    
    // Set speaker name and colors
    m_nameText->setString(firstLine.speaker);
    const SpeakerStyle& style = getSpeakerStyle(firstLine.speaker);
    m_nameText->setFillColor(style.nameColor);
    m_dialogText->setFillColor(style.textColor);
}

void DialogSystem::queueLine(const std::string& speaker, const std::string& text) {
    m_dialogQueue.emplace_back(speaker, text);
    
    // If this is the first line and nothing is playing, start it
    if (m_dialogQueue.size() == 1 && m_currentLineIndex == 0 && m_displayedText.empty()) {
        if (m_nameText && m_dialogText) {
            m_fullText = text;
            m_nameText->setString(speaker);
            const SpeakerStyle& style = getSpeakerStyle(speaker);
            m_nameText->setFillColor(style.nameColor);
            m_dialogText->setFillColor(style.textColor);
        }
    }
}

void DialogSystem::clear() {
    m_dialogQueue.clear();
    m_currentLineIndex = 0;
    m_fullText.clear();
    m_displayedText.clear();
    m_currentCharIndex = 0;
    m_typewriterTimer = 0.f;
}

void DialogSystem::update(float deltaTime) {
    if (!isActive() || !m_dialogText) {
        return;
    }
    
    // Update typewriter effect
    if (m_currentCharIndex < m_fullText.size()) {
        m_typewriterTimer += deltaTime;
        float timePerChar = 1.f / m_charsPerSecond;
        
        while (m_typewriterTimer >= timePerChar && m_currentCharIndex < m_fullText.size()) {
            m_typewriterTimer -= timePerChar;
            m_currentCharIndex++;
            m_displayedText = m_fullText.substr(0, m_currentCharIndex);
            m_dialogText->setString(m_displayedText);
        }
    }
    
    // Update continue indicator animation (pulsing)
    if (m_continueIndicator) {
        m_indicatorTimer += deltaTime * 2.f;
        float alpha = 0.5f + 0.5f * std::sin(m_indicatorTimer);
        sf::Color indicatorColor = m_continueIndicator->getFillColor();
        indicatorColor.a = static_cast<std::uint8_t>(180 * alpha);
        m_continueIndicator->setFillColor(indicatorColor);
    }
    
    // Handle input (real-time polling for responsiveness)
    bool advancePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
    
    if (advancePressed && !m_advanceKeyWasPressed) {
        advance();
    }
    
    m_advanceKeyWasPressed = advancePressed;
}

void DialogSystem::render(sf::RenderWindow& window) {
    if (!isActive()) {
        return;
    }
    
    // Draw backgrounds
    window.draw(m_boxBackground);
    window.draw(m_nameBackground);
    
    // Draw text
    if (m_nameText) {
        window.draw(*m_nameText);
    }
    if (m_dialogText) {
        window.draw(*m_dialogText);
    }
    
    // Draw continue indicator if line is complete
    if (isLineComplete() && m_continueIndicator) {
        window.draw(*m_continueIndicator);
    }
}

bool DialogSystem::isActive() const noexcept {
    return m_initialized && !m_dialogQueue.empty() && m_currentLineIndex < m_dialogQueue.size();
}

bool DialogSystem::isLineComplete() const noexcept {
    return m_currentCharIndex >= m_fullText.size();
}

void DialogSystem::advance() {
    if (!isActive()) {
        return;
    }
    
    if (!isLineComplete()) {
        // Skip to end of current line
        m_currentCharIndex = m_fullText.size();
        m_displayedText = m_fullText;
        if (m_dialogText) {
            m_dialogText->setString(m_displayedText);
        }
    }
    else {
        // Move to next line
        advanceToNextLine();
    }
}

void DialogSystem::advanceToNextLine() {
    m_currentLineIndex++;
    
    if (m_currentLineIndex < m_dialogQueue.size()) {
        // Set up the next line
        const DialogLine& line = m_dialogQueue[m_currentLineIndex];
        m_fullText = line.text;
        m_displayedText.clear();
        m_currentCharIndex = 0;
        m_typewriterTimer = 0.f;
        
        // Update speaker
        if (m_nameText && m_dialogText) {
            m_nameText->setString(line.speaker);
            const SpeakerStyle& style = getSpeakerStyle(line.speaker);
            m_nameText->setFillColor(style.nameColor);
            m_dialogText->setFillColor(style.textColor);
            m_dialogText->setString("");
        }
    }
    else {
        // Dialog complete - clear everything
        clear();
    }
}

void DialogSystem::registerSpeaker(const std::string& name, const SpeakerStyle& style) {
    m_speakerStyles[name] = style;
}

const SpeakerStyle& DialogSystem::getSpeakerStyle(const std::string& speaker) const {
    auto it = m_speakerStyles.find(speaker);
    if (it != m_speakerStyles.end()) {
        return it->second;
    }
    return m_defaultStyle;
}

void DialogSystem::setTypewriterSpeed(float charsPerSecond) noexcept {
    m_charsPerSecond = std::max(1.f, charsPerSecond);
}

void DialogSystem::setBoxOpacity(float opacity) noexcept {
    m_boxOpacity = std::clamp(opacity, 0.f, 1.f);
    
    // Update box colors
    sf::Color boxColor = m_boxBackground.getFillColor();
    boxColor.a = static_cast<std::uint8_t>(255 * m_boxOpacity);
    m_boxBackground.setFillColor(boxColor);
    
    sf::Color nameBoxColor = m_nameBackground.getFillColor();
    nameBoxColor.a = static_cast<std::uint8_t>(255 * m_boxOpacity);
    m_nameBackground.setFillColor(nameBoxColor);
}
