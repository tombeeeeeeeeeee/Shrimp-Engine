#pragma once
#include <config.h>
#include "../components/TransformComponent.h"
#include "../components/PhysicsComponent.h"

class MotionSystem
{
public:

    void Update(
        unordered_map<unsigned int, TransformComponent*>& transformComponents,
        unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, float deltaTime
    );
};

