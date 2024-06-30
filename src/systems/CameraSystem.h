#pragma once
#include <config.h>
#include "../components/CameraComponent.h"
#include "../components/TransformComponent.h"
#include "../factories/SceneManager.h"
#include "InputSystem.h"

class CameraSystem {
public:

    CameraSystem(GLFWwindow* window);

    /// <summary>
    /// Initialise the amera System
    /// </summary>
    /// <param name="cameraTransform"> Transform component for camera</param>
    /// <param name="cameraComponent"> Camera component for camera</param>
    void Initialise(TransformComponent& cameraTransform, CameraComponent& cameraComponent);

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
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        unsigned int cameraID, CameraComponent& cameraComponent, 
        SceneManager* scene,
        glm::mat4& _view, float dt);

private:
    void UpdateFrustum(TransformComponent& cameraTransform, CameraComponent& cameraComponent);

    /// <summary>
    /// Rotates camera
    /// </summary>
    void RotateCamera();

    /// <summary>
    /// transform component of camera
    /// </summary>
    TransformComponent* cameraTransform;

    /// <summary>
    /// camera component of camera
    /// </summary>
    CameraComponent* cameraComponent;

    /// <summary>
    /// Location of view matrix in shader (Should stay uniform)
    /// </summary>
    unsigned int viewLocation;

    /// <summary>
    /// Stores up direction (Understood around the system to be y axis is up.)
    /// </summary>
    glm::vec3 globalUp = { 0.0f, 1.0f, 0.0f };
    GLFWwindow* window;

    KeyAxis xInput;
    KeyAxis yInput;
    KeyAxis zInput;
};