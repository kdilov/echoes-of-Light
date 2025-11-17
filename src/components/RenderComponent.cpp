#include "components/RenderComponent.h"

#include <utility>

namespace eol {

RenderComponent::RenderComponent()
    : Component("Render")
    , m_placeholderTexture{}
    , m_sprite{m_placeholderTexture}
    , m_tint{sf::Color::White} {}

void RenderComponent::setTextureId(std::string textureId) {
    m_textureId = std::move(textureId);
}

const std::string& RenderComponent::getTextureId() const noexcept {
    return m_textureId;
}

sf::Sprite& RenderComponent::getSprite() noexcept {
    return m_sprite;
}

const sf::Sprite& RenderComponent::getSprite() const noexcept {
    return m_sprite;
}

void RenderComponent::setTint(const sf::Color& tint) noexcept {
    m_tint = tint;
}

const sf::Color& RenderComponent::getTint() const noexcept {
    return m_tint;
}

} // namespace eol

