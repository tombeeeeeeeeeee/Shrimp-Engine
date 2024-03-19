#include "ComponentFactory.h"

ComponentFactory::ComponentFactory(std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents, std::unordered_map<unsigned int, RenderComponent>& renderComponents, std::unordered_map<unsigned int, TransformComponent>& transformComponents) :
    physicsComponents(physicsComponents),
    renderComponents(renderComponents),
    transformComponents(transformComponents)
{
}

ComponentFactory::~ComponentFactory()
{

}

unsigned int ComponentFactory::MakeCamera(vec3 position, vec3 eulers)
{
    TransformComponent transform;
    transform.position = position;  transform.eulers = eulers;
    transformComponents[entityCount] = transform;
    return entityCount++;
}
