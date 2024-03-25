#include "CameraSystem.h"

CameraSystem::CameraSystem(unsigned int shader, GLFWwindow* window)
{
    this->window = window;

    glUseProgram(shader);
    viewLocation = glGetUniformLocation(shader, "view");
}

bool CameraSystem::Update(std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    unsigned int cameraID, CameraComponent& cameraComponent, float dt) 
{

    vec3 pos = transformComponents[cameraID].position();
    vec3 eulers = transformComponents[cameraID].eulers();
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);

    vec3& right = cameraComponent.right;
    vec3& up = cameraComponent.up;
    vec3& forwards = cameraComponent.forward;

    forwards = {
        glm::cos(theta) * glm::cos(phi),
        glm::sin(theta) * glm::cos(phi),
        glm::sin(phi)
    };
    right = normalize(cross(forwards, globalUp));
    up = normalize(cross(right, forwards));

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
        dPos = normalize(dPos);
        transformComponents[cameraID].globalTransform *= createTranslationMatrix(0.1f * dPos.x * forwards);
        transformComponents[cameraID].globalTransform *= createTranslationMatrix(0.1f * dPos.y * right);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }

    //Mouse
    vec3 dEulers = { 0.0f, 0.0f, 0.0f };
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetCursorPos(window, 320.0, 240.0);
    glfwPollEvents();

    dEulers.z = -0.01f * static_cast<float>(mouse_x - 320.0);
    dEulers.y = -0.01f * static_cast<float>(mouse_y - 240.0);

    transformComponents[cameraID].globalTransform *= rotationYAxisMatrix(fminf(89.0f, fmaxf(-89.0f,dEulers.y)));
    transformComponents[cameraID].globalTransform *= rotationZAxisMatrix(dEulers.z);

    eulers.z += dEulers.z;
    if (eulers.z > 360) {
        eulers.z -= 360;
    }
    else if (eulers.z < 0) {
        eulers.z += 360;
    }

    return false;
}