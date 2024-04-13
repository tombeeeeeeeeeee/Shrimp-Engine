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
    glDeleteTextures(textures.size(), textures.data());

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

MaterialAsset* AssetFactory::CubeMaterial()
{
    string textureFiles[] = {"img/cubeTexture.jpg","img/cubeNormal.png"};
    return GetMaterial(textureFiles, 5);
}

MeshAsset* AssetFactory::CubeMesh()
{
    if (meshAssets.find("cube") != meshAssets.end()) return meshAssets["cube"];

    float l = 1.0f;
    float w = 1.0f;
    float h = 1.0f;

    std::vector<float> vertices = 
    {
    //Pos,     //Texture Pos, //Normals
     l,  w, -h, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
     l, -w, -h, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    -l, -w, -h, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    -l, -w, -h, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
    -l,  w, -h, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
     l,  w, -h, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f,

    -l, -w,  h, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     l, -w,  h, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
     l,  w,  h, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
     l,  w,  h, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -l,  w,  h, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
    -l, -w,  h, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

    -l,  w,  h, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -l,  w, -h, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -l, -w, -h, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -l, -w, -h, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -l, -w,  h, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -l,  w,  h, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

     l, -w, -h, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     l,  w, -h, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     l,  w,  h, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     l,  w,  h, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     l, -w,  h, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     l, -w, -h, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -l, -w, -h, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     l, -w, -h, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     l, -w,  h, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     l, -w,  h, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -l, -w,  h, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    -l, -w, -h, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

     l,  w,  h, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     l,  w, -h, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -l,  w, -h, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -l,  w, -h, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -l,  w,  h, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     l,  w,  h, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    meshAssets["cube"] = sendMeshToVRAM(vertices, 36);

    return  meshAssets["cube"];
}

MaterialAsset* AssetFactory::RatMaterial()
{
    return GetMaterial("img/me.PNG", 1);
}

MeshAsset* AssetFactory::RatMesh()
{
    return GetMesh("models/rat.obj");
}

unsigned int AssetFactory::MakeTexture(const char* filename)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    
    unsigned char* data = stbi_load( filename, &width, &height, &channels, STBI_rgb_alpha);

    if (data == NULL) return 0;

    //make the texture
    unsigned int texture;
    glGenTextures(1, &texture);
    textures.push_back(texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    //load data
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);

    //texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)12);
    glEnableVertexAttribArray(1);

    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)20);
    glEnableVertexAttribArray(2);

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

    mat->materials[0] = MakeTexture((assetFolder + fileName).c_str());
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
            mat->materials[i] = MakeTexture((assetFolder + fileNames[i]).c_str());
    }
    materialAssets[fileNames[0]] = mat;
    mat->materialMask = fileMask;
    return mat;
}

MeshAsset* AssetFactory::GetMesh(std::string fileName)
{
    if (meshAssets.find(fileName) != meshAssets.end()) return meshAssets[fileName];

    const aiScene* scene = aiImportFile((assetFolder + fileName).c_str(), aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
    if (scene == nullptr) return nullptr;
    
    aiMesh* mesh = scene->mMeshes[0];
    

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
    vertices.reserve(vertexCount * 8);
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
    }

    return  sendMeshToVRAM( vertices, vertexCount, indices.size(), indices.data());
}

unsigned int AssetFactory::GetSkyBoxMaterial(std::string fileName[6])
{
    if(materialAssets.find("skyBox") != materialAssets.end()) return materialAssets["skyBox"]->materials[0];

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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
    textures.push_back(fullscreenQuad);

    mat->materialMask = 1;
    mat->shaderProgram = 0;

    materialAssets["skyBox"] = mat;
    return fullscreenQuad;
}
