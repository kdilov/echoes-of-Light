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
#include "components/HitboxComponent.h"
#include "components/LightComponent.h"
#include "components/LightEmitterComponent.h"
#include "components/LightSourceComponent.h"
#include "components/MeleeAttackComponent.h"
#include "components/MirrorComponent.h"
#include "components/PlayerComponent.h"
#include "components/PuzzleComponent.h"
#include "components/RenderComponent.h"
#include "components/TransformComponent.h"
#include "components/UpgradeComponent.h"
#include "components/LevelManager.h"
#include "components/SpawnerComponent.h"
#include "GameSettings.h"


// =============================================================
//   Helper Image Generators (unchanged from your original)
// =============================================================
namespace {

    sf::Image createSolidImage(unsigned int size, sf::Color color)
    {
        sf::Image img(sf::Vector2u(size, size), color);
        return img;
    }

    sf::Image createCircularFalloffImage(unsigned int size,
        sf::Color centerColor,
        sf::Color edgeColor)
    {
        sf::Image img(sf::Vector2u(size, size), sf::Color::Transparent);

        const float half = static_cast<float>(size) * 0.5f;

        for (unsigned int y = 0; y < size; ++y)
        {
            for (unsigned int x = 0; x < size; ++x)
            {
                float dx = static_cast<float>(x) - half + 0.5f;
                float dy = static_cast<float>(y) - half + 0.5f;
                float dist = std::sqrt(dx * dx + dy * dy);
                float norm = dist / half;

                if (norm <= 1.f)
                {
                    float t = std::clamp(norm, 0.f, 1.f);

                    auto mix = [t](uint8_t a, uint8_t b)
                        {
                            return static_cast<uint8_t>(a * (1.f - t) + b * t);
                        };

                    sf::Color c;
                    c.r = mix(centerColor.r, edgeColor.r);
                    c.g = mix(centerColor.g, edgeColor.g);
                    c.b = mix(centerColor.b, edgeColor.b);
                    c.a = mix(centerColor.a, edgeColor.a);

                    img.setPixel({ x, y }, c);
                }
            }
        }

        return img;
    }

} // namespace

// =============================================================
//   Constructor
// =============================================================
Game::Game()
    : initialized_(false)
    , currentFramerate(60)
    , idleTexture_{}
    , moveTexture_{}
    , debugWhiteTexture_{}
    , lightNodeTexture_{}
    , player_{}
    , lightBeacon_{}
    , enemy_{}
    , entities_{}
    , worldObjects_{}
    , combatSystem_{}
    , enemyAISystem_{}
    , lightSystem_{ combatSystem_ }
{
}

// =============================================================
//   Initialization
// =============================================================
bool Game::initialize()
{
    std::cout << "=== ECHOES OF LIGHT (Gameplay Initializing) ===\n";

    if (!loadResources())
        return false;

    // Load starting level
    levels_.setCurrentIndex(startLevelIndex_);
    if (!levels_.loadCurrentLevel()) {
        std::cerr << "ERROR: Failed to load starting level\n";
        return false;
    }
    std::cout << "Loaded first level successfully.\n";

    // Calculate tile size for this level
    recalculateTileSize();

    
    // Initialize dialog system
    if (!dialogSystem_.initialize(gameFont_)) {
        std::cerr << "ERROR: Failed to initialize dialog system\n";
        return false;
    }

    // Show intro dialog when game starts
    dialogSystem_.startDialog({
        {"Narrator", "The kingdom has fallen into darkness..."},
        {"Narrator", "Only one hero remains who can restore the light."},
        {"King", "You must travel through the echoes of time to save us."},
        {"Hero", "I will not fail you, my lord."}
        });

    // Setting up Enemy spawner system with enemy factory  
    spawnerSystem_.setEnemyFactory([this](const sf::Vector2f& position) {
        return createEnemyAtPosition(position);
        });

    createEntities();

    initialized_ = true;
    return true;
}

// =============================================================
//   Resource Loading
// =============================================================
bool Game::loadResources()
{
    std::string idlePath = findResourcePath("resources/sprites/Character_Idle.png");
    std::string movePath = findResourcePath("resources/sprites/Character_Move.png");

    if (!idleTexture_.loadFromFile(idlePath))
    {
        std::cerr << "ERROR: Failed to load idle sprite\n";
        return false;
    }

    if (!moveTexture_.loadFromFile(movePath))
    {
        std::cerr << "ERROR: Failed to load movement sprite\n";
        return false;
    }

    if (!debugWhiteTexture_.loadFromImage(createSolidImage(2, sf::Color::White)))
    {
        std::cerr << "ERROR: Failed to create debug texture\n";
        return false;
    }

    sf::Color center(255, 255, 230, 255);
    sf::Color edge(255, 255, 230, 80);

    if (!lightNodeTexture_.loadFromImage(createCircularFalloffImage(64, center, edge)))
    {
        std::cerr << "ERROR: Failed to create light node texture\n";
        return false;
    }

    // Load font for dialog system
    std::string fontPath = findResourcePath("resources/fonts/ScienceGothic.ttf");
    if (!gameFont_.openFromFile(fontPath)) {
        std::cerr << "ERROR: Failed to load game font\n";
        return false;
    }

    std::cout << "Textures loaded OK.\n";
    std::cout << "Font loaded OK.\n";
    return true;
}

// =============================================================
//   Create ALL Gameplay Entities (Original Logic Restored)
// =============================================================
void Game::createEntities()
{
    entities_.clear();
    worldObjects_.clear();
    worldObjects_.reserve(64);

    const Map& map = levels_.getCurrentMap();
    sf::Vector2f playerStartPos = GameSettings::center(); // Default fallback

    auto addWorld = [&](Entity&& e)
        {
            auto ptr = std::make_unique<Entity>();
            *ptr = std::move(e);
            entities_.push_back(ptr.get());
            worldObjects_.push_back(std::move(ptr));
        };

    // Scan through the map and create entities for each tile
    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            TileType tile = map.getTile(x, y);
            sf::Vector2f worldPos = tileToWorld(x, y);

            switch (tile) {
            case TileType::WALL:
                addWorld(createWallEntity(worldPos, sf::Vector2f(tileSize_, tileSize_)));
                break;

            case TileType::START:
                playerStartPos = worldPos;
                break;

            case TileType::END:
                // Could create an exit/goal entity here
                // For now, handled by playerReachedExit()
                break;

            case TileType::LIGHT_SOURCE:
                addWorld(createLightSourceNode("Light_" + std::to_string(x) + "_" + std::to_string(y),
                    worldPos, true));
                break;

            case TileType::MIRROR:
                addWorld(createMirrorEntity(
                    worldPos,
                    { 1, 1 },
                    GameSettings::relativeSize(0.052f, 0.015f),
                    eol::MirrorComponent::MirrorType::Flat));
                break;

            case TileType::SPAWNER:
                addWorld(createSpawnerEntity(
                    worldPos,
                    5.f,    // Spawn every 5 seconds
                    2       // Max 2 enemies per spawner
                ));
                break;

            case TileType::EMPTY:
            default:
                // Nothing to create
                break;
            }
        }
    }

    // Create player at the START position
    player_ = createPlayerEntity();
    if (auto* transform = player_.getComponent<eol::TransformComponent>()) {
        transform->setPosition(playerStartPos);
    }
    entities_.push_back(&player_);

    // Create light beacon (could place this as a tile from map )
    lightBeacon_ = createLightBeaconEntity();
    entities_.push_back(&lightBeacon_);

    // Create enemy (you could add an 'X' tile type for enemies)
    enemy_ = createEnemyEntity();
    entities_.push_back(&enemy_);

    std::cout << "Created " << entities_.size() << " entities from map.\n";
}

// =============================================================
//   Entity Creation Functions (ALL ORIGINAL LOGIC RESTORED)
// =============================================================
Entity Game::createPlayerEntity()
{
    Entity e;
    e.name = "Player";

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::center(),
        sf::Vector2f{ 0.75f, 0.75f },
        0.f));

    e.components.emplace_back(std::make_unique<eol::RenderComponent>());
    e.components.emplace_back(std::make_unique<eol::PlayerComponent>());

    auto collision = std::make_unique<eol::CollisionComponent>();
    collision->setBoundingBox(GameSettings::relativeSize(0.022f, 0.039f));
    collision->setSolid(true);
    e.components.emplace_back(std::move(collision));

    auto anim = std::make_unique<eol::AnimationComponent>();

    eol::Animation idle;
    idle.name = "idle";
    idle.texture = &idleTexture_;
    idle.frameCount = 4;
    idle.frameWidth = 128;
    idle.frameHeight = 128;
    idle.frameDuration = 0.15f;
    idle.loop = true;
    anim->addAnimation("idle", idle);

    eol::Animation walk;
    walk.name = "walk";
    walk.texture = &moveTexture_;
    walk.frameCount = 6;
    walk.frameWidth = 128;
    walk.frameHeight = 128;
    walk.frameDuration = 0.1f;
    walk.loop = true;
    anim->addAnimation("walk", walk);

    anim->setAnimation("idle");
    e.components.emplace_back(std::move(anim));

    e.components.emplace_back(std::make_unique<eol::LightComponent>());

    auto emitter = std::make_unique<eol::LightEmitterComponent>();
    emitter->setBeamLength(GameSettings::relativeX(0.625f));
    emitter->setBeamWidth(GameSettings::relativeMin(0.015f));
    emitter->setDamage(50.f);
    emitter->setMaxReflections(4);
    e.components.emplace_back(std::move(emitter));

    e.components.emplace_back(std::make_unique<eol::UpgradeComponent>());

    return e;
}

Entity Game::createLightBeaconEntity()
{
    Entity e;
    e.name = "LightBeacon";

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::relativePos(0.80f, 0.233f),
        sf::Vector2f{ 0.65f, 0.65f },
        0.f));

    auto src = std::make_unique<eol::LightSourceComponent>();
    src->setMovable(false);
    src->setActive(false);
    src->setFuel(0.f);
    e.components.emplace_back(std::move(src));

    auto puzzle = std::make_unique<eol::PuzzleComponent>();
    puzzle->setRequiredLight(1);
    puzzle->setSolved(false);
    puzzle->setLightRequirement(eol::PuzzleComponent::LightRequirement::PlayerOnly);
    e.components.emplace_back(std::move(puzzle));

    auto hitbox = std::make_unique<eol::HitboxComponent>();
    hitbox->setSize(GameSettings::relativeSize(0.05f, 0.05f));
    e.components.emplace_back(std::move(hitbox));

    auto light = std::make_unique<eol::LightComponent>();
    light->setRadius(GameSettings::relativeMin(0.324f));
    light->setBaseIntensity(0.15f);
    e.components.emplace_back(std::move(light));

    auto emitter = std::make_unique<eol::LightEmitterComponent>();
    emitter->setDirection(sf::Vector2f{0.f, -1.f});
    emitter->setBeamLength(GameSettings::relativeY(0.95f));
    emitter->setBeamWidth(GameSettings::relativeMin(0.012f));
    emitter->setDamage(55.f);
    emitter->setBeamDuration(0.18f);
    emitter->setCooldown(0.1f);
    emitter->setMaxReflections(6);
    emitter->setBeamColor(sf::Color(255, 242, 205, 255));
    emitter->setContinuousFire(true);
    emitter->setTriggerHeld(false);
    e.components.emplace_back(std::move(emitter));

    e.components.emplace_back(std::make_unique<eol::RenderComponent>());
    return e;
}

Entity Game::createEnemyEntity()
{
    Entity e;
    e.name = "Enemy";

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        GameSettings::relativePos(0.65f, 0.533f),
        sf::Vector2f{ 1.5f, 1.5f },
        0.f));

    auto hitbox = std::make_unique<eol::HitboxComponent>();
    hitbox->setSize(GameSettings::relativeSize(0.06f, 0.1f));
    e.components.emplace_back(std::move(hitbox));

    e.components.emplace_back(std::make_unique<eol::EnemyComponent>());

    auto render = std::make_unique<eol::RenderComponent>();
    auto& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect({ {0,0}, {1,1} });
    sprite.setOrigin({ 0.5f, 0.5f });
    sprite.setScale(GameSettings::relativeSize(0.022f, 0.05f));
    render->setTint(sf::Color(255, 110, 110, 240));
    e.components.emplace_back(std::move(render));

    auto melee = std::make_unique<eol::MeleeAttackComponent>();
    melee->setDamage(22.f);
    melee->setRange(GameSettings::relativeMin(0.083f));
    melee->setCooldown(1.2f);
    e.components.emplace_back(std::move(melee));

    auto ai = std::make_unique<eol::EnemyAIComponent>();
    const auto basePos = GameSettings::relativePos(0.65f, 0.533f);
    ai->setPatrolPoints({ basePos, basePos + sf::Vector2f{-GameSettings::relativeX(0.175f), 0.f} });
    ai->setDetectionRange(GameSettings::relativeMin(0.37f));
    ai->setAttackRange(GameSettings::relativeMin(0.014f));
    ai->setMoveSpeed(GameSettings::relativeMin(0.046f));
    e.components.emplace_back(std::move(ai));

    return e;
}

Entity Game::createMirrorEntity(const sf::Vector2f& position,
    const sf::Vector2f& normal,
    const sf::Vector2f& size,
    eol::MirrorComponent::MirrorType type)
{
    const auto normalize = [](sf::Vector2f v)
        {
            float len = std::sqrt(v.x * v.x + v.y * v.y);
            return (len > 0.0001f) ? sf::Vector2f(v.x / len, v.y / len)
                : sf::Vector2f(0.f, -1.f);
        };

    sf::Vector2f n = normalize(normal);
    sf::Vector2f tangent(-n.y, n.x);
    float rotation = std::atan2(tangent.y, tangent.x) * 180.f / 3.1415926f;

    sf::Vector2f center = position + sf::Vector2f(size.x * 0.5f, size.y * 0.5f);

    Entity e;
    e.name = "Mirror";

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        center, sf::Vector2f(size.x, size.y), rotation));

    auto comp = std::make_unique<eol::MirrorComponent>();
    comp->setNormal(n);
    comp->setSize(size);
    comp->setType(type);
    e.components.emplace_back(std::move(comp));

    auto render = std::make_unique<eol::RenderComponent>();
    auto& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect({ {0,0}, {1,1} });
    sprite.setOrigin({ 0.5f, 0.5f });
    render->setTint(sf::Color(160, 210, 255, 220));
    e.components.emplace_back(std::move(render));

    return e;
}

Entity Game::createLightSourceNode(
    const std::string& name,
    const sf::Vector2f& position,
    bool movable)
{
    Entity e;
    e.name = name;

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        position, sf::Vector2f{ 1.f, 1.f }, 0.f));

    auto src = std::make_unique<eol::LightSourceComponent>();
    src->setMovable(movable);
    src->setActive(true);
    src->setFuel(100.f);
    e.components.emplace_back(std::move(src));

    auto light = std::make_unique<eol::LightComponent>();
    light->setRadius(GameSettings::relativeMin(0.259f));
    light->setBaseIntensity(0.6f);
    e.components.emplace_back(std::move(light));

    auto render = std::make_unique<eol::RenderComponent>();
    auto& sprite = render->getSprite();
    sprite.setTexture(lightNodeTexture_);
    sf::Vector2u tex = lightNodeTexture_.getSize();
    sprite.setTextureRect({ {0,0}, sf::Vector2i(tex) });
    sprite.setOrigin({ tex.x * 0.5f, tex.y * 0.5f });
    render->setTint(movable ? sf::Color(255, 255, 200)
        : sf::Color(190, 220, 255));
    e.components.emplace_back(std::move(render));

    return e;
}

Entity Game::createWallEntity(const sf::Vector2f& pos, const sf::Vector2f& size)
{
    Entity e;
    e.name = "Wall";

    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        pos,
        sf::Vector2f(1.f, 1.f),  // Scale = 1, we set size directly
        0.f));

    auto coll = std::make_unique<eol::CollisionComponent>();
    coll->setBoundingBox(size);
    coll->setSolid(true);
    e.components.emplace_back(std::move(coll));

    auto render = std::make_unique<eol::RenderComponent>();
    auto& sprite = render->getSprite();
    sprite.setTexture(debugWhiteTexture_);
    sprite.setTextureRect({ {0, 0}, {2, 2} });
    sprite.setOrigin({ 1.f, 1.f });
    sprite.setScale(sf::Vector2f(size.x * 0.5f, size.y * 0.5f));
    render->setTint(sf::Color(80, 80, 100, 220));  // Match map color
    e.components.emplace_back(std::move(render));

    return e;
}

Entity Game::createSpawnerEntity(const sf::Vector2f& position, float interval, int maxEnemies)
{
    Entity e;
    e.name = "Spawner";

    // Position only - (invisible)
    e.components.emplace_back(std::make_unique<eol::TransformComponent>(
        position,
        sf::Vector2f{ 1.f, 1.f },
        0.f));

    // Spawner settings
    auto spawner = std::make_unique<eol::SpawnerComponent>();
    spawner->setSpawnInterval(interval);
    spawner->setMaxEnemies(maxEnemies);
    e.components.emplace_back(std::move(spawner));

    return e;
}

Entity Game::createEnemyAtPosition(const sf::Vector2f& position)
{
    // Create a standard enemy using existing function
    Entity e = createEnemyEntity();

    // Override the position
    if (auto* transform = e.getComponent<eol::TransformComponent>()) {
        transform->setPosition(position);
    }

    // Update patrol points to be around the spawn position
    if (auto* ai = e.getComponent<eol::EnemyAIComponent>()) {
        ai->setPatrolPoints({
            position,
            position + sf::Vector2f{-GameSettings::relativeX(0.1f), 0.f},
            position + sf::Vector2f{GameSettings::relativeX(0.1f), 0.f}
            });
    }

    return e;
}

// =============================================================
//   UPDATE (Scene system calls this)
// =============================================================
void Game::update(float dt, sf::RenderWindow& window)
{
    // Update dialog system first
    dialogSystem_.update(dt);

    // Only update gameplay if dialog is not active (pauses game during dialog)
    if (!dialogSystem_.isActive()) {
        inputSystem_.updateWithCollision(player_, dt, window, entities_);
        animationSystem_.update(entities_, dt);
        enemyAISystem_.update(entities_, dt, player_);
        combatSystem_.updateMeleeAttacks(entities_, dt);

        // Update spawners and add new enemies
        std::vector<Entity> newEnemies = spawnerSystem_.update(entities_, dt);
        for (auto& enemy : newEnemies) {
            auto ptr = std::make_unique<Entity>(std::move(enemy));
            entities_.push_back(ptr.get());
            worldObjects_.push_back(std::move(ptr));
        }


        // Check if player reached the exit
        if (playerReachedExit()) {
            levels_.nextLevel();

            if (levels_.isLevelComplete()) {
                // All levels complete - show victory message
                dialogSystem_.startDialog({
                    {"Narrator", "Congratulations! You have restored the light to all eras!"},
                    {"King", "The kingdom is saved. You are a true hero!"}
                    });
                // Could also transition to a credits scene here
            }
            else {
                // Load next level
                recalculateTileSize();
                createEntities();

                // Show level transition dialog
                dialogSystem_.startDialog({
                    {"Narrator", "You have found the path forward..."},
                    {"Narrator", "A new challenge awaits."}
                    });
            }
        }
    }

    // Light system updates regardless (for visual effects)
    lightSystem_.update(entities_, dt, window);
}

// =============================================================
//   RENDER (Scene system calls this)
// =============================================================
void Game::render(sf::RenderWindow& window)
{
   
    // Draw the map first (background layer)
    const Map& map = levels_.getCurrentMap();
    if (map.getWidth() > 0 && map.getHeight() > 0) {
        map.draw(window, tileSize_, mapOffset_);
    }

    renderSystem_.render(window, entities_);
    lightSystem_.render(window, entities_);

    // Render dialog on top of everything
    dialogSystem_.render(window);
}

// =============================================================
//   Utilities
// =============================================================
void Game::setFramerateLimit(unsigned int limit)
{
    currentFramerate = limit;
}

unsigned int Game::getFramerateLimit() const
{
    return currentFramerate;
}

std::string Game::findResourcePath(const std::string& relative) const
{
    const std::vector<std::string> paths = {
        relative,
        "../../../" + relative,
        "../../../../" + relative,
        "D:/code/echoes/echoes-of-Light/" + relative
    };

    for (auto& p : paths)
        if (std::filesystem::exists(p))
            return p;

    return relative;
}


bool Game::playerReachedExit() {
    // Get player's world position
    auto* t = player_.getComponent<eol::TransformComponent>();
    if (!t) return false;

    sf::Vector2f ppos = t->getPosition();

    // Get exit tile world position
    LevelObjects objs = levels_.scanObjects();
    if (objs.exitTile.x < 0) return false;

    // Convert exit tile to world coordinates (center of tile)
    sf::Vector2f exitWorld = tileToWorld(objs.exitTile.x, objs.exitTile.y);

    // Check distance
    float dx = ppos.x - exitWorld.x;
    float dy = ppos.y - exitWorld.y;
    float dist2 = dx * dx + dy * dy;

    // Within half a tile
    float threshold = tileSize_ * 0.5f;
    return dist2 < (threshold * threshold);
}

sf::Vector2f Game::tileToWorld(int tileX, int tileY) const {
    // Returns the center of the tile in world coordinates
    return sf::Vector2f(
        mapOffset_.x + (tileX + 0.5f) * tileSize_,
        mapOffset_.y + (tileY + 0.5f) * tileSize_
    );
}

void Game::recalculateTileSize()
{
    const Map& map = levels_.getCurrentMap();
    if (map.getWidth() > 0 && map.getHeight() > 0) {
        float tileSizeX = GameSettings::width() / static_cast<float>(map.getWidth());
        float tileSizeY = GameSettings::height() / static_cast<float>(map.getHeight());
        tileSize_ = std::min(tileSizeX, tileSizeY);

        // Calculate offset to center the map
        float mapPixelWidth = map.getWidth() * tileSize_;
        float mapPixelHeight = map.getHeight() * tileSize_;
        mapOffset_.x = (GameSettings::width() - mapPixelWidth) / 2.f;
        mapOffset_.y = (GameSettings::height() - mapPixelHeight) / 2.f;
    }
}



























































