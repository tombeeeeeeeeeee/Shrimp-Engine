#pragma once
#include <config.h>
#include "../components/TransformComponent.h"
#include "../components/PhysicsComponent.h"

class MotionSystem
{
public:

    /// <summary>
    /// Updates the movement/motion system. Placeholder for testing.
    /// </summary>
    /// <param name="transformComponents">All Transform Components</param>
    /// <param name="physicsComponents">physics Components</param>
    /// <param name="deltaTime"> time since last frame</param>
    void Update(
        std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
        std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, float deltaTime
    );
};

