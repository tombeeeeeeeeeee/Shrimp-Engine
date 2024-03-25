#include <config.h>
#include "controller/app.h"

#include "components/ComponentInclude.h"

#include "factories/AssetFactory.h"
#include "factories/ComponentFactory.h"

int main() {

	App* app = new App();

	AssetFactory* assetFactory = new AssetFactory("Assets/");
	ComponentFactory* componentFactory = new ComponentFactory(app->physicsComponents, app->renderComponents, app->transformComponents, *assetFactory);

	std::vector<unsigned int> gameObjects;
	int cubeCount = 40;
	for (int i = 0; i < cubeCount; i++)
	{
		float x = (20.0f * (float)rand() / RAND_MAX) - 10.0f;
		float y = (20.0f * (float)rand() / RAND_MAX) - 10.0f;
		float z = (20.0f * (float)rand() / RAND_MAX) - 10.0f;

		float xRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
		float yRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;
		float zRot = (360.0f * (float)rand() / RAND_MAX) - 180.0f;

		if(i % 2 == 0)
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

	app->Run();

	delete assetFactory;
	delete componentFactory;
	delete app;
	return 0;
}