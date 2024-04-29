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

    delete assetFactory;
    delete scene;

    delete cameraComponent;

    glfwTerminate();
}

App::App(App& app)
{
    shaders = app.shaders;
    motionSystem = app.motionSystem;
    cameraSystem = app.cameraSystem;
    renderSystem = app.renderSystem;

    assetFactory = app.assetFactory;
    scene = app.scene;

    cameraID = app.cameraID;
    cameraComponent = app.cameraComponent;
    window = app.window;
}

App& App::operator=(App const& other)
{
    for (unsigned int shader : shaders)
        glDeleteProgram(shader);

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;

    delete assetFactory;
    delete scene;

    delete cameraComponent;

    shaders = other.shaders;
    motionSystem = other.motionSystem;
    cameraSystem = other.cameraSystem;
    renderSystem = other.renderSystem;

    assetFactory = other.assetFactory;
    scene = other.scene;

    cameraID = other.cameraID;
    cameraComponent = other.cameraComponent;
    window = other.window;

    return *this;
}


void App::Run() 
{
    bool shouldClose = false;

    unsigned int cameraEntity = scene->MakeCamera({ 0.0f, 1.0f, 0.0f }, { 0.0f, .0f,0.0f });

    CameraComponent* camera = new CameraComponent();
    cameraComponent = camera;
    cameraID = cameraEntity;
    renderSystem->SetCameraID(cameraID);

    std::string skyboxTextureFiles[6] = {
    "img/px.png",
    "img/nx.png",
    "img/nz.png",
    "img/pz.png",
    "img/py.png",
    "img/ny.png",
    };

    renderSystem->Start(assetFactory->GetSkyBoxMaterial(skyboxTextureFiles));

    Start();

    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        std::unordered_map<unsigned int, TransformComponent>* transforms = scene->GetTransforms();
        std::unordered_map<unsigned int, RenderComponent>* renders = scene->GetRenders();
        std::unordered_map<unsigned int, LightComponent>* lights = scene->GetLights();
        scene->HierarchyUpdate();

        shouldClose = cameraSystem->Update(*transforms, cameraID, *cameraComponent, scene, viewMatrix, 1.0f / 60.0f, mouseInput);

        renderSystem->Update(*transforms, *renders, *lights, projectionMatrix, viewMatrix);

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
    unsigned int cubeEntity = scene->MakeEmptyTransform({ 0,0,0 }, { 0, 0, 0 });
    scene->AddRenderComponent(cubeEntity);
    scene->SetMesh(cubeEntity, assetFactory->GetMesh("models/Cerberus_LP.FBX"));
    std::string textureMaps[3] = { "img/Cerberus_A.tga", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    scene->SetScale(2, { 0.25, 0.25, 0.25 });

    //renderComponents[cubeEntity]->mesh = assetFactory->GetMesh("models/whale.obj");
    //std::string textureMaps[3] = { "img/whale.jpg", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    //transformComponents[2]->scale = { 0.05, 0.05, 0.05 };

    scene->SetMaterial(cubeEntity, assetFactory->GetMaterial(textureMaps, 7));

    scene->MakeAmbientLightEntity({0.8,0.8,0.8}, 0.001);
    scene->MakePointLightEntity(scene->GetPosition(2), 20, { 0.5,0.5,0.5 }, 2);
    scene->MakePointLightEntity({3,-5,12}, 200, { 255,192,20 }, 1/(float)255);
    scene->MakePointLightEntity({-12,-3,-2}, 200, { 50, 150, 255 }, 1/(float)255);
}

void App::Update()
{
    //Space to add things to run on update
    scene->SetLocalPosition(4, { 10 * (float)cos(glfwGetTime() * 2), -30 * (float)sin(glfwGetTime() * 0.05), 5 * (float)sin(glfwGetTime() * 2) });

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        renderSystem->exposure += 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        renderSystem->exposure -= 0.02f;
    }
}

void App::End()
{
    //space to add things for the end
}

void App::SetUpOpengl() 
{
    //glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    //Set the rendering region to the actual screen size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    //(left, top, width, height)
    glViewport(0, 0, w, h);

    //Material Shaders
    shaders.push_back(MakeShader());
    shaders.push_back(MakeShader("src/shaders/vertex.vert", "src/shaders/lightShader.frag"));
    //shaders.push_back(MakeShaderMatchingName("blinn"));

    //Post Process Shaders
    shaders.push_back(MakeShaderMatchingName("hdr"));
    shaders.push_back(MakeShaderMatchingName("irradiance"));
    shaders.push_back(MakeShaderMatchingName("prefilter"));
    shaders.push_back(MakeShaderMatchingName("brdf"));
    shaders.push_back(MakeShaderMatchingName("skybox"));

    glUseProgram(shaders[0]);

    projectionMatrix = ProjectionMatrix( 45.0f, (float)w/(float)h , 0.1f, 1000.0f);
    
}

void App::MakeSystems() 
{
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(window);
    renderSystem = new RenderSystem(shaders, cameraID, window);
}

void App::MakeFactories()
{
    assetFactory = new AssetFactory("Assets/");
    scene = new SceneManager(*assetFactory);
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
