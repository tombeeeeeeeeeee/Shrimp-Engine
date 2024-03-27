#pragma once
#include <config.h>
#include "../components/TransformComponent.h"
#include "../components/PhysicsComponent.h"

class MotionSystem
{
public:

    void Update(
        std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
        std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, float deltaTime
    );
};

