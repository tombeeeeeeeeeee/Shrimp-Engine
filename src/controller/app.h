#pragma once
#include <config.h>

#include "../components/ComponentInclude.h"
#include "../systems/SystemInclude.h"
#include "../view/shader.h"

class App {
public:
    App();
    ~App();
    void Run();

    void SetUpOpengl();
    void MakeSystems();

    //Components
    std::unordered_map<unsigned int, TransformComponent*> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent*> physicsComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, RenderComponent*> renderComponents;

    //Inputs
    unsigned int mouseInput = 0;

private:
    void SetUpGLFW();

    GLFWwindow* window;

    unsigned int shader;

    //Systems
    MotionSystem* motionSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
};

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);