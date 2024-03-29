#include "MotionSystem.h"

void MotionSystem::Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents, std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, float deltaTime)
{
   for (std::pair<unsigned int, PhysicsComponent*> entity : physicsComponents) {
   
       vec3 eulerDisplacement = entity.second->eulerVelocity * deltaTime;
       mat4 rotations = RotationXMatrix(eulerDisplacement.x);
       rotations *= RotationYMatrix(eulerDisplacement.y);
       rotations *= RotationZMatrix(eulerDisplacement.z);

       transformComponents[entity.first]->localTransform *= rotations;
   }
}
