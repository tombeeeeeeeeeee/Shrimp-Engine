#include "app.h"

App::App() { SetUpGLFW(); }

App::~App() 
{
    glDeleteProgram(shader);

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;

    glfwTerminate();
}


void App::Run() 
{
    bool shouldClose = false;

    while (!glfwWindowShouldClose(window) && !shouldClose) 
    {
        motionSystem->Update( transformComponents, physicsComponents, 1.0f / 60.0f);
        shouldClose = cameraSystem->Update(transformComponents, cameraID, *cameraComponent, 1.0f / 60.0f, mouseInput);

        renderSystem->Update(transformComponents, renderComponents);
    }
}

void App::SetUpGLFW() 
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

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

    shader = MakeShader();

    glUseProgram(shader);

    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    mat4 projection = ProjectionMatrix( 45.0f, (float)w/(float)h , 0.1f, 250.0f);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, projection.entries);
}

void App::MakeSystems() 
{
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
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
    else if(action == 1 && (mouseMask & buttonBinary) == 0)
    {
        app->mouseInput += buttonBinary;
    }
}
