#include "ComponentFactory.h"

ComponentFactory::ComponentFactory(
    std::unordered_map<unsigned int, LightComponent*>& lightComponents, std::unordered_map<unsigned int, PhysicsComponent*>& physicsComponents,
    std::unordered_map<unsigned int, RenderComponent*>& renderComponents, std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
    AssetFactory& _assFact) :
    lightComponents(lightComponents),
    physicsComponents(physicsComponents),
    renderComponents(renderComponents),
    transformComponents(transformComponents),
    assFact(_assFact)
{
}

ComponentFactory::~ComponentFactory()
{
    lightComponents.clear();
    physicsComponents.clear();
    renderComponents.clear();
    transformComponents.clear();
}

unsigned int ComponentFactory::MakeCamera(vec3 position, vec3 eulers)
{
    return MakeEmptyTransform(position, eulers);
}

unsigned int ComponentFactory::MakeEmptyTransform()
{
    TransformComponent* transform = new TransformComponent();
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int ComponentFactory::MakeEmptyTransform(vec3 position, vec3 eulers)
{
    TransformComponent* transform = new TransformComponent();
    transform->position = position;
    transform->eulers = eulers;
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

unsigned int ComponentFactory::MakeAmbientLightEntity(vec3 colour, float intensity)
{
    unsigned int ambi = MakeEmptyTransform();
    return AddAmbientLightComponent(ambi,colour,intensity);
}

unsigned int ComponentFactory::AddAmbientLightComponent(unsigned int entity, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int ComponentFactory::MakeDirectionalLightEntity(vec3 direction, vec3 colour, float intensity)
{
    vec3 trueDirection = GetNormalised(direction);
    
    unsigned int directional = MakeEmptyTransform();
    return AddDirectionalLightComponent(directional, trueDirection, colour, intensity);
}

unsigned int ComponentFactory::AddDirectionalLightComponent(unsigned int entity,vec3 direction, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    light->lightType = LightType::directional;
    light->direction = direction;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int ComponentFactory::MakePointLightEntity(vec3 pos, float range, vec3 colour, float intensity)
{
    unsigned int point = MakeEmptyTransform(pos);
    RenderComponent* ratRend = new RenderComponent();
    ratRend->mesh = assFact.GetMesh("models/rat.obj");
    ratRend->material = assFact.GetMaterial("img/me.PNG", 1);
    AddRenderComponent(point, ratRend);
    transformComponents[point]->scale = { 0.1,0.1,0.1 };
    return AddPointLightComponent(point, range, colour, intensity);
}

unsigned int ComponentFactory::AddPointLightComponent(unsigned int entity, float range, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    light->lightType = LightType::point;
    light->range = range;
    light->CalculateLinearQuadConstants();
    AddLightComponent(entity, light);
    return entity;
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

LightComponent* ComponentFactory::AddLightComponent(unsigned int _entity)
{
    LightComponent* light = new LightComponent();

    lightComponents[_entity] = light;

    return light;
}

LightComponent* ComponentFactory::AddLightComponent(unsigned int _entity, LightComponent* light)
{
    lightComponents[_entity] = light;

    return light;
}

