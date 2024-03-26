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

    vec3 pos = transformComponents[cameraID]->position();
    vec3 eulers = transformComponents[cameraID]->eulers();
   
    float theta = eulers.z;
    float phi = eulers.y;

    vec3& forwards = cameraComponent.forward;
    vec3& up = cameraComponent.up;
    vec3& right = cameraComponent.right;

    //cout << forwards << endl;

    forwards = {
        glm::sin(theta) * glm::cos(phi),
        glm::cos(theta) * glm::cos(phi),
        glm::sin(phi)
    };
   
     
    right = GetNormal(cross(forwards, globalUp));
    up = GetNormal(cross(right, forwards));

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

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        transformComponents[cameraID]->globalTransform *= rotationZAxisMatrix(180);
        cout << forwards << endl;
    }


    if (dPos.magnitude() > 0.1f) {
        dPos = GetNormal(dPos);
        transformComponents[cameraID]->globalTransform *= TranslationMatrix(0.1f * dPos.x * forwards);
        transformComponents[cameraID]->globalTransform *= TranslationMatrix(0.1f * dPos.y * right);
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }
    glfwPollEvents();

    if ((mouseInputMask & 2) == 2) RotateCamera();
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

    cameraTransform->globalTransform *= rotationYAxisMatrix(fminf(89.0f, fmaxf(-89.0f, dEulers.y)));
    cameraTransform->globalTransform *= rotationZAxisMatrix(-dEulers.z);

}

