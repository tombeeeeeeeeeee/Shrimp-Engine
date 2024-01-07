#pragma once
#include <config.h>
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"

class CameraSystem {
public:

    CameraSystem(unsigned int shader, GLFWwindow* window);

    bool Update(
        unordered_map<unsigned int, TransformComponent>& transformComponents,
        unsigned int cameraID, CameraComponent& cameraComponent, float dt);

private:
    unsigned int viewLocation;
    vec3 globalUp = { 0.0f, 1.0f, 0.0f };
    GLFWwindow* window;
};