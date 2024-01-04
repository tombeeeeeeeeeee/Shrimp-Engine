
#include "config.h"
#include "Material.h"
#include "Graphics.h"



int main(void)
{
	Graphics* graphics = new Graphics("Testing Window");

	graphics->InitialiseShaders();

	Material* faceMaterial = new Material("face", "src/img/me.PNG");
	faceMaterial->MapInitialise("src/img/vignette.png", MaterialMap::opacity);

	graphics->AddMaterialToDraw(faceMaterial);


	//The main ‘game’ loop.
	while (!glfwWindowShouldClose(graphics->window))
	{
		//Tell GLFW to check if anything is going on with input, etc.
		glfwPollEvents();

		graphics->Draw();
	}

	delete graphics;

	//If we get to this point, the window has closed, so clean up GLFW and exit.
	glfwTerminate();

	return 0;
}