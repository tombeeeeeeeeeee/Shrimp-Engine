#include "ComponentFactory.h"

ComponentFactory::ComponentFactory(std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents, std::unordered_map<unsigned int, RenderComponent*>& renderComponents, std::unordered_map<unsigned int, TransformComponent*>& transformComponents, AssetFactory& _assFact) :
    physicsComponents(physicsComponents),
    renderComponents(renderComponents),
    transformComponents(transformComponents),
    assFact(_assFact)
{
}

ComponentFactory::~ComponentFactory()
{
    physicsComponents.clear();
    transformComponents.clear();
    renderComponents.clear();
}

unsigned int ComponentFactory::MakeCamera(vec3 position, vec3 eulers)
{
    return MakeEmptyTransform(position, eulers);
}

unsigned int ComponentFactory::MakeEmptyTransform()
{
    TransformComponent* transform = new TransformComponent();
    transform->localTransform = TranslationMatrix({ 0,0,0 });
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int ComponentFactory::MakeEmptyTransform(vec3 position, vec3 eulers)
{
    TransformComponent* transform = new TransformComponent();
    transform->localTransform = TranslationMatrix(position);
    transform->localTransform *= RotationZMatrix(eulers.z);
    transform->localTransform *= RotationYMatrix(eulers.y);
    transform->localTransform *= RotationXMatrix(eulers.x);
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int ComponentFactory::MakeRat(vec3 position, vec3 eulers)
{
    unsigned int rat = MakeEmptyTransform(position, eulers);

    RenderComponent* ratRend = new RenderComponent();
    ratRend->mesh = assFact.GetMesh("models/rat.obj");
    ratRend->material = assFact.RatMaterial();
    AddRenderComponent(rat, ratRend);
    return rat;
}

unsigned int ComponentFactory::MakeCube(vec3 position, vec3 eulers)
{
    unsigned int cube = MakeEmptyTransform(position, eulers);

    RenderComponent* cubeRend = new RenderComponent();
    cubeRend->mesh = assFact.CubeMesh();
    cubeRend->material = assFact.CubeMaterial();
    AddRenderComponent(cube, cubeRend);
    return cube;
}

RenderComponent* ComponentFactory::AddRenderComponent(unsigned int _entity)
{
    RenderComponent* rend = new RenderComponent();

    renderComponents[_entity] = rend;
   
    return rend;
}

RenderComponent* ComponentFactory::AddRenderComponent(unsigned int _entity, RenderComponent* rend)
{
    renderComponents[_entity] = rend;

    return rend;
}

PhysicsComponent* ComponentFactory::AddPhysicsComponent(unsigned int _entity)
{
    PhysicsComponent* physics = new PhysicsComponent();

    physicsComponents[_entity] = physics;

    return physics;
}
