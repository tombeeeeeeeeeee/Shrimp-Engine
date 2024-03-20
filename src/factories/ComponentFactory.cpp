#include "ComponentFactory.h"

ComponentFactory::ComponentFactory(std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents, std::unordered_map<unsigned int, RenderComponent>& renderComponents, std::unordered_map<unsigned int, TransformComponent>& transformComponents, AssetFactory& _assFact) :
    physicsComponents(physicsComponents),
    renderComponents(renderComponents),
    transformComponents(transformComponents),
    assFact(_assFact)
{
}

ComponentFactory::~ComponentFactory()
{
    
}

unsigned int ComponentFactory::MakeCamera(vec3 position, vec3 eulers)
{
    return MakeEmptyTransform(position, eulers);
}

unsigned int ComponentFactory::MakeEmptyTransform()
{
    TransformComponent transform;
    transform.position = {0,0,0};  transform.eulers = { 0,0,0 };
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int ComponentFactory::MakeEmptyTransform(vec3 position, vec3 eulers)
{
    TransformComponent transform;
    transform.position = position;  transform.eulers = eulers;
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int ComponentFactory::MakeRat(vec3 position, vec3 eulers)
{
    unsigned int rat = MakeEmptyTransform(position, eulers);

    RenderComponent* ratRend = AddRenderComponent(rat);
    ratRend->mesh = assFact.GetMesh("models/rat.obj");
    ratRend->material = assFact.GetMaterial("img/cubeTexture.jpg",1);
    return rat;
}

unsigned int ComponentFactory::MakeCube(vec3 position, vec3 eulers)
{
    unsigned int cube = MakeEmptyTransform(position, eulers);

    RenderComponent* cubeRend = AddRenderComponent(cube);
    cubeRend->mesh = assFact.CubeMesh();
    cubeRend->material = assFact.CubeMaterial();
    return cube;
}

RenderComponent* ComponentFactory::AddRenderComponent(unsigned int _entity)
{
    RenderComponent* rend = new RenderComponent();

    renderComponents[_entity] = *rend;
   
    return rend;
}

PhysicsComponent* ComponentFactory::AddPhysicsComponent(unsigned int _entity)
{
    PhysicsComponent* physics = new PhysicsComponent();

    physicsComponents[_entity] = *physics;

    return physics;
}