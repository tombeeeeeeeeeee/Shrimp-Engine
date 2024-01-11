#include <config.h>
#include "controller/app.h"

#include "components/ComponentInclude.h"

#include "factories/factory.h"

int main() {

	App* app = new App();

	Factory* factory = new Factory(app->physicsComponents, app->renderComponents, app->transformComponents);

	factory->MakeCube({ 3.0f, 0.0f, 0.25f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 10.0f });

	factory->MakeRat({ 4.0f, 0.0f, 0.05f }, { 0.0f, 0.0f, 180.0f });

	unsigned int cameraEntity = factory->MakeCamera({ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f,0.0f });

	CameraComponent* camera = new CameraComponent();
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->SetUpOpengl();
	app->MakeSystems();

	app->Run();

	delete factory;
	delete app;
	return 0;
}