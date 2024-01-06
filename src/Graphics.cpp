#include "Graphics.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

Graphics::Graphics(const char* windowName, int width, int height)
{
	//Initialise GLFW, make sure it works. Put an error message here if you like.
	if (!glfwInit())
	{
		ColouredOutput("Failed to Initialise OpenGL.\n", Colour::red);
		return;
	}
	//Set resolution here, and give your window a different title.
	window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
	if (!window)
	{
		ColouredOutput("Failed to create window.\n", Colour::red);
		glfwTerminate(); //Again, you can put a real error message here.
		return;
	}

	//This tells GLFW that the window we created is the one we should render to.
	glfwMakeContextCurrent(window);

	//Tell GLAD to load all its OpenGL functions.
	if (!gladLoadGL())
	{
		ColouredOutput("Failed to load OpenGL functions.\n", Colour::red);
		return;
	}

	ColouredOutput("Opening Window", Colour::green);

	//Triangles for testing
	triangle = new TriangleMesh();

	Material* faceMaterial = new Material("face", "src/img/me.PNG");
	faceMaterial->MapInitialise("src/img/vignette.png", MaterialMap::opacity);

	AddMaterialToDraw(faceMaterial);
}

Graphics::~Graphics()
{
	glDeleteProgram(shaderProgram);

	delete triangle;
	for(Material* mat : drawMaterials)
		delete mat;
}

void Graphics::InitialiseShaders(string vertexShader, string fragmentShader)
{
	vector<unsigned int> shaderModules;
	shaderModules.push_back(makeModule(vertexShader, GL_VERTEX_SHADER));
	shaderModules.push_back(makeModule(fragmentShader, GL_FRAGMENT_SHADER));

	shaderProgram = glCreateProgram();
	for (unsigned int shaderModule : shaderModules)
		glAttachShader(shaderProgram, shaderModule); //attaches modules to shader program.

	//Links modules attached to program and creates executeble to run on GPU
	glLinkProgram(shaderProgram);


	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		char shaderLinkError[1024];
		glGetProgramInfoLog(shaderProgram, 1024, NULL, shaderLinkError);
		ColouredOutput("Shader Program Linking Error: ", Colour::red);
		ColouredOutput(shaderLinkError, Colour::red);
	}

	//After linking the shaderModules can be deleted
	for (unsigned int shaderModule : shaderModules)
		glDeleteShader(shaderModule);

	//Set texture units
	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram, "material"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "mask"), 1);

	//enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Grab locations on GPU
	modelLocation = glGetUniformLocation(shaderProgram, "model");
	viewLocation = glGetUniformLocation(shaderProgram, "view");
	projectionLocation = glGetUniformLocation(shaderProgram, "projection");

	cameraPos = vec3( -5, 0, 3);

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	mat4 projection = ProjectionMatrix(45, w/h, 0.1, 10);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projection.entries);
}

void Graphics::SetWindowName(const char* windowName)
{
	glfwSetWindowTitle(window, windowName);
}

void Graphics::SetWindowSize(int width, int height)
{
	glViewport(0, 0, width, height);
}


void Graphics::Draw()
{
	//Clear the screen � eventually do rendering code here.
	glClear(GL_COLOR_BUFFER_BIT);
	
	mat4 view = ViewMatrix(cameraPos, vec3(0,0,0));
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, view.entries);

	mat4 modelRotation = rotationZAxisMatrix(10 * glfwGetTime());
	mat4 model = createTranslationMatrix({ -0.2f, 0.4f, 0 }) * modelRotation;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model.entries);

	//Adaptive Screensize for window dragging
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	//Use shaders to draw triangle
	glUseProgram(shaderProgram);

	for(Material* mat : drawMaterials)
		mat->use();


	triangle->draw();

	//Swapping the buffers � this means this frame is over.
	glfwSwapBuffers(window);
}

void Graphics::AddMaterialToDraw(Material* mat)
{
	vector<Material*>::iterator it = drawMaterials.begin();

	while (it != drawMaterials.end())
	{
		if (*it == mat) return;
		it++;
	}

	drawMaterials.push_back(mat);
}

void Graphics::RemoveMaterialFromDraw(Material* mat)
{
	vector<Material*>::iterator it = drawMaterials.begin();

	while (it != drawMaterials.end())
	{
		if (*it == mat)
		{
			drawMaterials.erase(it);
			return;
		}
		it++;
	}
}

unsigned int Graphics::makeModule(string filepath, unsigned int moduleType)
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
