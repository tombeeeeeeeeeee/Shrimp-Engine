#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"

class AssetFactory {

public:
    AssetFactory(std::string assetFolder);

    ~AssetFactory();

    unsigned int MakeCamera(vec3 position, vec3 eulers);

    MaterialAsset CubeMaterial();
    MeshAsset CubeMesh();

    MaterialAsset RatMaterial();
    MeshAsset RatMesh();

    MaterialAsset* GetMaterial(std::string filenames[], int fileMask);
    unsigned int GetTexture(std::string filename);
    MeshAsset* GetMesh(std::string filename);

    void GetAllAssetsInFolder();

private:
    std::string assetFolder;

    std::unordered_map<std::string, MaterialAsset>& materialAssets;
    std::unordered_map<std::string, MeshAsset>& meshAssets;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    RenderComponent MakeCubeMesh(vec3 size);
    RenderComponent MakeObjMesh(const char* filepath, mat4 preTransform);
    RenderComponent MakeFbxMesh(const char* filepath, mat4 preTransform);
    unsigned int MakeTexture(const char* filename);

    MeshAsset sendMeshToGPU(std::vector<float>& vertices,float vertexCount);

    vec3 readVec3(std::vector<std::string> strings, mat4 preTransform, float w);
    vec3 readVec3(std::vector<std::string> strings);
    vec2 readVec2(std::vector<std::string> strings);
    void readFace(vector<string>& data, vector<vec3>& v, vector<vec2>& vt, vector<vec3>& vn, vector<float>& vertices);
    void readTriCorner(string& data, vector<vec3>& v, vector<vec2>& vt, vector<vec3>& vn, vector<float>& vertices);
    void readTriCorner(string& data, vector<vec3>& v, vector<vec2>& vt, vec3 normal, vector<float>& vertices);
    vec3 readTriCornerVertex(string& data, vector<vec3>& v);
};