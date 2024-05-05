#pragma once
#include <config.h>

#include "../components/ComponentInclude.h"
#include "../systems/SystemInclude.h"
#include "../view/shader.h"

#include "../factories/AssetFactory.h"
#include "../factories/SceneManager.h"

class App {
public:
    App();
    ~App();
    App(App& app);
    App& operator=(App const& other);

    void Run();

    void SetUpOpengl();
    void MakeSystems();

    void MakeFactories();

    //Components
    CameraComponent* cameraComponent;
    unsigned int cameraID;

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

    //Factories
    AssetFactory* assetFactory;
    SceneManager* scene;

    //CameraMatricies
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);