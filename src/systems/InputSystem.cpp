#include "InputSystem.h"

InputSystem* inputSystem = nullptr;

InputSystem::InputSystem(GLFWwindow* window)
{
    downKeys = new std::set<KeyCode>();
    pressedKeys = new std::set<KeyCode>();
    releasedKeys = new std::set<KeyCode>();


    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
}
InputSystem::~InputSystem()
{
    del(downKeys);
    del(pressedKeys);
    del(releasedKeys);
}

bool InputSystem::GetKeyDown(KeyCode keyCode) const noexcept
{
    return downKeys->count(keyCode) > 0;
}
bool InputSystem::GetKeyPressed(KeyCode keyCode) const noexcept
{
    return pressedKeys->count(keyCode) > 0;
}
bool InputSystem::GetKeyReleased(KeyCode keyCode) const noexcept
{
    return releasedKeys->count(keyCode) > 0;
}

void InputSystem::Update()
{
    pressedKeys->clear();
    releasedKeys->clear();

    inputSystem->cursorMovement = glm::vec2(0, 0);
    inputSystem->scrollInput = glm::vec2(0, 0);

    glfwPollEvents();
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        inputSystem->pressedKeys->insert((KeyCode)key);
        inputSystem->downKeys->insert((KeyCode)key);
    }
    else if (action == GLFW_RELEASE)
    {
        inputSystem->releasedKeys->insert((KeyCode)key);
        inputSystem->downKeys->erase((KeyCode)key);
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (!inputSystem->enabled) return;

    if (action == GLFW_PRESS)
    {
        inputSystem->pressedKeys->insert((KeyCode)button);
        inputSystem->downKeys->insert((KeyCode)button);
    }
    else
    {
        inputSystem->releasedKeys->insert((KeyCode)button);
        inputSystem->downKeys->erase((KeyCode)button);
    }
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
    if (!inputSystem->enabled) return;

    inputSystem->scrollInput = glm::vec2(x, y);
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    if (!inputSystem->enabled) return;

    glm::vec2 newPos(x, y);
    inputSystem->cursorMovement = newPos - inputSystem->cursorPos;
    inputSystem->cursorPos = newPos;
}