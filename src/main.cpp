
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "config.h"
#include "triangle_mesh.h"
#include "Material.h"

unsigned int makeShader(string vertexPath, string fragmentPath);
unsigned int makeModule(string filepath, unsigned int moduleType);

int main(void)
{
	GLFWwindow* window;

	//Initialise GLFW, make sure it works. Put an error message here if you like.
	if (!glfwInit())
	{
		ColouredOutput("Failed to Initialise OpenGL.\n", Colour::red);
		return -1;
	}
	//Set resolution here, and give your window a different title.
	window = glfwCreateWindow(1280, 720, "Testing Window!", nullptr, nullptr);
	if (!window)
	{
		ColouredOutput("Failed to create window.\n", Colour::red);
		glfwTerminate(); //Again, you can put a real error message here.
		return -1;
	}

	//This tells GLFW that the window we created is the one we should render to.
	glfwMakeContextCurrent(window);

	//Tell GLAD to load all its OpenGL functions.
	if (!gladLoadGL())
	{
		ColouredOutput("Failed to load OpenGL functions.\n", Colour::red);
		return -1;
	}

	ColouredOutput("Opening Window", Colour::green);

	unsigned int shader = makeShader(
		"src/shaders/vertex.txt",
		"src/shaders/fragment.txt"
	);

	//Window size capture ints
	int w,h;

	//Triangles for testing
	TriangleMesh* triangle = new TriangleMesh();
	Material* material = new Material("src/img/me.PNG");
	Material* mask = new Material("src/img/vignette.png");

	//Set texture units
	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "material"), 0);
	glUniform1i(glGetUniformLocation(shader, "mask"), 1);

	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//The main ‘game’ loop.
	while (!glfwWindowShouldClose(window))
	{
		//Tell GLFW to check if anything is going on with input, etc.
		glfwPollEvents();

		//Clear the screen – eventually do rendering code here.
		glClear(GL_COLOR_BUFFER_BIT);

		//Adaptive Screensize for window dragging
		glfwGetFramebufferSize(window, &w, &h);
		glViewport(0, 0, w, h);

		//Use shaders to draw triangle
		glUseProgram(shader);

		material->use(0);
		mask->use(1);

		triangle->draw();

		//Swapping the buffers – this means this frame is over.
		glfwSwapBuffers(window);

	}

	//If we get to this point, the window has closed, so clean up GLFW and exit.
	glDeleteProgram(shader);

	delete triangle;
	delete material;
	delete mask;

	glfwTerminate();

	return 0;
}

unsigned int makeShader(string vertexPath, string fragmentPath)
{
	vector<unsigned int> shaderModules;
	shaderModules.push_back(makeModule(vertexPath, GL_VERTEX_SHADER));
	shaderModules.push_back(makeModule(fragmentPath, GL_FRAGMENT_SHADER));

	unsigned int shader = glCreateProgram();
	for (unsigned int shaderModule : shaderModules)
		glAttachShader(shader, shaderModule); //attaches modules to shader program.
	
	//Links modules attached to program and creates executeble to run on GPU
	glLinkProgram(shader);

	int success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success)
	{
		char shaderLinkError[1024];
		glGetProgramInfoLog(shader, 1024, NULL, shaderLinkError);
		ColouredOutput("Shader Program Linking Error: ", Colour::red);
		ColouredOutput(shaderLinkError, Colour::red);
	}

	//After linking the shaderModules can be deleted
	for (unsigned int shaderModule : shaderModules)
		glDeleteShader(shaderModule);

	return shader;
}


unsigned int makeModule(string filepath, unsigned int moduleType)
{
	//Check if file is readable
	string shaderSource;
	FileToString(filepath, &shaderSource, false);

	const char* shaderSrc = shaderSource.c_str();
	
	//Create a shader module (What is a shader module?)
	unsigned int shaderModule = glCreateShader(moduleType);
	//Replaces shader module with the source code supplied
	glShaderSource(shaderModule, 1, &shaderSrc, NULL);
	//Complies source code
	glCompileShader(shaderModule);
	
	int success;
	glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char moduleCompError[1024];
		glGetShaderInfoLog(shaderModule, 1024, NULL, moduleCompError);
		ColouredOutput("Shader Module Compliation Error: ", Colour::red);
		ColouredOutput(moduleCompError, Colour::red);
	}
	
	return shaderModule;
	
}