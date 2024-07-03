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

    /// <summary>
    /// Update Camera Frustum
    /// </summary>
    /// <param name="cameraTransform"> transform component for camera</param>
    /// <param name="cameraComponent"> camera component for camera</param>
    void UpdateFrustum(TransformComponent& cameraTransform, CameraComponent& cameraComponent);


    /// <summary>
    /// Create frustum from camera information
    /// </summary>
    /// <param name="pos"> position of frustum tip</param>
    /// <param name="fov"> vertical field of view (degrees)</param>
    /// <param name="aspect"> aspect ratio</param>
    /// <param name="nearClip"> near clip plane distance</param>
    /// <param name="farClip"> far clip plane distance</param>
    /// <param name="up"> frustum up</param>
    /// <param name="forward"> frustum forward</param>
    /// <param name="right"> frustum right</param>
    /// <returns> new frustum</returns>
    Frustum CreateFrustum(glm::vec3 pos, float fov, float aspect, float nearClip, float farClip,
        glm::vec3 up, glm::vec3 forward, glm::vec3 right);

    /// <summary>
    /// Culls render components before sending to gpu.
    /// </summary>
    /// <param name="frustum"> frustum of camera</param>
    /// <param name="renderComponents"> all render components to be culled</param>
    /// <returns> render components in camera frustum</returns>
    std::unordered_map<unsigned int, RenderComponent> CheckOnFrustum(Frustum frustum, 
        std::unordered_map<unsigned int, RenderComponent>& renderComponents, std::unordered_map<unsigned int, TransformComponent>& transformComponents);

private:

    bool IsOnFrustum(Frustum frustum, glm::vec3 OOBB[8]);

    bool IsInfrontOfPlane(Plane plane, glm::vec3 OOBB[8]);

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