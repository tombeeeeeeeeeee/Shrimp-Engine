#pragma once
#include <config.h>
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

    unsigned int modelLocation;
    GLFWwindow* window;
};

