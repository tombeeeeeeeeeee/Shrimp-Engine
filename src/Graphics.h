#pragma once
#include <config.h>
#include "triangle_mesh.h"
#include "Material.h"

class Graphics
{
public:

	GLFWwindow* window;

	/// <summary>
	/// Create window, and initialise Open GL Libraries 
	/// </summary>
	/// <param name="windowName"> Name for Window </param>
	/// <param name="width"> Window Pixel Width</param>
	/// <param name="height"> Window Height</param>
	Graphics(const char* windowName, int width = 1280, int height = 720);
	~Graphics();

	/// <summary>
	/// Initialise the shaders
	/// </summary>
	/// <param name="vertexShader"> File path for vertex shader</param>
	/// <param name="fragmentShader">File path for fragment shader</param>
	void InitialiseShaders(
		string vertexShader = "src/shaders/vertex.txt",
		string fragmentShader = "src/shaders/fragment.txt"
	);

	void SetWindowName(const char* windowName);
	void SetWindowSize(int width, int height);

	/// <summary>
	/// Draw graphics (call once per update)
	/// </summary>
	void Draw();

	void AddMaterialToDraw(Material* mat);
	void RemoveMaterialFromDraw(Material* mat);

	//--TO ADD: Change to adding lambda's, such that a gameObject has a virtual draw function.
	//void AddObjectToDraw(GameObject* obj);
	//bool RemoveObjectFromDraw(GameObject* obj);

private:

	//vector<GameObject*> drawObjects;
	vector<Material*> drawMaterials;
	unsigned int shaderProgram;
	
	unsigned int modelLocation;


	//Testing Triangle
	TriangleMesh* triangle;

	unsigned int makeModule(string filepath, unsigned int moduleType);
};

