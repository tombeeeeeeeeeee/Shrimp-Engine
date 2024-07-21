#include "app.h"

#include "Debug.h"

#pragma region Default Camera Values
int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 1080;
float NEARCLIP = 0.1f;
float FARCLIP = 1000.0f;
float FOV = 70.0f;
#pragma endregion

App* App::main = nullptr;

App::App() 
{ 
    main = this;
    SetUpGLFW();

    if (EditorGUISystem::enabled) editorGUISystem->SetResizeFunctionCallback(SetScreenSize);
    else glfwSetWindowSizeCallback(window, SetScreenSize);
}

App::~App() 
{
    for(unsigned int shader : shaders)
        glDeleteProgram(shader);

    delete physicsSystem;
    delete cameraSystem;
    delete renderSystem;
    if constexpr (EditorGUISystem::enabled) delete editorGUISystem;

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
    editorGUISystem = app.editorGUISystem;

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
    if constexpr (EditorGUISystem::enabled) delete editorGUISystem;

    delete assetFactory;
    delete scene;

    delete cameraComponent;

    shaders = other.shaders;
    physicsSystem = other.physicsSystem;
    cameraSystem = other.cameraSystem;
    renderSystem = other.renderSystem;
    if constexpr (EditorGUISystem::enabled) editorGUISystem = other.editorGUISystem;

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

    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        inputSystem->Update();

        scene->HierarchyUpdate();

        // TODO: Add Delta Time
        shouldClose = cameraSystem->Update(*transforms, cameraID, *cameraComponent, scene, viewMatrix, projectionMatrix, 1.0f / 60.0f);

        ColouredOutput("Render Component count: ", white, false);
        ColouredOutput(renders->size(), red, true);
        std::unordered_map<unsigned int, RenderComponent> culledRenders = cameraSystem->CheckOnFrustum(cameraComponent->frustum, *renders, *transforms);

        ColouredOutput("Rendered Component count: ", white, false);
        ColouredOutput(culledRenders.size(), green, true);

        if constexpr (EditorGUISystem::enabled)
        {
           // TODO:
           //     When drawing editor gui, will need to draw the scene to a colour buffer,
           //     and display it on an editor window using the editor gui system.
           //     Also kind of need a dynamic projection matrix to allow coherent
           //     resizing of the scene view.
           unsigned int renderTexture;
           renderSystem->Update(*transforms, culledRenders, *lights, projectionMatrix, viewMatrix, &renderTexture);
           editorGUISystem->Update(window, scene, renderTexture);
        }
        else
        {
           renderSystem->Update(*transforms, culledRenders, *lights, projectionMatrix, viewMatrix);
        }
        glfwSwapBuffers(window);

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
    glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
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

void App::SetScreenSize(GLFWwindow* window, int width, int height)
{
   // Window parameter is just here to make this function compatible with the GLFW callback.
   // Since there is only ever one window managed by this class we don't care about it.
   SCREEN_WIDTH = width;
   SCREEN_HEIGHT = height;
   main->cameraComponent->aspectRatio = (float)width / (float)height;
   main->renderSystem->HDRBufferUpdate();
   main->renderSystem->OutputBufferUpdate();
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
    
    int lightCount = 20;
    
    //srand(212);
    srand(313);
    for (int i = 3; i < lightCount + 3; i++)
    {
        float z = (10.0f * (float)rand() / RAND_MAX - 5.0f);
        float x = (30.0f * (float)rand() / RAND_MAX - 5.0f);
        float y = (4.0f * (float)rand() / RAND_MAX + 10.0f);
    
        float b = (255 * (float)rand() / RAND_MAX);
        float g = (255 * (float)rand() / RAND_MAX);
        float r = (255 * (float)rand() / RAND_MAX);
    
        scene->MakePointLightEntity({z, y, x }, 100, { r,g,b }, 1 / (float)32);
        //scene->AddPhysicsComponent(i, 1);
        //scene->AddPhysicsShapeBox(i);
        //scene->SetIsGravitated(i, true);
        //scene->SetMomentOfInertiaScale(i, { 0.01f,0.01f,0.01f });
    }
}
 
void App::Update()
{
    if (inputSystem->GetKeyDown(num2)) {
        renderSystem->exposure += 0.02f;
    }
    if (inputSystem->GetKeyDown(num1)) {
        renderSystem->exposure -= 0.02f;
    }
    if (inputSystem->GetKeyDown(G))
    {
        physicsSystem->gravity = { 0,-9.81f,0 };
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

    //WHEN ADDING SHADERS ALSO ADD TO THE ENUM in RenderSystem.h

    //Material Shaders
    shaders.push_back(MakeShader());
    shaders.push_back(MakeShader("src/shaders/vertex.vert", "src/shaders/lightShader.frag"));

    //Post Process Shaders
    shaders.push_back(MakeShaderMatchingName("hdrBloom"));
    shaders.push_back(MakeShaderMatchingName("irradiance"));
    shaders.push_back(MakeShaderMatchingName("prefilter"));
    shaders.push_back(MakeShaderMatchingName("brdf"));
    shaders.push_back(MakeShaderMatchingName("skybox"));
    shaders.push_back(MakeShaderMatchingName("downSample"));
    shaders.push_back(MakeShaderMatchingName("upSample"));

    glUseProgram(shaders[0]);
    
}

void App::MakeSystems() 
{
    physicsSystem = new PhysicsSystem();
    cameraSystem = new CameraSystem(window);
    renderSystem = new RenderSystem(shaders, cameraID, window);
    inputSystem = new InputSystem(window);
    if constexpr (EditorGUISystem::enabled) editorGUISystem = new EditorGUISystem(window);
}

void App::MakeFactories()
{
    assetFactory = new AssetFactory("Assets/");
    scene = new SceneManager(*assetFactory);
}
