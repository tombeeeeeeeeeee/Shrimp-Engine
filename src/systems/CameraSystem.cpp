#include "CameraSystem.h"

CameraSystem::CameraSystem(GLFWwindow* window)
{
    this->window = window;
    cameraTransform = 0;
}

bool CameraSystem::Update(std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    unsigned int cameraID, CameraComponent& _cameraComponent, SceneManager* scene, glm:: mat4& view, float dt, unsigned int mouseInputMask)
{
    //if the camera transform hasn't been set yet, set it. (might just reset each tick)
    if (cameraTransform == nullptr) cameraTransform = &transformComponents[cameraID];
    if (cameraComponent == nullptr) cameraComponent = &_cameraComponent;

    if ((mouseInputMask & 2) == 2) RotateCamera();
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glm::vec3 pos = { 
        transformComponents[cameraID].globalTransform[3][0],
        transformComponents[cameraID].globalTransform[3][1],
        transformComponents[cameraID].globalTransform[3][2],
    };
    glm::quat rot = transformComponents[cameraID].rotation;

    glm::vec3& forwards = _cameraComponent.forward;
    glm::vec3& up = _cameraComponent.up;
    glm::vec3& right = _cameraComponent.right;
    
    forwards = rot * glm::vec3(1,0,0);

    //forwards = {
    //    2 * (rot.x * rot.z + rot.w * rot.y),
    //    2 * (rot.y * rot.z - rot.w * rot.x),
    //    1 - 2 * (rot.y * rot.y + rot.x * rot.x)
    //}; 
     
    right = glm::normalize(glm::cross(forwards, globalUp));
    up = glm::normalize(cross(right, forwards));

    view = glm::lookAt(pos, pos + forwards, up);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //Keys
    glm::vec3 dPos = { 0.0f, 0.0f, 0.0f };
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        dPos.x += 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        dPos.y -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        dPos.x -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        dPos.y += 1.0f;
    }

    if (glm::length(dPos) > 0.1f) {
        dPos = glm::normalize(dPos);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dPos *= 2;
        glm::vec3 dForward = 0.1f * dPos.x * forwards;
        glm::vec3 dRight = 0.1f * dPos.y * right;
        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + dForward + dRight);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + glm::vec3(0, 0, 0.1f));
    }                                                                              
                                                                                   
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {                 
        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position - glm::vec3(0, 0, 0.1f));
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }
    glfwPollEvents();


    return false;
}

void CameraSystem::RotateCamera()
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    
    glm::vec3 dEulers = { 0.0f, 0.0f, 0.0f };
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, w / 2, h / 2); 
    
    dEulers.z = -0.0005f * static_cast<float>(mouse_x - w / 2);
    dEulers.y = 0.0005f * static_cast<float>(mouse_y - h / 2);

    glm::vec3 eulerAngles = glm::eulerAngles(cameraTransform->rotation);

    if (abs(eulerAngles.y + dEulers.y) > 89.0f / 180.0f * glm::pi<float>()) 
        dEulers.y = 0;

    eulerAngles += dEulers;

    cameraTransform->rotation = glm::quat(eulerAngles);
}

