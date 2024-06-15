#include "app.h"

App::App() 
{ 
    SetUpGLFW();
}

App::~App() 
{
    for(unsigned int shader : shaders)
        glDeleteProgram(shader);

    delete physicsSystem;
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
    physicsSystem = app.physicsSystem;
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

    delete physicsSystem;
    delete cameraSystem;
    delete renderSystem;

    delete assetFactory;
    delete scene;

    delete cameraComponent;

    shaders = other.shaders;
    physicsSystem = other.physicsSystem;
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

    unsigned int cameraEntity = scene->MakeCamera({ 0.0f, 1.0f, 0.0f });

    CameraComponent* camera = new CameraComponent();
    cameraComponent = camera;
    cameraID = cameraEntity;
    renderSystem->SetCameraID(cameraID);

    Start();


    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        std::unordered_map<unsigned int, TransformComponent>* transforms = scene->GetTransforms();
        std::unordered_map<unsigned int, RenderComponent>* renders = scene->GetRenders();
        std::unordered_map<unsigned int, LightComponent>* lights = scene->GetLights();
        std::unordered_map<unsigned int, PhysicsComponent>* bodies = scene->GetPhysics();

        scene->HierarchyUpdate();

        shouldClose = cameraSystem->Update(*transforms, cameraID, *cameraComponent, scene, viewMatrix, 1.0f / 60.0f, mouseInput);

        renderSystem->Update(*transforms, *renders, *lights, projectionMatrix, viewMatrix);

        physicsSystem->CollisionPhase(*bodies, *transforms);

        Update();

        physicsSystem->IntegrationStep(*bodies, *transforms, scene, 1.0f/60.0f);
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

    //std::string skyboxTextureFiles[6] = {
    //"img/px.png",
    //"img/nx.png",
    //"img/nz.png",
    //"img/pz.png",
    //"img/py.png",
    //"img/ny.png",
    //};

    std::string skyboxTextureFiles[6] = {
    "img/darkness.png",
    "img/darkness.png",
    "img/darkness.png",
    "img/darkness.png",
    "img/darkness.png",
    "img/darkness.png",
    };

    renderSystem->Start(assetFactory->GetSkyBoxMaterial(skyboxTextureFiles));

    unsigned int cubeEntity = scene->MakeEmptyTransform();
    scene->AddRenderComponent(cubeEntity);
    scene->SetMesh(cubeEntity, assetFactory->GetMesh("models/Cerberus_LP.FBX"));
    std::string textureMaps[3] = { "img/Cerberus_A.tga", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    scene->SetScale(2, { 0.15f, 0.15f, 0.15f });
    scene->AddPhysicsComponent(2, 0);
    scene->SetIsGravitated(2, false);
    scene->AddPhysicsShapePill(2, { 0,21,7.5f }, { 0,-109,7.5f }, 2.25f);
    scene->AddPhysicsShapePill(2, { 4.25f,21,1 }, { 4.25f,-109,1 }, 2.25f);
    scene->AddPhysicsShapePill(2, { -4.25f,21,1 }, { -4.25f,-109,1 }, 2.25f);

    //renderComponents[cubeEntity]->mesh = assetFactory->GetMesh("models/whale.obj");
    //std::string textureMaps[3] = { "img/whale.jpg", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    //transformComponents[2]->scale = { 0.05, 0.05, 0.05 };

    scene->SetMaterial(cubeEntity, assetFactory->GetMaterial(textureMaps, 7));

    scene->MakeAmbientLightEntity({ 0.8f,0.8f,0.8f }, 0.0f);
    scene->MakePointLightEntity({ 0,0,0 }, 20, { 0.5f,0.5f,0.5f }, 2);
    int lightCount = 25;
    srand(2222);
    for (int i = 5; i < lightCount; i++)
    {
        float z = (20.0f * (float)rand() / RAND_MAX - 10.0f);
        float y = (110.0f * (float)rand() / RAND_MAX - 90.0f);
        float x = (9.0f * (float)rand() / RAND_MAX + 30.0f);

        float b = (255 * (float)rand() / RAND_MAX);
        float g = (255 * (float)rand() / RAND_MAX);
        float r = (255 * (float)rand() / RAND_MAX);

        scene->MakePointLightEntity({z, y, x }, 20, { r,g,b }, 1 / (float)255);
        if (i % 5 == 0 || i % 5 == 1)
        {
            scene->AddPhysicsComponent(i, 1);
            scene->AddPhysicsShapeBox(i);
            scene->SetIsGravitated(i, true);
            scene->SetMomentOfInertiaScale(i, { 0.1,0.1,0.1 });
        }
    }

    scene->MakePhysicsEntity();
    scene->SetLocalPosition(lightCount, { 0,0,-15 });
    scene->SetScale(lightCount, { 2000,2000,1 });
    scene->AddPhysicsShapeBox(lightCount);
    scene->SetIsGravitated(lightCount, false);
    scene->SetInverseMass(lightCount, 0);
}

void App::Update()
{
    //Space to add things to run on update
    //scene->SetLocalPosition(4, { 10 * (float)cos(glfwGetTime() * 2), -30 * (float)sin(glfwGetTime() * 0.05f), 5 * (float)sin(glfwGetTime() * 2) });

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        renderSystem->exposure += 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        renderSystem->exposure -= 0.02f;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        physicsSystem->gravity = { 0,0,-9.8f };
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

    //Post Process Shaders
    shaders.push_back(MakeShaderMatchingName("hdr"));
    shaders.push_back(MakeShaderMatchingName("irradiance"));
    shaders.push_back(MakeShaderMatchingName("prefilter"));
    shaders.push_back(MakeShaderMatchingName("brdf"));
    shaders.push_back(MakeShaderMatchingName("skybox"));

    glUseProgram(shaders[0]);


    projectionMatrix = glm::perspective( 45.0f, (float)w/(float)h , 0.1f, 1000.0f);
    
}

void App::MakeSystems() 
{
    physicsSystem = new PhysicsSystem();
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
    int buttonBinary = (int)pow(2, button);
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
