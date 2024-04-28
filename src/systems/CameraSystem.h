#pragma once
#include <config.h>
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"

class CameraSystem {
public:

    CameraSystem(GLFWwindow* window);

    /// <summary>
    /// Camera Update
    /// </summary>
    /// <param name="transformComponents"> All transform components</param>
    /// <param name="cameraID">ID for camera entity</param>
    /// <param name="cameraComponent"> component for the camera</param>
    /// <param name="dt"> Delta Time</param>
    /// <param name="mouseInputMask"> binary mask for mouse inputs</param>
    /// <returns></returns>
    bool Update(
        const std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
        unsigned int cameraID, CameraComponent& cameraComponent, mat4& _view, float dt,
        unsigned int mouseInputMask);

private:
    /// <summary>
    /// Rotates camera
    /// </summary>
    void RotateCamera();

    /// <summary>
    /// transform component of camera
    /// </summary>
    TransformComponent* cameraTransform;

    /// <summary>
    /// Location of view matrix in shader (Should stay uniform)
    /// </summary>
    unsigned int viewLocation;

    /// <summary>
    /// Stores up direction (Understood around the system to be z axis is up.)
    /// </summary>
    vec3 globalUp = { 0.0f, 0.0f, 1.0f };
    GLFWwindow* window;
};