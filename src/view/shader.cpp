#include "shader.h"

unsigned int makeModule(const string& filepath, unsigned int moduleType)
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

unsigned int MakeShader(const string& vertexShader, const string& geomShader ,const string& fragmentShader)
{
	vector<unsigned int> shaderModules;

	//Make Vertex Shader
	shaderModules.push_back(makeModule(vertexShader, GL_VERTEX_SHADER));

	//Make Fragment Shader
	shaderModules.push_back(makeModule(fragmentShader, GL_FRAGMENT_SHADER));


	unsigned int shaderProgram = glCreateProgram();
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

	return shaderProgram;
}


