#include "MotionSystem.h"

void MotionSystem::Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents, std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, float deltaTime)
{
    for (std::pair<unsigned int, PhysicsComponent*> entity : physicsComponents) {

        //For each transform component, add its velocities
        transformComponents[entity.first]->globalTransform *= TranslationMatrix(entity.second->velocity * deltaTime);
        //transformComponents[entity.first].eulers += entity.second.eulerVelocity * deltaTime;
    }
}
