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

    static App* main;

private:

    void SetUpGLFW();
    static void SetScreenSize(GLFWwindow* window, int width, int height);

    //WorkSpaces
    void Start();
    void Update();
    void End();

    GLFWwindow* window;

    std::vector<unsigned int> shaders;

    //Systems
    PhysicsSystem* physicsSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
    EditorGUISystem* editorGUISystem;

    //Factories
    AssetFactory* assetFactory;
    SceneManager* scene;

    //CameraMatricies
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};