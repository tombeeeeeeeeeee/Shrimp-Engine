#include "CameraSystem.h"

CameraSystem::CameraSystem(GLFWwindow* window)
{
    this->window = window;
    cameraTransform = 0;

    xInput.BindPair(A, D);
    yInput.BindPair(LControl, Space);
    zInput.BindPair(S, W);
}

bool CameraSystem::Update(std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    unsigned int cameraID, CameraComponent& _cameraComponent, SceneManager* scene, glm:: mat4& view, float dt)
{
    //if the camera transform hasn't been set yet, set it. (might just reset each tick)
    if (cameraTransform == nullptr) cameraTransform = &transformComponents[cameraID];
    if (cameraComponent == nullptr) cameraComponent = &_cameraComponent;

    if (inputSystem->GetKeyDown(MouseRight)) RotateCamera();

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
     
    right = glm::normalize(glm::cross(forwards, globalUp));
    up = glm::normalize(cross(right, forwards));

    view = glm::lookAt(pos, pos + forwards, up);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    //Keys
    glm::vec3 dPos = {(float)xInput, 0 , (float)zInput};

    if (glm::length(dPos) > 0.1f) 
    {
        dPos = glm::normalize(dPos);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) dPos *= 2;
        glm::vec3 dForward = 0.1f * dPos.z * glm::vec3(forwards.x, 0, forwards.z);
        glm::vec3 dRight = 0.1f * dPos.x * glm::vec3(right.x, 0, right.z);
        scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + dForward + dRight);
    }

    scene->SetLocalPosition(cameraID, transformComponents[cameraID].position + 0.1f * globalUp * (float)yInput);    

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
    
    dEulers.y = -0.0005f * static_cast<float>(mouse_x - w / 2); 
    dEulers.z = -0.0005f * static_cast<float>(mouse_y - h / 2);

    glm::quat zRot = glm::quat(glm::vec3(0, 0, dEulers.z));
    if(abs(glm::dot(zRot * cameraComponent->forward, globalUp)) < 0.95f)
        cameraTransform->rotation *= zRot;

    glm::quat yRot = glm::rotate(glm::identity<glm::quat>(), dEulers.y, globalUp);
    cameraTransform->rotation = yRot * cameraTransform->rotation;
}

