#include <config.h>
#include "controller/app.h"


int main() {

	App* app = new App();

	std::vector<unsigned int> gameObjects;
	int cubeCount = 20;
	srand(time(NULL));
	for (int i = 0; i < cubeCount; i++)
	{
		float x = (20.0f * (float)rand() / RAND_MAX) - 10.0f;
		float y = (20.0f * (float)rand() / RAND_MAX) - 10.0f;
		float z = (20.0f * (float)rand() / RAND_MAX) - 10.0f;

		float xRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
		float yRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
		float zRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;

		gameObjects.push_back(componentFactory->MakeCube({x, y, z}, {xRot, yRot, zRot}));

		//else
		//	gameObjects.push_back(componentFactory->MakeRat({x, y, z}, {xRot, yRot, zRot}));
	}

	unsigned int cameraEntity = componentFactory->MakeCamera({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f,0.0f });

	CameraComponent* camera = new CameraComponent();
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->SetUpOpengl();
	app->MakeSystems();
	app->MakeFactories();

	app->Run();

	delete app;
	return 0;
}