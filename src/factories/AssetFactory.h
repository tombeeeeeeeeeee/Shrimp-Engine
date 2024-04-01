#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"

class AssetFactory {

public:
    AssetFactory(std::string assetFolder);
    ~AssetFactory();
    AssetFactory(AssetFactory& assFact);

    MaterialAsset* CubeMaterial();
    MeshAsset* CubeMesh();

    MaterialAsset* RatMaterial();
    MeshAsset* RatMesh();

    MaterialAsset* GetMaterial(std::string filename, int fileMask);
    MaterialAsset* GetMaterial(std::string filenames[], int fileMask);
    MeshAsset* GetMesh(std::string filename);

    //void GetAllAssetsInFolder();

private:
    std::string assetFolder;

    std::unordered_map<std::string, MaterialAsset*> materialAssets;
    std::unordered_map<std::string, MeshAsset*> meshAssets;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    MeshAsset* MakeObjMesh(const char* filepath, mat4 preTransform);
    MeshAsset* MakeFbxMesh(const char* filepath, mat4 preTransform);
    unsigned int MakeTexture(const char* filename);

    MeshAsset* sendMeshToVRAM(std::vector<float>& vertices,float vertexCount);

    vec3 readVec3(std::vector<std::string> strings, mat4 preTransform, float w);
    vec3 readVec3(std::vector<std::string> strings);
    vec2 readVec2(std::vector<std::string> strings);
    void readFace(std::vector<string>& data, std::vector<vec3>& v, std::vector<vec2>& vt, std::vector<vec3>& vn, std::vector<float>& vertices);
    void readTriCorner(string& data, std::vector<vec3>& v, std::vector<vec2>& vt, std::vector<vec3>& vn, std::vector<float>& vertices);
    void readTriCorner(string& data, std::vector<vec3>& v, std::vector<vec2>& vt, vec3 GetNormalised, std::vector<float>& vertices);
    vec3 readTriCornerVertex(string& data, std::vector<vec3>& v);
};