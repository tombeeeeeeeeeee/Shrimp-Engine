#include "CameraSystem.h"

CameraSystem::CameraSystem(unsigned int shader, GLFWwindow* window)
{
    this->window = window;

    glUseProgram(shader);
    viewLocation = glGetUniformLocation(shader, "view");
}

bool CameraSystem::Update(std::unordered_map<unsigned int, TransformComponent*>& transformComponents,
    unsigned int cameraID, CameraComponent& cameraComponent, float dt, unsigned int mouseInputMask) 
{
    if (cameraTransform == nullptr) cameraTransform = transformComponents[cameraID];

    if ((mouseInputMask & 2) == 2) RotateCamera();
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    vec3 pos = transformComponents[cameraID]->position();
    vec3 eulers = transformComponents[cameraID]->eulers();
   
    float theta = eulers.z;
    float phi = eulers.y;

    vec3& forwards = cameraComponent.forward;
    vec3& up = cameraComponent.up;
    vec3& right = cameraComponent.right;

    forwards = {
        -glm::cos(theta) * glm::cos(phi),
        -glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    }; 
     
    right = GetNormalised(cross(forwards, globalUp));
    up = GetNormalised(cross(right, forwards));

    mat4 view = ViewMatrix(pos, pos + forwards, up);

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view.entries);

    //Keys
    vec3 dPos = { 0.0f, 0.0f, 0.0f };
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

    if (dPos.magnitude() > 0.1f) {
        dPos = GetNormalised(dPos);
        vec3 dForward = 0.1f * dPos.x * forwards;
        vec3 dRight = 0.1f * dPos.y * right;
        vec3 position = transformComponents[cameraID]->localPosition() + dForward + dRight;
        transformComponents[cameraID]->localTransform = SetPosition(transformComponents[cameraID]->localTransform, position);
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

    vec3 dEulers = { 0.0f, 0.0f, 0.0f };
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, w/2, h/2);

    dEulers.z = -0.01f * static_cast<float>(mouse_x - w/2);
    dEulers.y = -0.01f * static_cast<float>(mouse_y - h/2);

    vec3 position = cameraTransform->position();
     
    cameraTransform->localTransform *= RotationYMatrix(dEulers.y);
    cameraTransform->localTransform *= RotationZMatrix(dEulers.z);
}

