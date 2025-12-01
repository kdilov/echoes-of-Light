#include "Game.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include "components/AnimationComponent.h"
#include "components/CollisionComponent.h"
#include "components/EnemyAIComponent.h"
#include "components/EnemyComponent.h"
#include "components/LightComponent.h"
#include "components/LightEmitterComponent.h"
#include "components/MeleeAttackComponent.h"
#include "components/LightSourceComponent.h"
#include "components/MirrorComponent.h"
#include "components/PlayerComponent.h"
#include "components/PuzzleComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "components/UpgradeComponent.h"
#include "GameSettings.h"

namespace {
    constexpr char windowTitle[] = "Echoes of Light";

sf::Image createSolidImage(unsigned int size, sf::Color color) {
    sf::Image img(sf::Vector2u(size, size), color);
    return img;
}

sf::Image createCircularFalloffImage(unsigned int size,
                                     sf::Color centerColor,
                                     sf::Color edgeColor) {
    sf::Image img(sf::Vector2u(size, size), sf::Color::Transparent);

    const float half = static_cast<float>(size) * 0.5f;
    for (unsigned int y = 0; y < size; ++y) {
        for (unsigned int x = 0; x < size; ++x) {
            const float dx = static_cast<float>(x) - half + 0.5f;
            const float dy = static_cast<float>(y) - half + 0.5f;
            const float dist = std::sqrt(dx * dx + dy * dy);
            const float norm = dist / half;
            if (norm <= 1.f) {
                const float t = std::clamp(norm, 0.f, 1.f);
                const auto mixChannel = [t](std::uint8_t a, std::uint8_t b) {
                    return static_cast<std::uint8_t>(a * (1.f - t) + b * t);
                };

                sf::Color color;
                color.r = mixChannel(centerColor.r, edgeColor.r);
                color.g = mixChannel(centerColor.g, edgeColor.g);
                color.b = mixChannel(centerColor.b, edgeColor.b);
                color.a = mixChannel(centerColor.a, edgeColor.a);
                img.setPixel(sf::Vector2u{x, y}, color);
            }
        }
    }

    return img;
}
}

Game::Game()
    : window_{}
    , gameView_{}
    , clock_{}
    , idleTexture_{}
    , moveTexture_{}
    , initialized_{false}
    , currentResolutionIndex_{ 0 }
    , player_{}
    , lightBeacon_{}
    , enemy_{}
    , entities_{}
    , worldObjects_{}
    , combatSystem_{}
    , enemyAISystem_{}
    , lightSystem_{combatSystem_} {}

void Game::createWindow()  {
    window_.create(sf::VideoMode({ windowWidth, windowHeight }), windowTitle);
    window_.setFramerateLimit(framerateLimit);
}

int Game::run() {
    if (!initialized_ && !initialize()) {
        return -1;
    }

    while (window_.isOpen()) {
        float deltaTime = clock_.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();
    }

    return 0;
}

bool Game::initialize() {
    // Create window at default resolution
    window_.create(sf::VideoMode({ defaultWindowWidth, defaultWindowHeight }), windowTitle);
    window_.setFramerateLimit(framerateLimit);

    // Set up the scaled view - this maps the reference resolution to the window
    gameView_ = GameSettings::getScaledView(window_.getSize());
    window_.setView(gameView_);

    

    std::cout << "=== ECHOES OF LIGHT ===" << std::endl;
    std::cout << "Reference resolution: " << GameSettings::refWidth << "x" << GameSettings::refHeight << std::endl;

    if (!loadResources()) {
        return false;
    }

    createEntities();
    clock_.restart();

    std::cout << "Controls: WASD to move, E to pickup/drop mirrors, ESC to exit" << std::endl;
    std::cout << "Press F1/F2/F3 to change resolution" << std::endl;

    initialized_ = true;
    return true;
}

void Game::setResolution(unsigned int index) {
    const auto& resolutions = GameSettings::getAvailableResolutions();
    if (index >= resolutions.size()) {
        return;
    }

    const auto& res = resolutions[index];
    currentResolutionIndex_ = index;

    // Recreate window at new resolution
    window_.create(sf::VideoMode({ res.width, res.height }), windowTitle);
    window_.setFramerateLimit(framerateLimit);

    // Update the view to scale properly
    gameView_ = GameSettings::getScaledView(window_.getSize());
    window_.setView(gameView_);

    std::cout << "Resolution changed to: " << res.label << std::endl;
}

unsigned int Game::getCurrentResolutionIndex() const noexcept {
    return currentResolutionIndex_;
}

bool Game::loadResources() {
    const std::string idlePath = findResourcePath("resources/sprites/Character_Idle.png");
    const std::string movePath = findResourcePath("resources/sprites/Character_Move.png");

    if (!idleTexture_.loadFromFile(idlePath)) {
        std::cerr << "ERROR: Failed to load idle texture" << std::endl;
        return false;
    }

    if (!moveTexture_.loadFromFile(movePath)) {
        std::cerr << "ERROR: Failed to load move texture" << std::endl;
        return false;
    }

    if (!debugWhiteTexture_.loadFromImage(createSolidImage(2, sf::Color::White))) {
        std::cerr << "ERROR: Failed to create debug texture" << std::endl;
        return false;
    }

    const sf::Color centerColor(255, 255, 230, 255);
    const sf::Color edgeColor(255, 255, 230, 80);
    if (!lightNodeTexture_.loadFromImage(createCircularFalloffImage(64, centerColor, edgeColor))) {
        std::cerr << "ERROR: Failed to create light node texture" << std::endl;
        return false;
    }

    std::cout << "Textures loaded successfully" << std::endl;
    return true;
}

void Game::createEntities() {
    player_ = createPlayerEntity();
    lightBeacon_ = createLightBeaconEntity();
    enemy_ = createEnemyEntity();

    entities_.clear();
    entities_.push_back(&player_);
    entities_.push_back(&lightBeacon_);
    entities_.push_back(&enemy_);

    worldObjects_.clear();
    worldObjects_.reserve(12);

    auto addWorldEntity = [this](Entity&& entity) {
        auto handle = std::make_unique<Entity>();
        *handle = std::move(entity);
        entities_.push_back(handle.get());
        worldObjects_.push_back(std::move(handle));
        };

    // ========== MIRRORS ==========
    // Positions expressed as percentages of screen (0.0 - 1.0)

    // Prism mirror - right side of screen, slightly above center
    addWorldEntity(createMirrorEntity(
        GameSettings::relativePos(0.65f, 0.47f),
        sf::Vector2f{ -1.f, 1.f },
        GameSettings::relativeSize(0.07f, 0.033f),
        eol::MirrorComponent::MirrorType::Prism));

    // Splitter mirror - center-left, upper area
    addWorldEntity(createMirrorEntity(
        GameSettings::relativePos(0.45f, 0.30f),
        sf::Vector2f{ 1.f, 1.f },
        GameSettings::relativeSize(0.052f, 0.03f),
        eol::MirrorComponent::MirrorType::Splitter));

    // Flat mirror - center of screen
    addWorldEntity(createMirrorEntity(
        GameSettings::relativePos(0.50f, 0.50f),
        sf::Vector2f{ 1.f, 1.f },
        GameSettings::relativeSize(0.052f, 0.03f),
        eol::MirrorComponent::MirrorType::Flat));

    // ========== LIGHT SOURCES ==========
    addWorldEntity(createLightSourceNode(
        "PrismNode",
        GameSettings::relativePos(0.325f, 0.60f),
        true));

    addWorldEntity(createLightSourceNode(
        "AccessLight",
        GameSettings::relativePos(0.875f, 0.70f),
        false));

    // ========== WALLS ==========
    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.50f, 0.167f),
        GameSettings::relativeSize(0.25f, 0.037f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.125f, 0.50f),
        GameSettings::relativeSize(0.021f, 0.25f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.75f, 0.75f),
        GameSettings::relativeSize(0.0625f, 0.111f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.25f, 0.867f),
        GameSettings::relativeSize(0.125f, 0.037f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.3625f, 0.783f),
        GameSettings::relativeSize(0.021f, 0.133f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.833f, 0.37f),
        GameSettings::relativeSize(0.021f, 0.278f)));

    addWorldEntity(createWallEntity(
        GameSettings::relativePos(0.417f, 0.694f),
        GameSettings::relativeSize(0.156f, 0.037f)));

}

Entity Game::createPlayerEntity() {
    Entity entity;
    entity.name = "Player";

    // Player starts at center of screen
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::center(),
        sf::Vector2f{ 0.75f, 0.75f },
        0.f));

    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());
    entity.components.emplace_back(std::make_unique<eol::PlayerComponent>());

    // Collision box - relative to screen size
    auto collision = std::make_unique<eol::CollisionComponent>();
    collision->setBoundingBox(GameSettings::relativeSize(0.022f, 0.039f));
    collision->setSolid(true);
    entity.components.emplace_back(std::move(collision));

    auto animationComponent = std::make_unique<eol::AnimationComponent>();

    eol::Animation idleAnim;
    idleAnim.name = "idle";
    idleAnim.texture = &idleTexture_;
    idleAnim.frameCount = 4;
    idleAnim.frameWidth = 128;
    idleAnim.frameHeight = 128;
    idleAnim.frameDuration = 0.15f;
    idleAnim.loop = true;
    animationComponent->addAnimation("idle", idleAnim);

    eol::Animation walkAnim;
    walkAnim.name = "walk";
    walkAnim.texture = &moveTexture_;
    walkAnim.frameCount = 6;
    walkAnim.frameWidth = 128;
    walkAnim.frameHeight = 128;
    walkAnim.frameDuration = 0.1f;
    walkAnim.loop = true;
    animationComponent->addAnimation("walk", walkAnim);

    animationComponent->setAnimation("idle");
    entity.components.emplace_back(std::move(animationComponent));

    entity.components.emplace_back(std::make_unique<eol::LightComponent>());

    auto emitter = std::make_unique<eol::LightEmitterComponent>();
    emitter->setBeamLength(GameSettings::relativeX(0.625f));
    emitter->setBeamWidth(GameSettings::relativeMin(0.015f));
    emitter->setDamage(50.f);
    emitter->setMaxReflections(4);
    entity.components.emplace_back(std::move(emitter));

    entity.components.emplace_back(std::make_unique<eol::UpgradeComponent>());

    return entity;
}

Entity Game::createLightBeaconEntity() {
    Entity entity;
    entity.name = "LightBeacon";

    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::relativePos(0.80f, 0.233f),
        sf::Vector2f{ 0.65f, 0.65f },
        0.f));

    auto lightSource = std::make_unique<eol::LightSourceComponent>();
    lightSource->setActive(false);
    lightSource->setMovable(false);
    lightSource->setFuel(0.f);
    entity.components.emplace_back(std::move(lightSource));

    auto puzzle = std::make_unique<eol::PuzzleComponent>();
    puzzle->setRequiredLight(5);
    entity.components.emplace_back(std::move(puzzle));

    auto beaconLight = std::make_unique<eol::LightComponent>();
    beaconLight->setRadius(GameSettings::relativeMin(0.324f));
    beaconLight->setBaseIntensity(0.2f);
    entity.components.emplace_back(std::move(beaconLight));

    entity.components.emplace_back(std::make_unique<eol::RenderComponent>());
    return entity;
}

Entity Game::createEnemyEntity() {
    Entity entity;
    entity.name = "Enemy";

    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::relativePos(0.65f, 0.533f),
        sf::Vector2f{ 1.5f, 1.5f },
        0.f));

    entity.components.emplace_back(std::make_unique<eol::EnemyComponent>());

    auto render = std::make_unique<eol::RenderComponent>();
    sf::Sprite& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect(sf::IntRect(sf::Vector2i{ 0, 0 }, sf::Vector2i{ 1, 1 }));
    sprite.setOrigin(sf::Vector2f{ 0.5f, 0.5f });
    sf::Vector2f enemySize = GameSettings::relativeSize(0.022f, 0.05f);
    sprite.setScale(enemySize);
    render->setTint(sf::Color(255, 110, 110, 240));
    entity.components.emplace_back(std::move(render));

    auto enemyLight = std::make_unique<eol::LightComponent>();
    enemyLight->setBaseIntensity(0.15f);
    enemyLight->setRadius(GameSettings::relativeMin(0.167f));
    entity.components.emplace_back(std::move(enemyLight));

    auto melee = std::make_unique<eol::MeleeAttackComponent>();
    melee->setDamage(22.f);
    melee->setRange(GameSettings::relativeMin(0.083f));
    melee->setCooldown(1.2f);
    entity.components.emplace_back(std::move(melee));

    auto ai = std::make_unique<eol::EnemyAIComponent>();
    const sf::Vector2f basePos = GameSettings::relativePos(0.65f, 0.533f);
    ai->setPatrolPoints({
        basePos,
        basePos + sf::Vector2f{-GameSettings::relativeX(0.175f), 0.f}
        });
    ai->setDetectionRange(GameSettings::relativeMin(0.37f));
    ai->setAttackRange(GameSettings::relativeMin(0.014f));
    ai->setMoveSpeed(GameSettings::relativeMin(0.046f));
    entity.components.emplace_back(std::move(ai));

    return entity;
}

Entity Game::createMirrorEntity(const sf::Vector2f& position,
                                const sf::Vector2f& normal,
                                const sf::Vector2f& size,
                                eol::MirrorComponent::MirrorType type) {
    const auto normalizeVec = [](const sf::Vector2f& vec) {
        const float length = std::sqrt(vec.x * vec.x + vec.y * vec.y);
        if (length <= 0.0001f) {
            return sf::Vector2f{0.f, -1.f};
        }
        return sf::Vector2f{vec.x / length, vec.y / length};
    };

    const sf::Vector2f normalizedNormal = normalizeVec(normal);
    const sf::Vector2f tangent{-normalizedNormal.y, normalizedNormal.x};
    constexpr float radToDeg = 180.f / 3.1415926535f;
    const float rotation = std::atan2(tangent.y, tangent.x) * radToDeg;
    const sf::Vector2f center = position + sf::Vector2f{size.x * 0.5f, size.y * 0.5f};

    Entity entity;
    entity.name = "Mirror";
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        center,
        sf::Vector2f{size.x, size.y},
        rotation));

    auto mirror = std::make_unique<eol::MirrorComponent>();
    mirror->setNormal(normalizedNormal);
    mirror->setSize(size);
    mirror->setType(type);
    entity.components.emplace_back(std::move(mirror));

    auto render = std::make_unique<eol::RenderComponent>();
    sf::Sprite& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect(sf::IntRect(sf::Vector2i{0, 0}, sf::Vector2i{1, 1}));
    sprite.setOrigin(sf::Vector2f{0.5f, 0.5f});
    render->setTint(sf::Color(160, 210, 255, 220));
    entity.components.emplace_back(std::move(render));

    return entity;
}

Entity Game::createLightSourceNode(const std::string& name,
                                   const sf::Vector2f& position,
                                   bool movable) {
    Entity entity;
    entity.name = name;
    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        position,
        sf::Vector2f{1.0f, 1.0f},
        0.f));

    auto source = std::make_unique<eol::LightSourceComponent>();
    source->setMovable(movable);
    source->setActive(true);
    source->setFuel(100.f);
    entity.components.emplace_back(std::move(source));

    auto light = std::make_unique<eol::LightComponent>();
    light->setRadius(GameSettings::relativeMin(0.259f));
    light->setBaseIntensity(0.6f);
    entity.components.emplace_back(std::move(light));

    auto render = std::make_unique<eol::RenderComponent>();
    sf::Sprite& sprite = render->getSprite();
    sprite.setTexture(lightNodeTexture_);
    const auto texSize = lightNodeTexture_.getSize();
    sprite.setTextureRect(sf::IntRect(sf::Vector2i{0, 0}, sf::Vector2i(texSize)));
    sprite.setOrigin(sf::Vector2f{static_cast<float>(texSize.x) * 0.5f,
                                  static_cast<float>(texSize.y) * 0.5f});
    render->setTint(movable ? sf::Color(255, 255, 200) : sf::Color(190, 220, 255));
    entity.components.emplace_back(std::move(render));
    return entity;
}

Entity Game::createWallEntity(const sf::Vector2f& position, const sf::Vector2f& size) {
    Entity entity;
    entity.name = "Wall";

    entity.components.emplace_back(std::make_unique<eol::TransformComponent>(
        position,
        sf::Vector2f{ size.x / 2.f, size.y / 2.f },
        0.f));

    // Collision - this is what stops the player
    auto collision = std::make_unique<eol::CollisionComponent>();
    collision->setBoundingBox(size);
    collision->setSolid(true);
    entity.components.emplace_back(std::move(collision));

    // Render - so we can see the wall
    auto render = std::make_unique<eol::RenderComponent>();
    sf::Sprite& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect(sf::IntRect(sf::Vector2i{ 0, 0 }, sf::Vector2i{ 2, 2 }));
    sprite.setOrigin(sf::Vector2f{ 1.f, 1.f });
    render->setTint(sf::Color(160, 160, 180, 220));  
    entity.components.emplace_back(std::move(render));

    

    return entity;
}

void Game::handleEvents() {
    while (const auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
                window_.close();
                break;
                // Resolution hotkeys
            case sf::Keyboard::Key::F1:
                setResolution(0);  // 1920x1080
                break;
            case sf::Keyboard::Key::F2:
                setResolution(1);  // 1280x720
                break;
            case sf::Keyboard::Key::F3:
                setResolution(2);  // 800x600
                break;
            default:
                break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    inputSystem_.updateWithCollision(player_, deltaTime, window_, entities_);
    animationSystem_.update(entities_, deltaTime);
    enemyAISystem_.update(entities_, deltaTime, player_);
    combatSystem_.updateMeleeAttacks(entities_, deltaTime);
    lightSystem_.update(entities_, deltaTime, window_);

    // Level progression and win-condition logic will be reinstated when
    // LevelManager support is ready.
}

void Game::render() {
    window_.clear(sf::Color(20, 20, 30));
    // Make sure the scaled view is active
    window_.setView(gameView_);
    renderSystem_.render(window_, entities_);
    lightSystem_.render(window_, entities_);

    window_.display();
}

std::string Game::findResourcePath(const std::string& relativePath) const {
    const std::vector<std::string> possiblePaths = {
        relativePath,
        "../../../" + relativePath,
        "../../../../" + relativePath,
        "D:/code/echoes/echoes-of-Light/" + relativePath
    };

    for (const auto& path : possiblePaths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }

    return relativePath;
}

void Game::setResolution(const sf::Vector2u& res) {
   
    currentResolution = res;
    window_.setSize(res);

    // Reapply frame rate
    window_.setFramerateLimit(currentFramerate);
}

sf::Vector2u Game::getResolution() const {
    return currentResolution;
}

void Game::setFramerateLimit(unsigned int limit) {
    currentFramerate = limit;
    window_.setFramerateLimit(limit);
}

unsigned int Game::getFramerateLimit() const {
    return currentFramerate;
}

sf::RenderWindow& Game::getWindow() {
    return window_;
}

