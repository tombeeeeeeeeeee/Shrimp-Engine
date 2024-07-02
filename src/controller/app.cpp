#include "app.h"

#pragma region Default Camera Values
int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 1080;
float NEARCLIP = 0.1f;
float FARCLIP = 1000.0f;
float FOV = 70.0f;
#pragma endregion

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

    unsigned int cameraEntity;
    cameraComponent = new CameraComponent();
    *cameraComponent = scene->MakeCamera({ 0.0f, 0.0f, 0.0f }, FOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEARCLIP, FARCLIP, cameraEntity);
    cameraID = cameraEntity;
    renderSystem->SetCameraID(cameraID);
    Start();

    std::unordered_map<unsigned int, TransformComponent>* transforms = scene->GetTransforms();
    std::unordered_map<unsigned int, RenderComponent>* renders = scene->GetRenders();
    std::unordered_map<unsigned int, LightComponent>* lights = scene->GetLights();
    std::unordered_map<unsigned int, PhysicsComponent>* bodies = scene->GetPhysics();

    cameraSystem->UpdateFrustum((*transforms)[cameraID], *cameraComponent);
    falseFrustum = cameraComponent->frustum; 

    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        inputSystem->Update();

        scene->HierarchyUpdate();

        // TODO: Add Delta Time
        shouldClose = cameraSystem->Update(*transforms, cameraID, *cameraComponent, scene, viewMatrix, 1.0f / 60.0f);

        ColouredOutput("Number of render components: ", white, false);
        ColouredOutput(renders->size(), red);

        std::unordered_map<unsigned int, RenderComponent> culledRenders = cameraSystem->CheckOnFrustum(falseFrustum, *renders, *transforms);

        ColouredOutput("Number of psuedo culled components: ", white, false);
        ColouredOutput(culledRenders.size(), red);

        std::unordered_map<unsigned int, RenderComponent> doubleCulled = cameraSystem->CheckOnFrustum(cameraComponent->frustum, culledRenders, *transforms);

        ColouredOutput("Number of camera culled components: ", white, false);
        ColouredOutput(doubleCulled.size(), red);

        renderSystem->Update(*transforms, doubleCulled, *lights, projectionMatrix, viewMatrix);

        // TODO: Moved to Fixed Update
        physicsSystem->CollisionPhase(*bodies, *transforms);

        Update();

        // TODO: Moved to Fixed Update
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


    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shrimp Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
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
    "img/py.png",
    "img/ny.png",
    "img/pz.png",
    "img/nz.png",
    };

    //std::string skyboxTextureFiles[6] = {
    //"img/darkness.png",
    //"img/darkness.png",
    //"img/darkness.png",
    //"img/darkness.png",
    //"img/darkness.png",
    //"img/darkness.png",
    //};

    renderSystem->Start(assetFactory->GetSkyBoxMaterial(skyboxTextureFiles));

    unsigned int cubeEntity = scene->MakeEmptyTransform();
    scene->AddRenderComponent(cubeEntity);

    //Gun
    scene->SetMesh(cubeEntity, assetFactory->GetMesh("models/Cerberus_LP.FBX"));
    std::string textureMaps[3] = { "img/Cerberus_A.tga", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    scene->SetScale(2, { 0.15f, 0.15f, 0.15f });

    //Whale
    //scene->SetMesh(cubeEntity, assetFactory->GetMesh("models/whale.obj"));
    //std::string textureMaps[3] = { "img/whale.jpg", "img/Cerberus_PBR.tga", "img/Cerberus_N.tga" };
    scene->SetLocalEulers(2, {-90,0,0}, false);
    //scene->SetScale(2, { 0.05, 0.05, 0.05 });

    scene->AddPhysicsComponent(2, 0);
    scene->SetIsGravitated(2, false);
    scene->AddPhysicsShapePill(2, { 0,21,7.5f }, { 0,-109,7.5f }, 1.7f);
    scene->AddPhysicsShapePill(2, { 4.25f,21,1 }, { 4.25f,-109,1 }, 1.7f);
    scene->AddPhysicsShapePill(2, { -4.25f,21,1 }, { -4.25f,-109,1 }, 1.7f);

    scene->SetMaterial(cubeEntity, assetFactory->GetMaterial(textureMaps, 7));

    int lightCount = 150;

    srand(69420);
    //srand(212);
    for (int i = 3; i < lightCount + 3; i++)
    {
        float z = (30.0f * (float)rand() / RAND_MAX - 15.0F);
        float y = (30.0f * (float)rand() / RAND_MAX - 15.0F);
        float x = (30.0F * (float)rand() / RAND_MAX - 15.0F);

        float b = (255 * (float)rand() / RAND_MAX);
        float g = (255 * (float)rand() / RAND_MAX);
        float r = (255 * (float)rand() / RAND_MAX);

        scene->MakePointLightEntity({z, y, x }, 100, { r,g,b }, 1 / (float)255);
        //scene->AddPhysicsComponent(i, 1);
        //scene->AddPhysicsShapeBox(i);
        //scene->SetIsGravitated(i, true);
        //scene->SetMomentOfInertiaScale(i, { 0.01f,0.01f,0.01f });
    }
}
 
void App::Update()
{
    //Space to add things to run on update

    if (inputSystem->GetKeyDown(num2)) {
        renderSystem->exposure += 0.02f;
    }
    if (inputSystem->GetKeyDown(num1)) {
        renderSystem->exposure -= 0.02f;
    }
    if (inputSystem->GetKeyDown(G))
    {
        physicsSystem->gravity = { 0,0,-5.0f };
    }
    if (inputSystem->GetKeyDown(V))
    {
        falseFrustum = cameraComponent->frustum;
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


    projectionMatrix = glm::perspective(FOV, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, NEARCLIP, FARCLIP);
    
}

void App::MakeSystems() 
{
    physicsSystem = new PhysicsSystem();
    cameraSystem = new CameraSystem(window);
    renderSystem = new RenderSystem(shaders, cameraID, window);
    inputSystem = new InputSystem(window);
}

void App::MakeFactories()
{
    assetFactory = new AssetFactory("Assets/");
    scene = new SceneManager(*assetFactory);
}
