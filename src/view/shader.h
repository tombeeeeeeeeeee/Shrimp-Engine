#pragma once
#include <config.h>

/// <summary>
/// Creates shader module for a shader program
/// </summary>
/// <param name="filepath">file path of shader script</param>
/// <param name="moduleType">GLFW Module Type</param>
/// <returns>Shader module</returns>
unsigned int makeModule(const string& filepath, unsigned int moduleType);


/// <summary>
/// Creates shader program for the GPU
/// </summary>
/// <param name="vertexShader"> File path for vertex shader</param>
/// <param name="fragmentShader">File path for fragment shader</param>
unsigned int MakeShader(
	const string& vertexShader = "src/shaders/vertex.vert",
	const string& fragmentShader = "src/shaders/fragment.frag"
);

unsigned int MakeShaderMatchingName(const string& shaderHeader);
