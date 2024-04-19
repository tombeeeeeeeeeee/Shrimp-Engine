#include "app.h"

App::App() 
{ 
    SetUpGLFW();
}

App::~App() 
{
    for(unsigned int shader : shaders)
        glDeleteProgram(shader);

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;
    delete hierarchySystem;

    delete assetFactory;
    delete componentFactory;

    delete cameraComponent;

    glfwTerminate();
}

App::App(App& app)
{
    shaders = app.shaders;
    motionSystem = app.motionSystem;
    cameraSystem = app.cameraSystem;
    renderSystem = app.renderSystem;
    hierarchySystem = app.hierarchySystem;

    assetFactory = app.assetFactory;
    componentFactory = app.componentFactory;

    cameraID = app.cameraID;
    cameraComponent = app.cameraComponent;
    window = app.window;
}


void App::Run() 
{
    bool shouldClose = false;

    Start();

    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        motionSystem->Update( transformComponents, physicsComponents, 1.0f / 60.0f);
        hierarchySystem->Update();
        shouldClose = cameraSystem->Update(transformComponents, cameraID, *cameraComponent, viewMatrix, 1.0f / 60.0f, mouseInput);

        renderSystem->Update(transformComponents, renderComponents, lightComponents, projectionMatrix, viewMatrix);

        Update();
    }

    End();
}

void App::SetUpGLFW() 
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 16); //MSAA


    window = glfwCreateWindow(1920, 1080, "Shrimp Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        std::cout << "Couldn't load opengl" << std::endl;
        glfwTerminate();
    }
}
 
void App::Start()
{
    //Space to add things for the start
    std::string skyboxTextureFiles[6] = { 
        "img/px.png", 
        "img/nx.png", 
        "img/nz.png", 
        "img/pz.png", 
        "img/py.png", 
        "img/ny.png", 
    };
    renderSystem->SetSkyboxTexture(assetFactory->GetSkyBoxMaterial(skyboxTextureFiles));
    std::vector<unsigned int> gameObjects;
    int objectCount = 1;
    srand(time(NULL));
    for (int i = 0; i < objectCount; i++)
    {
        float x = (10.0f * (float)rand() / RAND_MAX) - 5.0f;
        float y = (10.0f * (float)rand() / RAND_MAX) - 5.0f;
        float z = (10.0f * (float)rand() / RAND_MAX) - 5.0f;

        float xRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
        float yRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
        float zRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;

        unsigned int cubeEntity = componentFactory->MakeEmptyTransform({ x, y, z }, { xRot, yRot, zRot });
        componentFactory->AddRenderComponent(cubeEntity);
        renderComponents[cubeEntity]->mesh = assetFactory->GetMesh("models/whale.obj");
        std::string textureMaps[3] = { "img/whale.jpg", "img/vignette.jpg", "img/cubeNormal.png" };
        renderComponents[cubeEntity]->material = assetFactory->GetMaterial(textureMaps, 7);
        gameObjects.push_back(cubeEntity);
        hierarchySystem->SetParent(cubeEntity, cubeEntity - 1);
    }
    transformComponents[1]->scale = { 0.05, 0.05, 0.05 };

    unsigned int cameraEntity = componentFactory->MakeCamera({ 0.0f, 1.0f, 0.0f }, { 0.0f, .0f,0.0f });

    CameraComponent* camera = new CameraComponent();
    cameraComponent = camera;
    cameraID = cameraEntity;
    renderSystem->SetCameraID(cameraID);
}

void App::Update()
{
    //Space to add things to run on update
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        renderComponents[1]->material->shaderProgram = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        renderComponents[1]->material->shaderProgram = 1;
    }
}

void App::End()
{
    //space to add things for the end
}

void App::SetUpOpengl() 
{
    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    //Set the rendering region to the actual screen size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    //(left, top, width, height)
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);


    //Default Shader Program
    shaders.push_back(MakeShader());
    shaders.push_back(MakeShaderMatchingName("phong"));

    shaders.push_back(MakeShaderMatchingName("skybox"));

    glUseProgram(shaders[0]);

    projectionMatrix = ProjectionMatrix( 45.0f, (float)w/(float)h , 0.1f, 250.0f);
    
}

void App::MakeSystems() 
{
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(window);
    renderSystem = new RenderSystem(shaders, cameraID, window);
    hierarchySystem = new HierarchySystem(transformComponents);
}

void App::MakeFactories()
{
    assetFactory = new AssetFactory("Assets/");
    componentFactory = new ComponentFactory(lightComponents, physicsComponents, renderComponents, transformComponents, *assetFactory);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    int buttonBinary = pow(2, button);
    App* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
    unsigned int mouseMask = app->mouseInput;

    if (action == 0 && (mouseMask & buttonBinary) == buttonBinary)
    {
        app->mouseInput -= buttonBinary;
    }
    else if (action == 1 && (mouseMask & buttonBinary) == 0)
    {
        app->mouseInput += buttonBinary;
    }
}
