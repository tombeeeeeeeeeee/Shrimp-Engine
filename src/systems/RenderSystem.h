#pragma once
#include <config.h>
#include "../factories/AssetFactory.h"
#include "../components/TransformComponent.h"
#include "../components/RenderComponent.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);

    /// <summary>
    /// Renders each update. 
    /// </summary>
    /// <param name="transformComponents"> All transforms components</param>
    /// <param name="renderComponents"> All Render Components</param>
    void Update(
        std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
        std::unordered_map<unsigned int, RenderComponent*>& renderComponents
    );

private:
    /// <summary>
    /// Missing Texture VRAM location
    /// </summary>
    unsigned int missingTextureTexture;

    /// <summary>
    /// Location of the Model Matrix 
    /// </summary>
    unsigned int modelLocation;

    /// <summary>
    /// Pointer to current window
    /// </summary>
    GLFWwindow* window;

    /// <summary>
    /// Function to make the texture used when a texture is missing
    /// </summary>
    void CreateMissingTexture();
};

