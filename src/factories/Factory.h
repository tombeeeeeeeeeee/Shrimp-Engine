#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"

class Factory {

public:
    Factory(
        std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
        std::unordered_map<unsigned int, RenderComponent>& renderComponents,
        std::unordered_map<unsigned int, TransformComponent>& transformComponents);

    ~Factory();

    unsigned int MakeCamera(vec3 position, vec3 eulers);

    void MakeCube(vec3 position, vec3 eulers, vec3 eulerVelocity);

    void MakeRat(vec3 position, vec3 eulers);

    unsigned int MakeEntity() { return entityCount++; }

private:

    unsigned int entityCount = 0;

    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents;
    std::unordered_map<unsigned int, RenderComponent>& renderComponents;
    std::unordered_map<unsigned int, TransformComponent>& transformComponents;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    RenderComponent MakeCubeMesh(vec3 size);

    RenderComponent MakeMesh(const char* filepath, mat4 preTransform);
    RenderComponent MakeObjMesh(const char* filepath, mat4 preTransform);
    RenderComponent MakeFbxMesh(const char* filepath, mat4 preTransform);
    unsigned int MakeTexture(const char* filename);

    RenderComponent sendMeshToGPU(std::vector<float>& vertices,float vertexCount);

    vec3 readVec3(std::vector<std::string> strings, mat4 preTransform, float w);
    vec3 readVec3(std::vector<std::string> strings);
    vec2 readVec2(std::vector<std::string> strings);
    void readFace(vector<string>& data, vector<vec3>& v, vector<vec2>& vt, vector<vec3>& vn, vector<float>& vertices);
    void readTriCorner(string& data, vector<vec3>& v, vector<vec2>& vt, vector<vec3>& vn, vector<float>& vertices);
    void readTriCorner(string& data, vector<vec3>& v, vector<vec2>& vt, vec3 normal, vector<float>& vertices);
    vec3 readTriCornerVertex(string& data, vector<vec3>& v);
};