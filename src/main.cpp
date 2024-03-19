#include <config.h>
#include "controller/app.h"

#include "components/ComponentInclude.h"

#include "factories/AssetFactory.h"
#include "factories/ComponentFactory.h"

int main() {

	App* app = new App();

	AssetFactory* assetFactory = new AssetFactory(app->physicsComponents, app->renderComponents, app->transformComponents);
	ComponentFactory* componentFactory = new ComponentFactory(app->physicsComponents, app->renderComponents, app->transformComponents);

	componentFactory->MakeCube({ 3.0f, 0.0f, 0.25f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 10.0f });

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