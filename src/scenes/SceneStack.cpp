#include "scenes/SceneStack.h"
#include <algorithm>

void SceneStack::pushScene(std::shared_ptr<Scene> scene)
{
    pending.push_back({ ActionType::Push, scene });
}

void SceneStack::popScene()
{
    if (!scenes.empty())
    {
        pending.push_back({ ActionType::Pop, nullptr });
    }
}

void SceneStack::replaceScene(std::shared_ptr<Scene> scene)
{
    if (!scenes.empty())
    {
        scenes.back()->onExit();
        scenes.pop_back();
    }
    scenes.push_back(scene);
    scene->onEnter();
}

void SceneStack::clear()
{
    pending.push_back({ ActionType::Clear, nullptr });
}

void SceneStack::handleEvent(const sf::Event& event)
{
    if (!scenes.empty())
        scenes.back()->handleEvent(event);
}


void SceneStack::update(float dt)
{
    // update scenes normally
    for (int i = scenes.size() - 1; i >= 0; --i)
    {
        scenes[i]->update(dt);
        if (scenes[i]->blocksUpdate())
            break;
    }

    applyPendingActions();
}

void SceneStack::applyPendingActions()
{
    for (auto& action : pending)
    {
        switch (action.type)
        {
        case ActionType::Push:
            if (action.scene)
            {
                scenes.push_back(action.scene);
                action.scene->onEnter();
            }
            break;

        case ActionType::Pop:
            if (!scenes.empty())
            {
                scenes.back()->onExit();
                scenes.pop_back();
            }
            break;

        case ActionType::Clear:
            for (auto& s : scenes)
                s->onExit();
            scenes.clear();
            break;
        }
    }
    pending.clear();
}


void SceneStack::render(sf::RenderWindow& window)
{
    if (scenes.empty())
        return;

    // Find the lowest non-transparent scene.
    int firstRender = static_cast<int>(scenes.size()) - 1;

    while (firstRender > 0 && scenes[firstRender]->isTransparent())
        --firstRender;

    // Render from the lowest visible scene to the top.
    for (int i = firstRender; i < scenes.size(); ++i)
        scenes[i]->render(window);
}

std::shared_ptr<Scene> SceneStack::getTopScene() const
{
    if (scenes.empty())
        return nullptr;
    return scenes.back();
}
