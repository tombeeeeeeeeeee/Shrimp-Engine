#pragma once
#include <config.h>
#include "../factories/AssetFactory.h"
#include "../components/TransformComponent.h"
#include "../components/RenderComponent.h"

const int SHRIMP_SHADER_PROGRAM_COUNT = 2;

class RenderSystem {
public:

    RenderSystem(std::vector<unsigned int>& _shaders, GLFWwindow* window);

    /// <summary>
    /// Renders each update. 
    /// </summary>
    /// <param name="transformComponents"> All transforms components</param>
    /// <param name="renderComponents"> All Render Components</param>
    void Update(
        std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
        std::unordered_map<unsigned int, RenderComponent*>& renderComponents,
        mat4& _view, mat4& _projection
    );

    void SetSkyboxTexture(unsigned int texture) { skyboxTexture = texture; };
    unsigned int GetSkyBoxTexture() { return skyboxTexture; };

private:
    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation;

    mat4 projectionMatrix, viewMatrix;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window;

    /// <summary>
    /// List of entities in order of the shader they use.
    /// </summary>
    std::array<std::vector<unsigned int>, SHRIMP_SHADER_PROGRAM_COUNT> entityShaderOrder;

    /// <summary>
    /// shader programs
    /// </summary>
    std::vector<unsigned int>* shaders;

    /// <summary>
    /// Function to make the texture used when a texture is missing
    /// </summary>
    void CreateMissingTexture();

    /// <summary>
    /// Draw Skybox as fullscreen quad.
    /// </summary>
    void DrawSkyBox();

    unsigned int skyboxTexture = 0;
};

