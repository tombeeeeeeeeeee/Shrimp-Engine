#include "SceneManager.h"

SceneManager::SceneManager(AssetFactory& _assFact) : assFact(_assFact)
{
}

SceneManager::~SceneManager()
{
    lightComponents.clear();
    physicsComponents.clear();
    renderComponents.clear();
    transformComponents.clear();
}

SceneManager& SceneManager::operator=(SceneManager const& other)
{
    lightComponents.clear();
    physicsComponents.clear();
    renderComponents.clear();
    transformComponents.clear();

    entityCount = other.entityCount;
    lightComponents = other.lightComponents;
    physicsComponents = other.physicsComponents;
    renderComponents = other.renderComponents;
    transformComponents = other.transformComponents;
    assFact = other.assFact;

    return *this;
}

const std::unordered_map<unsigned int, TransformComponent*> SceneManager::GetTransforms()
{
    return transformComponents;
}

const std::unordered_map<unsigned int, RenderComponent*> SceneManager::GetRenders()
{
    return renderComponents;
}

const std::unordered_map<unsigned int, LightComponent*> SceneManager::GetLights()
{
    return lightComponents;
}

unsigned int SceneManager::MakeCamera(vec3 position, vec3 eulers)
{
    return MakeEmptyTransform(position, eulers);
}

unsigned int SceneManager::MakeEmptyTransform()
{
    TransformComponent* transform = new TransformComponent();
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int SceneManager::MakeEmptyTransform(vec3 position, vec3 eulers)
{
    TransformComponent* transform = new TransformComponent();
    transform->position = position;
    transform->eulers = eulers;
    transformComponents[entityCount] = transform;
    return entityCount++;
}

const vec3 SceneManager::GetPosition(unsigned int entity)
{                                                                                                     
    if (transformComponents.find(entity) != transformComponents.end()) 
        return { transformComponents[entity]->globalTransform.entries[12], transformComponents[entity]->globalTransform.entries[13], transformComponents[entity]->globalTransform.entries[14] };
    return {NAN, NAN, NAN};
}

const vec3 SceneManager::GetLocalPosition(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
        return transformComponents[entity]->position;
    return { NAN, NAN, NAN };
}

//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetLocalPosition(unsigned int entity, vec3 position)
{
    if (transformComponents.find(entity) != transformComponents.end())
        transformComponents[entity]->position = position;
}

const vec3 SceneManager::GetEulers(unsigned int entity, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        mat4 globalTransform = transformComponents[entity]->globalTransform;

        float eulX = 0;
        float eulY = 0;
        float eulZ = 0;

        float sy = sqrt(globalTransform.entries[0] * globalTransform.entries[0] + globalTransform.entries[1] * globalTransform.entries[1]);

        bool singular = sy < 1e-6; // If

        float x, y, z;
        if (!singular)
        {
            x = atan2(globalTransform.entries[6], globalTransform.entries[10]);
            y = atan2(-globalTransform.entries[2], sy);
            z = atan2(globalTransform.entries[1], globalTransform.entries[0]);
        }
        else
        {
            x = atan2(-globalTransform.entries[9], globalTransform.entries[5]);
            y = atan2(-globalTransform.entries[2], sy);
            z = 0;
        }
        return vec3(x, y, z);

        if (abs(globalTransform.entries[2]) != 1)
        {
            eulY = asinf(globalTransform.entries[2]);
            float c = cos(eulY);
            eulX = atan2f(globalTransform.entries[6] / c, globalTransform.entries[10] / c);
            eulZ = atan2f(globalTransform.entries[1] / c, globalTransform.entries[0] / c);
        }
        else
        {
            eulZ = 0;
            eulY = LA_PI / 2.0f;
            if (globalTransform.entries[2] == -1)
            {
                eulX = atan2f(globalTransform.entries[4], globalTransform.entries[8]);
            }
            else
            {
                eulY *= -1;
                eulX = atan2f(-globalTransform.entries[4], -globalTransform.entries[8]);
            }
        }
        vec3 eul = { eulX, eulY, eulZ };
        if (!radians) eul *= 180 / LA_PI;
        return eul;
    }
    else return { NAN, NAN, NAN };
}

const vec3 SceneManager::GetLocalEulers(unsigned int entity, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        return radians ? transformComponents[entity]->eulers : transformComponents[entity]->eulers * (180.0f / LA_PI);
    }
    else return { NAN, NAN, NAN };
}
//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetLocalEulers(unsigned int entity, vec3 eulers, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
        transformComponents[entity]->eulers = eulers;
}

const vec3 SceneManager::GetScale(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        return transformComponents[entity]->scale;
    }
    else return { NAN, NAN, NAN };
}
//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetScale(unsigned int entity, vec3 scale)
{
    if (transformComponents.find(entity) != transformComponents.end())
        transformComponents[entity]->scale = scale;
}

const mat4 SceneManager::GetLocalTransform(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        return TranslationMatrix(transformComponents[entity]->position)
            * RotationXMatrix(transformComponents[entity]->eulers.x)
            * RotationXMatrix(transformComponents[entity]->eulers.y)
            * RotationXMatrix(transformComponents[entity]->eulers.z)
            * ScaleMatrix(transformComponents[entity]->scale);
    }
    else return { 
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
    };
}

const mat4 SceneManager::GetGlobalTransform(unsigned entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
        return transformComponents[entity]->globalTransform;
    else return {
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
    };
}



unsigned int SceneManager::MakeAmbientLightEntity(vec3 colour, float intensity)
{
    unsigned int ambi = MakeEmptyTransform();
    return AddAmbientLightComponent(ambi,colour,intensity);
}

unsigned int SceneManager::AddAmbientLightComponent(unsigned int entity, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakeDirectionalLightEntity(vec3 direction, vec3 colour, float intensity)
{
    vec3 trueDirection = GetNormalised(direction);
    
    unsigned int directional = MakeEmptyTransform();
    return AddDirectionalLightComponent(directional, trueDirection, colour, intensity);
}

unsigned int SceneManager::AddDirectionalLightComponent(unsigned int entity,vec3 direction, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    light->lightType = LightType::directional;
    light->direction = direction;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakePointLightEntity(vec3 pos, float range, vec3 colour, float intensity, bool debug)
{
    unsigned int point = MakeEmptyTransform(pos);

    //PlaceHolder light Render
    if (debug)
    {
        RenderComponent* lightCube = new RenderComponent();
        lightCube->mesh = assFact.GetMesh("models/cube.obj");
        lightCube->material = assFact.GetMaterial("img/light.png", 1);
        lightCube->material->shaderProgram = 1;
        AddRenderComponent(point, lightCube);
        transformComponents[point]->scale = { 0.5,0.5,0.5 };
    }

    return AddPointLightComponent(point, range, colour, intensity);
}

unsigned int SceneManager::AddPointLightComponent(unsigned int entity, float range, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    light->lightType = LightType::point;
    light->range = range;
    light->CalculateLinearQuadConstants();
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakeSpotLightEntity(vec3 pos, vec3 dir, float range, float cutOff, float outerCutOff, vec3 colour, float intensity, bool debug)
{
    unsigned int spot = MakeEmptyTransform(pos);

    //PlaceHolder light Render
    if (debug)
    {
        RenderComponent* lightCube = new RenderComponent();
        lightCube->mesh = assFact.GetMesh("models/cube.obj");
        lightCube->material = assFact.GetMaterial("img/light.png", 1);
        lightCube->material->shaderProgram = 1;
        AddRenderComponent(spot, lightCube);
        transformComponents[spot]->scale = { 0.1,0.1,0.1 };
    }

    return AddSpotLightComponent(spot, dir, range, cutOff, outerCutOff, colour, intensity);
}

unsigned int SceneManager::AddSpotLightComponent(unsigned int entity, vec3 dir, float range, float cutOff, float outerCutOff, vec3 colour, float intensity)
{
    LightComponent* light = new LightComponent();
    light->colour = colour * intensity;
    light->lightType = LightType::spot;
    light->range = range;
    light->direction = dir;
    light->CalculateLinearQuadConstants();
    light->cutOff = cos(LA_PI * cutOff / 180);
    light->outerCutOff = cos(LA_PI * outerCutOff / 180);
    AddLightComponent(entity, light);
    return entity;
}

void SceneManager::CalculateLinearQuadConstants(unsigned int entity)
{
    if (lightComponents.find(entity) == lightComponents.end()) return;

    float range = lightComponents[entity]->range;
    float linear, quad;
    if (range <= 7)
    {
        linear = 1 - range * 0.3 / 7;
        quad = 1.8 + (7 - range) * 0.5;
    }

    else if (range <= 13)
    {
        float ratio = range - (13 - 7);
        ratio /= 7;
        linear = 0.7 + (0.35 - 0.7) * ratio;
        quad = 1.8 + (0.44 - 1.8) * ratio;
    }

    else if (range <= 20)
    {
        float ratio = range - (20 - 13);
        ratio /= 13;
        linear = 0.35 + (0.22 - 0.35) * ratio;
        quad = 0.44 + (0.2 - 0.44) * ratio;
    }

    else if (range <= 32)
    {
        float ratio = range - (32 - 20);
        ratio /= 20;
        linear = 0.22 + (0.14 - 0.22) * ratio;
        quad = 0.2 + (0.07 - 0.2) * ratio;
    }

    else if (range <= 50)
    {
        float ratio = range - (50 - 32);
        ratio /= 32;
        linear = 0.14 + (0.09 - 0.14) * ratio;
        quad = 0.07 + (0.032 - 0.07) * ratio;
    }

    else if (range <= 65)
    {
        float ratio = range - (65 - 50);
        ratio /= 50;
        linear = 0.09 + (0.07 - 0.09) * ratio;
        quad = 0.032 + (0.017 - 0.032) * ratio;
    }

    else if (range <= 100)
    {
        float ratio = range - (100 - 65);
        ratio /= 65;
        linear = 0.07 + (0.045 - 0.07) * ratio;
        quad = 0.017 + (0.0075 - 0.017) * ratio;
    }

    else if (range <= 160)
    {
        float ratio = range - (160 - 100);
        ratio /= 100;
        linear = 0.045 + (0.027 - 0.045) * ratio;
        quad = 0.0075 + (0.0028 - 0.0075) * ratio;
    }
    else if (range <= 200)
    {
        float ratio = range - (200 - 160);
        ratio /= 160;
        linear = 0.027 + (0.022 - 0.027) * ratio;
        quad = 0.0028 + (0.0019 - 0.0028) * ratio;
    }
    else if (range <= 325)
    {
        float ratio = range - (325 - 200);
        ratio /= 200;
        linear = 0.022 + (0.014 - 0.022) * ratio;
        quad = 0.0019 + (0.0007 - 0.0019) * ratio;
    }
    else if (range <= 600)
    {
        float ratio = range - (600 - 325);
        ratio /= 325;

        linear = 0.014 + (0.007 - 0.014) * ratio;
        quad = 0.0007 + (0.0002 - 0.0007) * ratio;
    }
    else
    {
        float ratio = range - (3250 - 600);
        ratio /= 600;
        linear = 0.007 + (0.0014 - 0.007) * ratio;
        quad = 0.0002 + (0.000007 - 0.0002) * ratio;
    }

    lightComponents[entity]->linear = linear;
    lightComponents[entity]->quad = quad;
}

RenderComponent* SceneManager::AddRenderComponent(unsigned int _entity)
{
    RenderComponent* rend = new RenderComponent();

    renderComponents[_entity] = rend;
   
    return rend;
}

RenderComponent* SceneManager::AddRenderComponent(unsigned int _entity, RenderComponent* rend)
{
    renderComponents[_entity] = rend;

    return rend;
}

PhysicsComponent* SceneManager::AddPhysicsComponent(unsigned int _entity)
{
    PhysicsComponent* physics = new PhysicsComponent();

    physicsComponents[_entity] = physics;

    return physics;
}

LightComponent* SceneManager::AddLightComponent(unsigned int _entity)
{
    LightComponent* light = new LightComponent();

    lightComponents[_entity] = light;

    return light;
}

LightComponent* SceneManager::AddLightComponent(unsigned int _entity, LightComponent* light)
{
    lightComponents[_entity] = light;

    return light;
}

