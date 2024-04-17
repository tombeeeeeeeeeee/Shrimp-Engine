#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"

class AssetFactory {

public:
    AssetFactory(std::string assetFolder);
    ~AssetFactory();
    AssetFactory(AssetFactory& assFact);

    //RAT AND CUBE STUFF FOR TESTING
    MaterialAsset* CubeMaterial();
    MeshAsset* CubeMesh();

    MaterialAsset* RatMaterial();
    MeshAsset* RatMesh();

    /// <summary>
    /// Make a material asset with one texture
    /// </summary>
    /// <param name="filename">file location and name within asset folder</param>
    /// <param name="fileMask"> texture map mask</param>
    /// <returns>material asset</returns>
    MaterialAsset* GetMaterial(std::string filename, int fileMask);

    /// <summary>
    /// Make a material asset with multiple texture
    /// </summary>
    /// <param name="filename">file locations and names within asset folder</param>
    /// <param name="fileMask"> texture map mask</param>
    /// <returns>material asset</returns>
    MaterialAsset* GetMaterial(std::string filenames[], int fileMask);

    /// <summary>
    /// Get mesh from file
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    MeshAsset* GetMesh(std::string filename);

    /// <summary>
    /// Gets/Creates the skybox texture.
    /// </summary>
    /// <param name="filename"></param>
    /// <returns>Skybox Material Asset</returns>
    unsigned int GetSkyBoxMaterial(std::string filename[6]);

private:

    /// <summary>
    /// Folder that all assets can be found in.
    /// </summary>
    std::string assetFolder;

    /// <summary>
    /// All materials already in this factory
    /// </summary>
    std::unordered_map<std::string, MaterialAsset*> materialAssets;

    /// <summary>
    /// All meshes already in this factory
    /// </summary>
    std::unordered_map<std::string, MeshAsset*> meshAssets;


    //Cached mesh and material ids
    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> IBOs;
    std::vector<unsigned int> textures;

    /// <summary>
    /// Create texture from file
    /// </summary>
    /// <param name="filename"> file location with asset folder</param>
    /// <returns></returns>
    unsigned int MakeTexture(const char* filename, bool SRGB = false);

    /// <summary>
    /// Sends mesh to VRAM
    /// </summary>
    /// <param name="vertices"> a vector of vertices (xPos, yPos, zPos, xTexture, yTexture, xNormal, yNormal, zNormal)</param>
    /// <param name="vertexCount">Number of vertices</param>
    /// <param name="indexCount">Number of indcies</param>
    /// <param name="indcies">unsigned int pointer on VRAM to indices</param>
    /// <returns>mesh asset pointer</returns>
    MeshAsset* sendMeshToVRAM(std::vector<float>& vertices, int vertexCount, int indexCount = 0, unsigned int* indcies = nullptr);
};