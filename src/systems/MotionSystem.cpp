#include "MotionSystem.h"

void MotionSystem::Update(std::unordered_map<unsigned int, TransformComponent>& transformComponents, std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents, float deltaTime)
{
    //Rotates all physics components.
    //Is a placeholder atm and is used for testing.
   for (std::pair<unsigned int, PhysicsComponent> entity : physicsComponents)
   {
       vec3 eulerDisplacement = entity.second.eulerVelocity * deltaTime;

       transformComponents[entity.first].eulers += eulerDisplacement;
   }
}
