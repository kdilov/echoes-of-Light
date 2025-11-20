#include "components/AnimationComponent.h"
#include <iostream>

namespace eol {

    AnimationComponent::AnimationComponent()
        : Component("Animation") {
    }

    void AnimationComponent::addAnimation(const std::string& name, const Animation& anim) {
        m_animations[name] = anim;

        // Set first animation as current if none set
        if (m_currentAnimation.empty()) {
            m_currentAnimation = name;
        }
    }

    void AnimationComponent::setAnimation(const std::string& name) {
        // Don't restart if already playing this animation
        if (m_currentAnimation == name) {
            return;
        }

        // Check if animation exists
        if (m_animations.find(name) == m_animations.end()) {
            std::cerr << "Warning: Animation '" << name << "' not found!" << std::endl;
            return;
        }

        m_currentAnimation = name;
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_finished = false;
    }

    const std::string& AnimationComponent::getCurrentAnimationName() const noexcept {
        return m_currentAnimation;
    }

    void AnimationComponent::update(float deltaTime) {
        if (m_currentAnimation.empty() || m_animations.empty()) {
            return;
        }

        const Animation& anim = m_animations[m_currentAnimation];

        // Don't update if animation is finished and not looping
        if (m_finished && !anim.loop) {
            return;
        }

        m_elapsedTime += deltaTime;

        // Check if we should advance to next frame
        if (m_elapsedTime >= anim.frameDuration) {
            m_elapsedTime = 0.f;
            m_currentFrame++;

            // Check if animation finished
            if (m_currentFrame >= anim.frameCount) {
                if (anim.loop) {
                    m_currentFrame = 0;  // Loop back to start
                }
                else {
                    m_currentFrame = anim.frameCount - 1;  // Stay on last frame
                    m_finished = true;
                }
            }
        }
    }

    sf::IntRect AnimationComponent::getCurrentFrameRect() const {
        if (m_currentAnimation.empty() || m_animations.empty()) {
            return sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
        }

        const Animation& anim = m_animations.at(m_currentAnimation);

        // Frames are arranged horizontally in the sprite sheet
        return sf::IntRect(
            sf::Vector2i(m_currentFrame * anim.frameWidth, 0),
            sf::Vector2i(anim.frameWidth, anim.frameHeight)
        );
    }

    const sf::Texture* AnimationComponent::getCurrentTexture() const {
        if (m_currentAnimation.empty() || m_animations.empty()) {
            return nullptr;
        }

        return m_animations.at(m_currentAnimation).texture;
    }

    bool AnimationComponent::isFinished() const noexcept {
        return m_finished;
    }

    void AnimationComponent::reset() {
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
        m_finished = false;
    }

} // namespace eol