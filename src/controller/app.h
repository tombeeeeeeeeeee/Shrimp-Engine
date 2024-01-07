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
    unsigned int MakeEntity();
    unsigned int MakeCubeMesh(vec3 size);
    unsigned int MakeTexture(const char* filename);
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

    unsigned int entityCount = 0;
    GLFWwindow* window;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    unsigned int shader;

    //Systems
    MotionSystem* motionSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
};