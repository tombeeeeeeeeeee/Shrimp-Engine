#pragma once
#include <config.h>
#include "../factories/AssetFactory.h"
#include "../components/TransformComponent.h"
#include "../components/RenderComponent.h"


enum Shader
{
    defaultPBR = 0,
    light,
    hdrBloom,
    irradiance,
    prefilter,
    brdf,
    skybox,
    downSample,
    upSample,
    count,
};

struct bloomMip
{
    glm::vec2 size;
    glm::ivec2 intSize;
    unsigned int texture;
};

const int bloomMipMapCount = 6;

class RenderSystem {
public:

    RenderSystem(std::vector<unsigned int>& _shaders, unsigned int cameraID, GLFWwindow* window);

    void Start(unsigned int skyboxTexture);

    /// <summary>
    /// Renders each update. 
    /// </summary>
    /// <param name="transformComponents"> All transforms components</param>
    /// <param name="renderComponents"> All Render Components</param>
    void Update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents, 
        std::unordered_map<unsigned int, RenderComponent>& renderComponents,
        std::unordered_map<unsigned int, LightComponent>& lightComponents,
        glm::mat4& _view, glm::mat4& _projection, unsigned int* renderTexture = nullptr
    );

    void SetSkyboxTexture(unsigned int texture);
    unsigned int GetSkyBoxTexture() { return skyboxTexture; };

    void SetIrradianceMap(unsigned int skybox);
    void SetPrefilteredMap(unsigned int skybox);

    void SetCameraID(unsigned int _cameraID) { cameraID = _cameraID; }
    unsigned int GetCameraID() { return cameraID; };

    float exposure = 1.0f;

    void HDRBufferUpdate();
    void OutputBufferUpdate();

private:
    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation;

    glm::mat4 projectionMatrix, viewMatrix;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window;

    /// <summary>
    /// entity id for camera.
    /// </summary>
    unsigned int cameraID;

    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::array<std::vector<unsigned int>, Shader::count> entityShaderOrder;

    /// <summary>
    /// shader programs
    /// </summary>
    std::vector<unsigned int>* shaders;

    void BindLightUniform(unsigned int shaderProgram,
        std::unordered_map<unsigned int, LightComponent>& lightComponents,
        std::unordered_map<unsigned int, TransformComponent>& transComponents);

    void HDRBufferSetUp();
    void OutputBufferSetUp();
    
    unsigned int hdrFBO;
    unsigned int bloomBuffer;
    unsigned int colorBuffer;
    unsigned int rboDepth;

    unsigned int outputFBO = 0;
    unsigned int outputTexture = 0;

    unsigned int captureFBO;
    unsigned int captureRBO;
    unsigned int skyboxTexture;
    unsigned int irradianceMap;
    unsigned int brdfLUTTexture; 
    unsigned int prefilterMap;
    void IBLBufferSetup(unsigned int skybox);

    void BloomSetup();
    unsigned int mFBO;
    std::vector<bloomMip> bloomMips;

    void RenderBloom(unsigned int srcTexture);

    void RenderDownSamples(unsigned int srcTexture);
    void RenderUpSamples(float aspectRatio);

    void RenderQuad();
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    void RenderCube();
    unsigned int cubeVAO = 0;
    unsigned int cubeVBO = 0;



    //Captures for CubeMap
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[6] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };


    /// <summary>
    /// Draw Skybox as fullscreen quad.
    /// </summary>
    void DrawSkyBox();

    /// <summary>
    /// Function to make the texture used when a texture is missing
    /// </summary>
    void CreateMissingTexture();

};

