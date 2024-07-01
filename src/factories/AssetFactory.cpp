#include "AssetFactory.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif 
#include "../stb_image.h"
#include <assimp/postprocess.h>

AssetFactory::AssetFactory(std::string _assetFolder) : assetFolder(_assetFolder)
{ 
}


AssetFactory::~AssetFactory()
{
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());

    std::vector<unsigned int> texturesToDelete;
    for (std::unordered_map<std::string, unsigned int>::iterator iter = textures.begin(); iter != textures.end(); iter++) texturesToDelete.push_back(iter->second);

    glDeleteTextures(textures.size(), texturesToDelete.data());

    meshAssets.clear();
    materialAssets.clear();
}

AssetFactory::AssetFactory(AssetFactory& assFact)
{
    assetFolder = assFact.assetFolder;

    VBOs = assFact.VBOs;
    VAOs = assFact.VAOs;
    textures = assFact.textures;

    meshAssets = assFact.meshAssets;
    materialAssets = assFact.materialAssets;
}

AssetFactory& AssetFactory::operator=(AssetFactory const& other)
{
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());

    std::vector<unsigned int> texturesToDelete;
    for (std::unordered_map<std::string, unsigned int>::iterator iter = textures.begin(); iter != textures.end(); iter++) texturesToDelete.push_back(iter->second);

    glDeleteTextures(textures.size(), texturesToDelete.data());

    meshAssets.clear();
    materialAssets.clear();

    assetFolder = other.assetFolder;

    VBOs = other.VBOs;
    VAOs = other.VAOs;
    textures = other.textures;

    meshAssets = other.meshAssets;
    materialAssets = other.materialAssets;
    return *this;
}

unsigned int AssetFactory::MakeTexture(const char* filename, bool SRBG)
{
    if (textures.find(filename) != textures.end()) return textures[filename];

    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    
    unsigned char* data = stbi_load( filename, &width, &height, &channels, STBI_rgb_alpha);

    if (data == NULL) return 0;

    //make the texture
    unsigned int texture;
    glGenTextures(1, &texture);
    textures[filename] = (texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //load data
    if(SRBG)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
    stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

MeshAsset* AssetFactory::sendMeshToVRAM(std::vector<float>& vertices, int vertexCount, int indexCount, unsigned int* indices)
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 44, (void*)0);

    //texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 44, (void*)12);

    //normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 44, (void*)20);

    //Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 44, (void*)32);

    MeshAsset* mesh = new MeshAsset();
    mesh->VAO = VAO;
    mesh->IBO = 0;
    mesh->triCount = vertexCount / 3;

    unsigned int IBO;
    if (indexCount > 0)
    {
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        mesh->triCount = indexCount/3;
        mesh->IBO = IBO;
    }
 
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return mesh;
}

MaterialAsset* AssetFactory::GetMaterial(std::string fileName, int fileMask)
{
    if (materialAssets.find(fileName) != materialAssets.end()) return (materialAssets[fileName]);

    MaterialAsset* mat = new MaterialAsset();

    mat->materials[0] = MakeTexture((assetFolder + fileName).c_str(), fileMask == 1);
    mat->materialMask = fileMask;

    materialAssets[fileName] = mat;
    return mat;
}

MaterialAsset* AssetFactory::GetMaterial(std::string fileNames[], int fileMask)
{
    if (materialAssets.find(fileNames[0]) != materialAssets.end()) return (materialAssets[fileNames[0]]);

    MaterialAsset* mat = new MaterialAsset();

    int fileBinaryCheck = 1;
    for (int i = 0; i < MATERIAL_MAPCOUNT; i++, fileBinaryCheck *= 2)
    {
        if((fileMask & fileBinaryCheck) == fileBinaryCheck)
            mat->materials[i] = MakeTexture((assetFolder + fileNames[i]).c_str(), i == 0);
    }
    materialAssets[fileNames[0]] = mat;
    mat->materialMask = fileMask;
    return mat;
}

MeshAsset* AssetFactory::GetMesh(std::string fileName)
{
    if (meshAssets.find(fileName) != meshAssets.end()) return meshAssets[fileName];

    const aiScene* scene = aiImportFile((assetFolder + fileName).c_str(), aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_GenBoundingBoxes | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (scene == nullptr) return nullptr;
    
    aiMesh* mesh = scene->mMeshes[0];
    
    auto min = scene->mMeshes[0]->mAABB.mMin;
    auto max = scene->mMeshes[0]->mAABB.mMax;
    glm::vec3 botCorner = {min.x, min.y, min.z};
    glm::vec3 topCorner = {max.x, max.y, max.z};


    int faceCount = mesh->mNumFaces;
    std::vector<unsigned int> indices;
    for (int i = 0; i < faceCount; i++)
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    
        // generate a second triangle for quads
        if (mesh->mFaces[i].mNumIndices == 4)
        {
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
            indices.push_back(mesh->mFaces[i].mIndices[3]);
        }
    }
    
    int vertexCount = mesh->mNumVertices; 

    std::vector<float> vertices;
    vertices.reserve(vertexCount * 14);
    for (int i = 0; i < vertexCount; i++)
    {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        vertices.push_back(mesh->mTextureCoords[0][i].x);
        vertices.push_back(mesh->mTextureCoords[0][i].y);

        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);
         
        vertices.push_back(mesh->mTangents[i].x);
        vertices.push_back(mesh->mTangents[i].y);
        vertices.push_back(mesh->mTangents[i].z);
    }

    MeshAsset* meshAsset = sendMeshToVRAM( vertices, vertexCount, indices.size(), indices.data());
    meshAsset->bottomCorner = botCorner;
    meshAsset->topCorner = topCorner;
    return  meshAsset;
}

unsigned int AssetFactory::GetSkyBoxMaterial(std::string fileName[6], std::string name)
{
    if (materialAssets.find(name) != materialAssets.end()) return materialAssets[name]->materials[0];

    unsigned int fullscreenQuad;
    glGenTextures(1, &fullscreenQuad);
    glBindTexture(GL_TEXTURE_CUBE_MAP, fullscreenQuad);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; i++)
    {
        int width, height, channels;
        unsigned char* data = stbi_load((assetFolder + fileName[i]).c_str(), &width, &height, &channels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            ColouredOutput("FAILED TO LOAD CUBEMAP TEXTURE: ", Colour::red, false);
            ColouredOutput(fileName[i], Colour::red, true);
        }
        stbi_image_free(data);
    }

    MaterialAsset* mat = new MaterialAsset();

    mat->materials[0] = fullscreenQuad;
    if (textures.find(name) != textures.end()) glDeleteTextures(1, &textures[name]);
    textures[name] = fullscreenQuad;

    mat->materialMask = 1;
    mat->shaderProgram = 0;

    materialAssets[name] = mat;
    return fullscreenQuad;
}
