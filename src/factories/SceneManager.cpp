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

std::unordered_map<unsigned int, TransformComponent>* SceneManager::GetTransforms()
{
    return &transformComponents;
}

std::unordered_map<unsigned int, RenderComponent>* SceneManager::GetRenders()
{
    return &renderComponents;
}

std::unordered_map<unsigned int, LightComponent>* SceneManager::GetLights()
{
    return &lightComponents;
}

std::unordered_map<unsigned int, PhysicsComponent>* SceneManager::GetPhysics()
{
    return &physicsComponents;
}

unsigned int SceneManager::MakeCamera(glm::vec3 position, glm::vec3 eulers)
{
    return MakeEmptyTransform(position, eulers);
}

unsigned int SceneManager::MakeEmptyTransform()
{
    TransformComponent transform = TransformComponent();
    transformComponents[entityCount] = transform;
    return entityCount++;
}

unsigned int SceneManager::MakeEmptyTransform(glm::vec3 position, glm::vec3 eulers)
{
    TransformComponent transform = TransformComponent();
    transform.position = position;
    transform.eulers = eulers;
    transformComponents[entityCount] = transform;
    TransformsToUpdate.push_back(entityCount);
    return entityCount++;
}

const glm::vec3 SceneManager::GetPosition(unsigned int entity)
{                                                                                                     
    if (transformComponents.find(entity) != transformComponents.end()) 
        return { transformComponents[entity].globalTransform[3][0], transformComponents[entity].globalTransform[3][1], transformComponents[entity].globalTransform[3][2] };
    return {NAN, NAN, NAN};
}

const glm::vec3 SceneManager::GetLocalPosition(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
        return transformComponents[entity].position;
    return { NAN, NAN, NAN };
}

//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetLocalPosition(unsigned int entity, glm::vec3 position)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        transformComponents[entity].position = position;
        TransformsToUpdate.push_back(entity);
    }
}

const glm::vec3 SceneManager::GetEulers(unsigned int entity, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        glm::mat4 globalTransform = transformComponents[entity].globalTransform;

        float eulX = 0;
        float eulY = 0;
        float eulZ = 0;

        float sy = sqrt(globalTransform[0][0] * globalTransform[0][0] + globalTransform[0][1] * globalTransform[0][1]);

        bool singular = sy < 1e-6; // If

        float x, y, z;
        if (!singular)
        {
            x = atan2(globalTransform[1][2], globalTransform[2][2]);
            y = atan2(-globalTransform[0][2], sy);
            z = atan2(globalTransform[0][1], globalTransform[0][0]);
        }
        else
        {
            x = atan2(-globalTransform[2][1], globalTransform[1][1]);
            y = atan2(-globalTransform[0][2], sy);
            z = 0;
        }
        return glm::vec3(x, y, z);

        if (abs(globalTransform[0][2]) != 1)
        {
            eulY = asinf(globalTransform[0][2]);
            float c = cos(eulY);
            eulX = atan2f(globalTransform[1][2] / c, globalTransform[2][2] / c);
            eulZ = atan2f(globalTransform[0][1] / c, globalTransform[0][0] / c);
        }
        else
        {
            eulZ = 0;
            eulY = glm::pi<float>() / 2.0f;
            if (globalTransform[0][2] == -1)
            {
                eulX = atan2f(globalTransform[1][1], globalTransform[2][1]);
            }
            else
            {
                eulY *= -1;
                eulX = atan2f(-globalTransform[1][1], -globalTransform[2][1]);
            }
        }
        glm::vec3 eul = { eulX, eulY, eulZ };
        if (!radians) eul *= 180 / glm::pi<float>();
        return eul;
    }
    else return { NAN, NAN, NAN };
}

const glm::vec3 SceneManager::GetLocalEulers(unsigned int entity, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        return radians ? transformComponents[entity].eulers : transformComponents[entity].eulers * (180.0f / glm::pi<float>());
    }
    else return { NAN, NAN, NAN };
}
//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetLocalEulers(unsigned int entity, glm::vec3 eulers, bool radians)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        transformComponents[entity].eulers = eulers;
        TransformsToUpdate.push_back(entity);
    }
}

const glm::vec3 SceneManager::GetScale(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        return transformComponents[entity].scale;
    }
    else return { NAN, NAN, NAN };
}
//TODO Add flag that it needs to be updated in the heirarchy
void SceneManager::SetScale(unsigned int entity, glm::vec3 scale)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        transformComponents[entity].scale = scale;
        TransformsToUpdate.push_back(entity);
    }
}

const glm::mat4 SceneManager::GetLocalTransform(unsigned int entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
    {
        glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), transformComponents[entity].position);
        glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), transformComponents[entity].scale);
        glm::mat4 rotationX = glm::rotate(glm::identity<glm::mat4>(), transformComponents[entity].eulers.x, {1,0,0});
        glm::mat4 rotationY = glm::rotate(glm::identity<glm::mat4>(), transformComponents[entity].eulers.y, {0,1,0});
        glm::mat4 rotationZ = glm::rotate(glm::identity<glm::mat4>(), transformComponents[entity].eulers.z, {0,0,1});

        return translate * rotationX * rotationY * rotationZ * scale;
    }
    else return { 
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
    };
}

const glm::mat4 SceneManager::GetGlobalTransform(unsigned entity)
{
    if (transformComponents.find(entity) != transformComponents.end())
        return transformComponents[entity].globalTransform;
    else return {
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
        NAN, NAN, NAN, NAN,
    };
}



unsigned int SceneManager::MakeAmbientLightEntity(glm::vec3 colour, float intensity)
{
    unsigned int ambi = MakeEmptyTransform();
    return AddAmbientLightComponent(ambi,colour,intensity);
}

unsigned int SceneManager::AddAmbientLightComponent(unsigned int entity, glm::vec3 colour, float intensity)
{
    LightComponent light = LightComponent();
    light.colour = colour * intensity;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakeDirectionalLightEntity(glm::vec3 direction, glm::vec3 colour, float intensity)
{
    glm::vec3 trueDirection = glm::normalize(direction);
    unsigned int directional = MakeEmptyTransform();
    return AddDirectionalLightComponent(directional, trueDirection, colour, intensity);
}

unsigned int SceneManager::AddDirectionalLightComponent(unsigned int entity, glm::vec3 direction, glm::vec3 colour, float intensity)
{
    LightComponent light = LightComponent();
    light.colour = colour * intensity;
    light.lightType = LightType::directional;
    light.direction = direction;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakePointLightEntity(glm::vec3 pos, float range, glm::vec3 colour, float intensity, bool debug)
{
    unsigned int point = MakeEmptyTransform(pos);

    //PlaceHolder light Render
    if (debug)
    {
        RenderComponent lightCube = RenderComponent();
        lightCube.mesh = assFact.GetMesh("models/cube.obj");
        lightCube.material = assFact.GetMaterial("img/light.png", 1);
        lightCube.material->shaderProgram = 1;
        AddRenderComponent(point, lightCube);
        transformComponents[point].scale = { 0.5,0.5,0.5 };
    }

    return AddPointLightComponent(point, range, colour, intensity);
}

unsigned int SceneManager::AddPointLightComponent(unsigned int entity, float range, glm::vec3 colour, float intensity)
{
    LightComponent light = LightComponent();
    light.colour = colour * intensity;
    light.lightType = LightType::point;
    light.range = range;
    float linear, quad;
    CalculateLinearQuadConstants(range, linear, quad);
    light.linear = linear;
    light.quad = quad;
    AddLightComponent(entity, light);
    return entity;
}

unsigned int SceneManager::MakeSpotLightEntity(glm::vec3 pos, glm::vec3 dir, float range, float cutOff, float outerCutOff, glm::vec3 colour, float intensity, bool debug)
{
    unsigned int spot = MakeEmptyTransform(pos);

    //PlaceHolder light Render
    if (debug)
    {
        RenderComponent lightCube = RenderComponent();
        lightCube.mesh = assFact.GetMesh("models/cube.obj");
        lightCube.material = assFact.GetMaterial("img/light.png", 1);
        lightCube.material->shaderProgram = 1;
        AddRenderComponent(spot, lightCube);
        transformComponents[spot].scale = { 0.1,0.1,0.1 };
    }

    return AddSpotLightComponent(spot, dir, range, cutOff, outerCutOff, colour, intensity);
}

unsigned int SceneManager::AddSpotLightComponent(unsigned int entity, glm::vec3 dir, float range, float cutOff, float outerCutOff, glm::vec3 colour, float intensity)
{
    LightComponent light = LightComponent();
    light.colour = colour * intensity;
    light.lightType = LightType::spot;
    light.range = range;
    light.direction = dir;
    float linear, quad;
    CalculateLinearQuadConstants(range, linear, quad);
    light.linear = linear;
    light.quad = quad;
    light.cutOff = cos(glm::pi<float>() * cutOff / 180);
    light.outerCutOff = cos(glm::pi<float>() * outerCutOff / 180);
    AddLightComponent(entity, light);
    return entity;
}

LightComponent SceneManager::AddLightComponent(unsigned int _entity)
{
    LightComponent light = LightComponent();

    lightComponents[_entity] = light;

    return light;
}

LightComponent SceneManager::AddLightComponent(unsigned int _entity, LightComponent& light)
{
    lightComponents[_entity] = light;

    return light;
}

void SceneManager::CalculateLinearQuadConstants(unsigned int entity)
{
    if (lightComponents.find(entity) == lightComponents.end()) return;

    float range = lightComponents[entity].range;
    float linear, quad;
    if (range <= 7)
    {
        linear = 1 - range * 0.3f / 7;
        quad = 1.8f + (7 - range) * 0.5f;
    }

    else if (range <= 13)
    {
        float ratio = range - (13 - 7);
        ratio /= 7;
        linear = 0.7f + (0.35f - 0.7f) * ratio;
        quad = 1.8f + (0.44f - 1.8f) * ratio;
    }

    else if (range <= 20)
    {
        float ratio = range - (20 - 13);
        ratio /= 13;
        linear = 0.35f + (0.22f - 0.35f) * ratio;
        quad = 0.44f + (0.2f - 0.44f) * ratio;
    }

    else if (range <= 32)
    {
        float ratio = range - (32 - 20);
        ratio /= 20;
        linear = 0.22f + (0.14f - 0.22f) * ratio;
        quad = 0.2f + (0.07f - 0.2f) * ratio;
    }

    else if (range <= 50)
    {
        float ratio = range - (50 - 32);
        ratio /= 32;
        linear = 0.14f + (0.09f - 0.14f) * ratio;
        quad = 0.07f + (0.032f - 0.07f) * ratio;
    }

    else if (range <= 65)
    {
        float ratio = range - (65 - 50);
        ratio /= 50;
        linear = 0.09f + (0.07f - 0.09f) * ratio;
        quad = 0.032f + (0.017f - 0.032f) * ratio;
    }

    else if (range <= 100)
    {
        float ratio = range - (100 - 65);
        ratio /= 65;
        linear = 0.07f + (0.045f - 0.07f) * ratio;
        quad = 0.017f + (0.0075f - 0.017f) * ratio;
    }

    else if (range <= 160)
    {
        float ratio = range - (160 - 100);
        ratio /= 100;
        linear = 0.045f + (0.027f - 0.045f) * ratio;
        quad = 0.0075f + (0.0028f - 0.0075f) * ratio;
    }
    else if (range <= 200)
    {
        float ratio = range - (200 - 160);
        ratio /= 160;
        linear = 0.027f + (0.022f - 0.027f) * ratio;
        quad = 0.0028f + (0.0019f - 0.0028f) * ratio;
    }
    else if (range <= 325)
    {
        float ratio = range - (325 - 200);
        ratio /= 200;
        linear = 0.022f + (0.014f - 0.022f) * ratio;
        quad = 0.0019f + (0.0007f - 0.0019f) * ratio;
    }
    else if (range <= 600)
    {
        float ratio = range - (600 - 325);
        ratio /= 325;

        linear = 0.014f + (0.007f - 0.014f) * ratio;
        quad = 0.0007f + (0.0002f - 0.0007f) * ratio;
    }
    else
    {
        float ratio = range - (3250 - 600);
        ratio /= 600;
        linear = 0.007f + (0.0014f - 0.007f) * ratio;
        quad = 0.0002f + (0.000007f - 0.0002f) * ratio;
    }

    lightComponents[entity].linear = linear;
    lightComponents[entity].quad = quad;
}

void SceneManager::CalculateLinearQuadConstants(float range, float& linear, float& quad)
{
    if (range <= 7)
    {
        linear = 1 - range * 0.3f / 7;
        quad = 1.8f + (7 - range) * 0.5f;
    }

    else if (range <= 13)
    {
        float ratio = range - (13 - 7);
        ratio /= 7;
        linear = 0.7f + (0.35f - 0.7f) * ratio;
        quad = 1.8f + (0.44f - 1.8f) * ratio;
    }

    else if (range <= 20)
    {
        float ratio = range - (20 - 13);
        ratio /= 13;
        linear = 0.35f + (0.22f - 0.35f) * ratio;
        quad = 0.44f + (0.2f - 0.44f) * ratio;
    }

    else if (range <= 32)
    {
        float ratio = range - (32 - 20);
        ratio /= 20;
        linear = 0.22f + (0.14f - 0.22f) * ratio;
        quad = 0.2f + (0.07f - 0.2f) * ratio;
    }

    else if (range <= 50)
    {
        float ratio = range - (50 - 32);
        ratio /= 32;
        linear = 0.14f + (0.09f - 0.14f) * ratio;
        quad = 0.07f + (0.032f - 0.07f) * ratio;
    }

    else if (range <= 65)
    {
        float ratio = range - (65 - 50);
        ratio /= 50;
        linear = 0.09f + (0.07f - 0.09f) * ratio;
        quad = 0.032f + (0.017f - 0.032f) * ratio;
    }

    else if (range <= 100)
    {
        float ratio = range - (100 - 65);
        ratio /= 65;
        linear = 0.07f + (0.045f - 0.07f) * ratio;
        quad = 0.017f + (0.0075f - 0.017f) * ratio;
    }

    else if (range <= 160)
    {
        float ratio = range - (160 - 100);
        ratio /= 100;
        linear = 0.045f + (0.027f - 0.045f) * ratio;
        quad = 0.0075f + (0.0028f - 0.0075f) * ratio;
    }
    else if (range <= 200)
    {
        float ratio = range - (200 - 160);
        ratio /= 160;
        linear = 0.027f + (0.022f - 0.027f) * ratio;
        quad = 0.0028f + (0.0019f - 0.0028f) * ratio;
    }
    else if (range <= 325)
    {
        float ratio = range - (325 - 200);
        ratio /= 200;
        linear = 0.022f + (0.014f - 0.022f) * ratio;
        quad = 0.0019f + (0.0007f - 0.0019f) * ratio;
    }
    else if (range <= 600)
    {
        float ratio = range - (600 - 325);
        ratio /= 325;

        linear = 0.014f + (0.007f - 0.014f) * ratio;
        quad = 0.0007f + (0.0002f - 0.0007f) * ratio;
    }
    else
    {
        float ratio = range - (3250 - 600);
        ratio /= 600;
        linear = 0.007f + (0.0014f - 0.007f) * ratio;
        quad = 0.0002f + (0.000007f - 0.0002f) * ratio;
    }
}

const LightType SceneManager::GetLightType(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].lightType;
    }
    else return LightType::Not_A_Light;
}

void SceneManager::SetLightType(unsigned int entity, LightType lightType)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        lightComponents[entity].lightType = lightType;
    }
}

const glm::vec3 SceneManager::GetLightColour(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].colour;
    }
    else return {0,0,0};
}

void SceneManager::SetLightColour(unsigned int entity, glm::vec3 colour, float intensity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        lightComponents[entity].colour = colour * intensity;
    }
}

const glm::vec3 SceneManager::GetLightDirection(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].direction;
    }
    else return { NAN,NAN,NAN };
}

void SceneManager::SetLightDirection(unsigned int entity, glm::vec3 direction)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        lightComponents[entity].direction = direction;
    }
}

const float SceneManager::GetLightRange(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].range;
    }
    else return NAN;
}

void SceneManager::SetLightRange(unsigned int entity, float range)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        lightComponents[entity].range = range;
        CalculateLinearQuadConstants(entity);
    }
}

const float SceneManager::GetLinearAttenuation(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].linear;
    }
    else return NAN;
}

const float SceneManager::GetQuadraticAttenuation(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].quad;
    }
    else return NAN;
}

const float SceneManager::GetLightCutOff(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].cutOff;
    }
    else return NAN;
}

void SceneManager::SetLightCutOff(unsigned int entity, float cutOff)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
       lightComponents[entity].cutOff = cutOff;
       if (lightComponents[entity].outerCutOff < cutOff)
           lightComponents[entity].outerCutOff = cutOff;
    }
}

const float SceneManager::GetLightOuterCutOff(unsigned int entity)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        return lightComponents[entity].outerCutOff;
    }
    else return NAN;
}

void SceneManager::SetLightOuterCutOff(unsigned int entity, float outerCutOff)
{
    if (lightComponents.find(entity) != lightComponents.end())
    {
        lightComponents[entity].outerCutOff = outerCutOff;
        if (lightComponents[entity].cutOff > outerCutOff)
            lightComponents[entity].cutOff = outerCutOff;
    }
}

RenderComponent SceneManager::AddRenderComponent(unsigned int _entity)
{
    RenderComponent rend = RenderComponent();

    renderComponents[_entity] = rend;
   
    return rend;
}

RenderComponent SceneManager::AddRenderComponent(unsigned int _entity, RenderComponent& rend)
{
    renderComponents[_entity] = rend;

    return rend;
}

const MaterialAsset* SceneManager::GetMaterial(unsigned int entity)
{
    if (renderComponents.find(entity) != renderComponents.end())
    {
        return renderComponents[entity].material;
    }
    else return nullptr;
}

void SceneManager::SetMaterial(unsigned int entity, MaterialAsset* mat)
{
    if (renderComponents.find(entity) != renderComponents.end())
    {
       renderComponents[entity].material = mat;
    }
}

const MeshAsset* SceneManager::GetMesh(unsigned int entity)
{
    if (renderComponents.find(entity) != renderComponents.end())
    {
        return renderComponents[entity].mesh;
    }
    else return nullptr;
}

void SceneManager::SetMesh(unsigned int entity, MeshAsset* mesh)
{
    if (renderComponents.find(entity) != renderComponents.end())
    {
        renderComponents[entity].mesh = mesh;
    }
}

unsigned int SceneManager::MakePhysicsEntity()
{
    unsigned int physics = MakeEmptyTransform();
    AddPhysicsComponent(physics);
    return physics;
}

PhysicsComponent SceneManager::AddPhysicsComponent(unsigned int _entity, float invMass)
{
    PhysicsComponent physics = PhysicsComponent();

    physics.centreOfMass = glm::zero<glm::vec3>();
    physics.netDepen = glm::zero<glm::vec3>();
    physics.dX = physics.dY = physics.dZ = 0.0f;
    physics.force = glm::zero<glm::vec3>();
    physics.torque = glm::zero<glm::vec3>();
    physics.velocity = glm::zero<glm::vec3>();
    physics.angularVelocity = glm::zero<glm::vec3>();
    physics.angularMomentum = glm::zero<glm::vec3>();
    physics.isGravitated = false;
    physics.elasticCoef = 0.65f;
    physics.drag = 1;
    physics.angularDrag = 5;
    physics.invMass = invMass;
    physics.momentOfInertia = glm::zero<glm::vec3>();
    physics.invBodyIT = glm::zero<glm::mat3>();
    physics.invWorldIT = glm::zero<glm::mat3>();

    physicsComponents[_entity] = physics;

    return physics;
}

PhysicsComponent SceneManager::AddPhysicsComponent(
    unsigned int _entity, glm::vec3 momentOfInertia, 
    glm::vec3 force, glm::vec3 velocity, glm::vec3 netDepen, 
    glm::mat3 invBodyIT, glm::mat3 invWorldIT, glm::vec3 torque, 
    glm::vec3 angularVelocity, glm::vec3 angularMomentum, 
    float dX, float dY, float dZ, std::vector<Shape> shapes,
    float invMass, glm::vec3 centreOfMass, bool isGravitated, 
    float elasiticCoef, float drag, float angularDrag)
{
    PhysicsComponent physics = PhysicsComponent();

    physics.momentOfInertia = momentOfInertia;
    physics.force = force;
    physics.velocity = velocity;
    physics.netDepen = netDepen;
    physics.invBodyIT = invBodyIT;
    physics.invWorldIT = invWorldIT;
    physics.torque = torque;
    physics.angularVelocity = angularVelocity;
    physics.angularMomentum = angularMomentum;
    physics.dX = dX; physics.dY = dY; physics.dZ = dZ;
    physics.shapes = shapes;
    physics.invMass = invMass;
    physics.centreOfMass = centreOfMass;
    physics.isGravitated = isGravitated;
    physics.elasticCoef = elasiticCoef;
    physics.drag = drag;
    physics.angularDrag = angularDrag;

    physicsComponents[_entity] = physics;

    return physics;
}

const glm::vec3 SceneManager::GetMomentOfInertiaScale(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].momentOfInertia;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetMomentOfInertiaScale(unsigned int _entity, glm::vec3 scale)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].momentOfInertia = scale;
        UpdateBodyInertiaTensor(_entity);
    }
}

void SceneManager::SetMomentOfInertiaScale(unsigned int _entity, float x, float y, float z)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].momentOfInertia = glm::vec3(x,y,z);
        UpdateBodyInertiaTensor(_entity);
    }
}

const glm::vec3 SceneManager::GetForce(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].force;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetForce(unsigned int _entity, glm::vec3 force)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].force = force;
    }
}

void SceneManager::SetForce(unsigned int _entity, float x, float y, float z)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].force = glm::vec3(x, y, z);
    }
}

void SceneManager::AddForce(unsigned int _entity, glm::vec3 force)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].force += force;
    }
}

const glm::vec3 SceneManager::GetVelocity(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].velocity;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetVelocity(unsigned int _entity, glm::vec3 velocity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity = velocity;
    }
}

void SceneManager::SetVelocity(unsigned int _entity, float x, float y, float z)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity = glm::vec3(x, y, z);
    }
}

void SceneManager::AddVelocity(unsigned int _entity, glm::vec3 velocity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity += velocity;
    }
}

const glm::vec3 SceneManager::GetMomentum(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end() && physicsComponents[_entity].invMass != 0)
    {
        return physicsComponents[_entity].velocity / physicsComponents[_entity].invMass;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetMomentum(unsigned int _entity, glm::vec3 momentum)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity = momentum * physicsComponents[_entity].invMass;
    }
}

void SceneManager::SetMomentum(unsigned int _entity, float x, float y, float z)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity = glm::vec3(x, y, z) * physicsComponents[_entity].invMass;
    }
}

void SceneManager::AddMomentum(unsigned int _entity, glm::vec3 momentum)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].velocity += momentum * physicsComponents[_entity].invMass;
    }
}

const glm::vec3 SceneManager::GetNetDepenetration(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].netDepen;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetNetDepenetration(unsigned int _entity, glm::vec3 depenertration)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].netDepen = depenertration;
    }
}

const glm::mat3 SceneManager::GetInverseBodyInertiaTensor(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].invBodyIT;
    }
    else return glm::mat3(
        NAN, NAN, NAN,
        NAN, NAN, NAN,
        NAN, NAN, NAN
        );
}

void SceneManager::SetInverseBodyInertiaTensor(unsigned int _entity, glm::mat3 invBodyTensor)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].invBodyIT = invBodyTensor;
    }
}

void SceneManager::UpdateBodyInertiaTensor(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        float dx = physicsComponents[_entity].dX;
        float dy = physicsComponents[_entity].dY;
        float dz = physicsComponents[_entity].dZ;
        float invMass = physicsComponents[_entity].invMass;

        float scaleX = transformComponents[_entity].scale.x;
        float scaleY = transformComponents[_entity].scale.y;
        float scaleZ = transformComponents[_entity].scale.z;

        glm::vec3 scale = physicsComponents[_entity].momentOfInertia;
        physicsComponents[_entity].invBodyIT = glm::mat3(
            (scale.x * invMass * 12) / ((dy * dy + dz * dz) * scaleX), 0, 0,
            0, (scale.y * invMass * 12) / ((dx * dx + dz * dz) * scaleY), 0,
            0, 0, (scale.z * invMass * 12) / ((dx * dx + dy * dy) * scaleZ)
            );
    }
}


const glm::mat3 SceneManager::GetInverseWorldInertiaTensor(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].invWorldIT;
    }
    else return glm::mat3(
        NAN, NAN, NAN,
        NAN, NAN, NAN,
        NAN, NAN, NAN
    );
}

void SceneManager::SetInverseWorldInertiaTensor(unsigned int _entity, glm::mat3 invWorldTensor)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].invWorldIT = invWorldTensor;
    }
}

const glm::vec3 SceneManager::GetTorque(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].torque;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetTorque(unsigned int _entity, glm::vec3 torque)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].torque = torque;
    }
}

void SceneManager::AddTorque(unsigned int _entity, glm::vec3 torque)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].torque += torque;
    }
}

const glm::vec3 SceneManager::GetAngularVelocity(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].angularVelocity;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetAngularVelocity(unsigned int _entity, glm::vec3 angularVel)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].angularVelocity = angularVel;
    }
}

void SceneManager::AddAngularVelocity(unsigned int _entity, glm::vec3 angularVel)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].angularVelocity += angularVel;
    }
}

const glm::vec3 SceneManager::GetAngularMomentum(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].angularMomentum;
    }
    else return glm::vec3(NAN, NAN, NAN);
}

void SceneManager::SetAngularMomentum(unsigned int _entity, glm::vec3 angularMom)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].angularMomentum = angularMom;
    }
}

void SceneManager::AddAngularMomentum(unsigned int _entity, glm::vec3 angularMom)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].angularMomentum += angularMom;
    }
}

const std::vector<Shape>* SceneManager::GetPhysicsComponentShapes(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return &physicsComponents[_entity].shapes;
    }
    else return nullptr;
}

void SceneManager::SetPhysicsComponentShapes(unsigned int _entity, std::vector<Shape>* shapes)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].shapes = *shapes;
    }
}

void SceneManager::AddPhysicsShape(unsigned int _entity, Shape shape, glm::vec3 offset)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        for (int i = 0; i < shape.vertices.size(); i++)
        {
            shape.vertices[i].vert += offset;
        }
        physicsComponents[_entity].shapes.push_back(shape);
    }
}

void SceneManager::AddPhysicsSphere(unsigned int _entity, float radius, glm::vec3 offset)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        Vertex vert;
        vert.vert = offset;
        Shape shape;
        shape.radius = radius;
        shape.vertices.push_back(vert);
        physicsComponents[_entity].shapes.push_back(shape);
    }
}

void SceneManager::AddPhysicsShapePill(unsigned int _entity, float radius, glm::vec3 offset)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        Vertex vertBot;
        Vertex vertTop;
        vertBot.vert = offset + glm::vec3(0,radius,0);
        vertTop.vert = offset + glm::vec3(0,-radius,0);
        vertBot.edges.push_back(1);
        vertTop.edges.push_back(0);

        Shape shape;
        shape.radius = radius;
        shape.vertices.push_back(vertBot);
        shape.vertices.push_back(vertTop);
        physicsComponents[_entity].shapes.push_back(shape);
    }
}

void SceneManager::AddPhysicsShapePill(unsigned int _entity, glm::vec3 bottom, glm::vec3 top, float radius)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        Vertex vertBot;
        Vertex vertTop;
        vertBot.vert = bottom;
        vertTop.vert = top;
        vertBot.edges.push_back(1);
        vertTop.edges.push_back(0);

        Shape shape;
        shape.radius = radius;
        shape.vertices.push_back(vertBot);
        shape.vertices.push_back(vertTop);
        physicsComponents[_entity].shapes.push_back(shape);
    }
}

const float SceneManager::GetInverseMass(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].invMass;
    }
    else return NAN;
}

const float SceneManager::GetMass(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        if (physicsComponents[_entity].invMass != 0)
            return 1 / physicsComponents[_entity].invMass;
        else return NAN;
    }
    else return NAN;
}

void SceneManager::SetInverseMass(unsigned int _entity, float invMass)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].invMass = abs(invMass);
    }
}

void SceneManager::SetMass(unsigned int _entity, float mass)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        if (mass != 0)
            physicsComponents[_entity].invMass = abs(1 / mass);
    }
}

const glm::vec3 SceneManager::GetCentreOfMass(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].centreOfMass;
    }
    else return glm::vec3(NAN,NAN,NAN);
}

void SceneManager::SetCentreOfMass(unsigned int _entity, glm::vec3 CoM)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].centreOfMass = CoM;
    }
}

const bool SceneManager::GetIsGravitated(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].isGravitated;
    }
    else return false;
}

void SceneManager::SetIsGravitated(unsigned int _entity, bool gravitated)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].isGravitated = gravitated;
    }
}

const float SceneManager::GetElasticCoef(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].elasticCoef;
    }
    else return NAN;
}

void SceneManager::SetElasticCoef(unsigned int _entity, float elasticCoef)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].elasticCoef = elasticCoef;
    }
}

const float SceneManager::GetPhysicsDrag(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].drag;
    }
    else return NAN;
}

void SceneManager::SetPhysicsDrag(unsigned int _entity, float drag)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].drag = glm::clamp(drag, 0.0f, 10000.0f);
    }
}

const float SceneManager::GetPhysicsAngularDrag(unsigned int _entity)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        return physicsComponents[_entity].angularDrag;
    }
    else return NAN;
}

void SceneManager::SetPhysicsAngularDrag(unsigned int _entity, float angularDrag)
{
    if (physicsComponents.find(_entity) != physicsComponents.end())
    {
        physicsComponents[_entity].angularDrag = glm::clamp(angularDrag, 0.0f, 100.0f);
    }
}

void SceneManager::GlobalTransformUpdate(unsigned int entity, bool first)
{
    if (first && UpdatedGlobalTransform[entity])
        return;

    UpdatedGlobalTransform[entity] = true;

    TransformComponent* tc = &transformComponents[entity];
    glm::mat4 local = GetLocalTransform(entity);

    //Check if the entity has a parent
    if (tc->parent > 0)
        tc->globalTransform = transformComponents[tc->parent].globalTransform * local;
    else
        tc->globalTransform = local;

    //Send recursion to children.
    std::vector<unsigned int>::iterator iter;
    for (iter = tc->children.begin(); iter != tc->children.end(); iter++)
        GlobalTransformUpdate(*iter, false);
}


void SceneManager::SetParent(unsigned int child, unsigned int parent)
{
    bool realParent = transformComponents.find(parent) != transformComponents.end();
    if ((realParent || parent == 0) && transformComponents.find(child) != transformComponents.end())
    {
        transformComponents[child].parent = parent;
        if (realParent)
        {
            transformComponents[parent].children.push_back(child);
            TransformsToUpdate.push_back(child);
        }
    }
}

void SceneManager::HierarchyUpdate()
{
    std::vector<unsigned int>::iterator iter;
    for (iter = TransformsToUpdate.begin(); iter != TransformsToUpdate.end(); iter++)
        GlobalTransformUpdate(*iter);

    TransformsToUpdate.clear();
    UpdatedGlobalTransform.clear();
}