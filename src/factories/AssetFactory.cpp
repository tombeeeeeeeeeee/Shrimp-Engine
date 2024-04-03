#include "AssetFactory.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif 
#include "../stb_image.h"

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

MeshAsset* AssetFactory::MakeObjMesh(const char* filepath, mat4 preTransform)
{
    //Vectors to store .obj model info
    std::vector<vec3> v; 
    std::vector<vec2> vt; 
    std::vector<vec3> vn;
    std::vector<float> vertices;

    //number of attributes in each vector for resizing to avoid dynamic memory allocation
    size_t vertexCount = 0; 
    size_t texCoordCount = 0; 
    size_t vertexNormalCount = 0;
    size_t triCount = 0;

    //First run through to get count
    string line; std::ifstream model; std::vector<string> data;
    model.open(filepath);
    if (!model.is_open()) std::cout << "Failed to open: " << filepath << std::endl;
    while (getline(model, line))
    {
        data = StringSplit(line, " ");
        if (data.size() != 0)
        {
            if (data[0] == "v") vertexCount++;
            else if (data[0] == "vt") texCoordCount++;
            else if (data[0] == "vn") vertexNormalCount++;
            else if (data[0] == "f") triCount = data.size() - 3;
        }
     
    } model.close();

    //vector resizing to avoid dynamic memory allocation
    v.reserve(vertexCount);
    vt.reserve(texCoordCount);
    vn.reserve(vertexNormalCount);
    vertices.reserve(triCount * 3 * 8);


    //second run through to populate model data
    model.open(filepath);
    while (getline(model, line))
    {
        data = StringSplit(line, " ");

        if (data.size() != 0)
        {
            if (!data[0].compare("v")) v.push_back(readVec3(data, preTransform, 1));
            else if (!data[0].compare("vt")) vt.push_back(readVec2(data));
            else if (!data[0].compare("vn")) vn.push_back(readVec3(data, preTransform, 0));
            else if (!data[0].compare("f"))
                readFace(data, v, vt, vn, vertices);
        }
    } model.close();

    //turn populated data into usable information
   return sendMeshToVRAM(vertices, vertices.size() / 8);
}

MeshAsset* AssetFactory::MakeFbxMesh(const char* filepath, mat4 preTransform)
{
    return nullptr;
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

    //unsigned int IBO;
    //if (indexCount > 0)
    //{
    //    glGenBuffers(1, &IBO);
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    //
    //}


    MeshAsset* mesh = new MeshAsset();
    mesh->VAO = VAO;
    mesh->vertexCount = vertexCount;
    return mesh;
}

vec3 AssetFactory::readVec3(std::vector<std::string> strings, mat4 preTransform, float w)
{
    return (preTransform * vec4(stof(strings[1]), stof(strings[2]), stof(strings[3]), w)).xyz();
}

vec3 AssetFactory::readVec3(std::vector<std::string> strings)
{
    return vec3(stof(strings[1]), stof(strings[2]), stof(strings[3]));
}

vec2 AssetFactory::readVec2(std::vector<std::string> strings)
{
    return vec2(stof(strings[1]), stof(strings[2]));
}

void AssetFactory::readFace(std::vector<string>& data, std::vector<vec3>& v, std::vector<vec2>& vt, std::vector<vec3>& vn, std::vector<float>& vertices)
{
    size_t triCount = data.size() - 3;
    if (vn.size() == 0)
    {
        //for each triangle
        for (int i = 0; i < triCount; i++)
        {
            vec3 a = readTriCornerVertex(data[1], v);
            vec3 b = readTriCornerVertex(data[2 + i], v);
            vec3 c = readTriCornerVertex(data[3 + i], v);

            vec3 GetNormalised = cross(b - a, c - a);
            GetNormalised = GetNormalised.GetNormalised();

            readTriCorner(data[1], v, vt, GetNormalised,  vertices);
            readTriCorner(data[2 + i], v, vt, GetNormalised, vertices);
            readTriCorner(data[3 + i], v, vt, GetNormalised, vertices);
        }
    }
    else
    {
        //for each triangle
        for (int i = 0; i < triCount; i++)
        {
            readTriCorner(data[1], v, vt, vn, vertices);
            readTriCorner(data[2 + i], v, vt, vn, vertices);
            readTriCorner(data[3 + i], v, vt, vn, vertices);
        }
    }
}

vec3 AssetFactory::readTriCornerVertex(string& data, std::vector<vec3>& v)
{
    std::vector<string> cornerData = StringSplit(data, "/");

    return v[stoll(cornerData[0], 0) - 1];
}

void AssetFactory::readTriCorner(string& data, std::vector<vec3>& v, std::vector<vec2>& vt, std::vector<vec3>& vn, std::vector<float>& vertices)
{
    std::vector<string> cornerData = StringSplit(data, "/");

    vec3 vertexPos = v[stoll(cornerData[0], 0) - 1];
    vertices.push_back(vertexPos.x);
    vertices.push_back(vertexPos.y);
    vertices.push_back(vertexPos.z);

    vec2 texturePos = vt[stoll(cornerData[1], 0) - 1];
    vertices.push_back(texturePos.x);
    vertices.push_back(texturePos.y);

    vec3 normals = vn[stoll(cornerData[2], 0) - 1];
    vertices.push_back(normals.x);
    vertices.push_back(normals.y);
    vertices.push_back(normals.z);
}

void AssetFactory::readTriCorner(string& data, std::vector<vec3>& v, std::vector<vec2>& vt, vec3 GetNormalised, std::vector<float>& vertices)
{
    std::vector<string> cornerData = StringSplit(data, "/");

    vec3 vertexPos = v[stol(cornerData[0]) - 1];
    vertices.push_back(vertexPos.x);
    vertices.push_back(vertexPos.y);
    vertices.push_back(vertexPos.z);

    vec2 texturePos = vt[stol(cornerData[1]) - 1];
    vertices.push_back(texturePos.x);
    vertices.push_back(texturePos.y);

    vertices.push_back(GetNormalised.x);
    vertices.push_back(GetNormalised.y);
    vertices.push_back(GetNormalised.z);
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

    //return MakeObjMesh((assetFolder + fileName).c_str(), mat4());

    const aiScene* scene = aiImportFile((assetFolder + fileName).c_str(), 0);
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
