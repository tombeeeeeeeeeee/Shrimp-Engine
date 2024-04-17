#pragma once
#include <config.h>

#include "../components/ComponentInclude.h"
#include "../systems/SystemInclude.h"
#include "../view/shader.h"

#include "../factories/AssetFactory.h"
#include "../factories/ComponentFactory.h"

class App {
public:
    App();
    ~App();
    App(App& app);
    void Run();

    void SetUpOpengl();
    void MakeSystems();

    void MakeFactories();

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

    //WorkSpaces
    void Start();
    void Update();
    void End();

    GLFWwindow* window;

    std::vector<unsigned int> shaders;

    //Systems
    MotionSystem* motionSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
    HierarchySystem* hierarchySystem;

    //Factories
    AssetFactory* assetFactory;
    ComponentFactory* componentFactory;

    //CameraMatricies
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);