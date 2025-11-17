#pragma once

#include "components/Component.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <string>

namespace eol {

class RenderComponent : public Component {
public:
    RenderComponent();

    void setTextureId(std::string textureId);
    const std::string& getTextureId() const noexcept;

    sf::Sprite& getSprite() noexcept;
    const sf::Sprite& getSprite() const noexcept;

    void setTint(const sf::Color& tint) noexcept;
    const sf::Color& getTint() const noexcept;

private:
    std::string m_textureId;
    sf::Texture m_placeholderTexture;
    sf::Sprite m_sprite;
    sf::Color m_tint{sf::Color::White};
};

} // namespace eol

