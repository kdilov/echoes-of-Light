#pragma once
#include <vector>
#include <memory>
#include "Scene.h"





class SceneStack
{
public:
    SceneStack() = default;

    void pushScene(std::shared_ptr<Scene> scene);

    void popScene();
    
    void replaceScene(std::shared_ptr<Scene> scene);
    
    void clear();
    
    void handleEvent(const sf::Event& event);
    
    void update(float dt);
    
    void render(sf::RenderWindow& window);
    
    std::shared_ptr<Scene> getTopScene() const;

    bool empty() const { return scenes.empty(); }

private:
    std::vector<std::shared_ptr<Scene>> scenes;


    enum class ActionType { Push, Pop, Clear };

    struct PendingAction
    {
        ActionType type;
        std::shared_ptr<Scene> scene;  
    };

    std::vector<PendingAction> pending;

   
    void applyPendingActions();
};

