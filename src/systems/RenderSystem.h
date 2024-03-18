#pragma once
#include <config.h>
#include "../factories/AssetFactory.h"
#include "../components/TransformComponent.h"
#include "../components/RenderComponent.h"

class RenderSystem {
public:

    RenderSystem(unsigned int shader, GLFWwindow* window);

    void Update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, RenderComponent>& renderComponents
    );

private:
    unsigned int missingTextureTexture;
    unsigned int modelLocation;
    GLFWwindow* window;

    void CreateMissingTexture();
};

