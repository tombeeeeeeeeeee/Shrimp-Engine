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
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, RenderComponent> renderComponents;

private:
    void SetUpGLFW();

    GLFWwindow* window;

    unsigned int shader;

    //Systems
    MotionSystem* motionSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
};