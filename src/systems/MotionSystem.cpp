#include "MotionSystem.h"

void MotionSystem::Update(unordered_map<unsigned int, TransformComponent>& transformComponents, unordered_map<unsigned int, PhysicsComponent>& physicsComponents, float deltaTime)
{
    for (std::pair<unsigned int, PhysicsComponent> entity : physicsComponents) {

        //For each transform component, add its velocities
        transformComponents[entity.first].position += entity.second.velocity * deltaTime;
        transformComponents[entity.first].eulers += entity.second.eulerVelocity * deltaTime;

        if (transformComponents[entity.first].eulers.z > 360) {
            transformComponents[entity.first].eulers.z -= 360;
        }
    }
}
